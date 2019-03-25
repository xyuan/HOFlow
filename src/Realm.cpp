/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "Realm.h"
#include <Realms.h>
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vector>
#include <FieldTypeDef.h>
#include <Enums.h>
#include <BoundaryConditions.h>
#include <InitialConditions.h>
#include <InitialCondition.h>
#include <MaterialProperties.h>
#include <MaterialProperty.h>
#include <MaterialPropertyData.h>
#include <PropertyEvaluator.h>
#include <ReferencePropertyData.h>
#include <EquationSystems.h>
#include <EquationSystem.h>
#include <Algorithm.h>
#include <AuxFunctionAlgorithm.h>
#include <ConstantAuxFunction.h>
#include <Simulation.h>
#include "LinearSystem.h"
#include "TpetraLinearSystem.h"
#include "OutputInfo.h"
#include "SolutionOptions.h"
#include "TimeIntegrator.h"
#include "ComputeGeometryAlgorithmDriver.h"
#include "ComputeGeometryInteriorAlgorithm.h"
#include "ComputeGeometryBoundaryAlgorithm.h"
#include "master_element/MasterElement.h"

#include "hoflow_make_unique.h"

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

// basic c++
#include <map>
#include <cmath>
#include <utility>
#include <stdint.h>

//! Constructor of a computational domain
Realm::Realm(Realms & realms, const YAML::Node & node) :
    realms_(realms),
    name_("na"),
    type_("multi_physics"),
    inputDBName_("input_unknown"),
    spatialDimension_(3u),  // for convenience; can always get it from meta data
    solveFrequency_(1),
    computeGeometryAlgDriver_(0),
    l2Scaling_(1.0),
    metaData_(NULL),
    bulkData_(NULL),
    ioBroker_(NULL),
    resultsFileIndex_(99),
    boundaryConditions_(*this),
    initialConditions_(*this),
    materialProperties_(*this),
    equationSystems_(*this),
    solutionOptions_(new SolutionOptions()),
    outputInfo_(new OutputInfo()),
    node_(node),
    exposedBoundaryPart_(0),
    activateAura_(false),
    activateMemoryDiagnostic_(false),
    doPromotion_(false),
    promotionOrder_(0u),
    timeIntegrator_(0),
    nodeCount_(0),
    checkJacobians_(false),
    checkForMissingBcs_(false),
    provideEntityCount_(false),
    estimateMemoryOnly_(false),
    availableMemoryPerCoreGB_(0),
    timerCreateMesh_(0.0),
    timerPopulateMesh_(0.0),
    timerPopulateFieldData_(0.0),
    timerOutputFields_(0.0),
    timerCreateEdges_(0.0),
    timerNonconformal_(0.0),
    timerInitializeEqs_(0.0),
    timerPropertyEval_(0.0),
    timerAdapt_(0.0),
    timerTransferSearch_(0.0),
    timerTransferExecute_(0.0),
    timerSkinMesh_(0.0),
    timerPromoteMesh_(0.0),
    timerSortExposedFace_(0.0),
    wallTimeStart_(stk::wall_time()),
    inputMeshIdx_(-1)
{
    // nothing to do
}

//! Destructor of a computational domain
Realm::~Realm() {
    delete bulkData_;
    delete metaData_;
    delete ioBroker_;
    
    delete computeGeometryAlgDriver_;

    // prop algs
    std::vector<Algorithm *>::iterator ii;
    for( ii=initCondAlg_.begin(); ii!=initCondAlg_.end(); ++ii )
      delete *ii;
    for( ii=propertyAlg_.begin(); ii!=propertyAlg_.end(); ++ii ) {
      delete *ii;
    }

//    // any bc data
//    std::vector<AuxFunctionAlgorithm *>::iterator iaux;
//    for( iaux=bcDataAlg_.begin(); iaux!=bcDataAlg_.end(); ++iaux )
//      delete *iaux;

    delete solutionOptions_;
    delete outputInfo_;

    MasterElementRepo::clear();
}

void Realm::breadboard() {
    computeGeometryAlgDriver_ = new ComputeGeometryAlgorithmDriver(*this);
}

Simulation * Realm::root() { 
    return parent()->root(); 
}

Simulation * Realm::root() const { 
    return parent()->root(); 
}

Realms * Realm::parent() { 
    return & realms_; 
}

Realms * Realm::parent() const { 
    return & realms_; 
}

//! Initializes the computational domain with in the input file specified values
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
    double time = -HOFlowEnv::self().hoflow_time();
    ioBroker_->populate_mesh();
    time += HOFlowEnv::self().hoflow_time();
    timerPopulateMesh_ += time;
    HOFlowEnv::self().hoflowOutputP0() << "Realm::ioBroker_->populate_mesh() End" << std::endl;

    // output entity counts including max/min
    if ( provideEntityCount_ ) {
        provide_entity_count();
    }
    
    // Now the mesh is fully populated, so we're ready to populate
    // field-data including coordinates, and attributes and/or distribution factors
    // if those exist on the input mesh file.
    HOFlowEnv::self().hoflowOutputP0() << "Realm::ioBroker_->populate_field_data() Begin" << std::endl;
    time = -HOFlowEnv::self().hoflow_time();
    ioBroker_->populate_field_data();
    time += HOFlowEnv::self().hoflow_time();
    timerPopulateFieldData_ += time;
    HOFlowEnv::self().hoflowOutputP0() << "Realm::ioBroker_->populate_field_data() End" << std::endl;
    
    // manage HOFlowGlobalId for linear system
    set_global_id();
    
    // output and restart files
    create_output_mesh();
    
    populate_boundary_data();
    compute_geometry();
    equationSystems_.initialize();
    check_job(false);
    HOFlowEnv::self().hoflowOutputP0() << "Realm::initialize() End " << std::endl;
}

