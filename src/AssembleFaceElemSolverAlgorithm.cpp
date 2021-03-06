/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AssembleFaceElemSolverAlgorithm.h"

#include <EquationSystem.h>
#include <SolverAlgorithm.h>
#include <master_element/MasterElement.h>

#include <FieldTypeDef.h>
#include <LinearSystem.h>
#include <Realm.h>
//#include <TimeIntegrator.h>

// kernel
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
#include <SimdInterface.h>
#include <ScratchViews.h>
#include <CopyAndInterleave.h>

//==========================================================================
// Class Definition
//==========================================================================
// AssembleFaceElemSolverAlgorithm - add LHS/RHS for element-based contribution
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
AssembleFaceElemSolverAlgorithm::AssembleFaceElemSolverAlgorithm(
  Realm &realm,
  stk::mesh::Part *part,
  EquationSystem *eqSystem,
  unsigned nodesPerFace,
  unsigned nodesPerElem,
  bool interleaveMEViews)
  : SolverAlgorithm(realm, part, eqSystem),
    numDof_(eqSystem->linsys_->numDof()),
    nodesPerFace_(nodesPerFace),
    nodesPerElem_(nodesPerElem),
    rhsSize_(nodesPerFace*eqSystem->linsys_->numDof()),
    interleaveMEViews_(interleaveMEViews)
{
}

//--------------------------------------------------------------------------
//-------- initialize_connectivity -----------------------------------------
//--------------------------------------------------------------------------
void
AssembleFaceElemSolverAlgorithm::initialize_connectivity()
{
  eqSystem_->linsys_->buildFaceElemToNodeGraph(partVec_);
}

//--------------------------------------------------------------------------
//-------- execute ---------------------------------------------------------
//--------------------------------------------------------------------------
void
AssembleFaceElemSolverAlgorithm::execute()
{
  stk::mesh::BulkData & bulk_data = realm_.bulk_data();

  for (auto kernel : activeKernels_) {
    kernel->setup();
  }

  run_face_elem_algorithm(bulk_data,
    [&](SharedMemData_FaceElem &smdata)
    {
        set_zero(smdata.simdrhs.data(), smdata.simdrhs.size());
        set_zero(smdata.simdlhs.data(), smdata.simdlhs.size());

        for (auto kernel : activeKernels_)
          kernel->execute( smdata.simdlhs, smdata.simdrhs, smdata.simdFaceViews, smdata.simdElemViews, smdata.elemFaceOrdinal );

        for(int simdIndex=0; simdIndex<smdata.numSimdFaces; ++simdIndex) {
          extract_vector_lane(smdata.simdrhs, simdIndex, smdata.rhs);
          extract_vector_lane(smdata.simdlhs, simdIndex, smdata.lhs);
          apply_coeff(nodesPerElem_, smdata.connectedNodes[simdIndex],
                      smdata.scratchIds, smdata.sortPermutation, smdata.rhs, smdata.lhs, __FILE__);
        }
    }
  );
}