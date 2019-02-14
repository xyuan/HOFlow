/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AuxFunctionAlgorithm.h"

AuxFunctionAlgorithm::AuxFunctionAlgorithm(Realm & realm,
                                           stk::mesh::Part * part,
                                           stk::mesh::FieldBase * field,
                                           AuxFunction * auxFunction,
                                           stk::mesh::EntityRank entityRank) :
    Algorithm(realm, part),
    field_(field),
    auxFunction_(auxFunction),
    entityRank_(entityRank)
{
    // does nothing
}

AuxFunctionAlgorithm::~AuxFunctionAlgorithm() {
//    delete auxFunction_;
}

