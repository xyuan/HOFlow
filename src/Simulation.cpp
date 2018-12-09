/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include <iostream>

// yaml for parsing..
#include <yaml-cpp/yaml.h>

#include <Simulation.h>
#include <Realm.h>
#include <LinearSolver.h>

Simulation::Simulation(const YAML::Node& root_node) :
    m_root_node(root_node),
    realm_(NULL),
    linearSolver_(NULL)
{
    
}

Simulation::~Simulation() {
    
}

void Simulation::load(const YAML::Node& node) {
    
    // load the linear solver configs
    linearSolver_ = new LinearSolver();
    linearSolver_->load(node);

    // create the realms
    realm_ = new Realm();
    realm_->load(node);
}

void Simulation::initialize() {
    realm_->initialize();
}

void Simulation::run() {
    
}