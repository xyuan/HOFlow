/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HOFLOWPARSING_H
#define HOFLOWPARSING_H

#include <string>
#include <yaml-cpp/yaml.h>

/// Set @param result if the @param key is present in the @param node, else set it to the given default value
template<typename T>
void get_if_present(const YAML::Node & node, const std::string& key, T& result, const T& default_if_not_present = T()) {
    if (node[key]) {
        const YAML::Node value = node[key];
        result = value.as<T>();
    }
    else {
        result = default_if_not_present;
    }
}
/// this version doesn't change @param result unless the @param key is present in the @param node
template<typename T>
void get_if_present_no_default(const YAML::Node & node, const std::string& key, T& result) {
    if (node[key]) {
        const YAML::Node value = node[key];
        result = value.as<T>();
    }
}

class HOFlowParsing {
public:
    HOFlowParsing();
    ~HOFlowParsing();
};

#endif /* HOFLOWPARSING_H */

