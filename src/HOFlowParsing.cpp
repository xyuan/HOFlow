/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "HOFlowParsing.h"
#include <HOFlowParsingHelper.h>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <HOFlowEnv.h>

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

const YAML::Node expect_sequence(const YAML::Node & node, const std::string & key, bool optional) {
    return expect_type(node, key, YAML::NodeType::Sequence, optional);
}

