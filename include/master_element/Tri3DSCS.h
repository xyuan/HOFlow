/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef TRI3DSCS_H
#define TRI3DSCS_H

#include<master_element/MasterElement.h>

class Tri3DSCS : public MasterElement
{
public:

  Tri3DSCS();
  virtual ~Tri3DSCS();

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

   double isInElement(
     const double *elemNodalCoord,
     const double *pointCoord,
     double *isoParCoord);

  double parametric_distance(
    const std::vector<double> &x);

  void interpolatePoint(
    const int &nComp,
    const double *isoParCoord,
    const double *field,
    double *result);

  void general_shape_fcn(
    const int numIp,
    const double *isoParCoord,
    double *shpfc);

  void general_normal(
    const double *isoParCoord,
    const double *coords,
    double *normal);
};

#endif /* TRI3DSCS_H */