//! Loads information regarding the computational domain from the input file (also mesh file)

//! Reads the mesh file name in the input file in order to read the mesh file.
//! Reads all boundary and initial conditions as well as material properties.
//! Reads all equation systems specified in the input file.
void Realm::load(const YAML::Node & node) {
    name_ = node["name"].as<std::string>();
    inputDBName_ = node["mesh"].as<std::string>();
    get_if_present(node, "type", type_, type_);
    
    // provide a high level banner
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "Realm Options Review: " << name_ << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;

    get_if_present(node, "estimate_memory_only", estimateMemoryOnly_, false);
    get_if_present(node, "available_memory_per_core_GB", availableMemoryPerCoreGB_, 0.0);

    // exposed bc check
    get_if_present(node, "check_for_missing_bcs", checkForMissingBcs_, checkForMissingBcs_);

    // check for bad Jacobians in the mesh
    get_if_present(node, "check_jacobians", checkJacobians_, checkJacobians_);

    // entity count
    get_if_present(node, "provide_entity_count", provideEntityCount_, provideEntityCount_);

    get_if_present(node, "polynomial_order", promotionOrder_, promotionOrder_);
    if (promotionOrder_ >=1) {
        doPromotion_ = true;

        // with polynomial order set to 1, the HO method defaults down to the consistent mass matrix P1 discretization
        // super-element/faces are activated despite being unnecessary
        if (promotionOrder_ == 1) {
            HOFlowEnv::self().hoflowOutputP0() << "Activating the consistent-mass matrix P1 discretization..." << std::endl;
        }
    }

    // how often is the realm solved..
    get_if_present(node, "solve_frequency", solveFrequency_, solveFrequency_);

//    // automatic decomposition
//    get_if_present(node, "automatic_decomposition_type", autoDecompType_, autoDecompType_);
//    if ( "None" != autoDecompType_ ) {
//      HOFlowEnv::self().hoflowOutputP0() 
//        <<"Warning: When using automatic_decomposition_type, one must have a serial file" << std::endl;
//    }
//
//    // activate aura
//    get_if_present(node, "activate_aura", activateAura_, activateAura_);
//    if ( activateAura_ )
//      HOFlowEnv::self().hoflowOutputP0() << "Nalu will activate aura ghosting" << std::endl;
//    else
//      HOFlowEnv::self().hoflowOutputP0() << "Nalu will deactivate aura ghosting" << std::endl;

    // memory diagnostic
    get_if_present(node, "activate_memory_diagnostic", activateMemoryDiagnostic_, activateMemoryDiagnostic_);
    if ( activateMemoryDiagnostic_ )
      HOFlowEnv::self().hoflowOutputP0() << "Nalu will activate detailed memory pulse" << std::endl;
//
//    // allow for inconsistent restart (fields are missing)
//    get_if_present(node, "support_inconsistent_multi_state_restart", supportInconsistentRestart_, supportInconsistentRestart_);
//
//    // time step control
//    const bool dtOptional = true;
//    const YAML::Node y_time_step = expect_map(node,"time_step_control", dtOptional);
//    if ( y_time_step ) {
//        get_if_present(y_time_step, "target_courant", targetCourant_, targetCourant_);
//        get_if_present(y_time_step, "time_step_change_factor", timeStepChangeFactor_, timeStepChangeFactor_);
//    }
//
//    get_if_present(node, "balance_nodes", doBalanceNodes_, doBalanceNodes_);
//    get_if_present(node, "balance_nodes_iterations", balanceNodeOptions_.numIters, balanceNodeOptions_.numIters);
//    get_if_present(node, "balance_nodes_target", balanceNodeOptions_.target, balanceNodeOptions_.target);
//    if (node["balance_nodes_iterations"] || node["balance_nodes_target"] ) {
//        doBalanceNodes_ = true;
//    }


    //======================================
    // now other commands/actions
    //======================================

    // load output first so we can check for serializing i/o
    outputInfo_->load(node);
//    if (root()->serializedIOGroupSize_ == 0)
//    {
//      // only set from input file if command-line didn't set it
//      root()->setSerializedIOGroupSize(outputInfo_->serializedIOGroupSize_);
//    }

//    // Parse catalyst input file if requested
//    if(!outputInfo_->catalystFileName_.empty())
//    {
//    int error = Iovs::DatabaseIO::parseCatalystFile(outputInfo_->catalystFileName_,
//                                                    outputInfo_->catalystParseJson_);
//    if(error)
//      throw std::runtime_error("Catalyst file parse failed: " + outputInfo_->catalystFileName_);
//    }

    // solution options - loaded before create_mesh since we need to know if
    // adaptivity is on to create the proper MetaData
    solutionOptions_->load(node);
    
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
        materialProperties_.load(node);
        HOFlowEnv::self().hoflowOutputP0() << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "EqSys/options Review:      " << std::endl;
        HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;
        equationSystems_.load(node);
    }
    check_job(true);
}

