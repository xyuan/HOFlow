/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HOFLOWPARSINGHELPER_H
#define HOFLOWPARSINGHELPER_H

#include <yaml-cpp/yaml.h>
#include <iostream>

class HOFlowParsingHelper {
public:
    HOFlowParsingHelper();
    ~HOFlowParsingHelper();
    
    static void emit(YAML::Emitter& emout, const YAML::Node & node);
    /// uses Emitter to print node to stream
    static void emit(std::ostream & sout, const YAML::Node & node);
    static std::string line_info(const YAML::Node & node);
};

#endif /* HOFLOWPARSINGHELPER_H */

