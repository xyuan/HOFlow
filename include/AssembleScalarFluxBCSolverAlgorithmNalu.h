/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLESCALARFLUXBCSOLVERALGORITHMNALU_H
#define ASSEMBLESCALARFLUXBCSOLVERALGORITHMNALU_H

#include<SolverAlgorithm.h>
#include<FieldTypeDef.h>

namespace stk {
namespace mesh {
class Part;
}
}

class LinearSystem;
class Realm;

class AssembleScalarFluxBCSolverAlgorithmNalu : public SolverAlgorithm {
public:
    AssembleScalarFluxBCSolverAlgorithmNalu(Realm &realm,
                                        stk::mesh::Part *part,
                                        EquationSystem *eqSystem,
                                        ScalarFieldType *bcScalarQ);
    virtual ~AssembleScalarFluxBCSolverAlgorithmNalu() {}
    virtual void initialize_connectivity();
    virtual void execute();

private:
    ScalarFieldType *bcScalarQ_;
    GenericFieldType *exposedAreaVec_;
};

#endif /* ASSEMBLESCALARFLUXBCSOLVERALGORITHMNALU_H */

