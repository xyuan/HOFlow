/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AlgorithmElementInfoSupplier.h"

AlgorithmElementInfoSupplier::AlgorithmElementInfoSupplier(stk::mesh::BulkData & bulk_data, stk::mesh::MetaData & meta_data) :
    bulk_data_(bulk_data),
    meta_data_(meta_data)
{
    nDim_ = meta_data_.spatial_dimension();
}

AlgorithmElementInfoSupplier::~AlgorithmElementInfoSupplier() {
}

stk::mesh::Bucket::size_type get_bucket_length() {
    return bucketLength_;
}

void AlgorithmElementInfoSupplier::initialize_bucket(stk::mesh::Bucket & b) {
    bucket_ = b;
    bucketLength_ = bucket_.size();
    meSCS_ = MasterElementRepo::get_surface_master_element(bucket_.topology());
    meSCV_ = MasterElementRepo::get_volume_master_element(bucket_.topology());
}