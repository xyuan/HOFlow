/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef SOLVERALGORITHM_H
#define SOLVERALGORITHM_H

#include <Algorithm.h>
#include <KokkosInterface.h>

#include <stk_mesh/base/Entity.hpp>
#include <vector>

class EquationSystem;
class Realm;

class SolverAlgorithm : public Algorithm
{
public:

  SolverAlgorithm(
    Realm &realm,
    stk::mesh::Part *part,
    EquationSystem *eqSystem);
  virtual ~SolverAlgorithm() {}

  virtual void execute() = 0;
  virtual void initialize_connectivity() = 0;

protected:

  // Need to find out whether this ever gets called inside a modification cycle.
  void apply_coeff(
    const std::vector<stk::mesh::Entity> & sym_meshobj,
    std::vector<int> &scratchIds,
    std::vector<double> &scratchVals,
    const std::vector<double> &rhs,
    const std::vector<double> &lhs,
    const char *trace_tag=0);
  
  void apply_coeff(
    unsigned numMeshobjs,
    const stk::mesh::Entity* symMeshobjs,
    const SharedMemView<int*> & scratchIds,
    const SharedMemView<int*> & sortPermutation,
    const SharedMemView<const double*> & rhs,
    const SharedMemView<const double**> & lhs,
    const char *trace_tag);

  EquationSystem *eqSystem_;
};

#endif /* SOLVERALGORITHM_H */

