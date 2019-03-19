/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef SIMULATION_H
#define SIMULATION_H

#include <yaml-cpp/yaml.h>
#include <stk_util/diag/PrintTimer.hpp>
#include <stk_util/diag/Timer.hpp>

class YAML::Node;
class Realms;
class LinearSolvers;

//! Container that holds all data regearding the simulation and specifies the sequence of execution of the different methods
class Simulation {
public:
    Simulation(const YAML::Node & root_node);
    ~Simulation();
    void breadboard();
    void load(const YAML::Node & node);
    void initialize();
    void run();
    Simulation * root();
    Simulation * parent();
    bool debug();
    bool debug() const;
  
    const YAML::Node & m_root_node;
    Realms * realms_;
    LinearSolvers * linearSolvers_;
    static bool debug_;
};

#endif /* SIMULATION_H */

