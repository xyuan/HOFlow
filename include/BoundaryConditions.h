/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef BOUNDARYCONDITIONS_H
#define BOUNDARYCONDITIONS_H

#include <vector>
#include <Enums.h>
#include <yaml-cpp/yaml.h>

class Realm;
class Simulation;
class BoundaryCondition;
class YAML::Node;

typedef std::vector<BoundaryCondition *> BoundaryConditionVector;

class BoundaryConditions {
public:
    BoundaryConditions(Realm & realm);
    ~BoundaryConditions();
    BoundaryConditions * load(YAML::Node node);
    Simulation * root();
    Realm * parent();
    
    //! Ease of access function, an object of BoundaryConditions can treated like an array and its elements accessed
    size_t size() { return boundaryConditionVector_.size(); }
    
    //! Ease of access function, an object of BoundaryConditions can treated like an array and its elements accessed
    BoundaryCondition *operator[](int i) { return boundaryConditionVector_[i]; }
    
    Realm & realm_;
    BoundaryConditionVector boundaryConditionVector_;
};

#endif /* BOUNDARYCONDITIONS_H */

