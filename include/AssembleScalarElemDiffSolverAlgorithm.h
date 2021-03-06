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

/** Solver algorithm to compute the coefficients of the
 * diffusion equation.
 */
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
    ScalarFieldType * scalarQ_;
    ScalarFieldType * diffFluxCoeff_;
    VectorFieldType * coordinates_;
    const bool shiftedGradOp_;
    
    // Execution related stuff
    int getLeftNode(const int * lrscv, int * ip);
    int getRightNode(const int * lrscv, int * ip);
    double getSFValue(const double * shapeFunction, const int * offset, int * node);
    double getSFDeriv(const double * shapeFunctionDeriv, const int * offset, int * dim);
    double getFaceDet(const double * areav, const int * nDim, int * ip, int * dim);
};

#endif /* ASSEMBLESCALARELEMDIFFSOLVERALGORITHM_H */

