/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef STEADYSTATE_H
#define STEADYSTATE_H

#include <Enums.h>
#include <vector>
#include <string>

class Realm;
class Simulation;

class SteadyState {
public:
    SteadyState(Simulation * sim);
    ~SteadyState();
    
    /** Adds this object to every realm
     * and stores the realm in the realmVec_ variable.
     */
    void breadboard();
    
    /** Calls all methods that are necessary for a steady state run
     */
    void run_realm();
    
    std::vector<std::string> realmNamesVec_;
    Simulation * sim_{nullptr};
    std::vector<Realm*> realmVec_;
};

#endif /* STEADYSTATE_H */

