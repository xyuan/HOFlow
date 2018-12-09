/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "LinearSolver.h"
#include "TpetraLinearSolverConfig.h"
#include <yaml-cpp/yaml.h>
#include <iostream>


LinearSolver::LinearSolver() {
}

LinearSolver::~LinearSolver() {
}

void LinearSolver::load(const YAML::Node& node) {
    const YAML::Node nodes = node["linear_solvers"]; // extract the part about linear solver from the input file
    if ( nodes ) {
        const YAML::Node linear_solver_node = nodes[0]; // get the first linear solver declared in the input file
        std::string solver_type = linear_solver_node["type"]; // get the solver type specified in the input file
            
        if (solver_type == "tpetra") {
            TpetraLinearSolverConfig * linearSolverConfig = new TpetraLinearSolverConfig();
            linearSolverConfig->load(linear_solver_node);
            solverTpetraConfig_[linearSolverConfig->name()] = linearSolverConfig; 
        }
        // add elseif here if another solver should also be supported
        else {
            throw std::runtime_error("unknown solver type");
        }
    }
}
