/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef PROJECTEDNODALGRADIENTEQUATIONSYSTEM_H
#define PROJECTEDNODALGRADIENTEQUATIONSYSTEM_H

#include <Enums.h>
#include <EquationSystem.h>
#include <FieldTypeDef.h>
#include <NaluParsing.h>

#include <stk_mesh/base/FieldBase.hpp>
#include <stk_mesh/base/CoordinateSystems.hpp>

struct stk::topology;
class Realm;
class EquationSystem;

class ProjectedNodalGradientEquationSystem {
public:
    ProjectedNodalGradientEquationSystem(EquationSystems & equationSystems,
                                        const EquationType eqType,
                                        const std::string dofName, 
                                        const std::string deltaName, 
                                        const std::string independentDofName,
                                        const std::string eqSysName,
                                        const bool managesSolve = false);
    virtual ~ProjectedNodalGradientEquationSystem();

    void set_data_map(BoundaryConditionType BC, std::string name);
    std::string get_name_given_bc(BoundaryConditionType BC);
    void register_nodal_fields(stk::mesh::Part *part);
    void register_wall_bc(stk::mesh::Part *part, const stk::topology &theTopo, const WallBoundaryConditionData &wallBCData);
    void initialize();
    
    // names that customize this PNG system
    EquationType eqType_;
    std::string dofName_;
    std::string deltaName_;
    std::string independentDofName_;
    std::string eqSysName_;

    // who manages the solve? Often times, this is created by another EqSys
    const bool managesSolve_;

    // for exach equation, boundary data may be different
    std::map<BoundaryConditionType, std::string> dataMap_;

    // internal fields
    VectorFieldType *dqdx_;
    VectorFieldType *qTmp_;  
};

#endif /* PROJECTEDNODALGRADIENTEQUATIONSYSTEM_H */

