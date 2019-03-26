/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef SOLVERALGORITHMDRIVER_H
#define SOLVERALGORITHMDRIVER_H

#include<AlgorithmDriver.h>
#include<Enums.h>

#include<map>

class Realm;
class SolverAlgorithm;

class SolverAlgorithmDriver : public AlgorithmDriver
{
public:

  SolverAlgorithmDriver(
    Realm &realm);
  virtual ~SolverAlgorithmDriver();

  virtual void initialize_connectivity();
  virtual void pre_work();
  virtual void execute();
  virtual void post_work();
  
  // different types of algorithms... interior/flux; constraints and dirichlet
  std::map<std::string, SolverAlgorithm *> solverAlgorithmMap_;
  std::map<AlgorithmType, SolverAlgorithm *> solverAlgMap_;
  std::map<AlgorithmType, SolverAlgorithm *> solverConstraintAlgMap_;
  std::map<AlgorithmType, SolverAlgorithm *> solverDirichAlgMap_;
};

#endif /* SOLVERALGORITHMDRIVER_H */

