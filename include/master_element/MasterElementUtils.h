/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef MASTERELEMENTUTILS_H
#define MASTERELEMENTUTILS_H

#include <vector>
#include <array>
#include <limits>

#include <SimdInterface.h>
#include <KokkosInterface.h>

namespace MEconstants {
  static const double realmin = std::numeric_limits<double>::min();
}

  class LagrangeBasis;

  bool isoparameteric_coordinates_for_point_2d(
    LagrangeBasis& basis,
    const double* POINTER_RESTRICT elemNodalCoords,
    const double* POINTER_RESTRICT pointCoord,
    double* POINTER_RESTRICT isoParCoord,
    std::array<double,2> initialGuess,
    int maxIter,
    double tolerance,
    double deltaLimit = 1.0e4
  );

  bool isoparameteric_coordinates_for_point_3d(
    LagrangeBasis& basis,
    const double* POINTER_RESTRICT elemNodalCoords,
    const double* POINTER_RESTRICT pointCoord,
    double* POINTER_RESTRICT isoParCoord,
    std::array<double,3> initialGuess,
    int maxIter,
    double tolerance,
    double deltaLimit = 1.0e4
  );

#endif /* MASTERELEMENTUTILS_H */

