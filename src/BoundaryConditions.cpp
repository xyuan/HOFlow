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

void BoundaryConditions::load(YAML::Node node) {
}