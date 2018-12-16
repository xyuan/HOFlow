/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "Realm.h"
#include <HOFlowEnv.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vector>

Realm::Realm(Realms & realms, const YAML::Node & node) :
    realms_(realms),
    name_("na"),
    type_("multi_physics"),
    inputDBName_("input_unknown"),
    spatialDimension_(3u),  // for convenience; can always get it from meta data
    solveFrequency_(1),
    metaData_(NULL),
    bulkData_(NULL),
    ioBroker_(NULL),
    boundaryConditions_(*this),
    initialConditions_(*this),
    materialPropertys_(*this),
    equationSystems_(*this),
    node_(node)
{}

Realm::~Realm() {
    delete bulkData_;
    delete metaData_;
    delete ioBroker_;
}

void Realm::initialize() {
    HOFlowEnv::self().hoflowOutputP0() << "Realm::initialize() Begin " << std::endl;
    
    // field registration
    setup_nodal_fields();
    setup_edge_fields();
    setup_element_fields();

    // property maps and evaluation algorithms
    setup_property();

    // interior algorithm creation
    setup_interior_algorithms();

    // create boundary conditions
    setup_bc();
    
    // create initial conditions
    setup_initial_conditions();
    
    // Populate_mesh fills in the entities (nodes/elements/etc) and
    // connectivities, but no field-data. Field-data is not allocated yet.
    HOFlowEnv::self().hoflowOutputP0() << "Realm::ioBroker_->populate_mesh() Begin" << std::endl;
    ioBroker_->populate_mesh();
    HOFlowEnv::self().hoflowOutputP0() << "Realm::ioBroker_->populate_mesh() End" << std::endl;
    
    // Now the mesh is fully populated, so we're ready to populate
    // field-data including coordinates, and attributes and/or distribution factors
    // if those exist on the input mesh file.
    HOFlowEnv::self().hoflowOutputP0() << "Realm::ioBroker_->populate_field_data() Begin" << std::endl;
    ioBroker_->populate_field_data();
    HOFlowEnv::self().hoflowOutputP0() << "Realm::ioBroker_->populate_field_data() End" << std::endl;
    
    // output and restart files
    create_output_mesh();
    
    populate_boundary_data();
    
    HOFlowEnv::self().hoflowOutputP0() << "Realm::initialize() End " << std::endl;
}

void Realm::load(const YAML::Node& node) {
    name_ = node["name"].as<std::string>();
    inputDBName_ = node["mesh"].as<std::string>();
    get_if_present(node, "type", type_, type_);
    
    get_if_present(node, "solve_frequency", solveFrequency_, solveFrequency_);
    
    create_mesh();
    spatialDimension_ = metaData_->spatial_dimension();
    
    if ( type_ == "multi_physics" ) {
        HOFlowEnv::self().hoflowOutputP0() << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "Boundary Condition Review: " << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;
        boundaryConditions_.load(node);
        HOFlowEnv::self().hoflowOutputP0() << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "Initial Condition Review:  " << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;
        initialConditions_.load(node);
        HOFlowEnv::self().hoflowOutputP0() << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "Material Prop Review:      " << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;
        materialPropertys_.load(node);
        HOFlowEnv::self().hoflowOutputP0() << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "EqSys/options Review:      " << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;
        equationSystems_.load(node);
    }
}

void Realm::setup_nodal_fields() {
    // register global id and rank fields on all parts
    const stk::mesh::PartVector parts = metaData_->get_parts();
    
    for ( size_t ipart = 0; ipart < parts.size(); ++ipart ) {
        naluGlobalId_ = &(metaData_->declare_field<GlobalIdFieldType>(stk::topology::NODE_RANK, "nalu_global_id"));
        stk::mesh::put_field_on_mesh(*naluGlobalId_, *parts[ipart], nullptr);
    }

    // loop over all material props targets and register nodal fields
    std::vector<std::string> targetNames = get_physics_target_names();
    equationSystems_.register_nodal_fields(targetNames);
}

void Realm::setup_edge_fields() {
    // loop over all material props targets and register edge fields
    std::vector<std::string> targetNames = get_physics_target_names();
    equationSystems_.register_edge_fields(targetNames);
}

