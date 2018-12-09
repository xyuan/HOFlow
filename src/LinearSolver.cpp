/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "LinearSolver.h"
#include <yaml-cpp/yaml.h>
#include <iostream>


LinearSolver::LinearSolver() {
}

LinearSolver::~LinearSolver() {
}

void LinearSolver::load(const YAML::Node& node) {
    const YAML::Node nodes = node["linear_solvers"];
    if ( nodes ) {
        
        // loop through the linear solvers specified in the input file
        for ( size_t inode = 0; inode <  nodes.size(); ++inode ) {
            const YAML::Node linear_solver_node = nodes[inode];
            std::string solver_type = "tpetra";
            //get_if_present_no_default(linear_solver_node, "type", solver_type);      
            
            if (solver_type == "tpetra") {
                //TpetraLinearSolverConfig * linearSolverConfig = new TpetraLinearSolverConfig();
                //linearSolverConfig->load(linear_solver_node);
                //solverTpetraConfig_[linearSolverConfig->name()] = linearSolverConfig; 
            }
            // add elseif here if another solver should also be supported
            else {
                throw std::runtime_error("unknown solver type");
            }
        }
    }
}

