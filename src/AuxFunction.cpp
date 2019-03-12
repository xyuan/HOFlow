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
    // nothing to do
}

void AuxFunction::evaluate(
    const double * coords,
    const double time,
    const unsigned spatialDimension,
    const unsigned numPoints,
    double * fieldPtr,
    const unsigned fieldSize) const
{
    if(beginPos_ == 0 && endPos_ == fieldSize)
        do_evaluate(coords, time, spatialDimension, numPoints, fieldPtr, fieldSize);
    else
        do_evaluate(coords, time, spatialDimension, numPoints, fieldPtr, fieldSize, beginPos_, endPos_);
}

void AuxFunction::do_evaluate(
    const double * coords,
    const double time,
    const unsigned spatialDimension,
    const unsigned numPoints,
    double * fieldPtr,
    const unsigned fieldSize) const
{
    do_evaluate(coords, time, spatialDimension, numPoints, fieldPtr, fieldSize, 0, fieldSize);
}