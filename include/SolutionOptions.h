/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef SOLUTIONOPTIONS_H
#define SOLUTIONOPTIONS_H

#include <yaml-cpp/yaml.h>

class YAML::Node;

class SolutionOptions {
public:
    SolutionOptions();
    ~SolutionOptions();
    void load(const YAML::Node & y_node);
};

#endif /* SOLUTIONOPTIONS_H */

