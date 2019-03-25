/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "SolutionOptions.h"
#include <Enums.h>
#include <HOFlowEnv.h>

// basic c++
#include <stdexcept>
#include <utility>

//==========================================================================
// Class Definition
//==========================================================================
// SolutionOptions - holder for user options at the realm scope
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
SolutionOptions::SolutionOptions()
  : hybridDefault_(0.0),
    alphaDefault_(0.0),
    alphaUpwDefault_(1.0),
    upwDefault_(1.0),
    lamScDefault_(1.0),
    turbScDefault_(1.0),
    turbPrDefault_(1.0),
    nocDefault_(true),
    shiftedGradOpDefault_(false),
    skewSymmetricDefault_(false),
    tanhFormDefault_("classic"),
    tanhTransDefault_(2.0),
    tanhWidthDefault_(4.0),
    referenceDensity_(0.0),
    referenceTemperature_(298.0),
    thermalExpansionCoeff_(1.0),
    stefanBoltzmann_(5.6704e-8),
    includeDivU_(0.0),
    isTurbulent_(false),
//    turbulenceModel_(LAMINAR),
    meshMotion_(false),
    meshDeformation_(false),
    externalMeshDeformation_(false),
    activateUniformRefinement_(false),
    uniformRefineSaveAfter_(false),
    activateAdaptivity_(false),
//    errorIndicatorType_(EIT_NONE),
    adaptivityFrequency_(0),
    useMarker_(false),
    refineFraction_(0.0),
    unrefineFraction_(0.0),
    physicalErrIndCriterion_(0.0),
    physicalErrIndUnrefCriterionMultipler_(1.0),
    maxRefinementNumberOfElementsFraction_(0),
    adapterExtraOutput_(false),
    useAdapter_(false),
    maxRefinementLevel_(0),
    ncAlgGaussLabatto_(true),
    ncAlgUpwindAdvection_(true),
    ncAlgIncludePstab_(true),
    ncAlgDetailedOutput_(false),
    ncAlgCoincidentNodesErrorCheck_(false),
    ncAlgCurrentNormal_(false),
    ncAlgPngPenalty_(true),
    cvfemShiftMdot_(false),
    cvfemReducedSensPoisson_(false),
    inputVariablesRestorationTime_(1.0e8),
    inputVariablesInterpolateInTime_(false),
    inputVariablesPeriodicTime_(0.0),
    consistentMMPngDefault_(false),
    useConsolidatedSolverAlg_(false),
    useConsolidatedBcSolverAlg_(false),
    eigenvaluePerturb_(false),
    eigenvaluePerturbDelta_(0.0),
    eigenvaluePerturbBiasTowards_(3),
    eigenvaluePerturbTurbKe_(0.0),
    earthAngularVelocity_(7.2921159e-5),
    latitude_(0.0),
    raBoussinesqTimeScale_(-1.0),
    mdotAlgAccumulation_(0.0),
    mdotAlgInflow_(0.0),
    mdotAlgOpen_(0.0),
    quadType_("GaussLegendre")
    //accousticallyCompressible_(false)
{
    // nothing to do
}

//--------------------------------------------------------------------------
//-------- destructor ------------------------------------------------------
//--------------------------------------------------------------------------
SolutionOptions::~SolutionOptions()
{
    // nothing to do
}

