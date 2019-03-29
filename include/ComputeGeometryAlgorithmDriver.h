/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef COMPUTEGEOMETRYALGORITHMDRIVER_H
#define COMPUTEGEOMETRYALGORITHMDRIVER_H

#include<AlgorithmDriver.h>

class Realm;

class ComputeGeometryAlgorithmDriver : public AlgorithmDriver {
public:
  ComputeGeometryAlgorithmDriver(Realm &realm);
  virtual ~ComputeGeometryAlgorithmDriver() {}

  void pre_work();
  void post_work();
  void check_jacobians();
};

#endif /* COMPUTEGEOMETRYALGORITHMDRIVER_H */

