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

//! Stores one or multiple initial conditions (objects of the class InitialCondition)
class InitialConditions {
public:
    InitialConditions(Realm & realm);
    ~InitialConditions();
    InitialConditions * load(YAML::Node node);
    
    //! Ease of access function, an object of InitialConditions can treated like an array and checked for size
    size_t size() { return initialConditionVector_.size(); }
    
    //! Ease of access function, an object of InitialConditions can treated like an array and its elements accessed
    InitialCondition *operator[](int i) { return initialConditionVector_[i];}
    
    Realm & realm_;
    InitialConditionVector initialConditionVector_;
    Simulation * root();
    Realm * parent();
};

#endif /* INITIALCONDITIONS_H */

