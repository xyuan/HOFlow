/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

class BoundaryCondition {
public:
    BoundaryCondition(BoundaryConditions& bcs);
    ~BoundaryCondition();
    BoundaryCondition * load(const YAML::Node & node);
    
    std::string bcName_;
    std::string targetName_;
    BoundaryConditionType theBcType_;
    BoundaryConditions & boundaryConditions_;
};

#endif /* BOUNDARYCONDITION_H */

