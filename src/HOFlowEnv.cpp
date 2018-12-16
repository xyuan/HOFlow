/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "HOFlowEnv.h"

HOFlowEnv::HOFlowEnv() {
}

HOFlowEnv::~HOFlowEnv() {
}

HOFlowEnv & HOFlowEnv::self()
{
  static HOFlowEnv s;
  return s;
}

std::ostream & HOFlowEnv::hoflowOutputP0() {
    return *hoflowLogStream_;
}

