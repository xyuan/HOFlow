/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HEATCONDMASSBACKWARDEULERNODESUPPALG_H
#define HEATCONDMASSBACKWARDEULERNODESUPPALG_H

#include <SupplementalAlgorithm.h>
#include <FieldTypeDef.h>

#include <stk_mesh/base/Entity.hpp>

class Realm;

/** Specific implementation of the algorithm
 * for the heat conduction backwards euler scheme*/
class HeatCondMassBackwardEulerNodeSuppAlg : public SupplementalAlgorithm {
public:
    /** Initializes some variables and saves fields so variables*/
    HeatCondMassBackwardEulerNodeSuppAlg(Realm &realm);

    virtual ~HeatCondMassBackwardEulerNodeSuppAlg() {}
    
    /** Saves timestep to variable*/
    virtual void setup();

    virtual void node_execute(
      double *lhs,
      double *rhs,
      stk::mesh::Entity node);

    ScalarFieldType *temperatureN_;
    ScalarFieldType *temperatureNp1_;
    ScalarFieldType *density_;
    ScalarFieldType *specificHeat_;
    ScalarFieldType *dualNodalVolume_;
    double dt_;

};

#endif /* HEATCONDMASSBACKWARDEULERNODESUPPALG_H */

