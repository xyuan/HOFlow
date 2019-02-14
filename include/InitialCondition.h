/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef INITIALCONDITION_H
#define INITIALCONDITION_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include <Enums.h>

class InitialConditions;
class YAML::Node;

//! Stores a initial condition specified in the input file
class InitialCondition {
public:
    InitialCondition(InitialConditions & ics) : initialConditions_(ics), theIcType_(UserDataType_END) {}
    ~InitialCondition();
    InitialCondition * load(const YAML::Node & node);
    InitialConditions * parent();
    
    InitialConditions & initialConditions_;
    std::string icName_;
    std::vector<std::string> targetNames_;
    UserDataType theIcType_;
};

#endif /* INITIALCONDITION_H */

