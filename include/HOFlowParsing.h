/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HOFLOWPARSING_H
#define HOFLOWPARSING_H

#include <Enums.h>
#include <string>
#include <map>
#include <vector>
#include <yaml-cpp/yaml.h>

// our data types
struct Temperature {
    double temperature_;
    Temperature() : 
        temperature_(0.0)
    {}
};

struct Coordinates {
    double x_, y_, z_;
    Coordinates() : 
        x_(0.0),
        y_(0.0), 
        z_(0.0)
    {}
};

// base class
struct UserData {
    std::map<std::string, bool> bcDataSpecifiedMap_;
    std::map<std::string, UserDataType> bcDataTypeMap_;
    std::map<std::string, std::string> userFunctionMap_;
    std::map<std::string, std::vector<double> > functionParams_;
    std::map<std::string, std::vector<std::string> > functionStringParams_;

    // FIXME: must elevate temperature due to the temperature_bc_setup method
    Temperature temperature_;
    bool tempSpec_; 
    bool externalData_;
    UserData() : 
        tempSpec_(false), 
        externalData_(false) 
    {}
};

struct WallUserData : public UserData {
    bool isAdiabatic_;
    WallUserData() : 
        UserData(),
        isAdiabatic_(false)
    {}
};

// initial conditions
struct ConstantInitialConditionData : public InitialCondition {
    std::vector<std::string> fieldNames_;
    std::vector<std::vector<double> > data_;
    ConstantInitialConditionData(InitialConditions& ics) : 
        InitialCondition(ics) 
    {}
};

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

struct WallBoundaryConditionData : public BoundaryCondition {
    WallBoundaryConditionData(BoundaryConditions & bcs) : 
        BoundaryCondition(bcs)
    {};
    WallUserData userData_;
};

template<> struct convert<WallUserData> {
    static bool decode(const YAML::Node & node, WallUserData & rhs);
};

class HOFlowParsing {
public:
    HOFlowParsing();
    ~HOFlowParsing();
};

#endif /* HOFLOWPARSING_H */

