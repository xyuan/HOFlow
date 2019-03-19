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
    linsys_(NULL)
{
    // nothing to do
}

EquationSystem::~EquationSystem() {
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
//    int error = 0;
//
//    // zero the system
//    double timeA = HOFlowEnv::self().nalu_time();
//    linsys_->zeroSystem();
//    double timeB = HOFlowEnv::self().nalu_time();
//    timerAssemble_ += (timeB-timeA);
//
//    // apply all flux and dirichlet algs
//    timeA = HOFlowEnv::self().nalu_time();
//    solverAlgDriver_->execute();
//    timeB = HOFlowEnv::self().nalu_time();
//    timerAssemble_ += (timeB-timeA);
//
//    // load complete
//    timeA = HOFlowEnv::self().nalu_time();
//    linsys_->loadComplete();
//    timeB = HOFlowEnv::self().nalu_time();
//    timerLoadComplete_ += (timeB-timeA);
//
//    // solve the system; extract delta
//    timeA = HOFlowEnv::self().nalu_time();
//    error = linsys_->solve(deltaSolution);
//    timeB = HOFlowEnv::self().nalu_time();
//    timerSolve_ += (timeB-timeA);
//    timerPrecond_ += linsys_->get_timer_precond();
//
//    if ( realm_.hasPeriodic_) {
//        timeA = HOFlowEnv::self().nalu_time();
//        realm_.periodic_delta_solution_update(deltaSolution, linsys_->numDof());
//        timeB = HOFlowEnv::self().nalu_time();
//        timerMisc_ += (timeB-timeA);
//    }
//
//    // handle statistics
//    update_iteration_statistics(linsys_->linearSolveIterations());
//
//    if ( error > 0 )
//        HOFlowEnv::self().hoflowOutputP0() << "Error in " << userSuppliedName_ << "::solve_and_update()  " << std::endl;
}

bool EquationSystem::supp_alg_is_requested(std::string suppAlgName) {
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

bool EquationSystem::supp_alg_is_requested(std::vector<std::string> names) {
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