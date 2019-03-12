/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HOFLOWENV_H
#define HOFLOWENV_H

#include <mpi.h>
#include <fstream>
#include <streambuf>
  
  class HOFlowEmptyStreamBuffer : public std::filebuf {
  public:
    int overflow(int c) {return c;}
  };

class HOFlowEnv
{
 public:

  HOFlowEnv();
  ~HOFlowEnv();

  static HOFlowEnv &self();

  MPI_Comm parallelCommunicator_;
  int pSize_;
  int pRank_;
  std::streambuf *stdoutStream_;
  std::ostream *hoflowLogStream_;
  std::ostream *hoflowParallelStream_;
  bool parallelLog_;
  
  HOFlowEmptyStreamBuffer hoflowEmptyStreamBuffer_;
  std::filebuf hoflowStreamBuffer_;
  std::filebuf hoflowParallelStreamBuffer_;

  std::ostream & hoflowOutputP0();
  std::ostream & hoflowOutput();

  MPI_Comm parallel_comm();
  int parallel_size();
  int parallel_rank();
  void set_log_file_stream(std::string hoflowLogName, bool pprint = false);
  void close_log_file_stream();
  double hoflow_time();
};

#endif /* HOFLOWENV_H */

