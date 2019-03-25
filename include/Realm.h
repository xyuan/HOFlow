/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef REALM_H
#define REALM_H

#include <stk_util/util/ParameterList.hpp>
#include <yaml-cpp/yaml.h>
#include <Enums.h>
#include <FieldTypeDef.h>
#include <EquationSystems.h>
#include <BoundaryConditions.h>
#include <InitialConditions.h>
#include <MaterialProperties.h>
#include <SolutionOptions.h>
#include <string>
#include <vector>
#include <map>

class YAML::Node;
class stk::mesh::Part;
class stk::io::StkMeshIoBroker;
class Algorithm;
class AlgorithmDriver;
class AuxFunctionAlgorithm;
class EquationSystem;
class EquationSystems;
class OutputInfo;
class TpetraLinearSystem;
class Realms;
class Simulation;
class PropertyEvaluator;
class SolutionOptions;
class TimeIntegrator;
class MasterElement;
class TensorProductQuadratureRule;
class LagrangeBasis;
class ComputeGeometryAlgorithmDriver;


//! Stores information and methods for a specific computational domain
class Realm {
public:
    typedef size_t SizeType;
    Realm(Realms & realms, const YAML::Node & node);
    virtual ~Realm();
    virtual void breadboard();
    virtual void load(const YAML::Node & node);
    virtual void initialize();
    Simulation * root() const;
    Simulation * root();
    Realms * parent() const;
    Realms * parent();
    void create_mesh();
    void setup_nodal_fields();
    void setup_edge_fields();
    void setup_element_fields();
    void setup_interior_algorithms();
    void setup_bc();
    void setup_initial_conditions();
    void setup_property();
    void augment_property_map(PropertyIdentifier propID, ScalarFieldType *theField);
    void output_converged_results();
    void commit();
    void create_output_mesh();
    void input_variables_from_mesh();
    void augment_output_variable_list(const std::string fieldName);
    void augment_restart_variable_list(std::string restartFieldName);
    void register_nodal_fields(stk::mesh::Part *part);
    void register_interior_algorithm(stk::mesh::Part *part);
    void register_wall_bc(stk::mesh::Part *part, const stk::topology &theTopo);
    void provide_output();
    void set_global_id();
    void check_job(bool get_node_count);
    void dump_simulation_time();
    double provide_mean_norm();
    void provide_entity_count();
    void compute_geometry();
    const stk::mesh::PartVector & get_slave_part_vector();
    
    virtual void populate_initial_condition();
    virtual void populate_boundary_data();
    virtual void boundary_data_to_state_data();
    virtual double populate_variables_from_input(const double currentTime);
    virtual void populate_external_variables_from_input(const double currentTime) {}
    virtual void populate_derived_quantities();
    virtual void evaluate_properties();
    virtual double compute_adaptive_time_step();
    virtual void swap_states();
    virtual void predict_state();
    virtual void pre_timestep_work();
    virtual void output_banner();
    virtual void advance_time_step();
    virtual bool active_time_step();
    virtual void initial_work();
  
    std::string get_coordinates_name();
    bool has_non_matching_boundary_face_alg() const;
    bool get_shifted_grad_op(const std::string dofname);
    
    bool get_activate_memory_diagnostic();
    void provide_memory_summary();
    
    // consistent mass matrix for projected nodal gradient
    bool get_consistent_mass_matrix_png(const std::string dofname);
    int number_of_states();
    std::string name();
    stk::mesh::BucketVector const & get_buckets(stk::mesh::EntityRank rank,
                                                const stk::mesh::Selector & selector ,
                                                bool get_all = false) const;
    
    void post_converged_work();
    
    // time information; calls through timeIntegrator
    double get_current_time();
    double get_time_step();
    double get_gamma1();
    double get_gamma2();
    double get_gamma3();
    int get_time_step_count() const;
    double get_time_step_from_file();
    bool get_is_fixed_time_step();
    bool get_is_terminate_based_on_time();
    double get_total_sim_time();
    int get_max_time_step_count();
    
    std::string convert_bytes(double bytes);
    
    // get aura, bulk and meta data
    bool get_activate_aura();
    stk::mesh::BulkData & bulk_data();
    const stk::mesh::BulkData & bulk_data() const;
    stk::mesh::MetaData & meta_data();
    const stk::mesh::MetaData & meta_data() const;
    
    // inactive part
    stk::mesh::Selector get_inactive_selector();
    
    // mesh parts for all interior domains
    stk::mesh::PartVector interiorPartVec_;
    
    // push back equation to equation systems vector
    void push_equation_to_systems(EquationSystem *eqSystem);
    
    // provide all of the physics target names
    const std::vector<std::string> & get_physics_target_names();
    
    TimeIntegrator * timeIntegrator_;
    double wallTimeStart_;
    
    SizeType nodeCount_;
    bool estimateMemoryOnly_;
    double availableMemoryPerCoreGB_;
    double timerCreateMesh_;
    double timerPopulateMesh_;
    double timerPopulateFieldData_;
    double timerOutputFields_;
    double timerCreateEdges_;
    double timerNonconformal_;
    double timerInitializeEqs_;
    double timerPropertyEval_;
    double timerAdapt_;
    double timerTransferSearch_;
    double timerTransferExecute_;
    double timerSkinMesh_;
    double timerPromoteMesh_;
    double timerSortExposedFace_;
    
    int currentNonlinearIteration_;
    OutputInfo * outputInfo_;
    
    stk::mesh::PartVector emptyPartVector_;
    stk::mesh::PartVector bcPartVec_;
    
    Realms & realms_;
    std::string name_;
    std::string type_;
    std::string inputDBName_;
    unsigned spatialDimension_;
    int solveFrequency_;
    stk::mesh::MetaData *metaData_;
    stk::mesh::BulkData *bulkData_;
    stk::io::StkMeshIoBroker *ioBroker_;
    double l2Scaling_;
    
    size_t resultsFileIndex_;
    
    // part for all exposed surfaces in the mesh
    stk::mesh::Part *exposedBoundaryPart_;
    
    // hoflow field data
    GlobalIdFieldType *hoflowGlobalId_;
    
    // algorithm drivers managed by region
    ComputeGeometryAlgorithmDriver *computeGeometryAlgDriver_;
    
    BoundaryConditions boundaryConditions_;
    InitialConditions initialConditions_;
    MaterialProperties materialProperties_;

    EquationSystems equationSystems_;
    SolutionOptions *solutionOptions_;
    
    bool restarted_simulation();
    bool support_inconsistent_restart();
    
    // element promotion options
    bool doPromotion_; // conto
    unsigned promotionOrder_;
    
    // allow detailed output (memory) to be provided
    bool activateMemoryDiagnostic_;
    
    // allow aura to be optional
    bool activateAura_;
    
    size_t inputMeshIdx_;
    const YAML::Node & node_;
    
    bool isFinalOuterIter_{false};
    
    // cheack that all exposed surfaces have a bc applied
    bool checkForMissingBcs_;

    // check if there are negative Jacobians
    bool checkJacobians_;

    // some post processing of entity counts
    bool provideEntityCount_;
    
    std::string physics_part_name(std::string) const;
    std::vector<std::string> physics_part_names(std::vector<std::string>) const;
    std::map<PropertyIdentifier, ScalarFieldType *> propertyMap_;
    std::vector<Algorithm *> initCondAlg_;
    std::vector<Algorithm *> propertyAlg_;
};

#endif /* REALM_H */

