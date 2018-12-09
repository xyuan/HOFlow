/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef LINEARSOLVER_H
#define LINEARSOLVER_H

#include <yaml-cpp/yaml.h>

class YAML::Node;

class LinearSolver {
public:
    LinearSolver();
    ~LinearSolver();
    void load(const YAML::Node & node);
};

#endif /* LINEARSOLVER_H */

