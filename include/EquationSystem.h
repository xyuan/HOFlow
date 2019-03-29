/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef EQUATIONSYSTEM_H
#define EQUATIONSYSTEM_H

#include <HOFlowParsing.h>
#include <EquationSystems.h>
#include <Realm.h>
#include <vector>
#include <string>

class Algorithm;
class AlgorithmDriver;
class AuxFunctionAlgorithm;
class SolverAlgorithmDriver;
class InitialCondition;
class EquationSystems;
class LinearSystem;

struct stk::topology;
class stk::mesh::FieldBase;
class stk::mesh::Part;

typedef std::vector<stk::mesh::Part *> PartVector;

//! Represents a equation system specified in the input file. Base class representation of a PDE

//! EquationSystem defines the API supported by allconcrete implementations
//! of PDEs for performing the following actions:
//!
//! - Register computational fields
//! - Register computational algorithms for interior domain and boundary conditions
//! - Manage solve and update fo the PDE for a given timestep
class EquationSystem {
public:
    EquationSystem(EquationSystems & eqSystems,
                   const std::string name = "no_name",
                   const std::string eqnTypeName = "no_eqn_type_name");
    virtual ~EquationSystem();
    void set_nodal_gradient(const std::string & dofName);
    virtual void initial_work();
    virtual void populate_derived_quantities() {}
    
    // base class with desired default no-op
    virtual void initialize() {}
    virtual void register_nodal_fields(stk::mesh::Part * part) {}
    virtual void register_edge_fields(stk::mesh::Part * part) {}
    virtual void register_element_fields(stk::mesh::Part * part, const stk::topology & theTopo) {}
    virtual void register_interior_algorithm(stk::mesh::Part * part) {}
    virtual void register_wall_bc(stk::mesh::Part * part, const stk::topology & theTopo, const WallBoundaryConditionData & wallBCData) {}
    
    /** Reads parameters from the input file like name and max_iterations*/
    virtual void load(const YAML::Node & node);
    virtual bool system_is_converged();
    virtual void assemble_and_solve(stk::mesh::FieldBase *deltaSolution);
    virtual void solve_and_update() {}
    UserDataType get_bc_data_type(const UserData &, std::string & name);
    virtual void evaluate_properties();
    virtual void pre_iter_work();
    virtual void post_iter_work();
    virtual void post_iter_work_dep() {}
    
    virtual void predict_state() {}
    virtual void provide_output() {}
    virtual void pre_timestep_work();
    virtual void reinitialize_linear_system() {}
    virtual void dump_eq_time();
    virtual double provide_scaled_norm();
    virtual double provide_norm();
    virtual double provide_norm_increment();
    
    Simulation * root();
    EquationSystems * parent();

    void report_invalid_supp_alg_names();
    void report_built_supp_alg_names();
    bool supp_alg_is_requested(std::string name);
    bool supp_alg_is_requested(std::vector<std::string>);

    bool nodal_src_is_requested();
    
    void update_iteration_statistics(const int & iters);

    bool bc_data_specified(const UserData&, std::string &name);
    
    virtual void post_converged_work() {}
    
    std::vector<AuxFunctionAlgorithm *> bcDataAlg_;
    std::vector<Algorithm *> bcDataMapAlg_;
    std::vector<Algorithm *> copyStateAlg_;
    LinearSystem * linsys_;
    
    EquationSystems & equationSystems_;
    Realm & realm_;
    std::string name_;
    std::string userSuppliedName_;
    const std::string eqnTypeName_;
    int maxIterations_;
    double convergenceTolerance_;
    
    SolverAlgorithmDriver * solverAlgDriver_;
   
    double timerAssemble_;
    double timerLoadComplete_;
    double timerSolve_;
    double timerMisc_;
    double timerInit_;
    double timerPrecond_;
    double avgLinearIterations_;
    double maxLinearIterations_;
    double minLinearIterations_;
    int nonLinearIterationCount_;
    bool reportLinearIterations_;
    bool firstTimeStepSolve_;
    bool edgeNodalGradient_;
    
    size_t num_graph_entries_;

    // vector of property algorithms
    std::vector<Algorithm *> propertyAlg_;

    /// List of tasks to be performed before each EquationSystem::solve_and_update
    std::vector<AlgorithmDriver *> preIterAlgDriver_;

    /// List of tasks to be performed after each EquationSystem::solve_and_update
    std::vector<AlgorithmDriver*> postIterAlgDriver_;
};

#endif /* EQUATIONSYSTEM_H */

