/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "LinearSolverConfig.h"
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_RCP.hpp>

LinearSolverConfig::LinearSolverConfig()
  : params_(Teuchos::rcp(new Teuchos::ParameterList)),
    paramsPrecond_(Teuchos::rcp(new Teuchos::ParameterList))
{}

LinearSolverConfig::~LinearSolverConfig() {
}

