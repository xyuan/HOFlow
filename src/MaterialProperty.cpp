/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "MaterialProperty.h"
#include <yaml-cpp/yaml.h>

MaterialProperty::MaterialProperty(MaterialProperties & materialProperties, const std::string materialBlockName) :
    materialProperties_(materialProperties),
    materialBlockName_(materialBlockName),
    propertyTableName_("na")
{
    // nothing to do
}

MaterialProperty::~MaterialProperty() {
    // delete prop data and evaluator
    std::map<PropertyIdentifier, MaterialPropertyData *>::iterator ii;
    for( ii=propertyDataMap_.begin(); ii!=propertyDataMap_.end(); ++ii )
        delete (*ii).second;

    std::map<PropertyIdentifier, PropertyEvaluator *>::iterator ie;
    for( ie=propertyEvalMap_.begin(); ie!=propertyEvalMap_.end(); ++ie )
        delete (*ie).second;

    // delete reference property map
    std::map<std::string, ReferencePropertyData *>::iterator irf;
    for( irf=referencePropertyDataMap_.begin(); irf!=referencePropertyDataMap_.end(); ++irf )
        delete (*irf).second;
}

void MaterialProperty::load(const YAML::Node & node) {
}

