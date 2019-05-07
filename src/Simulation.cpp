/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include <Simulation.h>
#include <Realms.h>
#include <LinearSolvers.h>
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>
#include <TimeIntegrator.h>
#include <SteadyState.h>

#include <iostream>
#include <yaml-cpp/yaml.h>

// Definition outstide of class because of debug_ being a static variable
bool Simulation::debug_ = false;

Simulation::Simulation(const YAML::Node& root_node) :
    m_root_node(root_node),
    timeIntegrator_(NULL),
    steadyState_(NULL),
    realms_(NULL),
    linearSolvers_(NULL),
    simType_("")
{
    // nothing to do
}

Simulation::~Simulation() {
    delete realms_;
    delete timeIntegrator_;
    delete steadyState_;
    delete linearSolvers_;
}

void Simulation::load(const YAML::Node & node) {
    high_level_banner();
    
    // load the general simulation configs
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "Simulation Settings Review:  " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "=========================" << std::endl;
    
    const YAML::Node sim_node = node["simulation"];
    if (sim_node) {
        std::string simType = "steady";
        get_if_present(sim_node, "type", simType, simType);
        if ( simType == "steady" ) {
            simType_ = simType;
        } else if (simType == "transient") {
            simType_ = simType;
        } else {
            throw std::runtime_error("parser error Simulation::load, specified simulation type not defined");
        }
    } else {
        throw std::runtime_error("parser error Simulation::load, no simulation settings specified");
    }
    
    HOFlowEnv::self().hoflowOutputP0() << "Simulation Type: " << simType_ << std::endl;
    
    // load the linear solver configs
    linearSolvers_ = new LinearSolvers(*this);
    linearSolvers_->load(node);

    // create the realms
    realms_ = new Realms(*this);
    realms_->load(node);
    
    // create the time integrator if transient
    if (simType_ == "transient") {
        HOFlowEnv::self().hoflowOutputP0() << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "Time Integrator Review:  " << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "=========================" << std::endl;
        timeIntegrator_ = new TimeIntegrator(this);
        timeIntegrator_->load(node);
    } else {
        steadyState_ = new SteadyState(this);
    }
}

void Simulation::breadboard() {
    realms_->breadboard();
    if (simType_ == "transient") {
        timeIntegrator_->breadboard();
    } else {
        steadyState_->breadboard();
    }
}

void Simulation::initialize() {
    realms_->initialize();
    if (simType_ == "transient")
        timeIntegrator_->initialize();
}

void Simulation::run() {
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "*******************************************************" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "Simulation Shall Commence: number of processors = " << HOFlowEnv::self().parallel_size() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "*******************************************************" << std::endl;
    if (simType_ == "transient") {
        timeIntegrator_->integrate_realm();
    } else {
        steadyState_->run_realm();
    }
}

void Simulation::high_level_banner() {
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "=================================================================" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "                  HOFlow - Higher Order Flow                     " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "                   CFD solver based on CVFEM                     " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "        Now with heat equation, powered by Joseph Fourier        " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "=================================================================" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "   TPLs: Boost, HDF5, netCDF, STK, Trilinos, YAML_cpp and zlib   " << std::endl;

    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "      This project uses some suff from Sandia Corporation        " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "                            Thx...                               " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "-----------------------------------------------------------------" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
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

// static
stk::diag::TimerSet & Simulation::rootTimerSet() {
    static stk::diag::TimerSet s_timerSet(sierra::Diag::TIMER_ALL);
    return s_timerSet;
}

// static
stk::diag::Timer& Simulation::rootTimer() {
    static stk::diag::Timer s_timer = stk::diag::createRootTimer("HOFlow", rootTimerSet());
    return s_timer;
}

// static
stk::diag::Timer& Simulation::outputTimer() {
    static stk::diag::Timer s_timer("Output", rootTimer());
    return s_timer;
}