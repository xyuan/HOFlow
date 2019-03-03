/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "LinearSolverConfig.h"
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_RCP.hpp>
#include <TpetraLinearSolverConfig.h>

LinearSolverConfig::LinearSolverConfig() : 
    params_(Teuchos::rcp(new Teuchos::ParameterList)),
    paramsPrecond_(Teuchos::rcp(new Teuchos::ParameterList))
{
    // nothing to do
}

LinearSolverConfig::~LinearSolverConfig() {
}

