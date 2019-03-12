/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLENODALGRADBOUNDARYALGORITHM_H
#define ASSEMBLENODALGRADBOUNDARYALGORITHM_H

#include<Algorithm.h>
#include<FieldTypeDef.h>

class Realm;

class AssembleNodalGradBoundaryAlgorithm : public Algorithm
{
public:
  AssembleNodalGradBoundaryAlgorithm(
    Realm &realm,
    stk::mesh::Part *part,
    ScalarFieldType *scalarQ,
    VectorFieldType *dqdx,
    const bool useShifted);
  virtual ~AssembleNodalGradBoundaryAlgorithm() {}

  virtual void execute();

  ScalarFieldType *scalarQ_;
  VectorFieldType *dqdx_;
  const bool useShifted_;

};

#endif /* ASSEMBLENODALGRADBOUNDARYALGORITHM_H */

