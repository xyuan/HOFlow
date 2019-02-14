/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "HOFlowEnv.h"
#include <mpi.h>

HOFlowEnv::HOFlowEnv() :
    parallelCommunicator_(MPI_COMM_WORLD),
    pSize_(-1),
    pRank_(-1)
{
    // initialize
    MPI_Comm_size(parallelCommunicator_, &pSize_);
    MPI_Comm_rank(parallelCommunicator_, &pRank_);
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

MPI_Comm HOFlowEnv::parallel_comm() {
  return parallelCommunicator_;
}

int HOFlowEnv::parallel_rank() {
  return pRank_;
}
