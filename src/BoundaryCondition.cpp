/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "BoundaryCondition.h"

BoundaryCondition::BoundaryCondition(BoundaryConditions& bcs) : 
    boundaryConditions_(bcs)
{
}

BoundaryCondition::~BoundaryCondition() {
}

