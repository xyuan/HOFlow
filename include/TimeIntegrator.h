/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef TIMEINTEGRATOR_H
#define TIMEINTEGRATOR_H

#include <Enums.h>
// yaml for parsing..
#include <yaml-cpp/yaml.h>

#include <vector>
#include <string>

class Realm;
class Simulation;

/** Class responsible for the time dimension of the simulation.
 * 
 * Repeats the simulation for multiple timesteps. Manages
 * the timesteps and duration of the simulation.
 */
class TimeIntegrator {
public:
    TimeIntegrator() {}
    
    /** Initialize some variables*/
    TimeIntegrator(Simulation* sim);
    ~TimeIntegrator();
    
    /** Reads the time integrator settings from the input file.
     * If nothing is specified, default values are used.
     */
    void load(const YAML::Node & node) ;

    /** Adds this time integrator object to every realm
     * and stores the realm in the realmVec_ variable.
     */
    void breadboard();

    void initialize();
    Simulation *root();
    Simulation *parent();

    /** Calls all methods that are necessary for a timestep and increases
     * the timestep count
     */
    void integrate_realm();
    
    /** Calculates the mean norm of every realm and gives back
     * the mean value of all of them.
     */
    void provide_mean_norm();
    
    /** Decides if the simulation has to proceed or not*/
    bool simulation_proceeds();
    Simulation* sim_{nullptr};

    double totalSimTime_;
    double currentTime_;
    double timeStepFromFile_;
    double timeStepN_;
    double timeStepNm1_;
    double gamma1_;
    double gamma2_;
    double gamma3_;
    int timeStepCount_;
    int maxTimeStepCount_;
    bool secondOrderTimeAccurate_;
    bool adaptiveTimeStep_;
    bool terminateBasedOnTime_;
    int nonlinearIterations_;

    std::string name_;

    std::vector<std::string> realmNamesVec_;

    std::vector<Realm*> realmVec_;

    /** Returns the timestep dependend on the state
     * 
     * I assume this only makes sense, if the timestep
     * varies -> not implemented
     */
    double get_time_step(
    const HOFlowState & theState = HOFLOW_STATE_N) const;
    double get_current_time() const;
    double get_gamma1() const;
    double get_gamma2() const;
    double get_gamma3() const;
    int get_time_step_count() const;
    double get_time_step_from_file();
    bool get_is_fixed_time_step();
    bool get_is_terminate_based_on_time();
    double get_total_sim_time();
    int get_max_time_step_count();
    void compute_gamma();
};

#endif /* TIMEINTEGRATOR_H */

