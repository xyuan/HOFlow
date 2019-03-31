/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "HOFlowParsing.h"
#include <stk_util/util/ReportHandler.hpp>
#include <cctype>
#include <algorithm>
#include <Simulation.h>

const YAML::Node expect_type(const YAML::Node & node, const std::string & key, YAML::NodeType::value type, bool optional) {
    static std::string types[] = { "Undefined", "Null", "Scalar", "Sequence", "Map" };
    std::ostringstream err_msg;

    if (node[key]) {
        const YAML::Node value = node[key];

        if ((value.Type() != type)) {
            if (!HOFlowEnv::self().parallel_rank()) {
                err_msg << "Error: parsing expected type " << types[type] << " got type = " << types[value.Type()]
                << " for key= " << key
                << " at " << HOFlowParsingHelper::line_info(node)
                << " node= " << std::endl;
                HOFlowParsingHelper::emit(err_msg, node);
                err_msg << "Check indentation of input file.";
                std::cout << err_msg.str() << std::endl;
            }
            throw std::runtime_error(err_msg.str());
        }
        return value;
    } else {
        if ((!optional)) {
            if (!HOFlowEnv::self().parallel_rank()) {
                err_msg << "Error: parsing expected required value " << key
                << " but it was not found at"
                << HOFlowParsingHelper::line_info(node) << " for Node= "
                << std::endl;
                HOFlowParsingHelper::emit(err_msg, node);
                std::cout << err_msg.str() << std::endl;
            }
        throw std::runtime_error(err_msg.str());
        }
        return node[key];
    }
}

const YAML::Node expect_null(const YAML::Node & node, const std::string & key, bool optional) {
    return expect_type(node, key, YAML::NodeType::Null, optional);
}

const YAML::Node expect_scalar(const YAML::Node & node, const std::string & key, bool optional) {
    return expect_type(node, key, YAML::NodeType::Scalar, optional);
}

const YAML::Node expect_sequence(const YAML::Node & node, const std::string & key, bool optional) {
    return expect_type(node, key, YAML::NodeType::Sequence, optional);
}

void operator >>(const YAML::Node & node, WallBoundaryConditionData & wallBC) {
    wallBC.bcName_ = node["wall_boundary_condition"].as<std::string>();
    wallBC.targetName_ = node["target_name"].as<std::string>();
    wallBC.theBcType_ = WALL_BC;
    const YAML::Node wallUserData = node["wall_user_data"];
    wallBC.userData_ = wallUserData.as<WallUserData>();
    // check for typical rogue line command
    if (node["user_function_name"])
        throw std::runtime_error("user_function_data is misplaced; it must be under wall_user_data");
 }

const YAML::Node expect_map(const YAML::Node & node, const std::string & key, bool optional) {
    return expect_type(node, key, YAML::NodeType::Map, optional);
}

void operator >>(const YAML::Node & node, ConstantInitialConditionData & constIC) {
    constIC.theIcType_ = CONSTANT_UD;
    constIC.icName_ = node["constant"].as<std::string>();
    const YAML::Node & targets = node["target_name"];

    if (targets.Type() == YAML::NodeType::Scalar) {
        constIC.targetNames_.resize(1);
        constIC.targetNames_[0] = targets.as<std::string>();
        HOFlowEnv::self().hoflowOutputP0() << "constant IC: name: " << constIC.icName_ << " , target[" << 0 << "] = " << constIC.targetNames_[0] << std::endl;
        if (constIC.targetNames_[0].find(',') != std::string::npos) {
            throw std::runtime_error(
                "In " + constIC.icName_
                + " found ',' in target name - you must enclose in '[...]' for multiple targets");
        }
    } else {
      constIC.targetNames_.resize(targets.size());
      for (size_t i = 0; i < targets.size(); ++i) {
        constIC.targetNames_[i] = targets[i].as<std::string>();
        if (constIC.root()->debug())
            HOFlowEnv::self().hoflowOutputP0() << "constant IC: name: " << constIC.icName_ << " , target[" << i << "] = " << constIC.targetNames_[i] << std::endl;
        }
      }
    const YAML::Node value_node = node["value"];
    size_t value_size = value_node.size();
    constIC.fieldNames_.resize(value_size);
    constIC.data_.resize(value_size);
    if (constIC.root()->debug()) {
        HOFlowEnv::self().hoflowOutputP0() << "fieldNames_.size()= " << constIC.fieldNames_.size()
        << " value.size= " << constIC.data_.size() << std::endl;
    }
    size_t jv = 0;
    for (YAML::const_iterator i = value_node.begin(); i != value_node.end(); ++i, ++jv) {
        const YAML::Node key = i->first;
        const YAML::Node value = i->second;
        constIC.fieldNames_[jv] = key.as<std::string>();
        size_t nvals = value.size();
        if (nvals) {
            constIC.data_[jv].resize(nvals);
            for (size_t iv = 0; iv < nvals; ++iv) {
                constIC.data_[jv][iv] = value[iv].as<double>();
                if (constIC.root()->debug()) {
                    HOFlowEnv::self().hoflowOutputP0() << "fieldNames_= " << constIC.fieldNames_[jv] << " value= "
                    << constIC.data_[jv][iv] << std::endl;
                }
            }
        } else {
            constIC.data_[jv].resize(1);
            constIC.data_[jv][0] = value.as<double>();
            if (constIC.root()->debug()) {
                HOFlowEnv::self().hoflowOutputP0() << "fieldNames_= " << constIC.fieldNames_[jv] << " value= " << constIC.data_[jv][0] << std::endl;
            }
        }
    }
}

