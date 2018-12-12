/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include <iostream>

// yaml for parsing..
#include <yaml-cpp/yaml.h>

#include <Simulation.h>
#include <Realm.h>
#include <LinearSolvers.h>

Simulation::Simulation(const YAML::Node& root_node) :
    m_root_node(root_node),
    realm_(NULL),
    linearSolvers_(NULL)
{
}

Simulation::~Simulation() { 
}

void Simulation::load(const YAML::Node& node) {
    
    // load the linear solver configs
    linearSolvers_ = new LinearSolvers(*this);
    linearSolvers_->load(node);

    // create the realms
    realm_ = new Realm();
    realm_->load(node);
}

void Simulation::initialize() {
    realm_->initialize();
}

void Simulation::run() {
    
}