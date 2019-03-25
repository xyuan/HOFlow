/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "EquationSystem.h"
#include <LinearSystem.h>
#include <TpetraLinearSystem.h>
#include <Algorithm.h>
#include <AuxFunctionAlgorithm.h>
#include <SolverAlgorithmDriver.h>
#include <Enums.h>
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>
#include <ConstantAuxFunction.h>
#include <kernel/KernelBuilderLog.h>

#include <stk_mesh/base/Field.hpp>

// stk
#include <stk_mesh/base/MetaData.hpp>

#include <stk_util/parallel/ParallelReduce.hpp>

EquationSystem::EquationSystem(EquationSystems & eqSystems, const std::string name, const std::string eqnTypeName) :
    equationSystems_(eqSystems),
    realm_(eqSystems.realm_),
    name_(name),
    userSuppliedName_(name),
    eqnTypeName_(eqnTypeName),
    maxIterations_(1),
    convergenceTolerance_(1.0),
    solverAlgDriver_(new SolverAlgorithmDriver(realm_)),
    timerAssemble_(0.0),
    timerLoadComplete_(0.0),
    timerSolve_(0.0),
    timerMisc_(0.0),
    timerInit_(0.0),
    timerPrecond_(0.0),
    avgLinearIterations_(0.0),
    maxLinearIterations_(0.0),
    minLinearIterations_(1.0e10),
    nonLinearIterationCount_(0),
    reportLinearIterations_(false),
    firstTimeStepSolve_(true),
    edgeNodalGradient_(false),
    linsys_(NULL),
    num_graph_entries_(0)
{
    // nothing to do
}

EquationSystem::~EquationSystem() {
   delete solverAlgDriver_;

    if ( NULL != linsys_ )
      delete linsys_;

    // initial conditions and bc prop
    std::vector<AuxFunctionAlgorithm *>::iterator ii;
    for( ii=bcDataAlg_.begin(); ii!=bcDataAlg_.end(); ++ii )
      delete *ii;

    std::vector<Algorithm *>::iterator iim;
    for( iim=bcDataMapAlg_.begin(); iim!=bcDataMapAlg_.end(); ++iim )
      delete *iim;

    for( iim=copyStateAlg_.begin(); iim!=copyStateAlg_.end(); ++iim )
      delete *iim;

    // prop algs
    std::vector<Algorithm *>::iterator iip;
    for( iip=propertyAlg_.begin(); iip!=propertyAlg_.end(); ++iip ) {
      delete *iip;
    }

    for (auto it = preIterAlgDriver_.begin(); it != preIterAlgDriver_.end(); ++it) {
      delete *it;
    }

    for (auto it = postIterAlgDriver_.begin(); it != postIterAlgDriver_.end(); ++it) {
      delete *it;
    }
}

void EquationSystem::load(const YAML::Node & node) {
    get_required(node, "name", userSuppliedName_);
    get_required(node, "max_iterations", maxIterations_);
    get_required(node, "convergence_tolerance", convergenceTolerance_);
}

void EquationSystem::set_nodal_gradient(const std::string &dofName) {
    // determine nodal gradient
    std::map<std::string, std::string >::iterator ii = realm_.solutionOptions_->nodalGradMap_.find(dofName);
    if ( ii != realm_.solutionOptions_->nodalGradMap_.end() ) {
        if ( ii->second == "element" )
            edgeNodalGradient_ = false;
        else 
            throw std::runtime_error("only element nodal gradient supported");
    }
}

bool EquationSystem::system_is_converged() {
    bool isConverged = true;
    if ( NULL != linsys_ ) {
        isConverged = (linsys_->scaledNonLinearResidual() <  convergenceTolerance_ );
    }
    
    return isConverged;
}

void EquationSystem::assemble_and_solve(stk::mesh::FieldBase *deltaSolution) {
    int error = 0;

    // zero the system
    double timeA = HOFlowEnv::self().hoflow_time();
    linsys_->zeroSystem();
    double timeB = HOFlowEnv::self().hoflow_time();
    timerAssemble_ += (timeB-timeA);

    // apply all flux and dirichlet algs
    timeA = HOFlowEnv::self().hoflow_time();
    solverAlgDriver_->execute();
    timeB = HOFlowEnv::self().hoflow_time();
    timerAssemble_ += (timeB-timeA);

    // load complete
    timeA = HOFlowEnv::self().hoflow_time();
    linsys_->loadComplete();
    timeB = HOFlowEnv::self().hoflow_time();
    timerLoadComplete_ += (timeB-timeA);

    // solve the system; extract delta
    timeA = HOFlowEnv::self().hoflow_time();
    error = linsys_->solve(deltaSolution);
    timeB = HOFlowEnv::self().hoflow_time();
    timerSolve_ += (timeB-timeA);
    timerPrecond_ += linsys_->get_timer_precond();

    // handle statistics
    update_iteration_statistics(linsys_->linearSolveIterations());

    if ( error > 0 )
        HOFlowEnv::self().hoflowOutputP0() << "Error in " << userSuppliedName_ << "::solve_and_update()  " << std::endl;
}

