/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef CONSTANTAUXFUNCTION_H
#define CONSTANTAUXFUNCTION_H

#include <AuxFunction.h>
#include <vector>


class ConstantAuxFunction : public AuxFunction {
public:
    ConstantAuxFunction(const unsigned beginPos, const unsigned endPos, const std::vector<double> & values);
    ~ConstantAuxFunction();
    
private:
    const std::vector<double> values_;
};

#endif /* CONSTANTAUXFUNCTION_H */