void operator >>(const YAML::Node & node, std::map<std::string, bool> & mapName) {
    for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
        const YAML::Node & key = i->first;
        const YAML::Node & value = i->second;
        std::string stringName;
        stringName = key.as<std::string>();
        bool data;
        data = value.as<bool>();
        mapName[stringName] = data;
    }
}

void operator >>(const YAML::Node & node, std::map<std::string, double> & mapName) {
    for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
        const YAML::Node & key = i->first;
        const YAML::Node & value = i->second;
        std::string stringName;
        stringName = key.as<std::string>();
        double data;
        data = value.as<double>();
        mapName[stringName] = data;
    }
}

void operator >>(const YAML::Node & node, std::map<std::string, std::string> & mapName) {
    for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
        const YAML::Node & key = i->first;
        const YAML::Node & value = i->second;
        std::string stringName;
        stringName = key.as<std::string>();
        std::string data;
        data = value.as<std::string>();
        mapName[stringName] = data;
    }
}

void operator >>(const YAML::Node & node, std::map<std::string, std::vector<std::string> >& mapName) {
    for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
        const YAML::Node & key = i->first;
        const YAML::Node & targets = i->second;
        std::string stringName;
        stringName = key.as<std::string>();

        std::vector < std::string > &vecOfStrings = mapName[stringName];
        std::string theName;
        if (targets.Type() == YAML::NodeType::Scalar) {
            theName = targets.as<std::string>();
            vecOfStrings.push_back(theName);
        } else {
            for (size_t it = 0; it < targets.size(); ++it) {
                theName = targets[it].as<std::string>();
                vecOfStrings.push_back(theName);
            }
        }
    }
}

void operator >>(const YAML::Node & node, std::map<std::string, std::vector<double> >& mapName) {
    for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
        const YAML::Node & key = i->first;
        const YAML::Node & targets = i->second;
        std::string stringName;
        stringName = key.as<std::string>();

        std::vector<double> & vecOfDoubles = mapName[stringName];
        double value;
        if (targets.Type() == YAML::NodeType::Scalar) {
            value = targets.as<double>();
            vecOfDoubles.push_back(value);
        } else {
            for (size_t it = 0; it < targets.size(); ++it) {
                value = targets[it].as<double>();
                vecOfDoubles.push_back(value);
            }
        }
    }
}

bool case_insensitive_compare(std::string s1, std::string s2) {
    std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
    return (s1 == s2);
}

bool YAML::convert<Coordinates>::decode(const YAML::Node & node, Coordinates & cx) {
    if (!node.IsSequence() || node.size() < 2) {
        return false;
    }

    cx.x_ = node[0].as<double>();
    cx.y_ = node[1].as<double>();
    if (node.size() > 2)
        cx.z_ = node[2].as<double>();

    return true;
}

bool YAML::convert<Temperature>::decode(const YAML::Node & node, Temperature & t) {
    if (!node.IsScalar()) {
        return false;
    }

    t.temperature_ = node.as<double>();

    return true;
  }

bool YAML::convert<WallUserData>::decode(const YAML::Node & node, WallUserData & wallData) {
    if (node["temperature"]) {
        wallData.temperature_ = node["temperature"].as<Temperature>();
        wallData.bcDataSpecifiedMap_["temperature"] = true;
        wallData.bcDataTypeMap_["temperature"] = CONSTANT_UD;
        wallData.tempSpec_ = true;
    }
    
    if (node["adiabatic"]) {
        wallData.isAdiabatic_ = node["adiabatic"].as<bool>();
    }

    // function data
    const bool optional = true;
    const YAML::Node userFcnNode = expect_map(node, "user_function_name", optional);
    if (userFcnNode) {
        for (YAML::const_iterator i = userFcnNode.begin(); i != userFcnNode.end(); ++i) {
            const YAML::Node & key = i->first;
            const YAML::Node & value = i->second;
            std::string stringName = key.as<std::string>();
            std::string data = value.as<std::string>();
            wallData.bcDataSpecifiedMap_[stringName] = true;
            wallData.bcDataTypeMap_[stringName] = FUNCTION_UD;
            wallData.userFunctionMap_[stringName] = data;
        }

        // extract function name and parameters
        if (expect_map(node, "user_function_parameters", true)) {
            wallData.functionParams_ = node["user_function_parameters"].as<std::map<std::string, std::vector<double> > >();
        }

        // extract function name and string parameters
        if (expect_map(node, "user_function_string_parameters", true)) {
            wallData.functionStringParams_ =
                node["user_function_string_parameters"].as<std::map<std::string, std::vector<std::string> > >();
        }
    }
    
    return true;
}

bool YAML::convert<BoundaryConditionOptions>::decode(const YAML::Node & node, BoundaryConditionOptions & bcOptions) {
    bcOptions.bcSetName_ = node["boundary_conditions"].as<std::string>();
    node["wall_boundary_condition"] >> bcOptions.wallbc_;

    return true;
}

bool YAML::convert<std::map<std::string, std::vector<std::string> > >::decode(const YAML::Node & node, std::map<std::string, std::vector<std::string> >& mapName) {
    for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
        const YAML::Node & key = i->first;
        const YAML::Node & targets = i->second;
        std::string stringName;
        stringName = key.as<std::string>();

        std::vector < std::string > &vecOfStrings = mapName[stringName];
        std::string theName;
        if (targets.Type() == YAML::NodeType::Scalar) {
            theName = targets.as<std::string>();
            vecOfStrings.push_back(theName);
        } else {
            for (size_t it = 0; it < targets.size(); ++it) {
                theName = targets[it].as<std::string>();
                vecOfStrings.push_back(theName);
            }
        }
    }
    
    return true;
}