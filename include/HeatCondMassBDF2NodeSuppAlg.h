/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HEATCONDMASSBDF2NODESUPPALG_H
#define HEATCONDMASSBDF2NODESUPPALG_H

#include <SupplementalAlgorithm.h>
#include <FieldTypeDef.h>

#include <stk_mesh/base/Entity.hpp>

class Realm;

/** Specific implementation of the algorithm
 * for the heat conduction bdf2 scheme*/
class HeatCondMassBDF2NodeSuppAlg : public SupplementalAlgorithm {
public:
    /** Initializes some variables and save fields to variables*/
    HeatCondMassBDF2NodeSuppAlg(Realm &realm);

    virtual ~HeatCondMassBDF2NodeSuppAlg() {}
    
    /** Save some data to variables*/
    virtual void setup();

    virtual void node_execute(
      double *lhs,
      double *rhs,
      stk::mesh::Entity node);

    ScalarFieldType *temperatureNm1_;
    ScalarFieldType *temperatureN_;
    ScalarFieldType *temperatureNp1_;
    ScalarFieldType *density_;
    ScalarFieldType *specificHeat_;
    ScalarFieldType *dualNodalVolume_;
    double dt_;
    double gamma1_, gamma2_, gamma3_;
};

#endif /* HEATCONDMASSBDF2NODESUPPALG_H */

