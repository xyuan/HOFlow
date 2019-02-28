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

void HeatCondEquationSystem::manage_png(EquationSystems & eqSystems) {
    projectedNodalGradEqs_ 
      = new ProjectedNodalGradientEquationSystem(eqSystems, EQ_PNG, "dtdx", "qTmp", "temperature", "PNGGradEQS");
    
    // fill the map; only require wall (which is the same name)...
    projectedNodalGradEqs_->set_data_map(WALL_BC, "temperature");
}

void HeatCondEquationSystem::register_nodal_fields(stk::mesh::Part *part) {
    stk::mesh::MetaData & meta_data = realm_.meta_data();

    const int nDim = meta_data.spatial_dimension();

    const int numStates = realm_.number_of_states();

    // register dof; set it as a restart variable
    temperature_ = &(meta_data.declare_field<ScalarFieldType>(stk::topology::NODE_RANK, "temperature", numStates));
    stk::mesh::put_field_on_mesh(*temperature_, *part, nullptr);
    realm_.augment_restart_variable_list("temperature");

    dtdx_ =  &(meta_data.declare_field<VectorFieldType>(stk::topology::NODE_RANK, "dtdx"));
    stk::mesh::put_field_on_mesh(*dtdx_, *part, nDim, nullptr);

    // delta solution for linear solver
    tTmp_ =  &(meta_data.declare_field<ScalarFieldType>(stk::topology::NODE_RANK, "tTmp"));
    stk::mesh::put_field_on_mesh(*tTmp_, *part, nullptr);

    dualNodalVolume_ = &(meta_data.declare_field<ScalarFieldType>(stk::topology::NODE_RANK, "dual_nodal_volume"));
    stk::mesh::put_field_on_mesh(*dualNodalVolume_, *part, nullptr);

    coordinates_ =  &(meta_data.declare_field<VectorFieldType>(stk::topology::NODE_RANK, "coordinates"));
    stk::mesh::put_field_on_mesh(*coordinates_, *part, nDim, nullptr);

    // props
    density_ = &(meta_data.declare_field<ScalarFieldType>(stk::topology::NODE_RANK, "density"));
    stk::mesh::put_field_on_mesh(*density_, *part, nullptr);

    specHeat_ = &(meta_data.declare_field<ScalarFieldType>(stk::topology::NODE_RANK, "specific_heat"));
    stk::mesh::put_field_on_mesh(*specHeat_, *part, nullptr);

    thermalCond_ = &(meta_data.declare_field<ScalarFieldType>(stk::topology::NODE_RANK, "thermal_conductivity"));
    stk::mesh::put_field_on_mesh(*thermalCond_, *part, nullptr);

    // push to property list
    realm_.augment_property_map(DENSITY_ID, density_);
    realm_.augment_property_map(SPEC_HEAT_ID, specHeat_);
    realm_.augment_property_map(THERMAL_COND_ID, thermalCond_);

    // make sure all states are properly populated (restart can handle this)
    if ( numStates > 2 && (!realm_.restarted_simulation() || realm_.support_inconsistent_restart()) ) {
        ScalarFieldType &tempN = temperature_->field_of_state(stk::mesh::StateN);
        ScalarFieldType &tempNp1 = temperature_->field_of_state(stk::mesh::StateNP1);

        CopyFieldAlgorithm *theCopyAlgA
          = new CopyFieldAlgorithm(realm_, part,
                                   &tempNp1, &tempN,
                                   0, 1,
                                   stk::topology::NODE_RANK);

        copyStateAlg_.push_back(theCopyAlgA);
    }

    // register the fringe nodal field 
    if ( realm_.query_for_overset() && realm_.has_mesh_motion() ) {
        ScalarFieldType *fringeNode
          = &(meta_data.declare_field<ScalarFieldType>(stk::topology::NODE_RANK, "fringe_node"));
        stk::mesh::put_field_on_mesh(*fringeNode, *part, nullptr);
    }
}