UserDataType EquationSystem::get_bc_data_type(const UserData &userData, std::string &name) {
    UserDataType dataType = CONSTANT_UD;
    std::map<std::string, UserDataType>::const_iterator iter
        = userData.bcDataTypeMap_.find(name);
    if ( iter != userData.bcDataTypeMap_.end() ) {
        dataType = (*iter).second;
    }
    return dataType;
}

void EquationSystem::evaluate_properties() {
    for ( size_t k = 0; k < propertyAlg_.size(); ++k ) {
        propertyAlg_[k]->execute();
    }
}

void
EquationSystem::initial_work()
{
  std::vector<Algorithm *>::iterator ii;
  for( ii=copyStateAlg_.begin(); ii!=copyStateAlg_.end(); ++ii )
    (*ii)->execute();
}

void
EquationSystem::report_invalid_supp_alg_names()
{
  bool noInvalidNamesFound = KernelBuilderLog::self().print_invalid_kernel_names(
    eqnTypeName_, realm_.solutionOptions_->elemSrcTermsMap_
  );

  if (!noInvalidNamesFound) {
    KernelBuilderLog::self().print_valid_kernel_names(eqnTypeName_);

    std::string msg =
      "Invalid supplemental algorithms name(s) for " + eqnTypeName_ + ". See log for details.";

    throw std::runtime_error(msg);
  }
}
//--------------------------------------------------------------------------
void
EquationSystem::report_built_supp_alg_names()
{
  KernelBuilderLog::self().print_built_kernel_names(eqnTypeName_);
}
//--------------------------------------------------------------------------
bool
EquationSystem::supp_alg_is_requested(std::string suppAlgName)
{
  const auto& nameMap = realm_.solutionOptions_->elemSrcTermsMap_;
  auto it = nameMap.find(eqnTypeName_);
  if (it == nameMap.end()) {
    return false;
  }
  const std::vector<std::string>& nameVec = it->second;

  if (std::find(nameVec.begin(), nameVec.end(), suppAlgName) == nameVec.end()) {
    return false;
  }
  return true;
}

bool
EquationSystem::supp_alg_is_requested(std::vector<std::string> names)
{
  const auto& nameMap = realm_.solutionOptions_->elemSrcTermsMap_;
  auto it = nameMap.find(eqnTypeName_);

  if (it == nameMap.end()) {
    return false;
  }

  const std::vector<std::string>& nameVec = it->second;

  bool found = false;
  for(auto algName: names) {
    if (std::find(nameVec.begin(), nameVec.end(), algName) != nameVec.end()) {
      found = true;
      break;
    }
  }
  return found;
}

bool
EquationSystem::nodal_src_is_requested()
{
  auto isrc = realm_.solutionOptions_->srcTermsMap_.find(eqnTypeName_);

  return (isrc != realm_.solutionOptions_->srcTermsMap_.end());
}

void
EquationSystem::pre_iter_work()
{
  for (auto it: preIterAlgDriver_) {
    it->execute();
  }
}

void
EquationSystem::post_iter_work()
{
  for (auto it: postIterAlgDriver_) {
    it->execute();
  }
}

//--------------------------------------------------------------------------
//-------- pre_timestep_work -----------------------------------------------
//--------------------------------------------------------------------------
void
EquationSystem::pre_timestep_work()
{
  firstTimeStepSolve_ = true;
}

//--------------------------------------------------------------------------
//-------- provide_scaled_norm ---------------------------------------------
//--------------------------------------------------------------------------
double
EquationSystem::provide_scaled_norm()
{
  return ( (NULL != linsys_) ? linsys_->scaledNonLinearResidual() : 0.0 );
}

//--------------------------------------------------------------------------
//-------- provide_norm ---------------------------------------------
//--------------------------------------------------------------------------
double
EquationSystem::provide_norm()
{
  return ( (NULL != linsys_) ? linsys_->nonLinearResidual() : 0.0 );
}

