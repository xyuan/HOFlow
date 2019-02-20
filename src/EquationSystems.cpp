/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "EquationSystems.h"
#include <EquationSystem.h>
#include <HOFlowEnv.h>
#include <HeatCondEquationSystem.h>
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/MetaData.hpp>

EquationSystems::EquationSystems(Realm & realm) :
    realm_(realm)
{
}

EquationSystems::~EquationSystems() {
    for (size_t ie = 0; ie < equationSystemVector_.size(); ++ie)
        delete equationSystemVector_[ie];

//    for (auto it = preIterAlgDriver_.begin(); it != preIterAlgDriver_.end(); ++it) {
//        delete (*it);
//    }
//
//    for (auto it = postIterAlgDriver_.begin(); it != postIterAlgDriver_.end(); ++it) {
//        delete (*it);
//    }
}

void EquationSystems::initialize() {
    HOFlowEnv::self().hoflowOutputP0() << "EquationSystems::initialize(): Begin " << std::endl;
//    double start_time = HOFlowEnv::self().nalu_time();
    for( EquationSystem* eqSys : equationSystemVector_ ) {
//        if ( realm_.get_activate_memory_diagnostic() ) {
//            HOFlowEnv::self().hoflowOutputP0() << "NaluMemory::EquationSystems::initialize(): " << eqSys->name_ << std::endl;
//            realm_.provide_memory_summary();
//        }
//        double start_time_eq = HOFlowEnv::self().nalu_time();
        eqSys->initialize();
//        double end_time_eq = HOFlowEnv::self().nalu_time();
//        eqSys->timerInit_ += (end_time_eq - start_time_eq);
    }
//    double end_time = HOFlowEnv::self().nalu_time();
//    realm_.timerInitializeEqs_ += (end_time-start_time);
    HOFlowEnv::self().hoflowOutputP0() << "EquationSystems::initialize(): End " << std::endl;
}

void EquationSystems::register_nodal_fields(const std::vector<std::string> targetNames) {
    stk::mesh::MetaData & meta_data = realm_.meta_data();

    for ( size_t itarget = 0; itarget < targetNames.size(); ++itarget ) {
        stk::mesh::Part * targetPart = meta_data.get_part(targetNames[itarget]);
        if ( NULL == targetPart ) {
            HOFlowEnv::self().hoflowOutputP0() << "Trouble with part " << targetNames[itarget] << std::endl;
            throw std::runtime_error("Sorry, no part name found by the name " + targetNames[itarget]);
        }
        else {
            realm_.register_nodal_fields(targetPart);
            EquationSystemVector::iterator ii;
            for( ii=equationSystemVector_.begin(); ii!=equationSystemVector_.end(); ++ii )
                (*ii)->register_nodal_fields(targetPart);
        }
    }
}

void EquationSystems::register_edge_fields(const std::vector<std::string> targetNames) {
    stk::mesh::MetaData &meta_data = realm_.meta_data();
  
    for ( size_t itarget = 0; itarget < targetNames.size(); ++itarget ) {
        stk::mesh::Part *targetPart = meta_data.get_part(targetNames[itarget]);
        if ( NULL == targetPart ) {
            throw std::runtime_error("Sorry, no part name found by the name " + targetNames[itarget]);
        }
        else {
            // found the part; no need to subset
            EquationSystemVector::iterator ii;
            for( ii=equationSystemVector_.begin(); ii!=equationSystemVector_.end(); ++ii )
                (*ii)->register_edge_fields(targetPart);
        }
    }
}

