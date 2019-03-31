/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef INITIALCONDITIONS_H
#define INITIALCONDITIONS_H

#include <vector>
#include <yaml-cpp/yaml.h>
#include <Enums.h>

class Simulation;
class InitialCondition;
class Realm;

typedef std::vector<InitialCondition *> InitialConditionVector;

/** Container class that creates and stores one or multiple
 * objects of type InitialCondition
 */
class InitialConditions {
public:
    /** Initializes some variables*/
    InitialConditions(Realm & realm);
    
    ~InitialConditions();
    
    /** Creates a object for every initial condition specified in the
     * input file.
     */
    InitialConditions * load(YAML::Node node);
    
    /** Ease of access function, an object of InitialConditions can treated 
     * like an array and checked for size
     */
    size_t size() { return initialConditionVector_.size(); }
    
    /** Ease of access function, an object of InitialConditions can treated 
     * like an array and its elements accessed
     */
    InitialCondition *operator[](int i) { return initialConditionVector_[i];}
    
    Realm & realm_;
    InitialConditionVector initialConditionVector_;
    Simulation * root();
    Realm * parent();
};

#endif /* INITIALCONDITIONS_H */

