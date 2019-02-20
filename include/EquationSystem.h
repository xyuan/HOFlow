 /*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef EQUATIONSYSTEM_H
#define EQUATIONSYSTEM_H

#include <HOFlowParsing.h>
#include <Realm.h>
#include <string>

class EquationSystems;
struct stk::topology;
class stk::mesh::FieldBase;
class stk::mesh::Part;

typedef std::vector<stk::mesh::Part *> PartVector;

//! Stores a equation system specified in the input file. Base class representation of a PDE

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
    ~EquationSystem();
    
    virtual void register_wall_bc(stk::mesh::Part *part, 
                                  const stk::topology & theTopo, 
                                  const WallBoundaryConditionData & wallBCData) 
    {}
    
    // base class with desired default no-op
    virtual void register_nodal_fields(stk::mesh::Part * part) {}

    virtual void register_edge_fields(stk::mesh::Part * part) {}

    virtual void register_element_fields(stk::mesh::Part * part, const stk::topology & theTopo) {}
    
    virtual void register_wall_bc(stk::mesh::Part * part, const stk::topology & theTopo, const WallBoundaryConditionData & wallBCData) {}
    
    virtual void load(const YAML::Node & node);

    Simulation * root();
    EquationSystems * parent()
    
    EquationSystems & equationSystems_;
    Realm & realm_;
    std::string name_;
    std::string userSuppliedName_;
    const std::string eqnTypeName_;
    int maxIterations_;
    double convergenceTolerance_;
};

#endif /* EQUATIONSYSTEM_H */