void EquationSystems::register_element_fields(const std::vector<std::string> targetNames){
    stk::mesh::MetaData &meta_data = realm_.meta_data();
  
    for ( size_t itarget = 0; itarget < targetNames.size(); ++itarget ) {
        stk::mesh::Part *targetPart = meta_data.get_part(targetNames[itarget]);
        if ( NULL == targetPart ) {
            throw std::runtime_error("Sorry, no part name found by the name " + targetNames[itarget]);
        }
        else {
            // found the part; no need to subset
            const stk::topology the_topo = targetPart->topology();
            if( stk::topology::ELEMENT_RANK != targetPart->primary_entity_rank() ) {
                throw std::runtime_error("Sorry, parts need to be elements.. " + targetNames[itarget]);
            }
            EquationSystemVector::iterator ii;
            for( ii=equationSystemVector_.begin(); ii!=equationSystemVector_.end(); ++ii )
                (*ii)->register_element_fields(targetPart, the_topo);
        }
    }
}

void EquationSystems::register_interior_algorithm(const std::vector<std::string> targetNames) {
    stk::mesh::MetaData &meta_data = realm_.meta_data();
  
    for ( size_t itarget = 0; itarget < targetNames.size(); ++itarget ) {
        stk::mesh::Part *targetPart = meta_data.get_part(targetNames[itarget]);
        if ( NULL == targetPart ) {
            throw std::runtime_error("Sorry, no part name found by the name " + targetNames[itarget]);
        }
        else {
            // found the part; no need to subset
            if( stk::topology::ELEMENT_RANK != targetPart->primary_entity_rank() ) {
                throw std::runtime_error("Sorry, parts need to be elements.. " + targetNames[itarget]);
            }

            realm_.register_interior_algorithm(targetPart);
            EquationSystemVector::iterator ii;
            for( ii=equationSystemVector_.begin(); ii!=equationSystemVector_.end(); ++ii )
                (*ii)->register_interior_algorithm(targetPart);
        }
    }
}

void EquationSystems::load(const YAML::Node & y_node) {
    const YAML::Node y_equation_system = expect_map(y_node,"equation_systems");
    get_required(y_equation_system, "name", name_);
    get_required(y_equation_system, "max_iterations", maxIterations_);
    
    const YAML::Node y_solver = expect_map(y_equation_system, "solver_system_specification");
    solverSpecMap_ = y_solver.as<std::map<std::string, std::string> >();
    
    const YAML::Node y_systems = expect_sequence(y_equation_system, "systems");
    for ( size_t isystem = 0; isystem < y_systems.size(); ++isystem ) {
        const YAML::Node y_system = y_systems[isystem] ;
        EquationSystem *eqSys = 0;
        YAML::Node y_eqsys ;
        if( expect_map(y_system, "HeatConduction", true) ) {
            y_eqsys =  expect_map(y_system, "HeatConduction", true);
            if (root()->debug()) HOFlowEnv::self().hoflowOutputP0() << "eqSys = HeatConduction " << std::endl;
            eqSys = new HeatCondEquationSystem(*this);
        }
        else {
            if (!HOFlowEnv::self().parallel_rank()) {
                std::cout << "Error: parsing at " << HOFlowParsingHelper::info(y_system) 
                          << "... at parent ... " << HOFlowParsingHelper::info(y_node) << std::endl;
            }
            throw std::runtime_error("parser error EquationSystem::load: unknown equation system type");
        }

        // load; particular equation system push back to vector is controled by the constructor
        eqSys->load(y_eqsys);
    }
}

void EquationSystems::register_wall_bc(const std::string targetName, const WallBoundaryConditionData & wallBCData) {
    stk::mesh::MetaData & meta_data = realm_.meta_data();

    stk::mesh::Part *targetPart = meta_data.get_part(targetName);
    if ( NULL == targetPart ) {
        HOFlowEnv::self().hoflowOutputP0() << "Sorry, no part name found by the name " << targetName << std::endl;
    }
    else {
        // found the part
        const std::vector<stk::mesh::Part *> & mesh_parts = targetPart->subsets();
        for( std::vector<stk::mesh::Part *>::const_iterator i = mesh_parts.begin(); i != mesh_parts.end(); ++i ) {
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
//                      (*ii)->register_wall_bc(part, the_topo, wallBCData);
                }
            }
        }
    }
}