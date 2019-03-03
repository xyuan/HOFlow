/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HEATCONDEQUATIONSYSTEM_H
#define HEATCONDEQUATIONSYSTEM_H

#include <EquationSystem.h>
#include <FieldTypeDef.h>
#include <HOFlowParsing.h>
#include <stk_mesh/base/FieldBase.hpp>
#include <stk_mesh/base/CoordinateSystems.hpp>

struct stk::topology;

class Realm;
class AssembleNodalGradAlgorithmDriver;
class AlgorithmDriver;
class EquationSystems;
class ProjectedNodalGradientEquationSystem;

class HeatCondEquationSystem : public EquationSystem {
public:
    HeatCondEquationSystem(EquationSystems & equationSystems);
    virtual ~HeatCondEquationSystem();
    void manage_png(EquationSystems & eqSystems);
    void register_nodal_fields(stk::mesh::Part *part);
    void register_edge_fields(stk::mesh::Part *part);
    void register_element_fields(stk::mesh::Part *part, const stk::topology &theTopo);
    void register_interior_algorithm(stk::mesh::Part *part);
    virtual void load(const YAML::Node & node);
    void register_wall_bc(stk::mesh::Part * part, const stk::topology & partTopo, const WallBoundaryConditionData & wallBCData);
    void solve_and_update();
    void compute_projected_nodal_gradient();
    void initialize();
    void reinitialize_linear_system();
    void predict_state();
    virtual void load(const YAML::Node & node);
    
    ProjectedNodalGradientEquationSystem * projectedNodalGradEqs_;
    
    // allow equation system to manage a projected nodal gradient
    const bool managePNG_;

    ScalarFieldType *temperature_;
    VectorFieldType *dtdx_;
    ScalarFieldType *tTmp_;
    ScalarFieldType *dualNodalVolume_;
    VectorFieldType *coordinates_;
    ScalarFieldType *exact_temperature_;
    VectorFieldType *exact_dtdx_;
    VectorFieldType *exact_laplacian_;

    ScalarFieldType *thermalCond_;
    
    VectorFieldType *edgeAreaVec_;
 
    AssembleNodalGradAlgorithmDriver * assembleNodalGradAlgDriver_;
    bool isInit_;
    ProjectedNodalGradientEquationSystem * projectedNodalGradEqs_;
};

#endif /* HEATCONDEQUATIONSYSTEM_H */

