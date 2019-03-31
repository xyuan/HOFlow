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

class Simulation;
class InitialConditions;
class YAML::Node;

/** Stores a initial condition specified in the input file*/
class InitialCondition {
public:
    /** Initializes some variables*/
    InitialCondition(InitialConditions & ics);
    
    virtual ~InitialCondition();
    
    /** Reads the parameters of the initial condition in the input
     * file and stores them in a object.
     */
    InitialCondition * load(const YAML::Node & node);
    Simulation * root();
    InitialConditions * parent();
    
    InitialConditions & initialConditions_;
    std::string icName_;
    std::vector<std::string> targetNames_;
    UserDataType theIcType_;
};

#endif /* INITIALCONDITION_H */

