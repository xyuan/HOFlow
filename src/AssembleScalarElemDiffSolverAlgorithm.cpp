/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AssembleScalarElemDiffSolverAlgorithm.h"

#include <EquationSystem.h>
#include <SolverAlgorithm.h>
#include <AlgorithmElementInterface.h>
#include <cfdVector.h>

#include <FieldTypeDef.h>
#include <LinearSystem.h>
#include <Realm.h>
#include <SupplementalAlgorithm.h>
#include <TimeIntegrator.h>
#include <master_element/MasterElement.h>
#include <iostream>

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Part.hpp>

//==========================================================================
// Class Definition
//==========================================================================
// AssembleScalarElemDiffSolverAlgorithm - add LHS/RHS for scalar diff
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
AssembleScalarElemDiffSolverAlgorithm::AssembleScalarElemDiffSolverAlgorithm(Realm & realm,
                                                                             stk::mesh::Part * part,
                                                                             EquationSystem * eqSystem,
                                                                             ScalarFieldType * scalarQ,
                                                                             VectorFieldType * dqdx,
                                                                             ScalarFieldType * diffFluxCoeff) : 
        SolverAlgorithm(realm, part, eqSystem),
        scalarQ_(scalarQ),
        diffFluxCoeff_(diffFluxCoeff),
        shiftedGradOp_(realm.get_shifted_grad_op(scalarQ_->name()))
{
    // save off fields
    stk::mesh::MetaData & meta_data = realm_.meta_data();
    coordinates_ = meta_data.get_field<VectorFieldType>(stk::topology::NODE_RANK, realm_.get_coordinates_name());
}

//--------------------------------------------------------------------------
//-------- initialize_connectivity -----------------------------------------
//--------------------------------------------------------------------------
void AssembleScalarElemDiffSolverAlgorithm::initialize_connectivity() {
    eqSystem_->linsys_->buildElemToNodeGraph(partVec_);
}

//--------------------------------------------------------------------------
//-------- execute ---------------------------------------------------------
//--------------------------------------------------------------------------
void AssembleScalarElemDiffSolverAlgorithm::execute() {
    // Create AlgorithmElementInterface object to have easy 
    // access to the necessary data
    AlgorithmElementInterface intf(realm_, partVec_, scalarQ_);

    // supplemental algorithm setup
    const size_t supplementalAlgSize = supplementalAlg_.size();
    for ( size_t i = 0; i < supplementalAlgSize; ++i )
        supplementalAlg_[i]->setup();
    
    // Get buckets
    stk::mesh::Selector s_locally_owned_union = realm_.meta_data().locally_owned_part()
                                                & stk::mesh::selectUnion(partVec_) 
                                                & !(realm_.get_inactive_selector());

    stk::mesh::BucketVector const & elem_buckets = realm_.get_buckets( stk::topology::ELEMENT_RANK, s_locally_owned_union );
    std::cout << std::endl;
    std::cout << "Internal Assembling" << std::endl;
    
    // Iterate through all buckets
    for ( stk::mesh::BucketVector::const_iterator ib = elem_buckets.begin(); ib != elem_buckets.end() ; ++ib ) 
    {
        stk::mesh::Bucket & b = **ib;
        const stk::mesh::Bucket::size_type length = b.size();
        intf.bucket_pre_work(b);
        
        const int nDim = intf.nDim_;
        const int numScsIp = intf.numScsIp_;

        // resize possible supplemental element alg
        MasterElement * meSCS = intf.meSCS_;
        MasterElement * meSCV = intf.meSCV_;
        for ( size_t i = 0; i < supplementalAlgSize; ++i )
            supplementalAlg_[i]->elem_resize(meSCS, meSCV);
        
        //--------------------------------------------------------
        // Iterate through each element in bucket
        for ( size_t k = 0 ; k < length ; ++k )
        {
            // get elem
            stk::mesh::Entity elem = b[k];
            intf.element_pre_work(elem, diffFluxCoeff_, coordinates_, shiftedGradOp_);
            
            std::cout << "/////////// NEW Element ///////////" << std::endl;
            
            //----------------------------------------------------
            // Iterate through all integration points
            for ( int ip = 0; ip < numScsIp; ++ip ) 
            {
                intf.ip_pre_work(ip);
                
//                std::cout << "######## NEW IP ##########" << std::endl;
                
                const double muIp = intf.compute_muIP(ip);
//                cfdVector areaNormVec = intf.get_area_normal_vector(ip);
                double qDiff = 0.0;
                
                //------------------------------------------------
                // Iterate through all nodes of a element to get the face flux coefficient
                // contribution of each node to the current integration point
                int nodesPerElement = intf.nodesPerElement_;
                for ( int ic = 0; ic < nodesPerElement; ++ic ) 
                {
                    intf.node_pre_work(ip, ic);
//                    cfdVector dndx = intf.get_derived_shape_function(ip, ic);
                    
                    // Coefficient computation
//                    cfdVector lhsfacDiff = -muIp * areaNormVec;
//                    const double lhsfacDiff = -muIp * dndx & areaNormVec;
                    
                    
                    double lhsfacDiff = 0.0;
            
                    // Iterate through all spatial dimensions
                    for ( int j = 0; j < nDim; ++j )
                    {
                        const double dndx = intf.getSFDeriv(j);
                        const double areav = intf.getFaceDet(ip, j);
                        lhsfacDiff += -muIp * dndx * areav;
                    }
                    const double scalarQNP1 = intf.getScalarQNP1(ic);
//                    std::cout << "------- new node --------" << std::endl;
//                    std::cout << "lhsfacDiff " << ic << " = " << lhsfacDiff << std::endl;
//                    std::cout << "scalarQNP1 " << ic << " = " << scalarQNP1 << std::endl;
//                    std::cout << "qDiff " << ic << " = " << lhsfacDiff * scalarQNP1 << std::endl;
                    qDiff += lhsfacDiff * scalarQNP1;
                    intf.update_local_lhs(lhsfacDiff, ic);
                }
                intf.update_local_rhs(qDiff);     
            }

            // call supplemental
            for ( size_t i = 0; i < supplementalAlgSize; ++i )
                supplementalAlg_[i]->elem_execute(&intf.lhs_[0], &intf.rhs_[0], elem, meSCS, meSCV);
            
            apply_coeff(intf.connected_nodes_, intf.scratchIds_, intf.scratchVals_, intf.rhs_, intf.lhs_, __FILE__);
            std::cout << "printing rhs: " << std::endl;
            for (int i=0; i<3; ++i) {
                std::cout << intf.rhs_[i] << ", ";
            }
            std::cout << std::endl;
        }
    }
}