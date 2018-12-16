/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include <iostream>
#include <yaml-cpp/yaml.h>

#include <Simulation.h>
#include <Realms.h>
#include <LinearSolvers.h>

Simulation::Simulation(const YAML::Node& root_node) :
    m_root_node(root_node),
    realms_(NULL),
    linearSolvers_(NULL)
{}

Simulation::~Simulation() {
}

void Simulation::load(const YAML::Node& node) {
    
    // load the linear solver configs
    linearSolvers_ = new LinearSolvers(*this);
    linearSolvers_->load(node);

    // create the realms
    realms_ = new Realms();
    realms_->load(node);
}

void Simulation::initialize() {
    realms_->initialize();
}

void Simulation::run() {
    
}