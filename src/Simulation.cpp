/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include <Simulation.h>
#include <Realms.h>
#include <LinearSolvers.h>
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>
#include <iostream>
#include <yaml-cpp/yaml.h>

// Definition outstide of class because of debug_ being a static variable
bool Simulation::debug_ = false;

//! Constructor
Simulation::Simulation(const YAML::Node& root_node) :
    m_root_node(root_node),
    realms_(NULL),
    linearSolvers_(NULL)
{}

//! Destructor
Simulation::~Simulation() {
    delete realms_;
    delete linearSolvers_;
}

void Simulation::breadboard() {
    realms_->breadboard();
}

//! Loads the information necessary to do the simulation
void Simulation::load(const YAML::Node & node) {
    
    // load the linear solver configs
    linearSolvers_ = new LinearSolvers(*this);
    linearSolvers_->load(node);

    // create the realms
    realms_ = new Realms(*this);
    realms_->load(node);
}

//! Initializes all computational domains
void Simulation::initialize() {
    realms_->initialize();
}

//! Run the simulation
void Simulation::run() {
    std::cout << "running sim" << std::endl; 
}

Simulation * Simulation::root() {
    return this; 
}

Simulation * Simulation::parent() {
    return 0; 
}

bool Simulation::debug() {
    return debug_;
}

bool Simulation::debug() const {
    return debug_;
}