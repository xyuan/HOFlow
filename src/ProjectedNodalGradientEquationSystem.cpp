/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "ProjectedNodalGradientEquationSystem.h"
#include <AssemblePNGBoundarySolverAlgorithm.h>
#include <EquationSystem.h>
#include <EquationSystems.h>
#include <Enums.h>
#include <FieldFunctions.h>
#include <LinearSolvers.h>
#include <LinearSolver.h>
#include <TpetraLinearSolver.h>
#include <LinearSystem.h>
#include <TpetraLinearSystem.h>
#include <HOFlowEnv.h>
#include <Realm.h>
#include <Realms.h>
#include <Simulation.h>
#include <SolutionOptions.h>
#include <SolverAlgorithmDriver.h>

// stk_util
#include <stk_util/parallel/Parallel.hpp>

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/FieldParallel.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/GetEntities.hpp>
#include <stk_mesh/base/CoordinateSystems.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/SkinMesh.hpp>
#include <stk_mesh/base/Comm.hpp>

// stk_io
#include <stk_io/IossBridge.hpp>

#include <stk_topology/topology.hpp>

// stk_util
#include <stk_util/parallel/ParallelReduce.hpp>

#include <string>

ProjectedNodalGradientEquationSystem::ProjectedNodalGradientEquationSystem(EquationSystems & eqSystems,
                                                                            const EquationType eqType,
                                                                            const std::string dofName, 
                                                                            const std::string deltaName, 
                                                                            const std::string independentDofName,
                                                                            const std::string eqSysName,
                                                                            const bool managesSolve) :
    EquationSystem(eqSystems, eqSysName),
    eqType_(eqType),
    dofName_(dofName),
    deltaName_(deltaName),
    independentDofName_(independentDofName),
    eqSysName_(eqSysName),
    managesSolve_(managesSolve),
    dqdx_(NULL),
    qTmp_(NULL)
{
    // extract solver name and solver object
    std::string solverName = realm_.equationSystems_.get_solver_block_name(dofName);
    LinearSolver * solver = realm_.root()->linearSolvers_->create_solver(solverName, eqType_);
    linsys_ = LinearSystem::create(realm_, realm_.spatialDimension_, this, solver);

    // push back EQ to manager
    realm_.push_equation_to_systems(this);
}

ProjectedNodalGradientEquationSystem::~ProjectedNodalGradientEquationSystem() {
}

void ProjectedNodalGradientEquationSystem::set_data_map(BoundaryConditionType BC, std::string name) {
    dataMap_[BC] = name;
}

std::string ProjectedNodalGradientEquationSystem::get_name_given_bc(BoundaryConditionType BC) {
    std::map<BoundaryConditionType, std::string>::iterator it;
    it=dataMap_.find(BC);
    if ( it == dataMap_.end() )
        throw std::runtime_error("PNGEqSys::missing BC type specification (developer error)!");
    else
        return it->second;
}

void ProjectedNodalGradientEquationSystem::register_nodal_fields(stk::mesh::Part *part) {
    stk::mesh::MetaData & meta_data = realm_.meta_data();

    const int nDim = meta_data.spatial_dimension();

    dqdx_ =  &(meta_data.declare_field<VectorFieldType>(stk::topology::NODE_RANK, dofName_));
    stk::mesh::put_field_on_mesh(*dqdx_, *part, nDim, nullptr);

    // delta solution for linear solver
    qTmp_ =  &(meta_data.declare_field<VectorFieldType>(stk::topology::NODE_RANK, deltaName_));
    stk::mesh::put_field_on_mesh(*qTmp_, *part, nDim, nullptr);
}

void ProjectedNodalGradientEquationSystem::register_wall_bc(stk::mesh::Part *part,
                                                            const stk::topology &/*theTopo*/,
                                                            const WallBoundaryConditionData &/*wallBCData*/) {
    const AlgorithmType algType = WALL;

    // extract the field name for this bc type
    std::string fieldName = get_name_given_bc(WALL_BC);
    // create lhs/rhs algorithm;
    std::map<AlgorithmType, SolverAlgorithm *>::iterator its =
      solverAlgDriver_->solverAlgMap_.find(algType);
    if ( its == solverAlgDriver_->solverAlgMap_.end() ) {
        AssemblePNGBoundarySolverAlgorithm * theAlg = new AssemblePNGBoundarySolverAlgorithm(realm_, part, this, fieldName);
        solverAlgDriver_->solverAlgMap_[algType] = theAlg;
    }
    else {
        its->second->partVec_.push_back(part);
    }
}

void ProjectedNodalGradientEquationSystem::initialize() {
    solverAlgDriver_->initialize_connectivity();
    linsys_->finalizeLinearSystem();
}

//--------------------------------------------------------------------------
//-------- solve_and_update ------------------------------------------------
//--------------------------------------------------------------------------
void
ProjectedNodalGradientEquationSystem::solve_and_update()
{
  if ( managesSolve_ )
    solve_and_update_external();
}

//--------------------------------------------------------------------------
//-------- solve_and_update_external ---------------------------------------
//--------------------------------------------------------------------------
void
ProjectedNodalGradientEquationSystem::solve_and_update_external()
{
  for ( int k = 0; k < maxIterations_; ++k ) {

    // projected nodal gradient, load_complete and solve
    assemble_and_solve(qTmp_);
    
    // update
//    double timeA = HOFlowEnv::self().hoflow_time();
    field_axpby(
      realm_.meta_data(),
      realm_.bulk_data(),
      1.0, *qTmp_,
      1.0, *dqdx_, 
      realm_.get_activate_aura());
//    double timeB = NaluEnv::self().nalu_time();
//    timerAssemble_ += (timeB-timeA);   
  }
}