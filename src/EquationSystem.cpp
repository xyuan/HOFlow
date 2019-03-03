/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "EquationSystem.h"
#include <LinearSystem.h>
#include <TpetraLinearSystem.h>
#include <Enums.h>
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>

EquationSystem::EquationSystem(EquationSystems & eqSystems, const std::string name, const std::string eqnTypeName) :
    equationSystems_(eqSystems),
    realm_(eqSystems.realm_),
    name_(name),
    userSuppliedName_(name),
    eqnTypeName_(eqnTypeName),
    maxIterations_(1),
    convergenceTolerance_(1.0),
    avgLinearIterations_(0.0),
    maxLinearIterations_(0.0),
    minLinearIterations_(1.0e10),
    nonLinearIterationCount_(0),
    reportLinearIterations_(false),
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
        if ( ii->second == "edge" ) 
            edgeNodalGradient_ = true;
        else if ( ii->second == "element" )
            edgeNodalGradient_ = false;
        else 
            throw std::runtime_error("only edge or element nodal gradient supported");
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

