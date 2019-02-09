/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef BOUNDARYCONDITIONS_H
#define BOUNDARYCONDITIONS_H

#include <vector>

typedef std::vector<BoundaryCondition *> BoundaryConditionVector;

class BoundaryConditions {
public:
    BoundaryConditions(Realm & realm);
    ~BoundaryConditions();
    void load(YAML::Node node);
    size_t size();
    
    Realm & realm_;
    BoundaryConditionVector boundaryConditionVector_;
};

#endif /* BOUNDARYCONDITIONS_H */

