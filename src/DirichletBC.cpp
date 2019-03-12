/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "DirichletBC.h"
#include <EquationSystem.h>
#include <FieldTypeDef.h>
#include <LinearSystem.h>
#include <Realm.h>

DirichletBC::DirichletBC(
  Realm & realm,
  EquationSystem * eqSystem,
  stk::mesh::Part * part,
  stk::mesh::FieldBase * field,
  stk::mesh::FieldBase * bcValues,
  const unsigned beginPos,
  const unsigned endPos)
  : SolverAlgorithm(realm, part, eqSystem),
    field_(field),
    bcValues_(bcValues),
    beginPos_(beginPos),
    endPos_(endPos)
{}

void
DirichletBC::initialize_connectivity()
{
  eqSystem_->linsys_->buildDirichletNodeGraph(partVec_);
}

void
DirichletBC::execute()
{

  eqSystem_->linsys_->applyDirichletBCs(
    field_,
    bcValues_,
    partVec_,
    beginPos_,
    endPos_);
}