/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HOFLOWENV_H
#define HOFLOWENV_H

#include <mpi.h>
#include <fstream>
#include <streambuf>

//! Stores globally used stuff
class HOFlowEnv {
public:
    HOFlowEnv();
    ~HOFlowEnv();
    static HOFlowEnv & self();
    MPI_Comm parallel_comm();
    MPI_Comm parallelCommunicator_;
    int pSize_;
    int pRank_;
    int parallel_rank();
    std::ostream & hoflowOutputP0();
    std::ostream * hoflowLogStream_;
};

#endif /* HOFLOWENV_H */

