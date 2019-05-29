/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLESCALARFLUXBCSOLVERALGORITHM_H
#define ASSEMBLESCALARFLUXBCSOLVERALGORITHM_H

#include<SolverAlgorithm.h>
#include<FieldTypeDef.h>

namespace stk {
namespace mesh {
class Part;
}
}

class LinearSystem;
class Realm;

class AssembleScalarFluxBCSolverAlgorithm : public SolverAlgorithm {
public:
    AssembleScalarFluxBCSolverAlgorithm(Realm &realm,
                                        stk::mesh::Part *part,
                                        EquationSystem *eqSystem);
    virtual ~AssembleScalarFluxBCSolverAlgorithm() {}
    virtual void initialize_connectivity();
    virtual void execute();

private:
    ScalarFieldType *bcScalarQ_;
    GenericFieldType *exposedAreaVec_;
};

#endif /* ASSEMBLESCALARFLUXBCSOLVERALGORITHM_H */

