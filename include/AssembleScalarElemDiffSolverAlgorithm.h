/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLESCALARELEMDIFFSOLVERALGORITHM_H
#define ASSEMBLESCALARELEMDIFFSOLVERALGORITHM_H

#include<SolverAlgorithm.h>
#include<FieldTypeDef.h>

class stk::mesh::Part;
class Realm;

class AssembleScalarElemDiffSolverAlgorithm : public SolverAlgorithm {
public:
    AssembleScalarElemDiffSolverAlgorithm(
        Realm &realm,
        stk::mesh::Part *part,
        EquationSystem *eqSystem,
        ScalarFieldType *scalarQ,
        VectorFieldType *dqdx,
        ScalarFieldType *diffFluxCoeff);
    virtual ~AssembleScalarElemDiffSolverAlgorithm() {}
    virtual void initialize_connectivity();
    virtual void execute();

  private:

    ScalarFieldType *scalarQ_;
    ScalarFieldType *diffFluxCoeff_;
    VectorFieldType *coordinates_;

    const bool shiftedGradOp_;
};

#endif /* ASSEMBLESCALARELEMDIFFSOLVERALGORITHM_H */

