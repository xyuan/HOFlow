/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "master_element/Tri2DSCV.h"

#include <master_element/MasterElementFunctions.h>

//#include <master_element/MasterElementHO.h>
#include <master_element/MasterElementUtils.h>

#include <element_promotion/LagrangeBasis.h>
//#include <element_promotion/TensorProductQuadratureRule.h>
#include <element_promotion/QuadratureRule.h>
#include <AlgTraits.h>

#include <HOFlowEnv.h>
#include <FORTRAN_Proto.h>

#include <stk_util/util/ReportHandler.hpp>
#include <stk_topology/topology.hpp>

#include <iostream>

#include <cmath>
#include <limits>
#include <array>
#include <map>
#include <memory>

//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
Tri2DSCV::Tri2DSCV()
  : MasterElement()
{
  nDim_ = 2;
  nodesPerElement_ = 3;
  numIntPoints_ = 3;

  // define ip node mappings
  ipNodeMap_.resize(3);
  ipNodeMap_[0] = 0; ipNodeMap_[1] = 1; ipNodeMap_[2] = 2;

  const double seven36ths = 7.0/36.0;
  const double eleven18ths = 11.0/18.0;
  intgLoc_ = {
    seven36ths, seven36ths,
    eleven18ths, seven36ths,
    seven36ths, eleven18ths
  };

  intgLocShift_ = {
      0.0,  0.0,
      1.0,  0.0,
      0.0,  1.0
  };
}

//--------------------------------------------------------------------------
//-------- destructor ------------------------------------------------------
//--------------------------------------------------------------------------
Tri2DSCV::~Tri2DSCV()
{
  // does nothing
}

//--------------------------------------------------------------------------
//-------- ipNodeMap -------------------------------------------------------
//--------------------------------------------------------------------------
const int *
Tri2DSCV::ipNodeMap(
  int /*ordinal*/)
{
  // define scv->node mappings
  return &ipNodeMap_[0];
}

//--------------------------------------------------------------------------
//-------- shape_fcn -------------------------------------------------------
//--------------------------------------------------------------------------
void
Tri2DSCV::shape_fcn(double *shpfc)
{
  tri_shape_fcn(numIntPoints_, &intgLoc_[0], shpfc);
}

//--------------------------------------------------------------------------
//-------- shifted_shape_fcn -----------------------------------------------
//--------------------------------------------------------------------------
void
Tri2DSCV::shifted_shape_fcn(double *shpfc)
{
  tri_shape_fcn(numIntPoints_, &intgLocShift_[0], shpfc);
}

//--------------------------------------------------------------------------
//-------- tri_shape_fcn ---------------------------------------------------
//--------------------------------------------------------------------------
void
Tri2DSCV::tri_shape_fcn(
  const int  &npts,
  const double *isoParCoord,
  double *shape_fcn)
{
  for (int j = 0; j < npts; ++j ) {
    const int threej = 3*j;
    const int k = 2*j;
    const double xi = isoParCoord[k];
    const double eta = isoParCoord[k+1];
    shape_fcn[threej] = 1.0 - xi - eta;
    shape_fcn[1 + threej] = xi;
    shape_fcn[2 + threej] = eta;
  }
}

//--------------------------------------------------------------------------
//-------- determinant -----------------------------------------------------
//--------------------------------------------------------------------------
void Tri2DSCV::determinant(
  const int nelem,
  const double *coords,
  double *volume,
  double *error)
{
  int lerr = 0;

  SIERRA_FORTRAN(tri_scv_det)
    ( &nelem, &nodesPerElement_, &numIntPoints_, coords,
      volume, error, &lerr );
}