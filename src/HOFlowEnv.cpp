/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "HOFlowEnv.h"
#include <mpi.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>

#include <stk_util/environment/WallTime.hpp>

HOFlowEnv::HOFlowEnv() : 
    parallelCommunicator_(MPI_COMM_WORLD),
    pSize_(-1),
    pRank_(-1),
    stdoutStream_(std::cout.rdbuf()),
    hoflowLogStream_(&std::cout), // std::cout redirects to log file
    hoflowParallelStream_(new std::ostream(&hoflowParallelStreamBuffer_)),
    parallelLog_(false)
{
    // initialize
    MPI_Comm_size(parallelCommunicator_, &pSize_);
    MPI_Comm_rank(parallelCommunicator_, &pRank_);
}

HOFlowEnv & HOFlowEnv::self() {
    static HOFlowEnv s;
    return s;
}

std::ostream & HOFlowEnv::hoflowOutputP0() {
    return * hoflowLogStream_;
}

std::ostream & HOFlowEnv::hoflowOutput() {
    return * hoflowParallelStream_;
}

int HOFlowEnv::parallel_size() {
    return pSize_;
}

int HOFlowEnv::parallel_rank() {
    return pRank_;
}

MPI_Comm HOFlowEnv::parallel_comm() {
    return parallelCommunicator_;
}

void HOFlowEnv::set_log_file_stream(std::string hoflowLogName, bool pprint) {
    if ( pRank_ == 0 ) {
        hoflowStreamBuffer_.open(hoflowLogName.c_str(), std::ios::out);
        hoflowLogStream_->rdbuf(&hoflowStreamBuffer_);
    }
    else {
        hoflowLogStream_->rdbuf(&hoflowEmptyStreamBuffer_);
    }

    // default to an empty stream buffer for parallel unless pprint is set
    parallelLog_ = pprint;
    if (parallelLog_) {
        int numPlaces = static_cast<int>(std::log10(pSize_-1)+1);

        std::stringstream paddedRank;
        paddedRank << std::setw(numPlaces) << std::setfill('0') << parallel_rank();

        // inputname.log -> inputname.log.16.02 for the rank 2 proc of a 16 proc job
        std::string parallelLogName =
            hoflowLogName + "." + std::to_string(pSize_) + "." + paddedRank.str();
        hoflowParallelStreamBuffer_.open(parallelLogName.c_str(), std::ios::out);
        hoflowParallelStream_->rdbuf(&hoflowParallelStreamBuffer_);
    }
    else {
        hoflowParallelStream_->rdbuf(stdoutStream_);
    }
}

void HOFlowEnv::close_log_file_stream() {
    if ( pRank_ == 0 ) {
        hoflowStreamBuffer_.close();
    }
    if (parallelLog_) {
        hoflowParallelStreamBuffer_.close();
    }
}

HOFlowEnv::~HOFlowEnv() {
    close_log_file_stream();
    delete hoflowParallelStream_;

    // shut down MPI
    MPI_Finalize();
}

double HOFlowEnv::hoflow_time() {
    return stk::wall_time();
}