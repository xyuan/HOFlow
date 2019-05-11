/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ALGORITHMELEMENTINFOSUPPLIER_H
#define ALGORITHMELEMENTINFOSUPPLIER_H

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Part.hpp>

class AlgorithmElementInfoSupplier {
public:
    AlgorithmElementInfoSupplier(stk::mesh::BulkData & bulk_data, stk::mesh::MetaData & meta_data);
    ~AlgorithmElementInfoSupplier();
    
    void initialize_bucket(stk::mesh::Bucket & b);
    stk::mesh::Bucket::size_type get_bucket_length();
    
    stk::mesh::BulkData & bulk_data_;
    stk::mesh::MetaData & meta_data_;
    const int nDim_;
    stk::mesh::Bucket & bucket_;
    stk::mesh::Bucket::size_type bucketLength_;
    MasterElement * meSCS_;
    MasterElement * meSCV_;
    
    // space for LHS/RHS; nodesPerElem*nodesPerElem* and nodesPerElem
    std::vector<double> lhs_;
    std::vector<double> rhs_;
    std::vector<int> scratchIds_;
    std::vector<double> scratchVals_;
    std::vector<stk::mesh::Entity> connected_nodes_;
    
};

#endif /* ALGORITHMELEMENTINFOSUPPLIER_H */

