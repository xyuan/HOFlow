/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef LINEARSOLVERS_H
#define LINEARSOLVERS_H

#include <Enums.h>
#include <yaml-cpp/yaml.h>
#include <LinearSolver.h>
#include <TpetraLinearSolver.h>
#include <LinearSolverConfig.h>
#include <TpetraLinearSolverConfig.h>

#include <map>
#include <string>

class Simulation;
class LinearSolver;

/** Container class to store one or multiple linear solvers
 *
 * The linear solvers stored are created based on the information
 * specified in the input file
 */
class LinearSolvers {
public:
    /** Initializes some variables*/
    LinearSolvers(Simulation & sim);
    
    /** Deletes all solver configurations and solvers created*/
    ~LinearSolvers();
    
    /** Create objects based on the information in the input file
     * 
     * Parses the input file and creates an object for the solver
     * configuration. A solver created later will use this configuration
     */
    void load(const YAML::Node & node);
    
    /** Creates a solver for the equation system
     *
     * The created solver are stored in the map solvers_ with the
     * equation type and the coresponding solver.
     * E.g. equation type = temperature, solver = TpetraLinearSoilver*/
    LinearSolver * create_solver(std::string solverBlockName, EquationType theEQ);
    
    Simulation * root();
    Simulation * parent();
    
    typedef std::map<EquationType, LinearSolver *> SolverMap;
    typedef std::map<std::string, TpetraLinearSolverConfig *> SolverTpetraConfigMap;
    
    /** Stores one or multiple equation types with coresponding solvers*/
    SolverMap solvers_;
    
    /** Stores one or multiple solver configurations that are created based on the information form the input file*/
    SolverTpetraConfigMap solverTpetraConfig_;
    
    Simulation & sim_;
};

#endif /* LINEARSOLVERS_H */

