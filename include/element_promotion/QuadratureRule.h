/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef QUADRATURERULE_H
#define QUADRATURERULE_H

#include <vector>
#include <Teuchos_SerialDenseVector.hpp>

  // <abscissae, weights>
  std::pair<std::vector<double>, std::vector<double>>
  gauss_legendre_rule(int order);

  // <abscissae, weights>
  std::pair<std::vector<double>, std::vector<double>>
  gauss_lobatto_legendre_rule(int order, double xleft = -1.0, double xright = +1.0);

  // <abscissae, weights>
  std::pair<std::vector<double>, std::vector<double>>
  SGL_quadrature_rule(int order, std::vector<double> scsEndLocations);

  // a vector with -1 added at the first entry and +1 added at the last entry
  std::vector<double> pad_end_points(std::vector<double> x, double xleft = -1.0, double xright = +1.0);

#endif /* QUADRATURERULE_H */