//! Sets material properties to the nodal field
void Realm::setup_nodal_fields() {
    // register global id and rank fields on all parts
    const stk::mesh::PartVector parts = metaData_->get_parts();
    
    for ( size_t ipart = 0; ipart < parts.size(); ++ipart ) {
        hoflowGlobalId_ = &(metaData_->declare_field<GlobalIdFieldType>(stk::topology::NODE_RANK, "hoflow_global_id"));
        stk::mesh::put_field_on_mesh(*hoflowGlobalId_, *parts[ipart], nullptr);
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

//! Sets the in the input file specified bc's to the computational domain
void Realm::setup_bc() {
    // loop over all bcs and register
    for (size_t ibc = 0; ibc < boundaryConditions_.size(); ++ibc) {
        BoundaryCondition & bc = *boundaryConditions_[ibc];
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

//! Sets the in the input file specified ic's to the computational domain
void Realm::setup_initial_conditions() {
    // loop over all ics and register
    for (size_t j_ic = 0; j_ic < initialConditions_.size(); ++j_ic) {
        InitialCondition & initCond = *initialConditions_[j_ic];
        const std::vector<std::string> targetNames = initCond.targetNames_;
        
        // loop over all targetNames
        for (size_t itarget=0; itarget < targetNames.size(); ++itarget) {
            const std::string targetName = physics_part_name(targetNames[itarget]);

            // target need not be subsetted since nothing below will depend on topo
            stk::mesh::Part *targetPart = metaData_->get_part(targetName);
            
            // choose the defined ic type
            switch(initCond.theIcType_) {
                case CONSTANT_UD:
                {
                    const ConstantInitialConditionData & genIC = *reinterpret_cast<const ConstantInitialConditionData *>(&initCond);
                    ThrowAssert(genIC.data_.size() == genIC.fieldNames_.size());
                    
                    for (size_t ifield = 0; ifield < genIC.fieldNames_.size(); ++ifield) {
                        std::vector<double>  genSpec = genIC.data_[ifield];
                        stk::mesh::FieldBase * field = stk::mesh::get_field_by_name(genIC.fieldNames_[ifield], *metaData_);
                        ThrowAssert(field);

                        stk::mesh::FieldBase * fieldWithState = ( field->number_of_states() > 1 )
                          ? field->field_state(stk::mesh::StateNP1)
                          : field->field_state(stk::mesh::StateNone);

                        std::vector<double> userGen = genSpec;
                        ConstantAuxFunction * theGenFunc = new ConstantAuxFunction(0, genSpec.size(), userGen);
                        AuxFunctionAlgorithm * auxGen = new AuxFunctionAlgorithm(*this, targetPart,
                                                                                fieldWithState, 
                                                                                theGenFunc, 
                                                                                stk::topology::NODE_RANK);
                        initCondAlg_.push_back(auxGen);
                    }
                }
                break;
                default:
                    HOFlowEnv::self().hoflowOutputP0() << "Realm::setup_initial_conditions: unknown type: " << initCond.theIcType_ << std::endl;
                    throw std::runtime_error("Realm::setup_initial_conditions: unknown type:");
            }
        }
    }
}


void Realm::setup_property() {
    // loop overall material property blocks
    for ( size_t i = 0; i < materialProperties_.materialPropertyVector_.size(); ++i ) {

        MaterialProperty *matPropBlock = materialProperties_.materialPropertyVector_[i];

        // loop over all target names
        for (size_t itarget=0; itarget < matPropBlock->targetNames_.size(); ++itarget) {

            // target need not be subsetted since nothing below will depend on topo
            const std::string physicsPartName = physics_part_name(matPropBlock->targetNames_[itarget]);
            stk::mesh::Part *targetPart = metaData_->get_part(physicsPartName);

            // loop over propertyMap
            std::map<PropertyIdentifier, ScalarFieldType *>::iterator ii;
            for ( ii=propertyMap_.begin(); ii!=propertyMap_.end(); ++ii ) {

                // extract property id and field pointer
                PropertyIdentifier thePropId = (*ii).first;
                ScalarFieldType *thePropField = (*ii).second;

                // find the material property data object
                MaterialPropertyData *matData = NULL;
                std::map<PropertyIdentifier, MaterialPropertyData*>::iterator itf = matPropBlock->propertyDataMap_.find(thePropId);
                if ( itf == matPropBlock->propertyDataMap_.end() ) {
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
                    ConstantAuxFunction * theAuxFunc = new ConstantAuxFunction(theBegin, theEnd, userConstData);
                    AuxFunctionAlgorithm * auxAlg = new AuxFunctionAlgorithm(*this, targetPart, thePropField, theAuxFunc, stk::topology::NODE_RANK);
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

//! Reads the exodus file specified in the input file
void Realm::create_mesh() {
    HOFlowEnv::self().hoflowOutputP0() << "Realm::create_mesh(): Begin" << std::endl;
    stk::ParallelMachine pm = HOFlowEnv::self().parallel_comm();

    // news for mesh constructs
    metaData_ = new stk::mesh::MetaData();
    bulkData_ = new stk::mesh::BulkData(*metaData_, pm, stk::mesh::BulkData::NO_AUTO_AURA);
    ioBroker_ = new stk::io::StkMeshIoBroker( pm );
    ioBroker_->set_bulk_data(*bulkData_);

    // Initialize meta data (from exodus file); can possibly be a restart file..
    inputMeshIdx_ = ioBroker_->add_mesh_database( inputDBName_, stk::io::READ_MESH );
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
    /*// check whether to snap or interpolate data; all fields treated the same
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
    }*/
}

void Realm::augment_output_variable_list(const std::string fieldName) {
    outputInfo_->outputFieldNameSet_.insert(fieldName);
}

void Realm::augment_restart_variable_list(std::string restartFieldName) {
    outputInfo_->restartFieldNameSet_.insert(restartFieldName);
}

void Realm::register_nodal_fields(stk::mesh::Part *part) {
    // register high level common fields
    const int nDim = metaData_->spatial_dimension();
}

void Realm::register_interior_algorithm(stk::mesh::Part *part) {
    const AlgorithmType algType = INTERIOR;
    std::map<AlgorithmType, Algorithm *>::iterator it = computeGeometryAlgDriver_->algMap_.find(algType);
    
    if ( it == computeGeometryAlgDriver_->algMap_.end() ) {
        ComputeGeometryInteriorAlgorithm * theAlg = new ComputeGeometryInteriorAlgorithm(*this, part);
        computeGeometryAlgDriver_->algMap_[algType] = theAlg;
    }
    else {
        it->second->partVec_.push_back(part);
    }

    // Track parts that are registered to interior algorithms
    interiorPartVec_.push_back(part);
}

void Realm::register_wall_bc(stk::mesh::Part *part, const stk::topology &theTopo) {
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
        const double start_time = HOFlowEnv::self().hoflow_time();

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
            stk::all_reduce_max(HOFlowEnv::self().parallel_comm(), &elapsedWallTime, &g_elapsedWallTime, 1);
            // convert to hours
            g_elapsedWallTime /= 3600.0;
            // only force output the first time the timer is exceeded
            if ( g_elapsedWallTime > outputInfo_->userWallTimeResults_.second ) {
                forcedOutput = true;
                outputInfo_->userWallTimeResults_.first = false;
                HOFlowEnv::self().hoflowOutputP0() << "Realm::provide_output()::Forced Result output will be processed at current time: " << currentTime << std::endl;
                HOFlowEnv::self().hoflowOutputP0() <<  " Elapsed (max) WALL time: " << g_elapsedWallTime << " (hours)" << std::endl;
                // provide timer information
                dump_simulation_time();
            }
        }

        const bool isOutput = (timeStepCount >=outputInfo_->outputStart_ && modStep % outputInfo_->outputFreq_ == 0) || forcedOutput;

        if ( isOutput ) {
            HOFlowEnv::self().hoflowOutputP0() << "Realm shall provide output files at : currentTime/timeStepCount: "
                                           << currentTime << "/" <<  timeStepCount << " (" << name_ << ")" << std::endl;      
            // not set up for globals
            ioBroker_->process_output_request(resultsFileIndex_, currentTime);
            
            equationSystems_.provide_output();
        }
    }
}

void Realm::set_global_id()
{
    const stk::mesh::Selector s_universal = metaData_->universal_part();
    stk::mesh::BucketVector const& buckets = bulkData_->get_buckets( stk::topology::NODE_RANK, s_universal );

    for ( stk::mesh::BucketVector::const_iterator ib = buckets.begin(); ib != buckets.end(); ++ib ) {
        const stk::mesh::Bucket & b = **ib;
        const stk::mesh::Bucket::size_type length = b.size();
        stk::mesh::EntityId *hoflowGlobalIds = stk::mesh::field_data(*hoflowGlobalId_, b);

        for ( stk::mesh::Bucket::size_type k = 0; k < length; ++k ) {
            hoflowGlobalIds[k] = bulkData_->identifier(b[k]);
        }
    }
}

const std::vector<std::string> & Realm::get_physics_target_names() {
    return materialProperties_.targetNames_;
}

void Realm::populate_boundary_data() {
    /*// realm first
    for ( size_t k = 0; k < bcDataAlg_.size(); ++k ) {
        bcDataAlg_[k]->execute();
    }
    equationSystems_.populate_boundary_data();*/
}

void Realm::evaluate_properties() {
    double start_time = HOFlowEnv::self().hoflow_time();
    for ( size_t k = 0; k < propertyAlg_.size(); ++k ) {
        propertyAlg_[k]->execute();
    }
    equationSystems_.evaluate_properties();
    double end_time = HOFlowEnv::self().hoflow_time();
    timerPropertyEval_ += (end_time - start_time);
}

stk::mesh::BucketVector const& Realm::get_buckets( stk::mesh::EntityRank rank,
                                                   const stk::mesh::Selector & selector ,
                                                   bool get_all) const
{
    if (metaData_->spatial_dimension() == 3 && rank == stk::topology::EDGE_RANK)
        return bulkData_->get_buckets(rank, selector);

//    if (!get_all && solutionOptions_->useAdapter_ && solutionOptions_->maxRefinementLevel_ > 0) {
//        stk::mesh::Selector new_selector = selector;
//        if (rank != stk::topology::NODE_RANK) {
//            // adapterSelector_ avoids parent elements
//            new_selector = selector & adapterSelector_[rank];
//        }
//        return bulkData_->get_buckets(rank, new_selector);
//    }
//    else {
        return bulkData_->get_buckets(rank, selector);
//    }
}

bool Realm::get_consistent_mass_matrix_png(const std::string dofName ) {
    bool cmmPng = solutionOptions_->consistentMMPngDefault_;
    std::map<std::string, bool>::const_iterator iter = solutionOptions_->consistentMassMatrixPngMap_.find(dofName);
    if (iter != solutionOptions_->consistentMassMatrixPngMap_.end()) {
        cmmPng = (*iter).second;
    }
    
    return cmmPng;
}

bool Realm::has_non_matching_boundary_face_alg() const {
    return false; // hasNonConformal_ | hasOverset_; 
}

bool Realm::get_shifted_grad_op(const std::string dofName ) {
    bool factor = solutionOptions_->shiftedGradOpDefault_;
    std::map<std::string, bool>::const_iterator iter = solutionOptions_->shiftedGradOpMap_.find(dofName);
    if (iter != solutionOptions_->shiftedGradOpMap_.end()) {
        factor = (*iter).second;
    }
    return factor;
}

std::string Realm::get_coordinates_name() {
    return ( (solutionOptions_->meshMotion_ | solutionOptions_->meshDeformation_ | solutionOptions_->externalMeshDeformation_) 
           ? "current_coordinates" : "coordinates");
}

std::string Realm::name() {
  return name_;
}

int Realm::number_of_states() {
  const int numStates = (timeIntegrator_->secondOrderTimeAccurate_) ? 3 : 2;
  return numStates;
}


std::string Realm::physics_part_name(std::string name) const {
    return name;
}

std::vector<std::string> Realm::physics_part_names(std::vector<std::string> names) const {
    return names;
}

bool Realm::get_activate_aura()
{
    return activateAura_;
}

stk::mesh::BulkData & Realm::bulk_data()
{
    return *bulkData_;
}

const stk::mesh::BulkData & Realm::bulk_data() const
{
    return *bulkData_;
}

stk::mesh::MetaData & Realm::meta_data()
{
    return *metaData_;
}

const stk::mesh::MetaData & Realm::meta_data() const
{
    return *metaData_;
}

bool Realm::restarted_simulation()
{
//  return outputInfo_->activateRestart_ ;
    return false; //temp soluition
}

bool Realm::support_inconsistent_restart()
{
//  return supportInconsistentRestart_ ;
    return false; //temp solution
}

const stk::mesh::PartVector & Realm::get_slave_part_vector() {
//    if ( hasPeriodic_)
//        return periodicManager_->get_slave_part_vector();
//    else
        return emptyPartVector_;
}

stk::mesh::Selector Realm::get_inactive_selector() {
    // accumulate inactive parts relative to the universal part

    // provide inactive Overset part that excludes background surface
    //
    // Treat this selector differently because certain entities from interior
    // blocks could have been inactivated by the overset algorithm. 
    stk::mesh::Selector nothing;
//    stk::mesh::Selector inactiveOverSetSelector = (hasOverset_) ?
//        oversetManager_->get_inactive_selector() : nothing;
    stk::mesh::Selector inactiveOverSetSelector = nothing;

    stk::mesh::Selector otherInactiveSelector = (
        metaData_->universal_part()
        & !(stk::mesh::selectUnion(interiorPartVec_))
        & !(stk::mesh::selectUnion(bcPartVec_)));

    if (interiorPartVec_.empty() && bcPartVec_.empty()) {
        otherInactiveSelector = nothing;
    }

    return inactiveOverSetSelector | otherInactiveSelector;
}

void Realm::push_equation_to_systems(EquationSystem * eqSystem){
    equationSystems_.equationSystemVector_.push_back(eqSystem);
}

void Realm::check_job(bool get_node_count) {
  HOFlowEnv::self().hoflowOutputP0() << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "Realm memory Review:       " << name_ << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;

  // set number of nodes, check job run size
  if (get_node_count)
  {
    size_t localNodeCount = ioBroker_->get_input_io_region()->get_property("node_count").get_int();
    stk::all_reduce_sum(HOFlowEnv::self().parallel_comm(), &localNodeCount, &nodeCount_, 1);
    HOFlowEnv::self().hoflowOutputP0() << "Node count from meta data = " << nodeCount_ << std::endl;

    if (doPromotion_) {
      if (metaData_->is_commit()) {
        std::vector<size_t> counts;
        stk::mesh::comm_mesh_counts( *bulkData_ , counts);
        nodeCount_ = counts[0];
        HOFlowEnv::self().hoflowOutputP0() << "Node count after promotion = " << nodeCount_ << std::endl;
      }
      else {
        nodeCount_ = std::pow(promotionOrder_,spatialDimension_) * nodeCount_;
        HOFlowEnv::self().hoflowOutputP0() << "(Roughly) Estimated node count after promotion = " << nodeCount_ << std::endl;
      }
    }
  }

  /// estimate memory based on N*bandwidth, N = nodeCount*nDOF,
  ///   bandwidth = NCon(=27 for Hex mesh)*nDOF - we are very conservative here
  unsigned BWFactor = 27;
  const unsigned MatrixStorageFactor = 3;  // for CRS storage, need one A_IJ, and one I and one J, approx
  SizeType memoryEstimate = 0;
  double procGBScale = double(HOFlowEnv::self().parallel_size())*(1024.*1024.*1024.);
  for (unsigned ieq=0; ieq < equationSystems_.size(); ++ieq)
    {
      if (!equationSystems_[ieq]->linsys_)
        continue;
      SizeType numDof = equationSystems_[ieq]->linsys_->numDof();
      SizeType N = nodeCount_ * numDof;
      SizeType bandwidth = BWFactor * numDof;
      memoryEstimate += MatrixStorageFactor * N*bandwidth * sizeof(double);
    }
  HOFlowEnv::self().hoflowOutputP0() << "Total memory estimate for Matrix solve (per core)= "
                  << double(memoryEstimate)/procGBScale << " GB." << std::endl;

  SizeType memoryEstimateFields = 0;
  if (metaData_->is_commit()) {
    std::vector<size_t> counts;
    stk::mesh::comm_mesh_counts( *bulkData_ , counts);
    ThrowRequire(counts.size() >= 4);
    size_t nodeCount = counts[stk::topology::NODE_RANK];
    size_t edgeCount = counts[stk::topology::EDGE_RANK];
    size_t faceCount = counts[stk::topology::FACE_RANK];
    size_t elemCount = counts[stk::topology::ELEM_RANK];

    const stk::mesh::FieldVector & fields =  metaData_->get_fields();
    unsigned nfields = fields.size();
    for (unsigned ifld = 0; ifld < nfields; ++ifld)  {
      stk::mesh::FieldBase *field = fields[ifld];
      unsigned fszNode = field->max_size(stk::topology::NODE_RANK);
      unsigned fszEdge = field->max_size(stk::topology::EDGE_RANK);
      unsigned fszFace = field->max_size(stk::topology::FACE_RANK);
      unsigned fszElem = field->max_size(stk::topology::ELEM_RANK);

      memoryEstimateFields +=
          ( nodeCount * fszNode
          + edgeCount * fszEdge
          + faceCount * fszFace
          + elemCount * fszElem ) * sizeof(double);
    }
    HOFlowEnv::self().hoflowOutputP0() << "Total memory estimate for Fields (per core)= "
        << double(memoryEstimateFields)/procGBScale << " GB." << std::endl;
    memoryEstimate += memoryEstimateFields;
  }

  HOFlowEnv::self().hoflowOutputP0() << "Total memory estimate (per core) = "
                  << double(memoryEstimate)/procGBScale << " GB." << std::endl;

  if (metaData_->is_commit() && estimateMemoryOnly_) {
    throw std::runtime_error("Job requested memory estimate only, shutting down");
  }

  // here's where we can check for estimated memory > given available memory
  if (availableMemoryPerCoreGB_ != 0
      && double(memoryEstimate)/procGBScale > availableMemoryPerCoreGB_)
    {
      HOFlowEnv::self().hoflowOutputP0() << "ERROR: property available_memory_per_core_GB is set (= " << availableMemoryPerCoreGB_
                      << ") and estimated memory (= " << double(memoryEstimate)/procGBScale
                      << ") is greater,\n job too large to run, \naborting..." << std::endl;
      throw std::runtime_error("Job shutting down");
    }
}

void
Realm::provide_entity_count() {

  std::vector<size_t> counts;
  std::vector<size_t> minCounts;
  std::vector<size_t> maxCounts;
  stk::mesh::comm_mesh_counts( *bulkData_ , counts, minCounts, maxCounts);

  HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "Realm::provide_entity_count:   " << std::endl
		  << "nodes,    " << counts[0] << " min/max: " << minCounts[0] << "/" << maxCounts[0] << std::endl
		  << "edges,    " << counts[1] << " min/max: " << minCounts[1] << "/" << maxCounts[1] << std::endl
		  << "faces,    " << counts[2] << " min/max: " << minCounts[2] << "/" << maxCounts[2] << std::endl
		  << "elements, " << counts[3] << " min/max: " << minCounts[3] << "/" << maxCounts[3] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;

}

void
Realm::pre_timestep_work()
{
  // ask the equation system to do some work
  equationSystems_.pre_timestep_work();
}

void
Realm::dump_simulation_time()
{
  HOFlowEnv::self().hoflowOutputP0() << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "-------------------------------- " << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "Begin Timer Overview for Realm: " << name_ << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "-------------------------------- " << std::endl;

  // equation system time
  equationSystems_.dump_eq_time();

  const int nprocs = HOFlowEnv::self().parallel_size();

  // common
  const unsigned ntimers = 6;
  double total_time[ntimers] = {timerCreateMesh_, timerOutputFields_, timerInitializeEqs_, 
                                timerPropertyEval_, timerPopulateMesh_, timerPopulateFieldData_ };
  double g_min_time[ntimers] = {}, g_max_time[ntimers] = {}, g_total_time[ntimers] = {};

  // get min, max and sum over processes
  stk::all_reduce_min(HOFlowEnv::self().parallel_comm(), &total_time[0], &g_min_time[0], ntimers);
  stk::all_reduce_max(HOFlowEnv::self().parallel_comm(), &total_time[0], &g_max_time[0], ntimers);
  stk::all_reduce_sum(HOFlowEnv::self().parallel_comm(), &total_time[0], &g_total_time[0], ntimers);

  HOFlowEnv::self().hoflowOutputP0() << "Timing for IO: " << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "   io create mesh --  " << " \tavg: " << g_total_time[0]/double(nprocs)
                  << " \tmin: " << g_min_time[0] << " \tmax: " << g_max_time[0] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << " io output fields --  " << " \tavg: " << g_total_time[1]/double(nprocs)
                  << " \tmin: " << g_min_time[1] << " \tmax: " << g_max_time[1] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << " io populate mesh --  " << " \tavg: " << g_total_time[4]/double(nprocs)
                  << " \tmin: " << g_min_time[4] << " \tmax: " << g_max_time[4] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << " io populate fd   --  " << " \tavg: " << g_total_time[5]/double(nprocs)
                  << " \tmin: " << g_min_time[5] << " \tmax: " << g_max_time[5] << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "Timing for connectivity/finalize lysys: " << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "         eqs init --  " << " \tavg: " << g_total_time[2]/double(nprocs)
                  << " \tmin: " << g_min_time[2] << " \tmax: " << g_max_time[2] << std::endl;

  HOFlowEnv::self().hoflowOutputP0() << "Timing for property evaluation:         " << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "            props --  " << " \tavg: " << g_total_time[3]/double(nprocs)
                  << " \tmin: " << g_min_time[3] << " \tmax: " << g_max_time[3] << std::endl;

  // consolidated sort
  if (solutionOptions_->useConsolidatedSolverAlg_ ) {
    double g_totalSort= 0.0, g_minSort= 0.0, g_maxSort= 0.0;
    stk::all_reduce_min(HOFlowEnv::self().parallel_comm(), &timerSortExposedFace_, &g_minSort, 1);
    stk::all_reduce_max(HOFlowEnv::self().parallel_comm(), &timerSortExposedFace_, &g_maxSort, 1);
    stk::all_reduce_sum(HOFlowEnv::self().parallel_comm(), &timerSortExposedFace_, &g_totalSort, 1);
    
    HOFlowEnv::self().hoflowOutputP0() << "Timing for sort_mesh: " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "       sort_mesh  -- " << " \tavg: " << g_totalSort/double(nprocs)
                                   << " \tmin: " << g_minSort<< " \tmax: " << g_maxSort<< std::endl;
  }

  HOFlowEnv::self().hoflowOutputP0() << std::endl;
}

bool
Realm::get_activate_memory_diagnostic()
{
  return activateMemoryDiagnostic_;
}

void
Realm::provide_memory_summary() 
{
  size_t now, hwm;
  stk::get_memory_usage(now, hwm);
  // min, max, sum
  size_t global_now[3] = {now,now,now};
  size_t global_hwm[3] = {hwm,hwm,hwm};
  
  stk::all_reduce(HOFlowEnv::self().parallel_comm(), stk::ReduceSum<1>( &global_now[2] ) );
  stk::all_reduce(HOFlowEnv::self().parallel_comm(), stk::ReduceMin<1>( &global_now[0] ) );
  stk::all_reduce(HOFlowEnv::self().parallel_comm(), stk::ReduceMax<1>( &global_now[1] ) );
  
  stk::all_reduce(HOFlowEnv::self().parallel_comm(), stk::ReduceSum<1>( &global_hwm[2] ) );
  stk::all_reduce(HOFlowEnv::self().parallel_comm(), stk::ReduceMin<1>( &global_hwm[0] ) );
  stk::all_reduce(HOFlowEnv::self().parallel_comm(), stk::ReduceMax<1>( &global_hwm[1] ) );
  
  HOFlowEnv::self().hoflowOutputP0() << "Memory Overview: " << std::endl;
  HOFlowEnv::self().hoflowOutputP0() << "HOFlow memory: total (over all cores) current/high-water mark= "
                                 << std::setw(15) << convert_bytes(global_now[2])
                                 << std::setw(15) << convert_bytes(global_hwm[2])
                                 << std::endl;
  
  HOFlowEnv::self().hoflowOutputP0() << "HOFlow memory:   min (over all cores) current/high-water mark= "
                                 << std::setw(15) << convert_bytes(global_now[0])
                                 << std::setw(15) << convert_bytes(global_hwm[0])
                                 << std::endl;
  
  HOFlowEnv::self().hoflowOutputP0() << "HOFlow memory:   max (over all cores) current/high-water mark= "
                                  << std::setw(15) << convert_bytes(global_now[1])
                                  << std::setw(15) << convert_bytes(global_hwm[1])
                                  << std::endl;
}

//--------------------------------------------------------------------------
//-------- get_current_time() ----------------------------------------------
//--------------------------------------------------------------------------
double
Realm::get_current_time()
{
  return timeIntegrator_->get_current_time();
}

//--------------------------------------------------------------------------
//-------- get_time_step() ----------------------------------------------
//--------------------------------------------------------------------------
double
Realm::get_time_step()
{
  return timeIntegrator_->get_time_step();
}

double 
Realm::get_time_step_from_file() {
  return timeIntegrator_->get_time_step_from_file();
}

bool 
Realm::get_is_fixed_time_step() {
  return timeIntegrator_->get_is_fixed_time_step();
}

bool 
Realm::get_is_terminate_based_on_time() {
  return timeIntegrator_->get_is_terminate_based_on_time();
}

double 
Realm::get_total_sim_time() {
  return timeIntegrator_->get_total_sim_time();
}

int
Realm::get_max_time_step_count() {
  return timeIntegrator_->get_max_time_step_count();
}

//--------------------------------------------------------------------------
//-------- get_gamma1() ----------------------------------------------------
//--------------------------------------------------------------------------
double
Realm::get_gamma1()
{
  return timeIntegrator_->get_gamma1();
}

//--------------------------------------------------------------------------
//-------- get_gamma2() ----------------------------------------------------
//--------------------------------------------------------------------------
double
Realm::get_gamma2()
{
  return timeIntegrator_->get_gamma2();
}

//--------------------------------------------------------------------------
//-------- get_gamma3() ----------------------------------------------------
//--------------------------------------------------------------------------
double
Realm::get_gamma3()
{
  return timeIntegrator_->get_gamma3();
}

//--------------------------------------------------------------------------
//-------- get_time_step_count() ----------------------------------------------
//--------------------------------------------------------------------------
int
Realm::get_time_step_count() const
{
  return timeIntegrator_->get_time_step_count();
}

void
Realm::compute_geometry()
{
  // interior and boundary
  computeGeometryAlgDriver_->execute();
}

std::string Realm::convert_bytes(double bytes)
{
  const double K = 1024;
  const double M = K*1024;
  const double G = M*1024;

  std::ostringstream out;
  if (bytes < K) {
    out << bytes << " B";
  } else if (bytes < M) {
    bytes /= K;
    out << bytes << " K";
  } else if (bytes < G) {
    bytes /= M;
    out << bytes << " M";
  } else {
    bytes /= G;
    out << bytes << " G";
  }
  return out.str();
}

void
Realm::populate_initial_condition()
{
  for ( size_t k = 0; k < initCondAlg_.size(); ++k ) {
    initCondAlg_[k]->execute();
  }
}

//--------------------------------------------------------------------------
//-------- boundary_data_to_state_data -------------------------------------
//--------------------------------------------------------------------------
void
Realm::boundary_data_to_state_data()
{
  equationSystems_.boundary_data_to_state_data();
}

//--------------------------------------------------------------------------
//-------- populate_variables_from_input -----------------------------------
//--------------------------------------------------------------------------
double
Realm::populate_variables_from_input(const double currentTime)
{
  // no reading fields from mesh if this is a restart
  double foundTime = currentTime;
  if ( !restarted_simulation() && solutionOptions_->inputVarFromFileMap_.size() > 0 ) {
    std::vector<stk::io::MeshField> missingFields;
    foundTime = ioBroker_->read_defined_input_fields(solutionOptions_->inputVariablesRestorationTime_, &missingFields);
    if ( missingFields.size() > 0 ) {
      for ( size_t k = 0; k < missingFields.size(); ++k) {
        HOFlowEnv::self().hoflowOutputP0() << "WARNING: Realm::populate_variables_from_input for field "
            << missingFields[k].field()->name()
            << " is missing; will default to IC specification" << std::endl;
      }
    }
    HOFlowEnv::self().hoflowOutputP0() << "Realm::populate_variables_form_input() candidate input time: "
        << foundTime << " for Realm: " << name() << std::endl;
  }
  return foundTime;
}

//--------------------------------------------------------------------------
//-------- populate_derived_quantities -------------------------------------
//--------------------------------------------------------------------------
void
Realm::populate_derived_quantities()
{
  equationSystems_.populate_derived_quantities();
}

//--------------------------------------------------------------------------
//-------- initial_work -----------------------------------------------------
//--------------------------------------------------------------------------
void
Realm::initial_work()
{
  equationSystems_.initial_work();
}

void
Realm::output_banner()
{
    // output some info if you want
}

//--------------------------------------------------------------------------
//-------- compute_adaptive_time_step --------------------------------------
//--------------------------------------------------------------------------
double
Realm::compute_adaptive_time_step()
{
  // extract current time
  const double dtN = get_time_step();

  // do some calculations
  // ...

  return dtN;
}

void
Realm::swap_states()
{
  bulkData_->update_field_data_states();
}

//--------------------------------------------------------------------------
//-------- predict_state ---------------------------------------------------
//--------------------------------------------------------------------------
void
Realm::predict_state()
{
  equationSystems_.predict_state();
}

void
Realm::advance_time_step()
{
  // leave if we do not need to solve
  if ( !active_time_step() )
    return;

  HOFlowEnv::self().hoflowOutputP0() << name_ << "::advance_time_step() " << std::endl;

  HOFlowEnv::self().hoflowOutputP0() << "NLI"
                  << std::setw(8) << std::right << "Name"
                  << std::setw(22) << std::right << "Linear Iter"
                  << std::setw(16) << std::right << "Linear Res"
                  << std::setw(16) << std::right << "NLinear Res"
                  << std::setw(14) << std::right << "Scaled NLR" << std::endl;

  HOFlowEnv::self().hoflowOutputP0() << "---"
                  << std::setw(8) << std::right << "----"
                  << std::setw(22) << std::right << "-----------"
                  << std::setw(16) << std::right << "----------"
                  << std::setw(16) << std::right << "-----------"
                  << std::setw(14) << std::right << "----------" << std::endl;


  // evaluate properties based on latest state including boundary and and possible xfer
  evaluate_properties();

  const int numNonLinearIterations = equationSystems_.maxIterations_;
  for ( int i = 0; i < numNonLinearIterations; ++i ) {
    currentNonlinearIteration_ = i+1;
    HOFlowEnv::self().hoflowOutputP0()
      << currentNonlinearIteration_
      << "/" << numNonLinearIterations
      << std::setw(29) << std::right << "Equation System Iteration" << std::endl;

    isFinalOuterIter_ = ((i+1) == numNonLinearIterations);

    const bool isConverged = equationSystems_.solve_and_update();

    // evaluate properties based on latest np1 solution
    evaluate_properties();

    if ( isConverged ) {
      HOFlowEnv::self().hoflowOutputP0() << "norm convergence criteria met for all equation systems: " << std::endl;
      HOFlowEnv::self().hoflowOutputP0() << "max scaled norm is: " << equationSystems_.provide_system_norm() << std::endl;
      break;
    }
  }

}

bool
Realm::active_time_step()
{
  const int timeStepCount = get_time_step_count();
  const bool activeTimeStep = (timeStepCount % solveFrequency_ ) == 0 ? true : false;
  return activeTimeStep;
}

double
Realm::provide_mean_norm()
{
  return equationSystems_.provide_mean_system_norm();
}

void
Realm::post_converged_work()
{
  equationSystems_.post_converged_work();
}