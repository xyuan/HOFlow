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
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <InitialConditions.h>
#include <InitialCondition.h>
#include <BoundaryConditions.h>
#include <BoundaryCondition.h>
#include <HOFlowEnv.h>
#include <HOFlowParsingHelper.h>

class YAML::Node;

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

struct WallBoundaryConditionData : public BoundaryCondition {
    WallBoundaryConditionData(BoundaryConditions & bcs) : BoundaryCondition(bcs) {};
    WallUserData userData_;
};

struct BoundaryConditionOptions {
    std::string bcSetName_;
    WallBoundaryConditionData wallbc_;
};

// initial conditions
struct ConstantInitialConditionData : public InitialCondition {
    ConstantInitialConditionData(InitialConditions & ics) : InitialCondition(ics) {}
    std::vector<std::string> fieldNames_;
    std::vector<std::vector<double> > data_;
};

/// Set @param result if the @param key is present in the @param node, else set it to the given default value
template<typename T>
void get_if_present(const YAML::Node & node, const std::string & key, T & result, const T & default_if_not_present = T()) {
    if (node[key]) {
        const YAML::Node value = node[key];
        result = value.as<T>();
    }
    else {
        result = default_if_not_present;
    }
};

/// this version doesn't change @param result unless the @param key is present in the @param node
template<typename T>
void get_if_present_no_default(const YAML::Node & node, const std::string & key, T & result) {
    if (node[key]) {
        const YAML::Node value = node[key];
        result = value.as<T>();
    }
};

/// this version requires the @param key to be present
template<typename T> void get_required(const YAML::Node & node, const std::string & key, T & result) {
    if (node[key]) {
        const YAML::Node value = node[key];
        result = value.as<T>();
    }
    else {
        if (!HOFlowEnv::self().parallel_rank()) {
            std::ostringstream err_msg;
            err_msg << "\n\nError: parsing missing required key: " << key 
                    << " at " << HOFlowParsingHelper::line_info(node)
                    << " for node= " << std::endl;
            HOFlowParsingHelper::emit(err_msg, node);
            std::cout << err_msg.str() << std::endl;
        }
        throw std::runtime_error("Error: parsing missing required key: " + key);
    }
};

const YAML::Node expect_type(const YAML::Node & node, const std::string & key, YAML::NodeType::value type, bool optional=false);
const YAML::Node expect_null(const YAML::Node & node, const std::string & key, bool optional=false);
const YAML::Node expect_scalar(const YAML::Node & node, const std::string & key, bool optional=false);
const YAML::Node expect_sequence(const YAML::Node & node, const std::string & key, bool optional=false);
const YAML::Node expect_map(const YAML::Node & node, const std::string & key, bool optional=false);

void operator >> (const YAML::Node & node, WallBoundaryConditionData & rhs);
void operator >> (const YAML::Node & node, ConstantInitialConditionData & rhs);

void operator >> (const YAML::Node & node, std::map<std::string,bool>& mapName);
void operator >> (const YAML::Node & node, std::map<std::string,double>& mapName);
void operator >> (const YAML::Node & node, std::map<std::string,std::string>& mapName);
void operator >> (const YAML::Node & node, std::map<std::string,std::vector<std::string> >& mapName);
void operator >> (const YAML::Node & node, std::map<std::string,std::vector<double> >& mapName);

bool case_insensitive_compare(std::string s1, std::string s2);

template<> struct YAML::convert<Coordinates> {
    static bool decode(const YAML::Node & node, Coordinates & rhs);
};

template<> struct YAML::convert<Temperature> {
    static bool decode(const YAML::Node & node, Temperature & rhs);
};

template<> struct YAML::convert<UserData> {
    static bool decode(const YAML::Node & node, UserData & rhs);
};

template<> struct YAML::convert<WallUserData> {
    static bool decode(const YAML::Node & node, WallUserData & rhs);
};

template<> struct YAML::convert<BoundaryConditionOptions> {
    static bool decode(const YAML::Node & node, BoundaryConditionOptions & rhs);
};

template<> struct YAML::convert<std::map<std::string,std::vector<std::string> > > {
    static bool decode(const YAML::Node & node, std::map<std::string,std::vector<std::string> >& t);
};

#endif /* HOFLOWPARSING_H */