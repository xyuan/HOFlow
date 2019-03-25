/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLENODESOLVERALGORITHM_H
#define ASSEMBLENODESOLVERALGORITHM_H

#include<SolverAlgorithm.h>
#include<FieldTypeDef.h>

namespace stk {
namespace mesh {
class Part;
}
}

class Realm;

class AssembleNodeSolverAlgorithm : public SolverAlgorithm
{
public:

  AssembleNodeSolverAlgorithm(
    Realm &realm,
    stk::mesh::Part *part,
    EquationSystem *eqSystem);
  virtual ~AssembleNodeSolverAlgorithm() {}
  virtual void initialize_connectivity();
  virtual void execute();

  const int sizeOfSystem_;
};

#endif /* ASSEMBLENODESOLVERALGORITHM_H */

