/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ENUMS_H
#define ENUMS_H

#include <string>

enum AlgorithmType{
    INTERIOR  = 0,
    INFLOW    = 1,
    WALL      = 2,
    WALL_FCN  = 3,
    OPEN      = 4,
    MASS      = 5,
    SRC       = 6,
    SYMMETRY  = 7,
    WALL_HF   = 8,
    WALL_CHT  = 9,
    WALL_RAD  = 10,
    NON_CONFORMAL = 11,
    ELEM_SOURCE = 12,
    OVERSET = 13,
    WALL_ABL = 14,

    /** Set the reference pressure at a node.
     *
     *  Used only for continuity equation system. This needs to be the last
     *  algorithm applied to the linear system because it resets the row and
     *  overwrites contributions from other algorithms at this node.
     *
     * \sa FixPressureAtNodeAlgorithm
     */
    REF_PRESSURE = 15
};

enum EquationType {
    EQ_MOMENTUM = 0,
    EQ_CONTINUITY = 1,
    EQ_MIXTURE_FRACTION = 2,
    EQ_TURBULENT_KE = 3,
    EQ_TEMPERATURE = 4,
    EQ_INTENSITY = 5,
    EQ_ENTHALPY = 6,
    EQ_MESH_DISPLACEMENT = 7,
    EQ_SPEC_DISS_RATE = 8,
    EQ_MASS_FRACTION = 9,
    EQ_PNG   = 10,
    EQ_PNG_P = 11,
    EQ_PNG_Z = 12,
    EQ_PNG_H = 13,
    EQ_PNG_U = 14,
    EQ_PNG_TKE = 15, // FIXME... Last PNG managed like this..
    EquationSystemType_END
};

static const std::string EquationTypeMap[] = {
    "Momentum",
    "Continuity",
    "Mixture_Fraction",
    "Turbulent_KE",
    "Temperature",
    "Intensity",
    "Enthalpy",
    "MeshVelocity",
    "Specific_Dissipation_Rate",
    "Mass_Fraction",
    "PNG",
    "PNG_P",
    "PNG_Z",
    "PNG_H",
    "PNG_U",
    "PNG_TKE"
};

enum PetraType {
    PT_TPETRA,       //!< Nalu Tpetra interface
    PT_END
};

enum DOFStatus {
    DS_NotSet           = 0,
    DS_SkippedDOF       = 1 << 1,
    DS_OwnedDOF         = 1 << 2,
    DS_SharedNotOwnedDOF = 1 << 3,
    DS_GhostedDOF       = 1 << 4
};

enum BoundaryConditionType{
    INFLOW_BC    = 1,
    OPEN_BC      = 2,
    WALL_BC      = 3,
    SYMMETRY_BC  = 4,
    PERIODIC_BC  = 5,
    NON_CONFORMAL_BC = 6,
    OVERSET_BC = 7
};

enum UserDataType {
    CONSTANT_UD = 0,
    FUNCTION_UD = 1,
    USER_SUB_UD = 2,
    UserDataType_END
};

// prop enum and name below
enum PropertyIdentifier {
    DENSITY_ID = 0,
    VISCOSITY_ID = 1,
    SPEC_HEAT_ID = 2,
    THERMAL_COND_ID = 3,
    ABSORBTION_COEFF_ID = 4,
    ENTHALPY_ID = 5,
    LAME_MU_ID = 6,
    LAME_LAMBDA_ID = 7,
    SCATTERING_COEFF_ID = 8,
    PropertyIdentifier_END
};

// prop enum and name below
enum  MaterialPropertyType {
    CONSTANT_MAT = 0,
    MIXFRAC_MAT = 1,
    POLYNOMIAL_MAT = 2,
    IDEAL_GAS_MAT = 3,
    GEOMETRIC_MAT = 4,
    HDF5_TABLE_MAT = 5,
    GENERIC = 6,
    MaterialPropertyType_END
};

static const std::string PropertyIdentifierNames[] = {
    "density",
    "viscosity",
    "specific_heat",
    "thermal_conductivity",
    "absorption_coefficient",
    "enthalpy",
    "lame_mu",
    "lame_lambda",
    "scattering_coefficient"
};

#endif /* ENUMS_H */

