/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef COMPUTEGEOMETRYBOUNDARYALGORITHM_H
#define COMPUTEGEOMETRYBOUNDARYALGORITHM_H

#include<Algorithm.h>

class Realm;

class ComputeGeometryBoundaryAlgorithm : public Algorithm {
public:
    ComputeGeometryBoundaryAlgorithm(Realm &realm, stk::mesh::Part *part);
    virtual ~ComputeGeometryBoundaryAlgorithm() {}
    virtual void execute();
};

#endif /* COMPUTEGEOMETRYBOUNDARYALGORITHM_H */

