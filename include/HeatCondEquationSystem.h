/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HEATCONDEQUATIONSYSTEM_H
#define HEATCONDEQUATIONSYSTEM_H

#include <EquationSystem.h>

class HeatCondEquationSystem : public EquationSystem {
public:
    HeatCondEquationSystem(EquationSystems & equationSystems);
    virtual ~HeatCondEquationSystem();
    virtual void load(const YAML::Node & node);
    void register_wall_bc(stk::mesh::Part * part, const stk::topology & partTopo, const WallBoundaryConditionData & wallBCData);
};

#endif /* HEATCONDEQUATIONSYSTEM_H */

