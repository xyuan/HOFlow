/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "CopyFieldAlgorithm.h"

#include <Realm.h>
#include <FieldTypeDef.h>

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
// CopyFieldAlgorithm - copy fields from one to another on a set of select
//                      parts; begin/end can be the size of the field as well
//                      should it be operating on integration point data
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
CopyFieldAlgorithm::CopyFieldAlgorithm(
  Realm &realm,
  stk::mesh::Part *part,
  stk::mesh::FieldBase * fromField,
  stk::mesh::FieldBase * toField,
  const unsigned beginPos,
  const unsigned endPos,
  const stk::mesh::EntityRank entityRank)
  : Algorithm(realm, part),
    fromField_(fromField),
    toField_(toField),
    beginPos_(beginPos),
    endPos_(endPos),
    entityRank_(entityRank)
{
  // does nothing
}

//--------------------------------------------------------------------------
//-------- execute ---------------------------------------------------------
//--------------------------------------------------------------------------
void
CopyFieldAlgorithm::execute()
{
  stk::mesh::Selector selector = stk::mesh::selectUnion(partVec_);
  stk::mesh::BucketVector const& buckets =
    realm_.get_buckets( entityRank_, selector );

  for ( stk::mesh::BucketVector::const_iterator ib = buckets.begin();
        ib != buckets.end() ; ++ib ) {
    stk::mesh::Bucket & b = **ib ;
    const stk::mesh::Bucket::size_type length   = b.size();

    double * fromFieldData = (double*)stk::mesh::field_data(*fromField_, b);
    double * toFieldData = (double*)stk::mesh::field_data(*toField_, b);

    for(stk::mesh::Bucket::size_type k=0; k < length; ++k) {
      const int offSet = k*endPos_;
      for(unsigned i=beginPos_; i < endPos_; ++i) {
        toFieldData[offSet+i] = fromFieldData[offSet+i];
      }
    }
  }
}