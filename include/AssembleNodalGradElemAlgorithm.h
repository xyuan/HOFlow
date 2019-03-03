/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLENODALGRADELEMALGORITHM_H
#define ASSEMBLENODALGRADELEMALGORITHM_H

#include<Algorithm.h>
#include<FieldTypeDef.h>

// stk
#include <stk_mesh/base/Part.hpp>

class Realm;

class AssembleNodalGradElemAlgorithm : public Algorithm
{
public:

  AssembleNodalGradElemAlgorithm(
    Realm &realm,
    stk::mesh::Part *part,
    ScalarFieldType *scalarQ,
    VectorFieldType *dqdx,
    const bool useShifted = false);
  virtual ~AssembleNodalGradElemAlgorithm() {}

  virtual void execute();

  ScalarFieldType *scalarQ_;
  VectorFieldType *dqdx_;
  ScalarFieldType *dualNodalVolume_;
  VectorFieldType *coordinates_;

  const bool useShifted_;
};

#endif /* ASSEMBLENODALGRADELEMALGORITHM_H */

