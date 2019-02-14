/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef MATERIALPROPERTY_H
#define MATERIALPROPERTY_H

#include <Enums.h>
#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>
#include <map>

class MaterialProperties;
class MaterialPropertyData;
class ReferencePropertyData;
class PropertyEvaluator;
class YAML::Node;

//! Stores a material property specified in the input file
class MaterialProperty {
public:
    MaterialProperty(MaterialProperties & materialProperties, const std::string materialBlockName);
    ~MaterialProperty();
    void load(const YAML::Node & node);
    
    MaterialProperties & materialProperties_;
    const std::string materialBlockName_;
    std::string propertyTableName_;
    
    // vectors and maps required to manage full set of options
    std::vector<std::string> targetNames_;
    std::map<std::string, double> universalConstantMap_;
    std::map<PropertyIdentifier, MaterialPropertyData *> propertyDataMap_;
    std::map<std::string, ReferencePropertyData *> referencePropertyDataMap_; /* defines overall species ordering */
    std::map<PropertyIdentifier, PropertyEvaluator *> propertyEvalMap_;
    std::map<std::string, ReferencePropertyData *> tablePropertyMap_;
};

#endif /* MATERIALPROPERTY_H */

