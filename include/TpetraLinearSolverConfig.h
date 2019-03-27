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

/** A specialized class of LinearSolverConfig that stores the configuration for a TpetraLinearSolver
 *
 * This configuration is based on the information in the input file. The configuration
 * is unique for a specific linear solver created (object) and not type (class).
 */
class TpetraLinearSolverConfig : public LinearSolverConfig {
public:
    TpetraLinearSolverConfig();
    virtual ~TpetraLinearSolverConfig();
    
    /** Reads all parameters specified in the input file
     *
     * All parameters specified in the coresponding section
     * in the input file are read. If a parameter is not specified,
     * default values are used.
     */
    virtual void load(const YAML::Node & node) final;
};

#endif /* TPETRALINEARSOLVERCONFIG_H */

