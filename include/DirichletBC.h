/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef DIRICHLETBC_H
#define DIRICHLETBC_H

#include <SolverAlgorithm.h>

namespace stk{
namespace mesh{
class FieldBase;
}
}

class EquationSystem;
class Realm;

class DirichletBC : public SolverAlgorithm
{
public:

  DirichletBC(
    Realm & realm,
    EquationSystem * eqSystem,
    stk::mesh::Part * part,
    stk::mesh::FieldBase * field,
    stk::mesh::FieldBase * bcValues,
    const unsigned beginPos,
    const unsigned endPos);
  
  virtual ~DirichletBC() {}

  virtual void execute();
  virtual void initialize_connectivity();

private:
  stk::mesh::FieldBase * field_;
  stk::mesh::FieldBase * bcValues_;
  const unsigned beginPos_;
  const unsigned endPos_;

private:
  // make this non-copyable
  DirichletBC(const DirichletBC & other);
  DirichletBC & operator=(const DirichletBC & other);
};

#endif /* DIRICHLETBC_H */

