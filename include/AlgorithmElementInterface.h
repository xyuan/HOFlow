/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ALGORITHMELEMENTINTERFACE_H
#define ALGORITHMELEMENTINTERFACE_H

#include <FieldTypeDef.h>
#include <cfdVector.h>
#include <vector>

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Part.hpp>
#include <stk_mesh/base/Entity.hpp>

class Realm;
class MasterElement;

typedef std::vector<stk::mesh::Part *> PartVector;

class AlgorithmElementInterface {
public:
    AlgorithmElementInterface(Realm & realm, stk::mesh::PartVector & partVec, ScalarFieldType * scalarQ);
    ~AlgorithmElementInterface();
    
    /** Methods to do some pre-computations
     * Execute in the following order:
     * 1. bucket_pre_work()
     * 2. element_pre_work()
     * 3. ip_pre_work()
     * 4. node_pre_work()
     */
    
    void bucket_pre_work(stk::mesh::Bucket & b);
    void element_pre_work(stk::mesh::Entity & elem, 
                          ScalarFieldType * diffFluxCoeff, 
                          VectorFieldType * coordinates, 
                          const bool shiftedGradOp);
    void ip_pre_work(int ip);
    void node_pre_work(int ip, int ic);
    
    // Methods to get some values
    double compute_muIP(int ip);
    double getSFValue(int offSetSF, int ic);
    double getSFDeriv(int dim);
    double getFaceDet(int ip, int dim);
    double getScalarQNP1(int ic);
    cfdVector * get_area_normal_vector(int ip);
    cfdVector * get_derived_shape_function(int ip, int ic);
    
    void update_local_lhs(double lhsfacDiff, int ic);
    void update_local_rhs(double qDiff);
    void update_global_lhs_rhs();
    
    cfdVector * derivSF_;
    cfdVector * areaNormVec_;
    
    Realm & realm_;
    PartVector partVec_;
    ScalarFieldType * scalarQ_;
    stk::mesh::BulkData & bulk_data_;
    stk::mesh::MetaData & meta_data_;
    
    const int nDim_;
    
    std::vector<double> lhs_;
    std::vector<double> rhs_;
    std::vector<int> scratchIds_;
    std::vector<double> scratchVals_;
    std::vector<stk::mesh::Entity> connected_nodes_;

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