void Realm::setup_element_fields() {
    // loop over all material props targets and register element fields
    std::vector<std::string> targetNames = get_physics_target_names();
    equationSystems_.register_element_fields(targetNames);
}

void Realm::setup_interior_algorithms() {
    // loop over all material props targets and register interior algs
    std::vector<std::string> targetNames = get_physics_target_names();
    equationSystems_.register_interior_algorithm(targetNames);
}

void Realm::setup_bc() {
    // loop over all bcs and register
    for (size_t ibc = 0; ibc < boundaryConditions_.size(); ++ibc) {
        BoundaryCondition& bc = *boundaryConditions_[ibc];
        std::string name = physics_part_name(bc.targetName_);

        switch(bc.theBcType_) {
            case WALL_BC:
                equationSystems_.register_wall_bc(name, *reinterpret_cast<const WallBoundaryConditionData *>(&bc));
                break;
            default:
                throw std::runtime_error("unknown bc");
        }
    }
}

void Realm::setup_initial_conditions() {
    // loop over all ics and register
    for (size_t j_ic = 0; j_ic < initialConditions_.size(); ++j_ic) {
        InitialCondition & initCond = *initialConditions_[j_ic];
        const std::vector<std::string> targetNames = initCond.targetNames_;

        for (size_t itarget=0; itarget < targetNames.size(); ++itarget) {
            const std::string targetName = physics_part_name(targetNames[itarget]);

            // target need not be subsetted since nothing below will depend on topo
            stk::mesh::Part *targetPart = metaData_->get_part(targetName);

            switch(initCond.theIcType_) {
                case CONSTANT_UD:
                {
                    const ConstantInitialConditionData& genIC = *reinterpret_cast<const ConstantInitialConditionData *>(&initCond);
                    ThrowAssert(genIC.data_.size() == genIC.fieldNames_.size());
                    
                    for (size_t ifield = 0; ifield < genIC.fieldNames_.size(); ++ifield) {
                        std::vector<double>  genSpec = genIC.data_[ifield];
                        stk::mesh::FieldBase *field = stk::mesh::get_field_by_name(genIC.fieldNames_[ifield], *metaData_);
                        ThrowAssert(field);

                        stk::mesh::FieldBase *fieldWithState = ( field->number_of_states() > 1 )
                          ? field->field_state(stk::mesh::StateNP1)
                          : field->field_state(stk::mesh::StateNone);

                        std::vector<double> userGen = genSpec;
                        ConstantAuxFunction *theGenFunc = new ConstantAuxFunction(0, genSpec.size(), userGen);
                        AuxFunctionAlgorithm *auxGen
                          = new AuxFunctionAlgorithm( *this, targetPart,
                                                      fieldWithState, theGenFunc, stk::topology::NODE_RANK);
                        initCondAlg_.push_back(auxGen);
                    }
                }
                break;
                case FUNCTION_UD:
                {
                    const UserFunctionInitialConditionData& fcnIC = *reinterpret_cast<const UserFunctionInitialConditionData *>(&initCond);
                    equationSystems_.register_initial_condition_fcn(targetPart, fcnIC);
                }
                break;
                case USER_SUB_UD:
                    throw std::runtime_error("Realm::setup_initial_conditions: USER_SUB not supported: ");
                case UserDataType_END:
                    break;
                default:
                    HOFlowEnv::self().hoflowOutputP0() << "Realm::setup_initial_conditions: unknown type: " << initCond.theIcType_ << std::endl;
                    throw std::runtime_error("Realm::setup_initial_conditions: unknown type:");
            }
        }
    }
}

