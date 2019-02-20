/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "HOFlowParsingHelper.h"
#include <fstream>
#include <string>
#include <vector>

HOFlowParsingHelper::HOFlowParsingHelper() {
}

HOFlowParsingHelper::~HOFlowParsingHelper() {
}

void HOFlowParsingHelper::emit(YAML::Emitter & emout, const YAML::Node & node) {
    // recursive depth first
    YAML::NodeType::value type = node.Type();
    std::string out;
    switch (type) {
        case YAML::NodeType::Scalar:
            out = node.as<std::string>();
            emout << out;
            break;
        case YAML::NodeType::Sequence:
            emout << YAML::BeginSeq;
            for (unsigned int i = 0; i < node.size(); ++i) {
                const YAML::Node subnode = node[i] ;
                emit(emout, subnode);
            }
            emout << YAML::EndSeq;
            break;
        case YAML::NodeType::Map:
            emout << YAML::BeginMap ;
            for (YAML::const_iterator i = node.begin(); i != node.end(); ++i) {
                const YAML::Node key   = i->first;
                const YAML::Node value = i->second;
                out = key.as<std::string>() ;
                emout << YAML::Key << out;
                emout << YAML::Value;
                emit(emout, value);
            }
            emout << YAML::EndMap ;
            break;
        case YAML::NodeType::Null:
            emout << " (empty) ";
            break;
        default:
            std::cerr << "Warning: emit: unknown/unsupported node type" << std::endl;
            break;
    }
}


void HOFlowParsingHelper::emit(std::ostream & sout, const YAML::Node & node) {
    YAML::Emitter out;
    emit(out,node);
    sout << out.c_str() << std::endl;
}

std::string HOFlowParsingHelper::line_info(const YAML::Node & node) {
    std::ostringstream sout;
    sout << "(pos,line,column) = ("
         << node.Mark().pos << ", "
         << node.Mark().line << ", "
         << node.Mark().column << ")" ;
    //	 << "Unknown for now" ;
    return sout.str();
}

std::string HOFlowParsingHelper::info(const YAML::Node & node) {
    std::ostringstream sout;
    sout << "Node at " << line_info(node) << " => \n" ;
    emit(sout, node);
    return sout.str();
}

