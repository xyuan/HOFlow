/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLESCALARELEMDIFFSOLVERALGORITHMNALU_H
#define ASSEMBLESCALARELEMDIFFSOLVERALGORITHMNALU_H

#include<SolverAlgorithm.h>
#include<FieldTypeDef.h>

class stk::mesh::Part;
class Realm;

/** Solver Algorithm to compute the coefficients for the 
 * diffustion equation. Nalu version
 */
class AssembleScalarElemDiffSolverAlgorithmNalu : public SolverAlgorithm {
public:
    AssembleScalarElemDiffSolverAlgorithmNalu(
        Realm &realm,
        stk::mesh::Part *part,
        EquationSystem *eqSystem,
        ScalarFieldType *scalarQ,
        VectorFieldType *dqdx,
        ScalarFieldType *diffFluxCoeff);
    virtual ~AssembleScalarElemDiffSolverAlgorithmNalu() {}
    virtual void initialize_connectivity();
    virtual void execute();

  private:

    ScalarFieldType * scalarQ_;
    ScalarFieldType * diffFluxCoeff_;
    VectorFieldType * coordinates_;

    const bool shiftedGradOp_;
};

#endif /* ASSEMBLESCALARELEMDIFFSOLVERALGORITHMNALU_H */

