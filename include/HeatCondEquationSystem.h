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
class TpetraLinearSystem;
class ProjectedNodalGradientEquationSystem;

/** Specific implementation of the head conduction equation system.
 *
 * Derived class from EquationSystem
 * */
class HeatCondEquationSystem : public EquationSystem {
public:
    /** Initializes some variables and creates a linear solver and linear system*/
    HeatCondEquationSystem(EquationSystems & equationSystems);
    virtual ~HeatCondEquationSystem();
    
    /** Creates a new ProjectedNodalGradientEquationSystem
     *
     * @note Not needed for the heat equation
     *       but has to be removed safely
     */
    void manage_png(EquationSystems & eqSystems);
    
    /** Creates fields for node values
     *
     * Field for e.g. tempeperature, coordinates or 
     * properties like density
     */
    void register_nodal_fields(stk::mesh::Part *part);
    
    /** Creates fields for edge values
     *
     * Only relevant for edge based scheme
     * @note Can be removed but safely
     */
    void register_edge_fields(stk::mesh::Part *part);
    
    /** Creates fields for element values
     *
     * Only relevant for active adaptivity or overset mesh
     * @note Can be removed but safely
     */
    void register_element_fields(stk::mesh::Part *part, const stk::topology &theTopo);
    
    /** Creates solver algorithms
     *
     * Nodal gradient and solver algorithm. It differentiates between
     * consolidated and not consolidated solver algorithm.
     * @note Not sure what a consolidated solver alogirthm is
     */
    void register_interior_algorithm(stk::mesh::Part *part);
    
    /** Calls load method from parent class
     *
     * Reads parameters from the input file like name and max_iterations 
     */
    virtual void load(const YAML::Node & node);
    
    void register_wall_bc(stk::mesh::Part * part, const stk::topology & partTopo, const WallBoundaryConditionData & wallBCData);
    
    /** Outer solving loop for the heat equation
     *
     * Within the solving loop the method assemble_and_solve of the parent class
     * EquationSystem gets called
     */
    void solve_and_update();
    
    /** Compute projected nodal gradient
     *
     * @note Not needed for the heat equation
     *       but has to be removed safely
     */
    void compute_projected_nodal_gradient();
    
    /** Finishes initialization of solver algorithm and linear system*/
    void initialize();
    
    /** Deletes linear solver and linear system an creates new ones*/
    void reinitialize_linear_system();
    
    /** Copy fields to a new timestep?
     *
     * @note Seems like this isn't used anywhere
     *       Has to be checked carefully before
             the method can be removed
     */
    void predict_state();
    
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

    ScalarFieldType *density_;
    ScalarFieldType *specHeat_;
    ScalarFieldType *thermalCond_;
    
    VectorFieldType *edgeAreaVec_;
 
    AssembleNodalGradAlgorithmDriver * assembleNodalGradAlgDriver_;
    bool isInit_;
    ProjectedNodalGradientEquationSystem * projectedNodalGradEqs_;
};

#endif /* HEATCONDEQUATIONSYSTEM_H */

