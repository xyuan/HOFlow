/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef TPETRALINEARSOLVER_H
#define TPETRALINEARSOLVER_H

#include <LinearSolver.h>
#include <LocalGraphArrays.h>
#include <Enums.h>

#include <Kokkos_DefaultNode.hpp>
#include <Tpetra_Vector.hpp>
#include <Tpetra_CrsMatrix.hpp>
#include <Teuchos_GlobalMPISession.hpp>
#include <Teuchos_oblackholestream.hpp>

#include <Ifpack2_Factory.hpp>

class TpetraLinearSolverConfig;

typedef double Scalar;
typedef long GlobalOrdinal;
typedef int LocalOrdinal;
typedef Tpetra::Map<LocalOrdinal, GlobalOrdinal>::node_type Node;
typedef Teuchos::ScalarTraits<Scalar> STS;

/** A specialized class of LinearSolver for the TpetraLinearSolver*/
class TpetraLinearSolver : public LinearSolver {
public:
    /** Initializes some variables*/
    TpetraLinearSolver(std::string solverName,
                        TpetraLinearSolverConfig * config,
                        const Teuchos::RCP<Teuchos::ParameterList> params,
                        const Teuchos::RCP<Teuchos::ParameterList> paramsPrecond,
                        LinearSolvers * linearSolvers);
    virtual ~TpetraLinearSolver();
    
    /** Sets the variables matrix_ and rhs_ which represents A and b*/
    void setSystemObjects(Teuchos::RCP<LinSys::Matrix> matrix, Teuchos::RCP<LinSys::Vector> rhs);
    
    /** Creates a linear system of equations*/
    void setupLinearSolver(Teuchos::RCP<LinSys::Vector> sln,
                            Teuchos::RCP<LinSys::Matrix> matrix,
                            Teuchos::RCP<LinSys::Vector> rhs,
                            Teuchos::RCP<LinSys::MultiVector> coords);

    virtual void destroyLinearSolver() override;

    /** Compute the norm of the non-linear solution vector
     *
     *  @param[in] whichNorm [0, 1, 2] norm to be computed
     *  @param[in] sln The solution vector
     *  @param[out] norm The norm of the solution vector
     */
    int residual_norm(int whichNorm, Teuchos::RCP<LinSys::Vector> sln, double & norm);

    /** Solve the linear system Ax = b
     *
     *  This method is called once every non-linear iteration (outer iterations).
     *  The inner iterations are happening within the solver in the TPL.
     *  @param[out] sln The solution vector
     *  @param[out] iterationCount The number of linear solver iterations to convergence
     *  @param[out] scaledResidual The final residual norm
     *  @param[in]  isFinalOuterIter Is this the final outer iteration
     */
    int solve(Teuchos::RCP<LinSys::Vector> sln, int & iterationCount, double & scaledResidual, bool isFinalOuterIter);

    virtual PetraType getType() override { return PT_TPETRA; }

private:
    /** the solver parameters*/
    const Teuchos::RCP<Teuchos::ParameterList> params_;

    /** The preconditioner parameters*/
    const Teuchos::RCP<Teuchos::ParameterList> paramsPrecond_;
    
    Teuchos::RCP<LinSys::Matrix> matrix_;
    Teuchos::RCP<LinSys::Vector> rhs_;
    Teuchos::RCP<LinSys::LinearProblem> problem_;
    Teuchos::RCP<LinSys::SolverManager> solver_;
    Teuchos::RCP<LinSys::Preconditioner> preconditioner_;
    Teuchos::RCP<LinSys::MultiVector> coords_;

    std::string preconditionerType_;
};

#endif /* TPETRALINEARSOLVER_H */

