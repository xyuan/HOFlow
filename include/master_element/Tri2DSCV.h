/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef TRI2DSCV_H
#define TRI2DSCV_H

#include<master_element/MasterElement.h>

class Tri2DSCV : public MasterElement
{
public:
  Tri2DSCV();
  virtual ~Tri2DSCV();

  const int * ipNodeMap(int ordinal = 0);

  void determinant(
    const int nelem,
    const double *coords,
    double *areav,
    double * error );

  void shape_fcn(
    double *shpfc);

  void shifted_shape_fcn(
    double *shpfc);

  void tri_shape_fcn(
    const int &npts,
    const double *par_coord,
    double* shape_fcn);
};

#endif /* TRI2DSCV_H */

