/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLEPNGBOUNDARYSOLVERALGORITHM_H
#define ASSEMBLEPNGBOUNDARYSOLVERALGORITHM_H

#include<SolverAlgorithm.h>
#include<FieldTypeDef.h>

namespace stk {
namespace mesh {
class Part;
}
}

class Realm;

class AssemblePNGBoundarySolverAlgorithm : public SolverAlgorithm
{
public:

  AssemblePNGBoundarySolverAlgorithm(
    Realm &realm,
    stk::mesh::Part *part,
    EquationSystem *eqSystem,
    std::string independentDofName);
  virtual ~AssemblePNGBoundarySolverAlgorithm() {}
  virtual void initialize_connectivity();
  virtual void execute();

  ScalarFieldType *scalarQ_;
  GenericFieldType *exposedAreaVec_;
};

#endif /* ASSEMBLEPNGBOUNDARYSOLVERALGORITHM_H */

