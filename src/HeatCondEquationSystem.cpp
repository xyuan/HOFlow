/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "HeatCondEquationSystem.h"

HeatCondEquationSystem::HeatCondEquationSystem(EquationSystems & equationSystems) :
    EquationSystem(equationSystems, "HeatCondEQS", "temperature")
{
}

HeatCondEquationSystem::~HeatCondEquationSystem() {
}

void HeatCondEquationSystem::load(const YAML::Node & node) {
    EquationSystem::load(node);
}

void HeatCondEquationSystem::register_wall_bc(stk::mesh::Part * part, 
                                              const stk::topology & partTopo, 
                                              const WallBoundaryConditionData & wallBCData) {
     // not yet implemented
}