//--------------------------------------------------------------------------
//-------- load ------------------------------------------------------------
//--------------------------------------------------------------------------
void SolutionOptions::load(const YAML::Node & y_node) {
    const bool optional=true;
    const bool required=!optional;

    const YAML::Node y_solution_options = expect_map(y_node,"solution_options", optional);
    if(y_solution_options) {
        get_required(y_solution_options, "name", name_);

        // divU factor for stress
        get_if_present(y_solution_options, "divU_stress_scaling", includeDivU_, includeDivU_);

        // external mesh motion expected
        get_if_present(y_solution_options, "externally_provided_mesh_deformation", externalMeshDeformation_, externalMeshDeformation_);

        // shift mdot for continuity (CVFEM)
        get_if_present(y_solution_options, "shift_cvfem_mdot", cvfemShiftMdot_, cvfemShiftMdot_);

        // DEPRECATED shifted CVFEM pressure poisson;
        if ( y_solution_options["shift_cvfem_poisson"] ) {
          throw std::runtime_error("shift_cvfem_poisson line command is deprecated to the generalized solution options block command, shifted_gradient_operator");
        }

        // Reduce sensitivities for CVFEM PPE: M delta_p = b - A p; M has quadrature at edge midpoints of the element
        get_if_present(y_solution_options, "reduced_sens_cvfem_poisson", cvfemReducedSensPoisson_, cvfemReducedSensPoisson_);

        // check for consolidated solver alg (AssembleSolver)
        get_if_present(y_solution_options, "use_consolidated_solver_algorithm", useConsolidatedSolverAlg_, useConsolidatedSolverAlg_);

        // check for consolidated face-elem bc alg
        get_if_present(y_solution_options, "use_consolidated_face_elem_bc_algorithm", useConsolidatedBcSolverAlg_, useConsolidatedBcSolverAlg_);

        // eigenvalue purturbation; over all dofs...
        get_if_present(y_solution_options, "eigenvalue_perturbation", eigenvaluePerturb_);
        get_if_present(y_solution_options, "eigenvalue_perturbation_delta", eigenvaluePerturbDelta_);
        get_if_present(y_solution_options, "eigenvalue_perturbation_bias_towards", eigenvaluePerturbBiasTowards_);
        get_if_present(y_solution_options, "eigenvalue_perturbation_turbulent_ke", eigenvaluePerturbTurbKe_);

        // quadrature type for high order
        get_if_present(y_solution_options, "high_order_quadrature_type", quadType_);

        // extract turbulence model; would be nice if we could parse an enum..
        std::string specifiedTurbModel;
        std::string defaultTurbModel = "laminar";
        get_if_present(y_solution_options,
            "turbulence_model", specifiedTurbModel, defaultTurbModel);

        // initialize turbuelnce constants since some laminar models may need such variables, e.g., kappa
        initialize_turbulence_constants();

        // extract possible copy from input fields restoration time
        get_if_present(y_solution_options, "input_variables_from_file_restoration_time",
          inputVariablesRestorationTime_, inputVariablesRestorationTime_);

        // choice of interpolation or snapping to closest in the data base
        get_if_present(y_solution_options, "input_variables_interpolate_in_time",
          inputVariablesInterpolateInTime_, inputVariablesInterpolateInTime_);

        // allow for periodic sampling in time
        get_if_present(y_solution_options, "input_variables_from_file_periodic_time",
          inputVariablesPeriodicTime_, inputVariablesPeriodicTime_);

        // first set of options; hybrid, source, etc.
        const YAML::Node y_options = expect_sequence(y_solution_options, "options", required);
        if (y_options)
        {
            for (size_t ioption = 0; ioption < y_options.size(); ++ioption)
            {
              const YAML::Node y_option = y_options[ioption] ;
              if (expect_map(y_option, "hybrid_factor", optional)) {
                y_option["hybrid_factor"] >> hybridMap_ ;
              }
              else if (expect_map(y_option, "alpha", optional)) {
                y_option["alpha"] >> alphaMap_ ;
              }
              else if (expect_map(y_option, "alpha_upw", optional)) {
                y_option["alpha_upw"] >> alphaUpwMap_;
              }
              else if (expect_map(y_option, "upw_factor", optional)) {
                y_option["upw_factor"] >> upwMap_ ;
              }
              else if (expect_map(y_option, "limiter", optional)) {
                y_option["limiter"] >> limiterMap_ ;
              }
              else if (expect_map( y_option, "laminar_schmidt", optional)) {
                y_option["laminar_schmidt"] >> lamScMap_ ;
              }
              else if (expect_map( y_option, "laminar_prandtl", optional)) {
                y_option["laminar_prandtl"] >> lamPrMap_ ;
              }
              else if (expect_map( y_option, "turbulent_schmidt", optional)) {
                y_option["turbulent_schmidt"] >> turbScMap_ ;
              }
              else if (expect_map( y_option, "turbulent_prandtl", optional)) {
                y_option["turbulent_prandtl"] >> turbPrMap_ ;
              }
              else if (expect_map( y_option, "source_terms", optional)) {
                const YAML::Node ySrc = y_option["source_terms"];
                ySrc >> srcTermsMap_ ;
              }
              else if (expect_map( y_option, "element_source_terms", optional)) {
                const YAML::Node ySrc = y_option["element_source_terms"];
                ySrc >> elemSrcTermsMap_ ;
              }
              else if (expect_map( y_option, "source_term_parameters", optional)) {
                y_option["source_term_parameters"] >> srcTermParamMap_ ;
              }
              else if (expect_map( y_option, "element_source_term_parameters", optional)) {
                y_option["element_source_term_parameters"] >> elemSrcTermParamMap_ ;
              }
              else if (expect_map( y_option, "projected_nodal_gradient", optional)) {
                y_option["projected_nodal_gradient"] >> nodalGradMap_ ;
              }
              else if (expect_map( y_option, "noc_correction", optional)) {
                y_option["noc_correction"] >> nocMap_ ;
              }
              else if (expect_map( y_option, "shifted_gradient_operator", optional)) {
                y_option["shifted_gradient_operator"] >> shiftedGradOpMap_ ;
              }
              else if (expect_map( y_option, "skew_symmetric_advection", optional)) {
                y_option["skew_symmetric_advection"] >> skewSymmetricMap_;
              }
              else if (expect_map( y_option, "input_variables_from_file", optional)) {
                y_option["input_variables_from_file"] >> inputVarFromFileMap_ ;
              }
              else if (expect_map( y_option, "user_constants", optional)) {
                const YAML::Node y_user_constants = y_option["user_constants"];
                get_if_present(y_user_constants, "reference_density",  referenceDensity_, referenceDensity_);
                get_if_present(y_user_constants, "reference_temperature",  referenceTemperature_, referenceTemperature_);
                get_if_present(y_user_constants, "thermal_expansion_coefficient",  thermalExpansionCoeff_, thermalExpansionCoeff_);
                get_if_present(y_user_constants, "stefan_boltzmann",  stefanBoltzmann_, stefanBoltzmann_);
                get_if_present(y_user_constants, "earth_angular_velocity", earthAngularVelocity_, earthAngularVelocity_);
                get_if_present(y_user_constants, "latitude", latitude_, latitude_);
                get_if_present(y_user_constants, "boussinesq_time_scale", raBoussinesqTimeScale_, raBoussinesqTimeScale_);

                if (expect_sequence( y_user_constants, "gravity", optional) ) {
                  const int gravSize = y_user_constants["gravity"].size();
                  gravity_.resize(gravSize);
                  for (int i = 0; i < gravSize; ++i ) {
                    gravity_[i] = y_user_constants["gravity"][i].as<double>() ;
                  }
                }
                if (expect_sequence( y_user_constants, "east_vector", optional) ) {
                  const int vecSize = y_user_constants["east_vector"].size();
                  eastVector_.resize(vecSize);
                  for (int i = 0; i < vecSize; ++i ) {
                    eastVector_[i] = y_user_constants["east_vector"][i].as<double>() ;
                    //y_user_constants["east_vector"][i] >> eastVector_[i];
                  }
                }
                if (expect_sequence( y_user_constants, "north_vector", optional) ) {
                  const int vecSize = y_user_constants["north_vector"].size();
                  northVector_.resize(vecSize);
                  for (int i = 0; i < vecSize; ++i ) {
                    northVector_[i] = y_user_constants["north_vector"][i].as<double>() ;
                    //y_user_constants["north_vector"][i] >> northVector_[i];
                  }
                }
              }
              else if (expect_map( y_option, "non_conformal", optional)) {
                const YAML::Node y_nc = y_option["non_conformal"];
                get_if_present(y_nc, "gauss_labatto_quadrature",  ncAlgGaussLabatto_, ncAlgGaussLabatto_);
                get_if_present(y_nc, "upwind_advection",  ncAlgUpwindAdvection_, ncAlgUpwindAdvection_);
                get_if_present(y_nc, "include_pstab",  ncAlgIncludePstab_, ncAlgIncludePstab_);
                get_if_present(y_nc, "detailed_output",  ncAlgDetailedOutput_, ncAlgDetailedOutput_);
                get_if_present(y_nc, "activate_coincident_node_error_check",  ncAlgCoincidentNodesErrorCheck_, ncAlgCoincidentNodesErrorCheck_);
                get_if_present(y_nc, "current_normal",  ncAlgCurrentNormal_, ncAlgCurrentNormal_);
                get_if_present(y_nc, "include_png_penalty",  ncAlgPngPenalty_, ncAlgPngPenalty_);
              }
              else if (expect_map( y_option, "peclet_function_form", optional)) {
                y_option["peclet_function_form"] >> tanhFormMap_ ;
              }
              else if (expect_map( y_option, "peclet_function_tanh_transition", optional)) {
                y_option["peclet_function_tanh_transition"] >> tanhTransMap_ ;
              }
              else if (expect_map( y_option, "peclet_function_tanh_width", optional)) {
                y_option["peclet_function_tanh_width"] >> tanhWidthMap_ ;
              }
              // overload line command, however, push to the same tanh data structure
              else if (expect_map( y_option, "blending_function_form", optional)) {
                y_option["blending_function_form"] >> tanhFormMap_ ;
              }
              else if (expect_map( y_option, "tanh_transition", optional)) {
                y_option["tanh_transition"] >> tanhTransMap_ ;
              }
              else if (expect_map( y_option, "tanh_width", optional)) {
                y_option["tanh_width"] >> tanhWidthMap_ ;
              }
              else if (expect_map( y_option, "consistent_mass_matrix_png", optional)) {
                y_option["consistent_mass_matrix_png"] >> consistentMassMatrixPngMap_ ;
              }
              else {
                  if (!HOFlowEnv::self().parallel_rank())
                  {
                    std::cout << "Error: parsing at " << HOFlowParsingHelper::info(y_option)
                      //<< "... at parent ... " << HOFlowParsingHelper::info(y_node)
                              << std::endl;
                  }
                  throw std::runtime_error("unknown solution option: "+ HOFlowParsingHelper::info(y_option));
              }
            }
        }
    }

    // over view PPE specifications
    HOFlowEnv::self().hoflowOutputP0() << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "PPE review:   " << std::endl;
    HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;

    if ( cvfemShiftMdot_ )
      HOFlowEnv::self().hoflowOutputP0() << "Shifted CVFEM mass flow rate" << std::endl;
    if ( cvfemReducedSensPoisson_)
      HOFlowEnv::self().hoflowOutputP0() << "Reduced sensitivities CVFEM Poisson" << std::endl;

    // sanity checks; if user asked for shifted Poisson, then user will have reduced sensitivities
    if ( get_shifted_grad_op("pressure") ) {
      if ( !cvfemReducedSensPoisson_) {
        HOFlowEnv::self().hoflowOutputP0() << "Reduced sensitivities CVFEM Poisson will be set since reduced grad_op is requested" << std::endl;
        cvfemReducedSensPoisson_ = true;
      }
    }

    // overview gradient operator for CVFEM
    if ( shiftedGradOpMap_.size() > 0 ) {
      HOFlowEnv::self().hoflowOutputP0() << std::endl;
      HOFlowEnv::self().hoflowOutputP0() << "CVFEM gradient operator review:   " << std::endl;
      HOFlowEnv::self().hoflowOutputP0() << "===========================" << std::endl;
      for ( const auto& shiftIt : shiftedGradOpMap_ ) {
        HOFlowEnv::self().hoflowOutputP0() << " dof: " << shiftIt.first
                                       << " shifted: " << (shiftIt.second ? "yes" : "no") << std::endl; 
      }
    }
}

