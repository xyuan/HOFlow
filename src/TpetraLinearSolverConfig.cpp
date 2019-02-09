/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "TpetraLinearSolverConfig.h"
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>
#include <yaml-cpp/yaml.h>
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_RCP.hpp>
#include <BelosTypes.hpp>

#include <string>
#include <iostream>

//! Constructor
TpetraLinearSolverConfig::TpetraLinearSolverConfig() {
}

//! Destructor
TpetraLinearSolverConfig::~TpetraLinearSolverConfig() {
}

//! Loads all parameters for the linear solver from the input file
void TpetraLinearSolverConfig::load(const YAML::Node& node) {
    name_ = node["name"].as<std::string>();
    method_ = node["method"].as<std::string>();
    get_if_present(node, "preconditioner", precond_, std::string("default"));
    solverType_ = "tpetra";

    double tol;
    int max_iterations, kspace, output_level;

    get_if_present(node, "tolerance", tolerance_, 1.e-4);
    get_if_present(node, "final_tolerance", finalTolerance_, tolerance_);
    get_if_present(node, "max_iterations", max_iterations, 50);
    get_if_present(node, "kspace", kspace, 50);
    get_if_present(node, "output_level", output_level, 0);

    tol = tolerance_;

    //Teuchos::RCP<Teuchos::ParameterList> params = Teuchos::params(); // Commented out from Nalu
    params_->set("Convergence Tolerance", tol);
    params_->set("Maximum Iterations", max_iterations);
    if (output_level > 0)
    {
        params_->set("Verbosity", Belos::Errors + Belos::Warnings + Belos::StatusTestDetails);
        params_->set("Output Style",Belos::Brief); 
    }

    params_->set("Output Frequency", output_level);
    Teuchos::RCP<std::ostream> belosOutputStream = Teuchos::rcpFromRef (HOFlowEnv::self().hoflowOutputP0());
    params_->set("Output Stream", belosOutputStream);
    params_->set("Num Blocks", kspace);
    params_->set("Maximum Restarts", std::max(1,max_iterations/kspace));
    std::string orthoType = "ICGS";
    params_->set("Orthogonalization",orthoType);
    params_->set("Implicit Residual Scaling", "Norm of Preconditioned Initial Residual");

    if (precond_ == "sgs") {
        preconditionerType_ = "RELAXATION";
        paramsPrecond_->set("relaxation: type","Symmetric Gauss-Seidel");
        paramsPrecond_->set("relaxation: sweeps",1);
    }
    else if (precond_ == "mt_sgs") {
        preconditionerType_ = "RELAXATION";
        paramsPrecond_->set("relaxation: type","MT Symmetric Gauss-Seidel");
        paramsPrecond_->set("relaxation: sweeps",1);
    }
    else if (precond_ == "jacobi" || precond_ == "default") {
        preconditionerType_ = "RELAXATION";
        paramsPrecond_->set("relaxation: type","Jacobi");
        paramsPrecond_->set("relaxation: sweeps",1);
    }
    else if (precond_ == "ilut" ) {
        preconditionerType_ = "ILUT";
    }
    else if (precond_ == "riluk" ) {
        preconditionerType_ = "RILUK";
    }
    else {
      throw std::runtime_error("invalid linear solver preconditioner specified ");
    }
    
    get_if_present(node, "recompute_preconditioner", recomputePreconditioner_, recomputePreconditioner_);
    get_if_present(node, "reuse_preconditioner",     reusePreconditioner_,     reusePreconditioner_);
    // Deleted the reading parameters for writeMatrixFiles. 
    // Always use the value defined in LinearSolverConfig.h
}

