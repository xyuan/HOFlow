/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef SUPPLEMENTALALGORITHM_H
#define SUPPLEMENTALALGORITHM_H

#include <master_element/MasterElement.h>
#include <KokkosInterface.h>
#include <vector>

#include <stk_mesh/base/Types.hpp>
#include <stk_mesh/base/Entity.hpp>

class Realm;

class SupplementalAlgorithm
{
public:
  
  SupplementalAlgorithm(
    Realm &realm);
  
  virtual ~SupplementalAlgorithm() {}

  virtual void setup() {}

  virtual void elem_execute(
    double *lhs,
    double *rhs,
    stk::mesh::Entity element,
    MasterElement *meSCS,
    MasterElement *meSCV) {}
  
  virtual void node_execute(
    double *lhs,
    double *rhs,
    stk::mesh::Entity node) {}
  
  virtual void elem_resize(
    MasterElement *meSCS,
    MasterElement *meSCV) {}

  Realm &realm_;  
};

#endif /* SUPPLEMENTALALGORITHM_H */

