/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef COMPUTEGEOMETRYINTERIORALGORITHM_H
#define COMPUTEGEOMETRYINTERIORALGORITHM_H

#include<Algorithm.h>

// stk
#include <stk_mesh/base/Part.hpp>

class Realm;

class ComputeGeometryInteriorAlgorithm : public Algorithm
{
public:
  ComputeGeometryInteriorAlgorithm(Realm &realm, stk::mesh::Part *part);
  ~ComputeGeometryInteriorAlgorithm();
  void execute();
};

#endif /* COMPUTEGEOMETRYINTERIORALGORITHM_H */

