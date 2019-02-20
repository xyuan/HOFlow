/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "EquationSystem.h"

EquationSystem::EquationSystem(EquationSystems & eqSystems,
                               const std::string name,
                               const std::string eqnTypeName) 
{
    // nothing to do
}

EquationSystem::~EquationSystem() {
}

virtual void EquationSystem::load(const YAML::Node & node) {
    get_required(node, "name", userSuppliedName_);
    get_required(node, "max_iterations", maxIterations_);
    get_required(node, "convergence_tolerance", convergenceTolerance_);
}

