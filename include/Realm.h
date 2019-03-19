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
class TpetraLinearSystem;
class Realms;
class Simulation;
class PropertyEvaluator;
class SolutionOptions;
class MasterElement;
class TensorProductQuadratureRule;
class LagrangeBasis;
class ComputeGeometryAlgorithmDriver;


//! Stores information and methods for a specific computational domain
class Realm {
public:
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
    void populate_boundary_data();
    const stk::mesh::PartVector & get_slave_part_vector();
    
    virtual void evaluate_properties();
    std::string get_coordinates_name();
    bool has_non_matching_boundary_face_alg() const;
    bool get_shifted_grad_op(const std::string dofname);
    
    // consistent mass matrix for projected nodal gradient
    bool get_consistent_mass_matrix_png(const std::string dofname);
    int number_of_states();
    std::string name();
    stk::mesh::BucketVector const & get_buckets(stk::mesh::EntityRank rank,
                                                const stk::mesh::Selector & selector ,
                                                bool get_all = false) const;
    
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
    
    // allow aura to be optional
    bool activateAura_;
    
    size_t inputMeshIdx_;
    const YAML::Node & node_;
    
    bool isFinalOuterIter_{false};
    
    std::string physics_part_name(std::string) const;
    std::vector<std::string> physics_part_names(std::vector<std::string>) const;
    std::map<PropertyIdentifier, ScalarFieldType *> propertyMap_;
    std::vector<Algorithm *> initCondAlg_;
    std::vector<Algorithm *> propertyAlg_;
};

#endif /* REALM_H */

