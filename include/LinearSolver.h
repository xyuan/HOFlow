/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef LINEARSOLVER_H
#define LINEARSOLVER_H

#include <LinearSolverTypes.h>
#include <LocalGraphArrays.h>
#include <Enums.h>

#include <Kokkos_DefaultNode.hpp>
#include <Tpetra_Vector.hpp>
#include <Tpetra_CrsMatrix.hpp>
#include <Teuchos_GlobalMPISession.hpp>
#include <Teuchos_oblackholestream.hpp>

#include <Ifpack2_Factory.hpp>

// Header files defining default types for template parameters.
// These headers must be included after other MueLu/Xpetra headers.
typedef double Scalar;
typedef long GlobalOrdinal;
typedef int LocalOrdinal;
typedef Tpetra::Map<LocalOrdinal, GlobalOrdinal>::node_type Node;
typedef Teuchos::ScalarTraits<Scalar> STS;

class LinearSolvers;
//class TpetraLinearSolver;
//class TpetraLinearSolverConfig;
class LinearSolverConfig;
class Simulation;

/** An abstract representation of a linear solver in Nalu
 *
 *  Defines the basic API supported by the linear solvers for use within Nalu.
 *  See concrete implementations such as sierra::nalu::TpetraLinearSolver for
 *  more details.
 */
class LinearSolver {
public:
    LinearSolver(std::string name, LinearSolvers * linearSolvers, LinearSolverConfig * config);
    virtual ~LinearSolver();

    //! User-friendly identifier for this particular solver instance
    std::string name_;

    //! Type of solver instance as defined in sierra::nalu::PetraType
    virtual PetraType getType() = 0;

    //! Utility method to cleanup solvers during simulation
    virtual void destroyLinearSolver() = 0;

    Simulation * root();
    LinearSolvers * parent();
    LinearSolvers * linearSolvers_;
    
protected:
    LinearSolverConfig * config_;
    bool recomputePreconditioner_;
    bool reusePreconditioner_;
    double timerPrecond_;

public:
    //! Flag indicating whether the preconditioner is recomputed on each invocation
    bool & recomputePreconditioner();
    
    //! Flag indicating whether the preconditioner is reused on each invocation
    bool & reusePreconditioner();
    
    //! Reset the preconditioner timer to 0.0 for future accumulation
    void zero_timer_precond();

    //! Get the preconditioner timer for the last invocation
    double get_timer_precond();

    //! Get the solver configuration specified in the input file
    LinearSolverConfig* getConfig();
};

#endif /* LINEARSOLVER_H */

