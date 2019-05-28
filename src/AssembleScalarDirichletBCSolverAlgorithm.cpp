/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AssembleScalarDirichletBCSolverAlgorithm.h"

#include <EquationSystem.h>
#include <FieldTypeDef.h>
#include <LinearSystem.h>
#include <Realm.h>
#include <TimeIntegrator.h>
#include <master_element/MasterElement.h>

#include <iostream>

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
// AssembleScalarDirichletBCSolverAlgoithm
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
AssembleScalarDirichletBCSolverAlgorithm::AssembleScalarDirichletBCSolverAlgorithm(
  Realm &realm,
  stk::mesh::Part *part,
  EquationSystem *eqSystem,
  ScalarFieldType *bcScalarQ)
  : SolverAlgorithm(realm, part, eqSystem),
    bcScalarQ_(bcScalarQ)
{
  // save off fields
  stk::mesh::MetaData & meta_data = realm_.meta_data();
  exposedAreaVec_ = meta_data.get_field<GenericFieldType>(meta_data.side_rank(), "exposed_area_vector");
}

//--------------------------------------------------------------------------
//-------- initialize_connectivity -----------------------------------------
//--------------------------------------------------------------------------
void
AssembleScalarDirichletBCSolverAlgorithm::initialize_connectivity()
{
  eqSystem_->linsys_->buildFaceToNodeGraph(partVec_);
}

//--------------------------------------------------------------------------
//-------- execute ---------------------------------------------------------
//--------------------------------------------------------------------------
void AssembleScalarDirichletBCSolverAlgorithm::execute() {
    stk::mesh::BulkData & bulk_data = realm_.bulk_data();
    stk::mesh::MetaData & meta_data = realm_.meta_data();

    const int nDim = meta_data.spatial_dimension();


    // space for LHS/RHS; nodesPerFace*nodesPerFace and nodesPerFace
    std::vector<double> lhs;
    std::vector<double> rhs;
    std::vector<int> scratchIds;
    std::vector<double> scratchVals;
    std::vector<stk::mesh::Entity> connected_nodes;

    // nodal fields to gather
    std::vector<double> ws_bcScalarQ;

    // master element
    std::vector<double> ws_face_shape_function;

    // define some common selectors
    stk::mesh::Selector s_locally_owned_union = meta_data.locally_owned_part()
      &stk::mesh::selectUnion(partVec_);

    // Iterate through selected buckets
    stk::mesh::BucketVector const & face_buckets = realm_.get_buckets( meta_data.side_rank(), s_locally_owned_union );
    for ( stk::mesh::BucketVector::const_iterator ib = face_buckets.begin(); ib != face_buckets.end() ; ++ib ) {
        stk::mesh::Bucket & b = **ib ;

        // face master element
        MasterElement *meFC = MasterElementRepo::get_surface_master_element(b.topology());
        const int nodesPerFace = meFC->nodesPerElement_;
        const int numScsBip = meFC->numIntPoints_;

        // mapping from ip to nodes for this ordinal; face perspective (use with face_node_relations)
        const int *faceIpNodeMap = meFC->ipNodeMap();

        // resize some things; matrix related
        const int lhsSize = nodesPerFace*nodesPerFace;
        const int rhsSize = nodesPerFace;
        lhs.resize(lhsSize);
        rhs.resize(rhsSize);
        scratchIds.resize(rhsSize);
        scratchVals.resize(rhsSize);
        connected_nodes.resize(nodesPerFace);

        // algorithm related; element
        ws_bcScalarQ.resize(nodesPerFace);
        ws_face_shape_function.resize(numScsBip*nodesPerFace);

        // pointers
        double *p_lhs = &lhs[0];
        double *p_rhs = &rhs[0];
        double *p_bcScalarQ = &ws_bcScalarQ[0];
        double *p_face_shape_function = &ws_face_shape_function[0];

        // shape functions
        meFC->shape_fcn(&p_face_shape_function[0]);

        const size_t length   = b.size();

        // Iterate though each boundary face in bucket
        for ( size_t k = 0 ; k < length ; ++k ) {
            // zero lhs/rhs
            for ( int p = 0; p < lhsSize; ++p )
                p_lhs[p] = 0.0;
            for ( int p = 0; p < rhsSize; ++p )
                p_rhs[p] = 0.0;

            // get face
            stk::mesh::Entity face = b[k];

            //======================================
            // gather nodal data off of face
            //======================================
            stk::mesh::Entity const * face_node_rels = bulk_data .begin_nodes(face);
            int num_face_nodes = bulk_data.num_nodes(face);
            // sanity check on num nodes
            ThrowAssert( num_face_nodes == nodesPerFace );

            // Make list of connected nodes and get scalar values of the nodes
            for ( int ni = 0; ni < num_face_nodes; ++ni ) {
                // get the node and form connected_node
                stk::mesh::Entity node = face_node_rels[ni];
                connected_nodes[ni] = node;
                std::cout << "node: " << node << std::endl;

                // gather scalar
                p_bcScalarQ[ni] = *stk::mesh::field_data(*bcScalarQ_, node);
                std::cout << "bcScalarQ = " << p_bcScalarQ[ni] << std::endl;
            }

            // pointer to face data
            double * areaVec = stk::mesh::field_data(*exposedAreaVec_, face);

            // Iterate through each integration point
            for ( int ip = 0; ip < numScsBip; ++ip ) {
                const int localFaceNode = faceIpNodeMap[ip];
                const int offSetSF_face = ip*nodesPerFace;
                std::cout << "localFaceNode = " << localFaceNode << std::endl;

                // interpolate to bip
                double fluxBip = 0.0;

                // Iterate through each node of the face
                for ( int ic = 0; ic < nodesPerFace; ++ic ) {
                    const double r = p_face_shape_function[offSetSF_face+ic];
                    std::cout << "shape function of node " << ic << ": " << r << std::endl;
                    fluxBip += r*p_bcScalarQ[ic];
                }

                const int offset = ip*nDim;
                double areaNorm = 0.0;
                for (int idir = 0; idir < nDim; ++idir) {
                    areaNorm += areaVec[offset+idir]*areaVec[offset+idir];
                }
                areaNorm = std::sqrt(areaNorm);

                p_rhs[localFaceNode] += fluxBip*areaNorm;
                std::cout << "rhs value: " << fluxBip*areaNorm << std::endl;
            }

            apply_coeff(connected_nodes, scratchIds, scratchVals, rhs, lhs, __FILE__);
        }
    }
}