void Realm::setup_property() {
    // loop over all target names
    const std::vector<std::string> targetNames = get_physics_target_names();

    for (size_t itarget=0; itarget < targetNames.size(); ++itarget) {
        // target need not be subsetted since nothing below will depend on topo
        stk::mesh::Part *targetPart = metaData_->get_part(targetNames[itarget]);

        // loop over propertyMap
        std::map<PropertyIdentifier, ScalarFieldType *>::iterator ii;

        for ( ii=propertyMap_.begin(); ii!=propertyMap_.end(); ++ii ) {
            // extract property id and field pointer
            PropertyIdentifier thePropId = (*ii).first;
            ScalarFieldType *thePropField = (*ii).second;

            // find the material property data object
            MaterialPropertyData *matData = NULL;
            std::map<PropertyIdentifier, MaterialPropertyData*>::iterator itf =
                materialPropertys_.propertyDataMap_.find(thePropId);
            if ( itf == materialPropertys_.propertyDataMap_.end() ) {
                // will need to throw
                HOFlowEnv::self().hoflowOutputP0() << "issue with property: " << PropertyIdentifierNames[thePropId] << std::endl;
                throw std::runtime_error("Please add property specification ");
            }
            else {
                matData = (*itf).second;
            }
            switch( matData->type_) {
            case CONSTANT_MAT:
            {
                // set the default begin/end
                int theBegin = 0;
                int theEnd = 1;

                // create everything
                std::vector<double> userConstData(1);
                userConstData[0] = matData->constValue_;
                ConstantAuxFunction *theAuxFunc = new ConstantAuxFunction(theBegin, theEnd, userConstData);
                AuxFunctionAlgorithm *auxAlg = new AuxFunctionAlgorithm( *this, targetPart, thePropField, theAuxFunc, stk::topology::NODE_RANK);
                propertyAlg_.push_back(auxAlg);
            }
            break;
            case MaterialPropertyType_END:
            break;
            default:
                throw std::runtime_error("Realm::setup_property: unknown type:");
            }
        }
    }
}

void Realm::augment_property_map(PropertyIdentifier propID, ScalarFieldType *theField) {
    propertyMap_[propID] = theField;
}

void Realm::output_converged_results() {
    provide_output();
}

void Realm::commit() {
    metaData_->commit();
}

void Realm::create_mesh() {
    HOFlowEnv::self().hoflowOutputP0() << "Realm::create_mesh(): Begin" << std::endl;

    // news for mesh constructs
    metaData_ = new stk::mesh::MetaData();
    bulkData_ = new stk::mesh::BulkData(*metaData_, pm, activateAura_ ? stk::mesh::BulkData::AUTO_AURA : stk::mesh::BulkData::NO_AUTO_AURA);
    ioBroker_ = new stk::io::StkMeshIoBroker( pm );
    ioBroker_->set_bulk_data(*bulkData_);

    // Initialize meta data (from exodus file); can possibly be a restart file..
    inputMeshIdx_ = ioBroker_->add_mesh_database( inputDBName_, restarted_simulation() ? stk::io::READ_RESTART : stk::io::READ_MESH );
    ioBroker_->create_input_mesh();

    HOFlowEnv::self().hoflowOutputP0() << "Realm::create_mesh() End" << std::endl;
}

