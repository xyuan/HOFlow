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
                                        ScalarFieldType *bcScalarQ);
    virtual ~AssembleScalarDirichletBCSolverAlgorithm() {}
    virtual void initialize_connectivity();
    virtual void execute();

private:
    ScalarFieldType *bcScalarQ_;
    GenericFieldType *exposedAreaVec_;
};

#endif /* ASSEMBLESCALARDIRICHLETBCSOLVERALGORITHM_H */

