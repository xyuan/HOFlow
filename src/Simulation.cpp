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

#include <iostream>
#include <yaml-cpp/yaml.h>

// Definition outstide of class because of debug_ being a static variable
bool Simulation::debug_ = false;

//! Constructor
Simulation::Simulation(const YAML::Node& root_node) :
    m_root_node(root_node),
    timeIntegrator_(NULL),
    realms_(NULL),
    linearSolvers_(NULL)
{}

//! Destructor
Simulation::~Simulation() {
    delete realms_;
    delete timeIntegrator_;
    delete linearSolvers_;
}

void Simulation::breadboard() {
    realms_->breadboard();
    timeIntegrator_->breadboard();
}

//! Loads the information necessary to do the simulation
void Simulation::load(const YAML::Node & node) {
    high_level_banner();
    
    // load the linear solver configs
    linearSolvers_ = new LinearSolvers(*this);
    linearSolvers_->load(node);

    // create the realms
    realms_ = new Realms(*this);
    realms_->load(node);
    
    // create the time integrator
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "Time Integrator Review:  " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "=========================" << std::endl;
    timeIntegrator_ = new TimeIntegrator(this);
    timeIntegrator_->load(node);
}

//! Initializes all computational domains
void Simulation::initialize() {
    realms_->initialize();
    timeIntegrator_->initialize();
}

//! Run the simulation
void Simulation::run() {
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "*******************************************************" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "Simulation Shall Commence: number of processors = " << HOFlowEnv::self().parallel_size() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "*******************************************************" << std::endl;
    timeIntegrator_->integrate_realm();
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

stk::diag::TimerSet &
Simulation::rootTimerSet()
{
  static stk::diag::TimerSet s_timerSet(sierra::Diag::TIMER_ALL);

  return s_timerSet;
}

//static
stk::diag::Timer& Simulation::rootTimer()
{
  static stk::diag::Timer s_timer = stk::diag::createRootTimer("Nalu", rootTimerSet());

  return s_timer;
}

//static
stk::diag::Timer& Simulation::outputTimer()
{
  static stk::diag::Timer s_timer("Output", rootTimer());
  return s_timer;
}

void Simulation::high_level_banner() {
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "=================================================================" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "                  HOFlow - Higher Order Flow                     " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "                   CFD solver based on CVFEM                     " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "        Now with heat equation, powered by Joseph Fourier        " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "=================================================================" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0()
    HOFlowEnv::self().hoflowOutputP0() << "   TPLs: Boost, HDF5, netCDF, STK, Trilinos, YAML_cpp and zlib   " << std::endl;

    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "      This project uses some suff from Sandia Corporation        " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "                            Thx...                               " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "-----------------------------------------------------------------" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
}