/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef TPETRALINEARSOLVERCONFIG_H
#define TPETRALINEARSOLVERCONFIG_H

#include <yaml-cpp/yaml.h>
#include <LinearSolverConfig.h>

#include <string>

class LinearSolverConfig;

class TpetraLinearSolverConfig : public LinearSolverConfig {
public:
    TpetraLinearSolverConfig();
    ~TpetraLinearSolverConfig();
    virtual void load(const YAML::Node& node) final;
};

#endif /* TPETRALINEARSOLVERCONFIG_H */

