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

#endif /* ENUMS_H */

