/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "ConstantBCAuxFunctionAlgorithm.h"

#include <AuxFunction.h>
#include <FieldTypeDef.h>
#include <Realm.h>
#include <Simulation.h>

#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Selector.hpp>

ConstantBCAuxFunctionAlgorithm::ConstantBCAuxFunctionAlgorithm(Realm & realm,
                                                               stk::mesh::Part * part,
                                                               ScalarFieldType * field,
                                                               stk::mesh::EntityRank entityRank,
                                                               double bc_value) :
    Algorithm(realm, part),
    field_(field),
    entityRank_(entityRank),
    bcValue_(bc_value)
{
    // nothing to do
}

ConstantBCAuxFunctionAlgorithm::~ConstantBCAuxFunctionAlgorithm() {
    // nothing to do
}

void ConstantBCAuxFunctionAlgorithm::execute() {
    // make sure that partVec_ is size one
    ThrowAssert( partVec_.size() == 1 );

    stk::mesh::MetaData & meta_data = realm_.meta_data();

    stk::mesh::Selector selector = stk::mesh::selectUnion(partVec_) & stk::mesh::selectField(*field_);
    stk::mesh::BucketVector const & buckets = realm_.get_buckets( entityRank_, selector );

    for ( stk::mesh::BucketVector::const_iterator ib = buckets.begin(); ib != buckets.end() ; ++ib ) {
        stk::mesh::Bucket & b = **ib ;
        
        for ( size_t bc_index = 0; bc_index < b.size(); ++bc_index ) {
            double * value = stk::mesh::field_data<ScalarFieldType>(*field_, b[bc_index]);
            *value = bcValue_;
        }
    }    
}