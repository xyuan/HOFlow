/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "SupplementalAlgorithm.h"

// stk_mesh/base/fem
#include <stk_mesh/base/Entity.hpp>

SupplementalAlgorithm::SupplementalAlgorithm(Realm &realm) : 
    realm_(realm)
{
    // nothing to do
}