//--------------------------------------------------------------------------
//-------- provide_norm_increment ------------------------------------------
//--------------------------------------------------------------------------
double
EquationSystem::provide_norm_increment()
{
  return ( (NULL != linsys_) ? 1.0 : 0.0 );
}

//--------------------------------------------------------------------------
//-------- dump_eq_time ----------------------------------------------------
//--------------------------------------------------------------------------
void
EquationSystem::dump_eq_time()
{
  // subtract out preconditioning time from solve time
  timerSolve_ -= timerPrecond_;

  double l_timer[6] = {timerAssemble_, timerLoadComplete_, timerSolve_, timerMisc_, timerInit_, timerPrecond_};
  double g_min[6] = {};
  double g_max[6] = {};
  double g_sum[6] = {};

  int nprocs = HOFlowEnv::self().parallel_size();

  HOFlowEnv::self().hoflowOutputP0() << "Timing for Eq: " << userSuppliedName_ << std::endl;

  // get max, min, and sum over processes
  stk::all_reduce_sum(HOFlowEnv::self().parallel_comm(), &l_timer[0], &g_sum[0], 6);
  stk::all_reduce_min(HOFlowEnv::self().parallel_comm(), &l_timer[0], &g_min[0], 6);
  stk::all_reduce_max(HOFlowEnv::self().parallel_comm(), &l_timer[0], &g_max[0], 6);

  // output
  HOFlowEnv::self().hoflowOutputP0() << "             init --  " << " \tavg: " << g_sum[4]/double(nprocs)
                  << " \tmin: " << g_min[4] << " \tmax: " << g_max[4] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "         assemble --  " << " \tavg: " << g_sum[0]/double(nprocs)
                  << " \tmin: " << g_min[0] << " \tmax: " << g_max[0] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "    load_complete --  " << " \tavg: " << g_sum[1]/double(nprocs)
                  << " \tmin: " << g_min[1] << " \tmax: " << g_max[1] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "            solve --  " << " \tavg: " << g_sum[2]/double(nprocs)
                  << " \tmin: " << g_min[2] << " \tmax: " << g_max[2] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "    precond setup --  " << " \tavg: " << g_sum[5]/double(nprocs)
                  << " \tmin: " << g_min[5] << " \tmax: " << g_max[5] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "             misc --  " << " \tavg: " << g_sum[3]/double(nprocs)
                  << " \tmin: " << g_min[3] << " \tmax: " << g_max[3] << std::endl;

  if (reportLinearIterations_)
    HOFlowEnv::self().hoflowOutputP0() << "linear iterations -- " << " \tavg: " << avgLinearIterations_
                    << " \tmin: " << minLinearIterations_ << " \tmax: "
                    << maxLinearIterations_ << std::endl;

  // reset anytime these are called; 
  // some EquationSystems have no linear system, e.g., LowMach holds .. uvw_p
  timerAssemble_ = 0.0;
  timerLoadComplete_ = 0.0;
  timerMisc_ = 0.0;
  timerSolve_ = 0.0;
  timerInit_ = 0.0;
  timerPrecond_ = 0.0;
  if ( NULL != linsys_ )
    linsys_->zero_timer_precond();
  avgLinearIterations_ = 0.0;
  minLinearIterations_ = 1.0e10;
  maxLinearIterations_ = 0.0;
  nonLinearIterationCount_ = 0;
}

//--------------------------------------------------------------------------
//-------- update_iteration_statistics -------------------------------------
//--------------------------------------------------------------------------
void
EquationSystem::update_iteration_statistics(
  const int & iters)
{
  const double iterations = (double)iters;
  avgLinearIterations_ = (nonLinearIterationCount_*avgLinearIterations_
                          + iterations)/(nonLinearIterationCount_+1);
  maxLinearIterations_ = std::max(maxLinearIterations_,iterations);
  minLinearIterations_ = std::min(minLinearIterations_,iterations);
  nonLinearIterationCount_ += 1;
  reportLinearIterations_ = true;
}

//--------------------------------------------------------------------------
//-------- bc_data_specified ----------------------------------------------------
//--------------------------------------------------------------------------
bool
EquationSystem::bc_data_specified(
  const UserData &userData, std::string &name)
{
  bool isSpecified = false;
  std::map<std::string, bool>::const_iterator iter
    = userData.bcDataSpecifiedMap_.find(name);
  if ( iter != userData.bcDataSpecifiedMap_.end() ) {
    isSpecified = true;
  }
  return isSpecified;
}