/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "LinearSolvers.h"
#include <LinearSolver.h>
#include <LinearSolverConfig.h>
#include <TpetraLinearSolverConfig.h>
#include <Simulation.h>

#include <yaml-cpp/yaml.h>
#include <iterator>

LinearSolvers::LinearSolvers(Simulation & sim) :
    sim_(sim)
{
}

LinearSolvers::~LinearSolvers() {
    /*for (SolverMap::const_iterator pos=solvers_.begin(); pos!=solvers_.end(); ++pos) {
        delete pos->second;
    }
    for (SolverTpetraConfigMap::const_iterator pos=solverTpetraConfig_.begin(); pos!=solverTpetraConfig_.end(); ++pos) {
        delete pos->second;
    }*/
}

void LinearSolvers::load(const YAML::Node & node) {
    const YAML::Node nodes = node["linear_solvers"]; // extract the part about linear solver from the input file
    if (nodes) {
        for ( size_t inode = 0; inode <  nodes.size(); ++inode ) { // loop through multiple defined linear solvers
            std::cout << "linear solver " << inode << std::endl;
            const YAML::Node linear_solver_node = nodes[inode]; // get the linear solver declared in the input file
            std::string solver_type = linear_solver_node["type"].as<std::string>(); // get the solver type specified in the input file
            std::cout << "solver type " << solver_type << std::endl;

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
}

LinearSolver *LinearSolvers::create_solver(std::string solverBlockName, EquationType theEQ) {
    /* Deactivated because implementation is not finished
    // provide unique name
    std::string solverName = EquationTypeMap[theEQ] + "_Solver";

    LinearSolver *theSolver = NULL;

    // check in tpetra map...
    bool foundT = false;
    SolverTpetraConfigMap::const_iterator iterT = solverTpetraConfig_.find(solverBlockName);
    if (iterT != solverTpetraConfig_.end()) {
        TpetraLinearSolverConfig *linearSolverConfig = (*iterT).second;
        foundT = true;
        theSolver = new TpetraLinearSolver(solverName,
                                         linearSolverConfig,
                                         linearSolverConfig->params(),
                                         linearSolverConfig->paramsPrecond(), this);
    }

    // error check; none found
    if ( !foundT ) {
        throw std::runtime_error("solver name block not found; error in solver creation; check: " + solverName);
    }

    // set it and return
    solvers_[theEQ] = theSolver;
    return theSolver;
    */
    return NULL;
}
