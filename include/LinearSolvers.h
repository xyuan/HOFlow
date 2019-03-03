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

//! Stores one or multiple linear solvers (objects of the class LinearSolver)
class LinearSolvers {
public:
    LinearSolvers(Simulation & sim);
    ~LinearSolvers();
    void load(const YAML::Node & node);
    LinearSolver *create_solver(std::string solverBlockName, EquationType theEQ);
    Simulation *root();
    Simulation *parent();
    
    typedef std::map<EquationType, LinearSolver *> SolverMap;
    typedef std::map<std::string, TpetraLinearSolverConfig *> SolverTpetraConfigMap;
    SolverMap solvers_; // mapping solvers to equation types
    SolverTpetraConfigMap solverTpetraConfig_; // solver configurations of tpetra type solvers, acces with name from input file
    
    Simulation & sim_;
};

#endif /* LINEARSOLVERS_H */