double
SolutionOptions::get_alpha_factor(const std::string& dofName) const
{
  double factor = alphaDefault_;
  auto iter = alphaMap_.find(dofName);

  if (iter != alphaMap_.end())
    factor = iter->second;

  return factor;
}

double
SolutionOptions::get_alpha_upw_factor(const std::string& dofName) const
{
  double factor = alphaUpwDefault_;
  auto iter = alphaUpwMap_.find(dofName);

  if (iter != alphaUpwMap_.end())
    factor = iter->second;

  return factor;
}

double
SolutionOptions::get_upw_factor(const std::string& dofName) const
{
  double factor = upwDefault_;
  auto iter = upwMap_.find(dofName);

  if (iter != upwMap_.end())
    factor = iter->second;

  return factor;
}

bool
SolutionOptions::primitive_uses_limiter(const std::string& dofName) const
{
  bool usesIt = false;
  auto iter = limiterMap_.find(dofName);
  if (iter != limiterMap_.end())
    usesIt = iter->second;

  return usesIt;
}

bool
SolutionOptions::get_shifted_grad_op(const std::string& dofName) const
{
  bool factor = shiftedGradOpDefault_;
  auto iter = shiftedGradOpMap_.find(dofName);

  if (iter != shiftedGradOpMap_.end())
    factor = iter->second;

  return factor;
}

bool
SolutionOptions::get_skew_symmetric(const std::string& dofName) const
{
  bool factor = skewSymmetricDefault_;
  auto iter = skewSymmetricMap_.find(dofName);

  if (iter != skewSymmetricMap_.end())
    factor = iter->second;

  return factor;
}

std::vector<double>
SolutionOptions::get_gravity_vector(const unsigned nDim) const
{
  if ( nDim != gravity_.size() )
    throw std::runtime_error("SolutionOptions::get_gravity_vector():Error Expected size does not equaly nDim");
  else
    return gravity_;
}

bool
SolutionOptions::get_noc_usage(
  const std::string &dofName ) const
{
  bool factor = nocDefault_;
  std::map<std::string, bool>::const_iterator iter
    = nocMap_.find(dofName);
  if (iter != nocMap_.end()) {
    factor = (*iter).second;
  }
  return factor;
}

bool SolutionOptions::has_set_boussinesq_time_scale()
{
  return (raBoussinesqTimeScale_ > std::numeric_limits<double>::min());
}