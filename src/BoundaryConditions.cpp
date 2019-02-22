/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "BoundaryConditions.h"
#include <BoundaryCondition.h>
#include <Realm.h>

BoundaryConditions::BoundaryConditions(Realm & realm) :
    realm_(realm)
{
}

BoundaryConditions::~BoundaryConditions() {
    for ( size_t iboundary_condition = 0; iboundary_condition < boundaryConditionVector_.size(); ++iboundary_condition ) {
        delete boundaryConditionVector_[iboundary_condition];
    }
}

BoundaryConditions * BoundaryConditions::load(YAML::Node node) {
    BoundaryCondition tmp_boundary_condition(*this);

    if(node["boundary_conditions"]) {
        const YAML::Node boundary_conditions = node["boundary_conditions"];
        for ( size_t iboundary_condition = 0; iboundary_condition < boundary_conditions.size(); ++iboundary_condition ) {
            const YAML::Node boundary_condition_node = boundary_conditions[iboundary_condition];
            BoundaryCondition* bc = tmp_boundary_condition.load(boundary_condition_node);
            boundaryConditionVector_.push_back(bc);
        }
    }
    else {
        throw std::runtime_error("parser error BoundaryConditions::load");
    }

    return this;
}

Simulation * BoundaryConditions::root() { 
    return parent()->root(); 
}

Realm * BoundaryConditions::parent() { 
    return &realm_; 
}