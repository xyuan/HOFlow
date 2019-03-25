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

class HeatCondMassBDF2NodeSuppAlg : public SupplementalAlgorithm
{
public:

  HeatCondMassBDF2NodeSuppAlg(
    Realm &realm);

  virtual ~HeatCondMassBDF2NodeSuppAlg() {}

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

