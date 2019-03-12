/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef CONSTANTAUXFUNCTION_H
#define CONSTANTAUXFUNCTION_H

#include <AuxFunction.h>
#include <vector>

#include<vector>

class ConstantAuxFunction : public AuxFunction {
public:
    ConstantAuxFunction(
        const unsigned beginPos,
        const unsigned endPos,
        const std::vector<double> & values);

    virtual ~ConstantAuxFunction() {}

    virtual void do_evaluate(
        const double * coords,
        const double time,
        const unsigned spatialDimension,
        const unsigned numPoints,
        double * fieldPtr,
        const unsigned fieldSize,
        const unsigned beginPos,
        const unsigned endPos) const;

private:
    const std::vector<double> values_;
};

#endif /* CONSTANTAUXFUNCTION_H */

