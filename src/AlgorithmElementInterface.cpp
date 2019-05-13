/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "AlgorithmElementInterface.h"

#include <Realm.h>

AlgorithmElementInterface::AlgorithmElementInterface(Realm & realm, stk::mesh::PartVector & partVec, ScalarFieldType * scalarQ) :
    realm_(realm),
    partVec_(partVec),
    scalarQ_(scalarQ),
    bulk_data_(realm_.bulk_data()),
    meta_data_(realm_.meta_data()),
    nDim_(meta_data_.spatial_dimension())
{
    // nothing to do
}

AlgorithmElementInterface::~AlgorithmElementInterface() {
    // nothing to do
}

inline stk::mesh::BucketVector const & AlgorithmElementInterface::get_elem_buckets() {
    // define some common selectors
    stk::mesh::Selector s_locally_owned_union = meta_data_.locally_owned_part()
        & stk::mesh::selectUnion(partVec_) 
        & !(realm_.get_inactive_selector());
    
    return realm_.get_buckets( stk::topology::ELEMENT_RANK, s_locally_owned_union );
}

inline void AlgorithmElementInterface::bucket_pre_work(stk::mesh::Bucket & b) {
    // extract master element
    meSCS_ = MasterElementRepo::get_surface_master_element(b.topology());
    meSCV_ = MasterElementRepo::get_volume_master_element(b.topology());

    // extract master element specifics
    nodesPerElement_ = meSCS_->nodesPerElement_;
    numScsIp_ = meSCS_->numIntPoints_;
    lrscv_ = meSCS_->adjacentNodes();

    // resize some things; matrix related
    lhsSize_ = nodesPerElement_*nodesPerElement_;
    rhsSize_ = nodesPerElement_;
    lhs_.resize(lhsSize_);
    rhs_.resize(rhsSize_);
    scratchIds_.resize(rhsSize_);
    scratchVals_.resize(rhsSize_);
    connected_nodes_.resize(nodesPerElement_);

    // algorithm related
    ws_coordinates_.resize(nodesPerElement_*nDim_);
    ws_scalarQNp1_.resize(nodesPerElement_);
    ws_diffFluxCoeff_.resize(nodesPerElement_);
    ws_scs_areav_.resize(numScsIp_*nDim_);
    ws_dndx_.resize(nDim_*numScsIp_*nodesPerElement_);
    ws_deriv_.resize(nDim_*numScsIp_*nodesPerElement_);
    ws_det_j_.resize(numScsIp_);
    ws_shape_function_.resize(numScsIp_*nodesPerElement_);

    // pointer to lhs/rhs
    p_lhs_ = &lhs_[0];
    p_rhs_ = &rhs_[0];

    p_coordinates_ = &ws_coordinates_[0];

    p_scalarQNp1_ = &ws_scalarQNp1_[0];
    p_diffFluxCoeff_ = &ws_diffFluxCoeff_[0];
    p_scs_areav_ = &ws_scs_areav_[0];
    p_dndx_ = &ws_dndx_[0];
    p_shape_function_ = &ws_shape_function_[0];
    
    // extract shape function
    meSCS_->shape_fcn(&p_shape_function_[0]);
}

inline void AlgorithmElementInterface::element_pre_work(stk::mesh::Entity & elem, 
                                                        ScalarFieldType * diffFluxCoeff, 
                                                        VectorFieldType * coordinates, 
                                                        const bool shiftedGradOp) 
{
    ScalarFieldType & scalarQNp1   = scalarQ_->field_of_state(stk::mesh::StateNP1);
    
    // zero lhs/rhs
    for ( int p = 0; p < lhsSize_; ++p )
      p_lhs_[p] = 0.0;
    for ( int p = 0; p < rhsSize_; ++p )
      p_rhs_[p] = 0.0;

    stk::mesh::Entity const * node_rels = bulk_data_.begin_nodes(elem);
    int num_nodes = bulk_data_.num_nodes(elem);

    // sanity check on num nodes
    ThrowAssert( num_nodes == nodesPerElement_ );

    // Iterate through each node in element
    for ( int ni = 0; ni < num_nodes; ++ni ) {
        stk::mesh::Entity node = node_rels[ni];

        // set connected nodes
        connected_nodes_[ni] = node;

        // pointers to real data, get coordinates of the node
        const double * coords = stk::mesh::field_data(*coordinates, node );

        // gather scalars, get field value of the node
        p_scalarQNp1_[ni]    = *stk::mesh::field_data(scalarQNp1, node );
        p_diffFluxCoeff_[ni] = *stk::mesh::field_data(*diffFluxCoeff, node );

        // gather vectors, get coordinates for each dimension of the node
        const int niNdim = ni*nDim_;
        for ( int i=0; i < nDim_; ++i ) {
            p_coordinates_[niNdim+i] = coords[i];
        }
    }

    // compute geometry
    double scs_error = 0.0;
    meSCS_->determinant(1, &p_coordinates_[0], &p_scs_areav_[0], &scs_error);

    // compute dndx
    if ( shiftedGradOp )
        meSCS_->shifted_grad_op(1, &ws_coordinates_[0], &ws_dndx_[0], &ws_deriv_[0], &ws_det_j_[0], &scs_error);
    else
        meSCS_->grad_op(1, &ws_coordinates_[0], &ws_dndx_[0], &ws_deriv_[0], &ws_det_j_[0], &scs_error);
}

inline void AlgorithmElementInterface::ip_pre_work(int ip) {
    // left and right nodes for this ip
    node_left_ = lrscv_[2 * ip];
    node_right_ = lrscv_[2 * ip + 1];

    // corresponding matrix rows
    row_left_ = node_left_ * nodesPerElement_;
    row_right_ = node_right_ * nodesPerElement_;
}

inline void AlgorithmElementInterface::node_pre_work(int ip, int ic) {
    offSetDnDx_ = (nDim_ * nodesPerElement_ * ip) + (ic * nDim_);
}

inline double AlgorithmElementInterface::compute_muIP(int ip) {
    double muIp = 0.0;
    const int offSetSF = ip *nodesPerElement_;
                
    // Iterate through all nodes of a element to get the flux coefficient
    // contribution of each node to the current integration point
    for ( int ic = 0; ic < nodesPerElement_; ++ic ) {
        const double sf = getSFValue(offSetSF, ic);
        muIp += sf * p_diffFluxCoeff_[ic];
    }
    return muIp;
}

inline double AlgorithmElementInterface::getSFValue(int offSetSF, int ic) {
    return p_shape_function_[offSetSF + ic];
}

inline double AlgorithmElementInterface::getSFDeriv(int dim) {
    return p_dndx_[offSetDnDx_ + dim];
}

inline double AlgorithmElementInterface::getFaceDet(int ip, int dim) {
    return p_scs_areav_[ip * nDim_ + dim];
}

inline void AlgorithmElementInterface::update_local_lhs(double lhsfacDiff, int ic) {
    // lhs; il then ir
    p_lhs_[row_left_+ic] += lhsfacDiff;
    p_lhs_[row_right_+ic] -= lhsfacDiff;
}

inline void AlgorithmElementInterface::update_local_rhs(double qDiff) {
    // rhs; il then ir
    p_rhs_[node_left_] -= qDiff;
    p_rhs_[node_right_] += qDiff;
}

inline double AlgorithmElementInterface::getScalarQNP1(int ic) {
    return p_scalarQNp1_[ic];
}

inline void AlgorithmElementInterface::update_global_lhs_rhs() {
//    apply_coeff(connected_nodes_, scratchIds_, scratchVals_, rhs_, lhs_, __FILE__);
}