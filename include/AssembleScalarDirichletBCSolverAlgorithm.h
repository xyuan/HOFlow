/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLESCALARDIRICHLETBCSOLVERALGORITHM_H
#define ASSEMBLESCALARDIRICHLETBCSOLVERALGORITHM_H

#include<SolverAlgorithm.h>
#include<FieldTypeDef.h>

namespace stk {
namespace mesh {
class Part;
}
}

class LinearSystem;
class Realm;

class AssembleScalarDirichletBCSolverAlgorithm : public SolverAlgorithm {
public:
    AssembleScalarDirichletBCSolverAlgorithm(Realm &realm,
                                        stk::mesh::Part *part,
                                        EquationSystem *eqSystem,
                                        ScalarFieldType *scalarQ,
                                        ScalarFieldType *bcScalarQ,
                                        ScalarFieldType *diffFluxCoeff);
    virtual ~AssembleScalarDirichletBCSolverAlgorithm() {}
    virtual void initialize_connectivity();
    virtual void execute();

private:
    ScalarFieldType *scalarQ_;
    ScalarFieldType *bcScalarQ_;
    GenericFieldType *exposedAreaVec_;
    ScalarFieldType *diffFluxCoeff_;
    VectorFieldType *coordinates_;
};

#endif /* ASSEMBLESCALARDIRICHLETBCSOLVERALGORITHM_H */

