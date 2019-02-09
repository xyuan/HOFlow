/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "ConstantAuxFunction.h"

//! Constructor
ConstantAuxFunction::ConstantAuxFunction(const unsigned beginPos, const unsigned endPos, const std::vector<double> & values) :
    AuxFunction(beginPos, endPos),
    values_(values)
{
    //ThrowRequire(endPos_ <= values_.size());
}

//! Destructor
ConstantAuxFunction::~ConstantAuxFunction() {
}

