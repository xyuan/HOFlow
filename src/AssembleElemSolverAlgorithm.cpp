/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AssembleElemSolverAlgorithm.h"
#include <EquationSystem.h>
#include <SolverAlgorithm.h>
#include <master_element/MasterElement.h>

#include <FieldTypeDef.h>
#include <LinearSystem.h>
#include <Realm.h>
#include <TimeIntegrator.h>

#include <kernel/Kernel.h>

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/GetEntities.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Part.hpp>

// stk topo
#include <stk_topology/topology.hpp>

#include <KokkosInterface.h>
#include <ScratchViews.h>
#include <CopyAndInterleave.h>

//==========================================================================
// Class Definition
//==========================================================================
// AssembleElemSolverAlgorithm - add LHS/RHS for element-based contribution
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
AssembleElemSolverAlgorithm::AssembleElemSolverAlgorithm(
  Realm &realm,
  stk::mesh::Part *part,
  EquationSystem *eqSystem,
  stk::mesh::EntityRank entityRank,
  unsigned nodesPerEntity,
  bool interleaveMEViews)
  : SolverAlgorithm(realm, part, eqSystem),
    entityRank_(entityRank),
    nodesPerEntity_(nodesPerEntity),
    rhsSize_(nodesPerEntity*eqSystem->linsys_->numDof()),
    interleaveMEViews_(interleaveMEViews)
{
}

//--------------------------------------------------------------------------
//-------- initialize_connectivity -----------------------------------------
//--------------------------------------------------------------------------
void
AssembleElemSolverAlgorithm::initialize_connectivity()
{
  eqSystem_->linsys_->buildElemToNodeGraph(partVec_);
}

//--------------------------------------------------------------------------
//-------- execute ---------------------------------------------------------
//--------------------------------------------------------------------------
void
AssembleElemSolverAlgorithm::execute()
{
  stk::mesh::BulkData & bulk_data = realm_.bulk_data();

  // set any data
  const size_t activeKernelsSize = activeKernels_.size();
  for ( size_t i = 0; i < activeKernelsSize; ++i )
    activeKernels_[i]->setup(*realm_.timeIntegrator_);

  run_algorithm(bulk_data, [&](SharedMemData& smdata)
  {
      set_zero(smdata.simdrhs.data(), smdata.simdrhs.size());
      set_zero(smdata.simdlhs.data(), smdata.simdlhs.size());

      // call supplemental; gathers happen inside the elem_execute method
      for ( size_t i = 0; i < activeKernelsSize; ++i )
        activeKernels_[i]->execute( smdata.simdlhs, smdata.simdrhs, smdata.simdPrereqData );

      for(int simdElemIndex=0; simdElemIndex<smdata.numSimdElems; ++simdElemIndex) {
        extract_vector_lane(smdata.simdrhs, simdElemIndex, smdata.rhs);
        extract_vector_lane(smdata.simdlhs, simdElemIndex, smdata.lhs);
        apply_coeff(nodesPerEntity_, smdata.elemNodes[simdElemIndex],
                    smdata.scratchIds, smdata.sortPermutation, smdata.rhs, smdata.lhs, __FILE__);
      }
  });
}