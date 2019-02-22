/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "BoundaryCondition.h"
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>

BoundaryCondition::BoundaryCondition(BoundaryConditions & bcs) : 
    boundaryConditions_(bcs)
{
}

BoundaryCondition::~BoundaryCondition() {
}

BoundaryCondition * BoundaryCondition::load(const YAML::Node & node) {
    if ( node["wall_boundary_condition"] ) {
        WallBoundaryConditionData & wallBC = * new WallBoundaryConditionData(*parent());
        node >> wallBC;
        HOFlowEnv::self().hoflowOutputP0() << "Wall BC name:        " << wallBC.bcName_
                        << " on " << wallBC.targetName_ << std::endl;
        return & wallBC;
    }

    else {
        throw std::runtime_error("parser error BoundaryConditions::load: no such bc type");
    }
    return 0;
}

Simulation * BoundaryCondition::root() { 
    return parent()->root(); 
}

BoundaryConditions * BoundaryCondition::parent() { 
    return & boundaryConditions_; 
}