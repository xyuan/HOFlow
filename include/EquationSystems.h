/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef EQUATIONSYSTEMS_H
#define EQUATIONSYSTEMS_H

#include <vector>
#include <map>
#include <string>
#include <HOFlowParsing.h>
#include <yaml-cpp/yaml.h>
#include <Enums.h>

// stk_util
#include <stk_util/parallel/Parallel.hpp>
#include <stk_util/environment/WallTime.hpp>
#include <stk_util/environment/perf_util.hpp>
#include <stk_util/environment/FileUtils.hpp>

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/FieldParallel.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/GetEntities.hpp>
#include <stk_mesh/base/CoordinateSystems.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Comm.hpp>
#include <stk_mesh/base/CreateEdges.hpp>
#include <stk_mesh/base/SkinBoundary.hpp>
#include <stk_mesh/base/FieldBLAS.hpp>

// stk_io
#include <stk_io/StkMeshIoBroker.hpp>
#include <stk_io/IossBridge.hpp>
#include <stk_io/InputFile.hpp>
#include <Ioss_SubSystem.h>

// stk_util
#include <stk_util/parallel/ParallelReduce.hpp>

class EquationSystem;
class YAML::Node;
class Simulation;
class stk::mesh::Part;
class Realm;

typedef std::vector<EquationSystem *> EquationSystemVector;

//! Stores one or multiple equation systems (objects of the class EqationSystem)
class EquationSystems {
public:
    EquationSystems(Realm & realm);
    ~EquationSystems();
    void load(const YAML::Node & node);
    void initialize();
    void register_nodal_fields(const std::vector<std::string> targetNames);
    void register_edge_fields(const std::vector<std::string> targetNames);
    void register_element_fields(const std::vector<std::string> targetNames);
    void register_interior_algorithm(const std::vector<std::string> targetNames);
    void register_wall_bc(const std::string targetName, const WallBoundaryConditionData & wallBCData);
    Simulation * root();
    Realm * parent();
    
    //! Ease of access function, an object of EquationSystems can treated like an array and its elements accessed
    size_t size() { return equationSystemVector_.size(); }
    
    //! Ease of access function, an object of EquationSystems can treated like an array and its elements accessed
    EquationSystem *operator[](int i) { return equationSystemVector_[i]; }
    
    Realm & realm_;
    std::string name_;
    int maxIterations_;
    EquationSystemVector equationSystemVector_;
    std::map<std::string, std::string> solverSpecMap_;
};

#endif /* EQUATIONSYSTEMS_H */

