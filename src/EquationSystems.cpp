/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "EquationSystems.h"
#include <HOFlowEnv.h>
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/MetaData.hpp>

EquationSystems::EquationSystems() {
}

EquationSystems::~EquationSystems() {
}

void EquationSystems::load(YAML::Node node) {
}

void EquationSystems::register_wall_bc(const std::string targetName, const WallBoundaryConditionData &wallBCData) {
    stk::mesh::MetaData & meta_data = realm_.meta_data();

    stk::mesh::Part *targetPart = meta_data.get_part(targetName);
    if ( NULL == targetPart ) {
        HOFlowEnv::self().hoflowOutputP0() << "Sorry, no part name found by the name " << targetName << std::endl;
    }
    else {
        // found the part
        const std::vector<stk::mesh::Part*> & mesh_parts = targetPart->subsets();
        for( std::vector<stk::mesh::Part*>::const_iterator i = mesh_parts.begin(); i != mesh_parts.end(); ++i ) {
            ThrowRequire(*i != nullptr);
            stk::mesh::Part * const part = *i;
            const stk::topology the_topo = part->topology();

            if ( !(meta_data.side_rank() == part->primary_entity_rank()) ) {
                HOFlowEnv::self().hoflowOutputP0() << "Sorry, part is not a face " << targetName;
            }
            else {
                realm_.register_wall_bc(part, the_topo);
                EquationSystemVector::iterator ii;
                for( ii = equationSystemVector_.begin(); ii != equationSystemVector_.end(); ++ii ) {
                      (*ii)->register_wall_bc(part, the_topo, wallBCData);
                }
            }
        }
    }
}

