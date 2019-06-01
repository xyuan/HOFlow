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
                                                                                   ScalarFieldType *scalarQ,
                                                                                   ScalarFieldType *bcScalarQ,
                                                                                   ScalarFieldType *diffFluxCoeff) : 
    SolverAlgorithm(realm, part, eqSystem),
    scalarQ_(scalarQ),
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
    stk::mesh::BulkData & bulk_data = realm_.bulk_data();
    stk::mesh::MetaData & meta_data = realm_.meta_data();

    const int nDim = meta_data.spatial_dimension();

    // space for LHS/RHS; nodesPerElem*nodesPerElem* and nodesPerElem
    std::vector<double> lhs;
    std::vector<double> rhs;
    std::vector<int> scratchIds;
    std::vector<double> scratchVals;
    std::vector<stk::mesh::Entity> connected_nodes;
    
    std::vector<int> boundaryNodeLocation;
    
    // deal with state
    ScalarFieldType & scalarQNp1   = scalarQ_->field_of_state(stk::mesh::StateNP1);

    // nodal fields to gather
    std::vector<double> ws_coordinates;
    std::vector<double> ws_scalarQNp1;
    std::vector<double> ws_diffFluxCoeff;
    double ws_bcScalarQ;

    // geometry related to populate
    std::vector<double> ws_scs_areav;
    std::vector<double> ws_dndx;
    std::vector<double> ws_deriv;
    std::vector<double> ws_det_j;
    std::vector<double> ws_face_shape_function;
    
    std::vector<bool> ws_is_boundary_node;

    // define some common selectors
    stk::mesh::Selector s_locally_owned_union = meta_data.locally_owned_part()
        &stk::mesh::selectUnion(partVec_);
    std::cout << std::endl;
    std::cout << "Dirichlet BC Assembling" << std::endl;
    
    
    // Iterate through selected buckets
    stk::mesh::BucketVector const & face_buckets = realm_.get_buckets( meta_data.side_rank(), s_locally_owned_union );
    for ( stk::mesh::BucketVector::const_iterator ib = face_buckets.begin(); ib != face_buckets.end() ; ++ib ) {
        stk::mesh::Bucket & b = **ib;
        const stk::mesh::Bucket::size_type length = b.size();
        
        std::cout << "/////////// NEW Boundary ///////////" << std::endl;

        // Get the first face just to determine the topology
        stk::mesh::Entity face = b[0];     
        stk::mesh::Entity const temp_elem = *bulk_data.begin_elements(face);
        stk::topology elem_topo = bulk_data.bucket(temp_elem).topology();   
        
//        auto tempo = b.getPartition();

        // extract master element
        MasterElement * meSCS = MasterElementRepo::get_surface_master_element(elem_topo);
        MasterElement * meFC = MasterElementRepo::get_surface_master_element(b.topology());

        // extract master element specifics
        const int nodesPerElement = meSCS->nodesPerElement_; 
        const int numScsIp = meSCS->numIntPoints_;
        const int numScsBip = meFC->numIntPoints_;
        const int nodesPerFace = meFC->nodesPerElement_;
        
        // mapping from ip to nodes for this ordinal; face perspective (use with face_node_relations)
        const int * faceIpNodeMap = meFC->ipNodeMap();

        // resize some things; matrix related
        const int lhsSize = nodesPerElement*nodesPerElement;
        const int rhsSize = nodesPerElement;
        lhs.resize(lhsSize);
        rhs.resize(rhsSize);
        scratchIds.resize(rhsSize);
        scratchVals.resize(rhsSize);
        connected_nodes.resize(nodesPerElement);
        
        boundaryNodeLocation.resize(nodesPerFace);

        // algorithm related
        ws_coordinates.resize(nodesPerElement*nDim);
        ws_scalarQNp1.resize(nodesPerElement);
        ws_diffFluxCoeff.resize(nodesPerElement);
        ws_scs_areav.resize(numScsIp*nDim);
        ws_dndx.resize(nDim*numScsIp*nodesPerElement);
        ws_deriv.resize(nDim*numScsIp*nodesPerElement);
        ws_det_j.resize(numScsIp);
        ws_face_shape_function.resize(numScsBip*nodesPerElement);
        
        ws_is_boundary_node.resize(nodesPerElement);

        // pointer to lhs/rhs
        double * p_lhs = &lhs[0];
        double * p_rhs = &rhs[0];

        double * p_coordinates = &ws_coordinates[0];

        double * p_scalarQNp1 = &ws_scalarQNp1[0];
        double * p_bcScalarQ = &ws_bcScalarQ;
        double * p_diffFluxCoeff = &ws_diffFluxCoeff[0];
        double * p_scs_areav = &ws_scs_areav[0];
        double * p_dndx = &ws_dndx[0];
        double * p_face_shape_function = &ws_face_shape_function[0];

        // extract shape function
        meFC->shape_fcn(&p_face_shape_function[0]);
        
        // Iterate through each element in bucket
        for ( size_t k = 0 ; k < length ; ++k ) {
            std::cout << "new boundary element" << std::endl;
            
            // get element and face
            stk::mesh::Entity face = b[k];
            stk::mesh::Entity elem = *bulk_data.begin_elements(face);
            
            // get nodes of the current element
            stk::mesh::Entity const * elem_node_rels = bulk_data.begin_nodes(elem);
            stk::mesh::Entity const * face_node_rels = bulk_data.begin_nodes(face);
            int num_nodes = bulk_data.num_nodes(elem);
            int num_face_nodes = bulk_data.num_nodes(face);
            
            // sanity check on num nodes
            ThrowAssert( num_nodes == nodesPerElement );

            // zero lhs/rhs/is_boundary_node
            for ( int p = 0; p < lhsSize; ++p )
                p_lhs[p] = 0.0;
            for ( int p = 0; p < rhsSize; ++p )
                p_rhs[p] = 0.0;
            for ( int p = 0; p < num_nodes; ++p)
                ws_is_boundary_node[p] = false;
            
            // Make list of connected nodes of the element and get scalar values
            for ( int ni = 0; ni < num_nodes; ++ni ) {
                stk::mesh::Entity node = elem_node_rels[ni];

                // set connected nodes
                connected_nodes[ni] = node;

                // pointers to real data, get coordinates of the node
                const double * coords = stk::mesh::field_data(*coordinates_, node);

                // gather scalars, get field value of the node
                p_scalarQNp1[ni]    = *stk::mesh::field_data(scalarQNp1, node );
                p_diffFluxCoeff[ni] = *stk::mesh::field_data(*diffFluxCoeff_, node);

                // gather vectors, get coordinates for each dimension of the node
                const int niNdim = ni*nDim;
                for ( int i=0; i < nDim; ++i ) {
                    p_coordinates[niNdim+i] = coords[i];
                }
                
                // Check if a boundary node and mark it in is_boundary vector
                bool is_boundary = false;
                for (int i=0; i < num_face_nodes; ++i) {
                    if (node == face_node_rels[i])
                        is_boundary = true;
                }
                
                ws_is_boundary_node[ni] = is_boundary;
            }
            
            // get boundary value
            p_bcScalarQ = stk::mesh::field_data(*bcScalarQ_, face);
            const double temperatureBip = *p_bcScalarQ;
            std::cout << "Boundary temperature: " << temperatureBip << std::endl;

            // compute geometry
            double scs_error = 0.0;
            meSCS->determinant(1, &p_coordinates[0], &p_scs_areav[0], &scs_error);

            // compute dndx
            meSCS->grad_op(1, &ws_coordinates[0], &ws_dndx[0], &ws_deriv[0], &ws_det_j[0], &scs_error);
            
            // pointer to face data
            double * areaVec = stk::mesh::field_data(*exposedAreaVec_, face);
       
            // Determin where in the local lhs and rhs the boundary nodes are
            for (int i=0; i<num_face_nodes; ++i) {
                const int localFaceNode = faceIpNodeMap[i];
                stk::mesh::Entity face_node = face_node_rels[localFaceNode];
                
                for (int j=0; j<num_nodes; ++j) {
                    stk::mesh::Entity elem_node = connected_nodes[j];
                    if (face_node == elem_node) {
                        boundaryNodeLocation[i] = j;
                    }
                }  
            }
  
            // Iterate through all integration points
            for ( int ip = 0; ip < numScsBip; ++ip ) {
                std::cout << "new IP" << std::endl;

                // save off ip values; offset to Shape Function
                double muIp = 0.0;
                const int offSetSF_face = ip*nodesPerFace;
                
                // Iterate through all nodes of a element to get the flux coefficient
                // contribution of each node to the current integration point
                for ( int ic = 0; ic < nodesPerFace; ++ic ) {
                    const double r = p_face_shape_function[offSetSF_face+ic];
                    muIp += r*p_diffFluxCoeff[ic];
                }

                double qDiff = 0.0;
                
                const int pos = boundaryNodeLocation[ip];
                
                // Iterate through all nodes of a element to get the face flux coefficient
                // contribution of each node to the current integration point
                for ( int ic = 0; ic < nodesPerElement; ++ic ) {
                    // diffusion
                    double lhsfacDiff = 0.0;
                    const int offSetDnDx = nDim*nodesPerElement*ip + ic*nDim;
                    // Iterate through all spatial dimensions
                    for ( int j = 0; j < nDim; ++j ) {
                        lhsfacDiff += -muIp*p_dndx[offSetDnDx+j]*areaVec[ip*nDim+j];
                    }
                    
                    std::cout << "currently at node " << connected_nodes[ic] << std::endl;
                    
                    double temperatureValue;
                    if (ws_is_boundary_node[ic]) {
                        temperatureValue = temperatureBip;
                        std::cout << "node is a boundary node, temperature: " << temperatureValue << std::endl;
                    }
                    else {
                        p_lhs[(pos*nodesPerElement)+ic] += lhsfacDiff;
                        temperatureValue = p_scalarQNp1[ic];
                        std::cout << "node is a internal node, temperature: " << temperatureValue << std::endl;
                    }
                    
                    

                    qDiff += lhsfacDiff*temperatureValue; 
                }

                p_rhs[pos] -= qDiff;
            }
            apply_coeff(connected_nodes, scratchIds, scratchVals, rhs, lhs, __FILE__);
            std::cout << "printing lhs: " << std::endl;
            for (int i=0; i<lhsSize; ++i) {
                std::cout << p_lhs[i] << ", ";
            }
            std::cout << std::endl;

            std::cout << "printing rhs: " << std::endl;
            for (int i=0; i<rhsSize; ++i) {
                std::cout << p_rhs[i] << ", ";
            }
            std::cout << std::endl;
            
            std::cout << "printing connected_nodes: " << std::endl;
            for (int i=0; i<num_nodes; ++i) {
                std::cout << connected_nodes[i] << ", ";
            }
            std::cout << std::endl;
        }
    }
}
