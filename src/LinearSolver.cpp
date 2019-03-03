/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "LinearSolver.h"
#include <HOFlowEnv.h>
#include <LinearSolverTypes.h>

#include <stk_util/util/ReportHandler.hpp>

// Tpetra support
#include <BelosLinearProblem.hpp>
#include <BelosMultiVecTraits.hpp>
#include <BelosOperatorTraits.hpp>
#include <BelosSolverFactory.hpp>
#include <BelosSolverManager.hpp>
#include <BelosConfigDefs.hpp>
#include <BelosLinearProblem.hpp>
#include <BelosTpetraAdapter.hpp>

#include <Ifpack2_Factory.hpp>
#include <Kokkos_DefaultNode.hpp>
#include <Kokkos_Serial.hpp>
#include <Teuchos_ArrayRCP.hpp>
#include <Teuchos_DefaultMpiComm.hpp>
#include <Teuchos_OrdinalTraits.hpp>
#include <Tpetra_CrsGraph.hpp>
#include <Tpetra_Export.hpp>
#include <Tpetra_Operator.hpp>
#include <Tpetra_Map.hpp>
#include <Tpetra_MultiVector.hpp>
#include <Tpetra_Vector.hpp>

#include <Teuchos_ParameterXMLFileReader.hpp>

#include <iostream>

LinearSolver::LinearSolver(std::string name, LinearSolvers * linearSolvers, LinearSolverConfig * config) : 
    name_(name),
    linearSolvers_(linearSolvers),
    config_(config),
    recomputePreconditioner_(config->recomputePreconditioner()),
    reusePreconditioner_(config->reusePreconditioner()),
    timerPrecond_(0.0)
{
    // nothing to do
}

LinearSolver::~LinearSolver() {
}

Simulation * LinearSolver::root() { 
    return linearSolvers_->root(); 
}

LinearSolvers * LinearSolver::parent() { 
    return linearSolvers_; 
}

bool & LinearSolver::recomputePreconditioner() {
    return recomputePreconditioner_;
}

bool & LinearSolver::reusePreconditioner() {
    return reusePreconditioner_;
}

void LinearSolver::zero_timer_precond() { 
    timerPrecond_ = 0.0;
}

double LinearSolver::get_timer_precond() { 
    return timerPrecond_;
}

LinearSolverConfig * LinearSolver::getConfig() { 
    return config_; 
}