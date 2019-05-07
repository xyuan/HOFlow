/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AssembleNodeSolverAlgorithm.h"

#include <EquationSystem.h>
#include <SolverAlgorithm.h>

#include <FieldTypeDef.h>
#include <LinearSystem.h>
#include <Realm.h>
#include <SupplementalAlgorithm.h>
#include <TimeIntegrator.h>

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/GetEntities.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Part.hpp>

//==========================================================================
// Class Definition
//==========================================================================
// AssembleNodeSolverAlgorithm - add LHS/RHS for node-based contribution
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
AssembleNodeSolverAlgorithm::AssembleNodeSolverAlgorithm(
  Realm &realm,
  stk::mesh::Part *part,
  EquationSystem *eqSystem)
  : SolverAlgorithm(realm, part, eqSystem),
    sizeOfSystem_(eqSystem->linsys_->numDof())
{
  // nothing to do
}

void
AssembleNodeSolverAlgorithm::initialize_connectivity()
{
  eqSystem_->linsys_->buildNodeGraph(partVec_);
}
//--------------------------------------------------------------------------
//-------- execute ---------------------------------------------------------
//--------------------------------------------------------------------------
void
AssembleNodeSolverAlgorithm::execute()
{
  stk::mesh::MetaData & meta_data = realm_.meta_data();

  // space for LHS/RHS
  const int lhsSize = sizeOfSystem_*sizeOfSystem_;
  const int rhsSize = sizeOfSystem_;
  std::vector<double> lhs(lhsSize);
  std::vector<double> rhs(rhsSize);
  std::vector<int> scratchIds(rhsSize);
  std::vector<double> scratchVals(rhsSize);
  std::vector<stk::mesh::Entity> connected_nodes(1);

  // pointers
  double *p_lhs = &lhs[0];
  double *p_rhs = &rhs[0];

  // supplemental algorithm size and setup
  const size_t supplementalAlgSize = supplementalAlg_.size();
  for ( size_t i = 0; i < supplementalAlgSize; ++i )
    supplementalAlg_[i]->setup();

  // define some common selectors
  stk::mesh::Selector s_locally_owned_union = meta_data.locally_owned_part()
    & stk::mesh::selectUnion(partVec_) 
    & !(stk::mesh::selectUnion(realm_.get_slave_part_vector()))
    & !(realm_.get_inactive_selector());

  stk::mesh::BucketVector const& node_buckets =
    realm_.get_buckets( stk::topology::NODE_RANK, s_locally_owned_union );
  for ( stk::mesh::BucketVector::const_iterator ib = node_buckets.begin();
        ib != node_buckets.end() ; ++ib ) {
    stk::mesh::Bucket & b = **ib ;
    const stk::mesh::Bucket::size_type length   = b.size();

    for ( stk::mesh::Bucket::size_type k = 0 ; k < length ; ++k ) {

      // get node
      stk::mesh::Entity node = b[k];
      connected_nodes[0] = node;

      for ( int i = 0; i < lhsSize; ++i )
        p_lhs[i] = 0.0;
      for ( int i = 0; i < rhsSize; ++i )
        p_rhs[i] = 0.0;

      // call supplemental, e.g. temporal solver
      for ( size_t i = 0; i < supplementalAlgSize; ++i )
        supplementalAlg_[i]->node_execute( &lhs[0], &rhs[0], node);

      apply_coeff(connected_nodes, scratchIds, scratchVals, rhs, lhs, __FILE__);

    }
  }
}