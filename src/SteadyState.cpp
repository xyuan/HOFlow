/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "SteadyState.h"

#include <Enums.h>
#include <Realm.h>
#include <Realms.h>
#include <Simulation.h>
#include <OutputInfo.h>
#include <SolutionOptions.h>
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>

SteadyState::SteadyState(Simulation * sim) :
    sim_(sim)
{
    // nothing to do 
}

SteadyState::~SteadyState() {
}

void SteadyState::breadboard() {   
    for (size_t irealm = 0; irealm < sim_->realms_->realmVector_.size(); ++irealm) {
        Realm * realm = sim_->realms_->realmVector_[irealm];
        realmNamesVec_.push_back(realm->name_);
        realmVec_.push_back(realm);
    }
}

void SteadyState::run_realm() {
    std::vector<Realm *>::iterator ii;

    //=====================================
    // start-up procedure
    //=====================================

    // initial conditions
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->populate_initial_condition();
    }

    // populate boundary data
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->populate_boundary_data();
    }  

    // copy boundary data to solution state
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->boundary_data_to_state_data();
    }

    // derived conditions from dofs (interior and boundary)
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->populate_derived_quantities();
    }

    // compute properties based on initial/restart conditions
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->evaluate_properties();
    }

    // perform any initial work
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->initial_work();
    }

    // provide output/restart for initial condition
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->output_converged_results();
    }

    //=====================================
    // Solving
    //=====================================

    // state management
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->swap_states();
        (*ii)->predict_state();
    }

    // pre-step work; mesh motion, search, etc
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->pre_timestep_work();
    }

    // populate boundary data
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->populate_boundary_data();
    }

    // output banner
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->output_banner();
    }

    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->run_steady_state();
    }

    // process any post converged work
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->post_converged_work();
    }

    // provide output/restart after nonlinear iteration
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->output_converged_results();
    }

    // inform the user that the simulation is complete
    HOFlowEnv::self().hoflowOutputP0() << "*******************************************************" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "Simulation is Complete" << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "*******************************************************" << std::endl;

    // dump time
    for ( ii = realmVec_.begin(); ii!=realmVec_.end(); ++ii) {
        (*ii)->dump_simulation_time();
    }
}

