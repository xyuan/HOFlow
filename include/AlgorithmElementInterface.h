/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ALGORITHMELEMENTINTERFACE_H
#define ALGORITHMELEMENTINTERFACE_H

#include <master_element/MasterElement.h>
#include<FieldTypeDef.h>
#include <Realm.h>

#include <vector>

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Part.hpp>

class stk::mesh::Part;
class Realm;

typedef std::vector<stk::mesh::Part *> PartVector;

class AlgorithmElementInterface {
public:
    AlgorithmElementInterface(Realm & realm, stk::mesh::PartVector & partVec);
    ~AlgorithmElementInterface();
    stk::mesh::BucketVector const & get_elem_buckets();
    void bucket_pre_work(stk::mesh::Bucket & b);
    void element_pre_work(stk::mesh::Entity & elem, ScalarFieldType * scalarQ, ScalarFieldType * diffFluxCoeff, VectorFieldType * coordinates, const bool shiftedGradOp);
    void ip_pre_work(int ip);
    void node_pre_work(int ip, int ic);
    double compute_muIP(int ip);
    double getSFValue(int offSetSF, int ic);
    double getSFDeriv(int dim);
    double getFaceDet(int ip, int dim);
    double getScalarQNP1(int ic);
    void update_local_lhs(double lhsfacDiff, int ic);
    void update_local_rhs(double qDiff);
    void update_global_lhs_rhs();
    
    Realm & realm_;
    PartVector partVec_;
    stk::mesh::BulkData & bulk_data_;
    stk::mesh::MetaData & meta_data_;
    
    const int nDim_;
    
    std::vector<double> lhs_;
    std::vector<double> rhs_;
    std::vector<int> scratchIds_;
    std::vector<double> scratchVals_;
    std::vector<stk::mesh::Entity> connected_nodes_;
    ScalarFieldType & scalarQNp1_;

    // nodal fields to gather
    std::vector<double> ws_coordinates_;
    std::vector<double> ws_scalarQNp1_;
    std::vector<double> ws_diffFluxCoeff_;

    // geometry related to populate
    std::vector<double> ws_scs_areav_;
    std::vector<double> ws_dndx_;
    std::vector<double> ws_deriv_;
    std::vector<double> ws_det_j_;
    std::vector<double> ws_shape_function_;
    
    // current master element
    MasterElement * meSCS_;
    MasterElement * meSCV_;
    
    int nodesPerElement_;
    int numScsIp_; // number of surface integration points
    const int * lrscv_; // adjacent nodes

    // resize some things; matrix related
    int lhsSize_; // local lhs size
    int rhsSize_; // local rhs size
    
    // pointer to lhs/rhs
    double * p_lhs_;
    double * p_rhs_;

    double * p_coordinates_;
    double * p_scalarQNp1_;
    double * p_diffFluxCoeff_;
    double * p_scs_areav_;
    double * p_dndx_;
    double * p_shape_function_;
    
    // left and right nodes for current ip
    int node_left_;
    int node_right_;

    // corresponding matrix rows for current ip
    int row_left_;
    int row_right_;
    
    int offSetDnDx_;
};

#endif /* ALGORITHMELEMENTINTERFACE_H */

