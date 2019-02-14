/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AuxFunction.h"

AuxFunction::AuxFunction(const unsigned beginPos, const unsigned endPos) :
    beginPos_(beginPos),
    endPos_(endPos)
{}

AuxFunction::~AuxFunction() {
}

