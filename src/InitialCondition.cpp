/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "InitialCondition.h"
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>

InitialCondition::InitialCondition(InitialConditions & ics) : 
    initialConditions_(ics), 
    theIcType_(UserDataType_END) 
{
    // nothing to do
}

InitialCondition::~InitialCondition() {
    // nothing to do
}

InitialCondition * InitialCondition::load(const YAML::Node & node) {
    if (node["constant"]) {
        HOFlowEnv::self().hoflowOutputP0() << "Initial Is Type constant " << std::endl;
        ConstantInitialConditionData & constIC = * new ConstantInitialConditionData(*parent());
        node >> constIC;
        return & constIC;
    } else {
        throw std::runtime_error("parser error InitialConditions::load; unsupported IC type");
    }
    return 0;
}

Simulation * InitialCondition::root() { 
    return parent()->root(); 
}

InitialConditions * InitialCondition::parent() { 
    return & initialConditions_; 
}