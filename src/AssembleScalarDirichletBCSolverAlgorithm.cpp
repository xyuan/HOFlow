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
AssembleScalarDirichletBCSolverAlgorithm::AssembleScalarDirichletBCSolverAlgorithm(Realm &realm,
                                                                                   stk::mesh::Part *part,
                                                                                   EquationSystem *eqSystem,
                                                                                   ScalarFieldType *bcScalarQ,
                                                                                   ScalarFieldType *diffFluxCoeff) : 
    SolverAlgorithm(realm, part, eqSystem),
    bcScalarQ_(bcScalarQ),
    diffFluxCoeff_(diffFluxCoeff)
{
    // save off fields
    stk::mesh::MetaData & meta_data = realm_.meta_data();
    exposedAreaVec_ = meta_data.get_field<GenericFieldType>(meta_data.side_rank(), "exposed_area_vector");
    coordinates_ = meta_data.get_field<VectorFieldType>(stk::topology::NODE_RANK, realm_.get_coordinates_name());
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
//    stk::mesh::BulkData & bulk_data = realm_.bulk_data();
//    stk::mesh::MetaData & meta_data = realm_.meta_data();
//
//    const int nDim = meta_data.spatial_dimension();
//
//    // space for LHS/RHS; nodesPerElem*nodesPerElem* and nodesPerElem
//    std::vector<double> lhs;
//    std::vector<double> rhs;
//    std::vector<int> scratchIds;
//    std::vector<double> scratchVals;
//    std::vector<stk::mesh::Entity> connected_nodes;
//
//    // nodal fields to gather
//    std::vector<double> ws_coordinates;
//    double ws_bcScalarQ;
//    std::vector<double> ws_diffFluxCoeff;
//
//    // geometry related to populate
//    std::vector<double> ws_scs_areav;
//    std::vector<double> ws_dndx;
//    std::vector<double> ws_deriv;
//    std::vector<double> ws_det_j;
//    std::vector<double> ws_shape_function;
//
//    // define some common selectors
//    stk::mesh::Selector s_locally_owned_union = meta_data.locally_owned_part()
//        &stk::mesh::selectUnion(partVec_);
//    
//    // Iterate through selected buckets
//    stk::mesh::BucketVector const & face_buckets = realm_.get_buckets( meta_data.side_rank(), s_locally_owned_union );
//    for ( stk::mesh::BucketVector::const_iterator ib = face_buckets.begin(); ib != face_buckets.end() ; ++ib ) {
//        stk::mesh::Bucket & b = **ib;
//        const stk::mesh::Bucket::size_type length = b.size();
//
//        
//
//        // extract master element
//        MasterElement * meSCS = MasterElementRepo::get_surface_master_element(b.topology());
//        MasterElement * meSCV = MasterElementRepo::get_volume_master_element(b.topology());
//
//        // extract master element specifics
//        const int nodesPerElement = meSCS->nodesPerElement_;
//        const int numScsIp = meSCS->numIntPoints_;
//        const int * lrscv = meSCS->adjacentNodes();
//
//        // resize some things; matrix related
//        const int lhsSize = nodesPerElement*nodesPerElement;
//        const int rhsSize = nodesPerElement;
//        lhs.resize(lhsSize);
//        rhs.resize(rhsSize);
//        scratchIds.resize(rhsSize);
//        scratchVals.resize(rhsSize);
//        connected_nodes.resize(nodesPerElement);
//
//        // algorithm related
//        ws_coordinates.resize(nodesPerElement*nDim);
//        ws_diffFluxCoeff.resize(nodesPerElement);
//        ws_scs_areav.resize(numScsIp*nDim);
//        ws_dndx.resize(nDim*numScsIp*nodesPerElement);
//        ws_deriv.resize(nDim*numScsIp*nodesPerElement);
//        ws_det_j.resize(numScsIp);
//        ws_shape_function.resize(numScsIp*nodesPerElement);
//
//        // pointer to lhs/rhs
//        double * p_lhs = &lhs[0];
//        double * p_rhs = &rhs[0];
//
//        double * p_coordinates = &ws_coordinates[0];
//
//        double * p_bcScalarQ = &ws_bcScalarQ;
//        double * p_diffFluxCoeff = &ws_diffFluxCoeff[0];
//        double * p_scs_areav = &ws_scs_areav[0];
//        double * p_dndx = &ws_dndx[0];
//        double * p_shape_function = &ws_shape_function[0];
//
//        // extract shape function
//        meSCS->shape_fcn(&p_shape_function[0]);
//        
//        // Iterate through each element in bucket
//        for ( size_t k = 0 ; k < length ; ++k ) {
////            // get elem
////            stk::mesh::Entity elem = b[k];
//            
//            // get face
//            stk::mesh::Entity face = b[k];
//
//            // zero lhs/rhs
//            for ( int p = 0; p < lhsSize; ++p )
//              p_lhs[p] = 0.0;
//            for ( int p = 0; p < rhsSize; ++p )
//              p_rhs[p] = 0.0;
//
//            //===============================================
//            // gather nodal data; this is how we do it now..
//            //===============================================
//            stk::mesh::Entity const * node_rels = bulk_data.begin_nodes(elem);
//            int num_nodes = bulk_data.num_nodes(elem);
//
//            // sanity check on num nodes
//            ThrowAssert( num_nodes == nodesPerElement );
//            
//            // Iterate through each node in element
//            for ( int ni = 0; ni < num_nodes; ++ni ) {
//                stk::mesh::Entity node = node_rels[ni];
//
//                // set connected nodes
//                connected_nodes[ni] = node;
//
//                // pointers to real data, get coordinates of the node
//                const double * coords = stk::mesh::field_data(*coordinates_, node );
//
//                // get diffusion coefficients
//                p_diffFluxCoeff[ni] = *stk::mesh::field_data(*diffFluxCoeff_, node );
//
//                // gather vectors, get coordinates for each dimension of the node
//                const int niNdim = ni*nDim;
//                for ( int i=0; i < nDim; ++i ) {
//                    p_coordinates[niNdim+i] = coords[i];
//                }
//            }
//            
//            p_bcScalarQ = stk::mesh::field_data(bcScalarQ_, face);
//
//            // compute geometry
//            double scs_error = 0.0;
//            meSCS->determinant(1, &p_coordinates[0], &p_scs_areav[0], &scs_error);
//
//            // compute dndx
//            meSCS->grad_op(1, &ws_coordinates[0], &ws_dndx[0], &ws_deriv[0], &ws_det_j[0], &scs_error);
//            
//            // pointer to face data
//            double * areaVec = stk::mesh::field_data(*exposedAreaVec_, face);
//            
//            // Iterate through all integration points
//            for ( int ip = 0; ip < numScsIp; ++ip ) {
//                // left and right nodes for this ip
//                const int il = lrscv[2*ip];
//                const int ir = lrscv[2*ip+1];
//
//                // corresponding matrix rows
//                const int rowL = il*nodesPerElement;
//                const int rowR = ir*nodesPerElement;
//
//                // save off ip values; offset to Shape Function
//                double muIp = 0.0;
//                const int offSetSF = ip*nodesPerElement;
//                
//                // Iterate through all nodes of a element to get the flux coefficient
//                // contribution of each node to the current integration point
//                for ( int ic = 0; ic < nodesPerElement; ++ic ) {
//                    const double r = p_shape_function[offSetSF+ic];
//                    muIp += r*p_diffFluxCoeff[ic];
//                }
//                
//                // Compute area related to current IP
//                const int offset = ip*nDim;
//                double areaNorm = 0.0;
//                for (int idir = 0; idir < nDim; ++idir) {
//                    areaNorm += areaVec[offset+idir]*areaVec[offset+idir];
//                }
//                areaNorm = std::sqrt(areaNorm);
//
//                double qDiff = 0.0;
//                
//                // Iterate through all nodes of a element to get the face flux coefficient
//                // contribution of each node to the current integration point
//                for ( int ic = 0; ic < nodesPerElement; ++ic ) {
//                    // diffusion
//                    double lhsfacDiff = 0.0;
//                    const int offSetDnDx = nDim*nodesPerElement*ip + ic*nDim;
//                    // Iterate through all spatial dimensions
//                    for ( int j = 0; j < nDim; ++j ) {
//                        lhsfacDiff += -muIp*p_dndx[offSetDnDx+j]*areaVec[ip*nDim+j];
//                        
//                        std::cout << std::endl;
//                        std::cout << "Node: " << ic << " Dimension: " << j << std::endl;
//                        std::cout << "lhsfacDiff sum = " << lhsfacDiff << std::endl;
//                        std::cout << "p_dndx[offSetDnDx+j] = " << p_dndx[offSetDnDx+j] << std::endl;
//                        std::cout << "p_scs_areav[ip*nDim+j] = " << areaVec[ip*nDim+j] << std::endl;
//                    }
//
//                    qDiff += lhsfacDiff*p_bcScalarQ;
//
//                    // lhs; il then ir
//                    p_lhs[rowL+ic] += lhsfacDiff;
//                }
//
//                // rhs; il then ir
//                p_rhs[il] -= qDiff;        
//            }
//            apply_coeff(connected_nodes, scratchIds, scratchVals, rhs, lhs, __FILE__);
//        }
//    }
}
