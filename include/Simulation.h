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
class LinearSolvers;
class TimeIntegrator;
class Realms;

/** Container class that holds all data regarding the simulation.
 *
 * Contained data is e.g. realms, solvers etc. 
 * Within this class also the sequence of execution of the simulation
 * is defined.
 * The load method creates needed objects based on the data specified
 * in the input file.
 * The initialize method initializes the created objects.
 * Finally at the run method, solver algorithms are executed.
 */
class Simulation {
public:
    /** Initialize some variables*/
    Simulation(const YAML::Node & root_node);
    
    /** Delete objects*/
    ~Simulation();
    
    /** Create objects based on informations in the input file*/
    void load(const YAML::Node & node);
    
    /** Stuff that has to be done before initialization
     * 
     * @note Not sure if this is really needed, may be able to put
     * these things in the initailize method
     */
    void breadboard();
    
    /** Initialize objects created in the load method*/
    void initialize();
    
    /** Run the simulation -> execute solver algorithms*/
    void run();
    
    /** Prints a banner to the logfile as first output*/
    void high_level_banner();
    
    Simulation * root();
    Simulation * parent();
    bool debug();
    bool debug() const;
    
    /** Setting for the timer implemented in STK, enables all timers.*/
    static stk::diag::TimerSet & rootTimerSet();
    
    /** Creates a root timer in STK with the settings from the rootTimerSet method.*/
    static stk::diag::Timer & rootTimer();
    
    /** Outputs the time from the STK timer.*/
    static stk::diag::Timer & outputTimer();
    
    /** Contains the whole input file.*/
    const YAML::Node & m_root_node;
    
    TimeIntegrator * timeIntegrator_;
    Realms * realms_;
    LinearSolvers * linearSolvers_;
    static bool debug_;
    std::string simType_;
};

#endif /* SIMULATION_H */

