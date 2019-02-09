/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef EQUATIONSYSTEMS_H
#define EQUATIONSYSTEMS_H

typedef std::vector<EquationSystem *> EquationSystemVector;

//! Stores one or multiple equation systems (objects of the class EqationSystem)
class EquationSystems {
public:
    EquationSystems();
    ~EquationSystems();
    void load(const YAML::Node & node);
    void initialize();
    void register_nodal_fields(const std::vector<std::string> targetNames);
    void register_edge_fields(const std::vector<std::string> targetNames);
    void register_element_fields(const std::vector<std::string> targetNames);
    void register_interior_algorithm(const std::vector<std::string> targetNames);
    void register_wall_bc(const std::string targetName, const WallBoundaryConditionData & wallBCData);
    
    Realm &realm_;
    std::string name_;
    int maxIterations_;
    EquationSystemVector equationSystemVector_;
};

#endif /* EQUATIONSYSTEMS_H */

