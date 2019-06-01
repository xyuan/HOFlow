/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AssembleScalarFluxBCSolverAlgorithm.h"

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
// AssembleScalarFluxBCSolverAlgoithm - scalar flux bc, Int bcScalarQ*area
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
AssembleScalarFluxBCSolverAlgorithm::AssembleScalarFluxBCSolverAlgorithm(Realm &realm,
                                                                         stk::mesh::Part *part,
                                                                         EquationSystem *eqSystem) : 
    SolverAlgorithm(realm, part, eqSystem)
{
  // save off fields
  stk::mesh::MetaData & meta_data = realm_.meta_data();
  bcScalarQ_ = meta_data.get_field<ScalarFieldType>(meta_data.side_rank(), "heat_flux_bc");
  exposedAreaVec_ = meta_data.get_field<GenericFieldType>(meta_data.side_rank(), "exposed_area_vector");
}

//--------------------------------------------------------------------------
//-------- initialize_connectivity -----------------------------------------
//--------------------------------------------------------------------------
void
AssembleScalarFluxBCSolverAlgorithm::initialize_connectivity()
{
  eqSystem_->linsys_->buildFaceToNodeGraph(partVec_);
}

//--------------------------------------------------------------------------
//-------- execute ---------------------------------------------------------
//--------------------------------------------------------------------------
void AssembleScalarFluxBCSolverAlgorithm::execute() {
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
    double ws_bcScalarQ;

    // master element
    std::vector<double> ws_face_shape_function;

    // define some common selectors
    stk::mesh::Selector s_locally_owned_union = meta_data.locally_owned_part()
      &stk::mesh::selectUnion(partVec_);
    
    std::cout << std::endl;
    std::cout << "Flux BC Assembling" << std::endl;

    // Iterate through selected buckets
    stk::mesh::BucketVector const & face_buckets = realm_.get_buckets( meta_data.side_rank(), s_locally_owned_union );
    for ( stk::mesh::BucketVector::const_iterator ib = face_buckets.begin(); ib != face_buckets.end() ; ++ib ) {
        stk::mesh::Bucket & b = **ib;
        
        std::cout << "/////////// NEW Boundary ///////////" << std::endl;

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
        ws_face_shape_function.resize(numScsBip*nodesPerFace);

        // pointers
        double *p_lhs = &lhs[0];
        double *p_rhs = &rhs[0];
        double *p_bcScalarQ = &ws_bcScalarQ;
        double *p_face_shape_function = &ws_face_shape_function[0];

        // shape functions
        meFC->shape_fcn(&p_face_shape_function[0]);

        const size_t length   = b.size();
//        std::cout << "length: " << length << std::endl;

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
            stk::mesh::Entity const * face_node_rels = bulk_data.begin_nodes(face);
            int num_face_nodes = bulk_data.num_nodes(face);
            // sanity check on num nodes
            ThrowAssert( num_face_nodes == nodesPerFace );

            // Make list of connected nodes
            for ( int ni = 0; ni < num_face_nodes; ++ni ) {
                // get the node and form connected_node
                stk::mesh::Entity node = face_node_rels[ni];
                connected_nodes[ni] = node;
            }
            
            // get definied boundary heat flux
            p_bcScalarQ = stk::mesh::field_data(*bcScalarQ_, face);
            std::cout << "bcScalarQ = " << *p_bcScalarQ << std::endl;
            
            // pointer to face data
            double * areaVec = stk::mesh::field_data(*exposedAreaVec_, face);

            // Iterate through each integration point
            for ( int ip = 0; ip < numScsBip; ++ip ) {
                const int localFaceNode = faceIpNodeMap[ip];
//                std::cout << "localFaceNode = " << localFaceNode << std::endl;

                // interpolate to bip
                const double fluxBip = *p_bcScalarQ;

                const int offset = ip*nDim;
                double areaNorm = 0.0;
                for (int idir = 0; idir < nDim; ++idir) {
                    areaNorm += areaVec[offset+idir]*areaVec[offset+idir];
                }
                areaNorm = std::sqrt(areaNorm);

                p_rhs[localFaceNode] += fluxBip*areaNorm;
            }

            apply_coeff(connected_nodes, scratchIds, scratchVals, rhs, lhs, __FILE__);
            std::cout << "printing rhs: " << std::endl;
            for (int i=0; i<rhsSize; ++i) {
                std::cout << p_rhs[i] << ", ";
            }
            std::cout << std::endl;
        }
    }
}

