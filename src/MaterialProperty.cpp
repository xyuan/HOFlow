/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "MaterialProperty.h"
#include <yaml-cpp/yaml.h>

#include <MaterialPropertyData.h>
#include <ReferencePropertyData.h>
#include <PropertyEvaluator.h>
#include <HOFlowEnv.h>
#include <HOFlowParsing.h>

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

void MaterialProperty::load(const YAML::Node & y_prop) {
    // extract the set of target names
    const YAML::Node targets = y_prop["target_name"];
    if (targets.Type() == YAML::NodeType::Scalar) {
        targetNames_.resize(1);
        targetNames_[0] = targets.as<std::string>() ;
    }
    else {
        targetNames_.resize(targets.size());
        for (size_t i=0; i < targets.size(); ++i) {
            targetNames_[i] = targets[i].as<std::string>() ;
        }
    }

    // property constants
    const YAML::Node y_constants = expect_map(y_prop, "constant_specification", true);
    if ( y_constants ) {
        universalConstantMap_ = y_constants.as<std::map<std::string, double> >() ;
    }

    // extract the sequence of types
    const YAML::Node y_specs = expect_sequence(y_prop, "specifications", false);
    if (y_specs) {
        for (size_t ispec = 0; ispec < y_specs.size(); ++ispec) {
            const YAML::Node y_spec = y_specs[ispec];

            // new the info object
            MaterialPropertyData * matData = new MaterialPropertyData();

            std::string thePropName;
            std::string thePropType;
            get_required(y_spec, "name", thePropName);
            get_required(y_spec, "type", thePropType);

            // extract propery enum
            bool foundIt = false;
            PropertyIdentifier thePropEnum = PropertyIdentifier_END;
            for ( int k=0; k < PropertyIdentifier_END; ++k ) {
                if ( thePropName == PropertyIdentifierNames[k] ) {
                    thePropEnum = PropertyIdentifier(k);
                    foundIt = true;
                    break;
                }
            }
            if ( !foundIt ) {
                throw std::runtime_error("could not find property name from enum list");
            }

            if ( thePropType == "constant" ) {
                matData->type_ = CONSTANT_MAT;

                // check for standard constant
                const YAML::Node standardConst = y_spec["value"];
                if ( standardConst ) {
                    double theValue = 0.0;
                    theValue = standardConst.as<double>() ;
                    matData->constValue_ = theValue;
                    HOFlowEnv::self().hoflowOutputP0() << thePropName
                                                   << " is a constant property: " << theValue << std::endl;
                }
            }
            else {
              throw std::runtime_error("unknown property type");  
            }

            // store the map
            propertyDataMap_[thePropEnum] = matData;
        }  
    } 
}