void Realm::create_output_mesh() {
    // exodus output file creation
    if (outputInfo_->hasOutputBlock_) {
        HOFlowEnv::self().hoflowOutputP0() << "Realm::create_output_mesh(): Begin" << std::endl;

        if (outputInfo_->outputFreq_ == 0)
            return;

        // if we are adapting, skip when no I/O happens before first adapt step
        if (solutionOptions_->useAdapter_ && outputInfo_->meshAdapted_ == false &&
            solutionOptions_->adaptivityFrequency_ <= outputInfo_->outputFreq_) {
            return;
        }

        std::string oname =  outputInfo_->outputDBName_;
        if (solutionOptions_->useAdapter_ && solutionOptions_->maxRefinementLevel_) {
            static int fileid = 0;
            std::ostringstream fileid_ss;
            fileid_ss << std::setfill('0') << std::setw(4) << (fileid+1);
            if (fileid++ > 0) oname += "-s" + fileid_ss.str();
        }

        if (!outputInfo_->catalystFileName_.empty()||!outputInfo_->paraviewScriptName_.empty()) {
            outputInfo_->outputPropertyManager_->add(Ioss::Property("CATALYST_BLOCK_PARSE_JSON_STRING",
                                                     outputInfo_->catalystParseJson_));
            std::string input_deck_name = "%B";
            stk::util::filename_substitution(input_deck_name);
            outputInfo_->outputPropertyManager_->add(Ioss::Property("CATALYST_BLOCK_PARSE_INPUT_DECK_NAME", input_deck_name));

            if(!outputInfo_->paraviewScriptName_.empty())
              outputInfo_->outputPropertyManager_->add(Ioss::Property("CATALYST_SCRIPT", outputInfo_->paraviewScriptName_.c_str()));

            outputInfo_->outputPropertyManager_->add(Ioss::Property("CATALYST_CREATE_SIDE_SETS", 1));

            resultsFileIndex_ = ioBroker_->create_output_mesh( oname, stk::io::WRITE_RESULTS, *outputInfo_->outputPropertyManager_, "catalyst" );
        }
        else {
            resultsFileIndex_ = ioBroker_->create_output_mesh( oname, stk::io::WRITE_RESULTS, *outputInfo_->outputPropertyManager_);
        }

        // Tell stk_io how to output element block nodal fields:
        // if 'true' passed to function, then output them as nodeset fields;
        // if 'false', then output as nodal fields (on all nodes of the mesh, zero-filled)
        // The option is provided since some post-processing/visualization codes do not
        // correctly handle nodeset fields.
        ioBroker_->use_nodeset_for_part_nodes_fields(resultsFileIndex_, outputInfo_->outputNodeSet_);

        // FIXME: add_field can take user-defined output name, not just varName
        for ( std::set<std::string>::iterator itorSet = outputInfo_->outputFieldNameSet_.begin();
            itorSet != outputInfo_->outputFieldNameSet_.end(); ++itorSet ) {
            std::string varName = *itorSet;
            stk::mesh::FieldBase *theField = stk::mesh::get_field_by_name(varName, *metaData_);
            if ( NULL == theField ) {
                HOFlowEnv::self().hoflowOutputP0() << " Sorry, no field by the name " << varName << std::endl;
            }
            else {
                // 'varName' is the name that will be written to the database
                // For now, just using the name of the stk field
                ioBroker_->add_field(resultsFileIndex_, *theField, varName);
            }
        }
        // reset this flag
        outputInfo_->meshAdapted_ = false;

        HOFlowEnv::self().hoflowOutputP0() << "Realm::create_output_mesh() End" << std::endl;
    }
}

void Realm::input_variables_from_mesh() {
    // check whether to snap or interpolate data; all fields treated the same
    const stk::io::MeshField::TimeMatchOption fieldInterpOption = solutionOptions_->inputVariablesInterpolateInTime_
        ? stk::io::MeshField::LINEAR_INTERPOLATION
        : stk::io::MeshField::CLOSEST;

    // check for periodic cycling of data based on start time and periodic time; scale time set to unity
    if ( solutionOptions_->inputVariablesPeriodicTime_ > 0.0 ) {
        ioBroker_->get_mesh_database(inputMeshIdx_).set_periodic_time(solutionOptions_->inputVariablesPeriodicTime_, 
            solutionOptions_->inputVariablesRestorationTime_, 
            stk::io::InputFile::CYCLIC).set_scale_time(1.0);
    }
    
    std::map<std::string, std::string>::const_iterator iter;
    for ( iter = solutionOptions_->inputVarFromFileMap_.begin(); iter != solutionOptions_->inputVarFromFileMap_.end(); ++iter) {
        std::string varName = iter->first;
        std::string userName = iter->second;

        stk::mesh::FieldBase *theField = stk::mesh::get_field_by_name(varName,*metaData_);
        if ( NULL == theField ) {
            HOFlowEnv::self().hoflowOutputP0() << " Sorry, no field by the name " << varName << std::endl;
        }
        else {
            ioBroker_->add_input_field(stk::io::MeshField(*theField, userName, fieldInterpOption));
        }
    }
}

void Realm::augment_output_variable_list(const std::string fieldName) {
    outputInfo_->outputFieldNameSet_.insert(fieldName);
}

