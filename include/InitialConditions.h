/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef INITIALCONDITIONS_H
#define INITIALCONDITIONS_H

#include <vector>

typedef std::vector<InitialCondition *> InitialConditionVector;

//! Stores one or multiple initial conditions (objects of the class InitialCondition)
class InitialConditions {
public:
    InitialConditions(Realm & realm);
    ~InitialConditions();
    void load(YAML::Node node);
    size_t size();
    
    Realm & realm_;
    InitialConditionVector initialConditionVector_;
    Realm * parent();
};

#endif /* INITIALCONDITIONS_H */

