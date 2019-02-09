/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ENUMS_H
#define ENUMS_H

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

#endif /* ENUMS_H */