void Realm::register_interior_algorithm(stk::mesh::Part *part) {
    const AlgorithmType algType = INTERIOR;
    std::map<AlgorithmType, Algorithm *>::iterator it = computeGeometryAlgDriver_->algMap_.find(algType);
    
    if ( it == computeGeometryAlgDriver_->algMap_.end() ) {
        ComputeGeometryInteriorAlgorithm *theAlg = new ComputeGeometryInteriorAlgorithm(*this, part);
        computeGeometryAlgDriver_->algMap_[algType] = theAlg;
    }
    else {
        it->second->partVec_.push_back(part);
    }

    // Track parts that are registered to interior algorithms
    interiorPartVec_.push_back(part);
}

void Realm::register_wall_bc(stk::mesh::Part *part, const stk::topology &theTopo) {
    //====================================================
    // Register face (boundary condition) data
    //====================================================

    // push back the part for book keeping and, later, skin mesh
    bcPartVec_.push_back(part);

    const int nDim = metaData_->spatial_dimension();

    // register fields
    MasterElement *meFC = MasterElementRepo::get_surface_master_element(theTopo);
    const int numScsIp = meFC->numIntPoints_;

    GenericFieldType *exposedAreaVec_ = &(metaData_->declare_field<GenericFieldType>(static_cast<stk::topology::rank_t>(metaData_->side_rank()), "exposed_area_vector"));
    stk::mesh::put_field_on_mesh(*exposedAreaVec_, *part, nDim*numScsIp, nullptr);

    //====================================================
    // Register wall algorithms
    //====================================================
    const AlgorithmType algType = WALL;
    std::map<AlgorithmType, Algorithm *>::iterator it = computeGeometryAlgDriver_->algMap_.find(algType);
    if ( it == computeGeometryAlgDriver_->algMap_.end() ) {
        ComputeGeometryBoundaryAlgorithm *theAlg = new ComputeGeometryBoundaryAlgorithm(*this, part);
        computeGeometryAlgDriver_->algMap_[algType] = theAlg;
    }
    else {
        it->second->partVec_.push_back(part);
    }
}

void Realm::provide_output() {
    stk::diag::TimeBlock mesh_output_timeblock(Simulation::outputTimer());

    if ( outputInfo_->hasOutputBlock_ ) {
        if (outputInfo_->outputFreq_ == 0) {
            return;
        }
        const double start_time = NaluEnv::self().nalu_time();

        // process output via io
        const double currentTime = get_current_time();
        const int timeStepCount = get_time_step_count();
        const int modStep = timeStepCount - outputInfo_->outputStart_;

        // check for elapsed WALL time threshold
        bool forcedOutput = false;
        if ( outputInfo_->userWallTimeResults_.first) {
            const double elapsedWallTime = stk::wall_time() - wallTimeStart_;
            // find the max over all core
            double g_elapsedWallTime = 0.0;
            stk::all_reduce_max(NaluEnv::self().parallel_comm(), &elapsedWallTime, &g_elapsedWallTime, 1);
            // convert to hours
            g_elapsedWallTime /= 3600.0;
            // only force output the first time the timer is exceeded
            if ( g_elapsedWallTime > outputInfo_->userWallTimeResults_.second ) {
                forcedOutput = true;
                outputInfo_->userWallTimeResults_.first = false;
                NaluEnv::self().naluOutputP0() << "Realm::provide_output()::Forced Result output will be processed at current time: " << currentTime << std::endl;
                NaluEnv::self().naluOutputP0() <<  " Elapsed (max) WALL time: " << g_elapsedWallTime << " (hours)" << std::endl;
                // provide timer information
                dump_simulation_time();
            }
        }

        const bool isOutput = (timeStepCount >=outputInfo_->outputStart_ && modStep % outputInfo_->outputFreq_ == 0) || forcedOutput;

        if ( isOutput ) {
            HOFlowEnv::self().hoflowOutputP0() << "Realm shall provide output files at : currentTime/timeStepCount: "
                                           << currentTime << "/" <<  timeStepCount << " (" << name_ << ")" << std::endl;      
            // not set up for globals
            if (!doPromotion_) {
                ioBroker_->process_output_request(resultsFileIndex_, currentTime);
            }
            else {
                promotionIO_->write_database_data(currentTime);
            }
            equationSystems_.provide_output();
        }
    }
}