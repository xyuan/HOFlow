/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "MaterialProperties.h"
#include <HOFlowParsing.h>
#include <MaterialProperty.h>

MaterialProperties::MaterialProperties(Realm & realm) :
    realm_(realm)
{
}

MaterialProperties::~MaterialProperties() {
    for ( size_t i = 0; i < materialPropertyVector_.size(); ++i ) {
        delete materialPropertyVector_[i];
    }
}

void MaterialProperties::load(const YAML::Node & node) {
    const YAML::Node y_material_propertys = node["material_properties"];
    if (y_material_propertys) {

        // support two input file options
        const YAML::Node y_props = expect_sequence(y_material_propertys, "properties", true);
        if (y_props) {
            for (size_t iprop = 0; iprop < y_props.size(); ++iprop) {
                const YAML::Node y_prop = y_props[iprop] ;

                // extract name
                std::string materialBlockName = "na";
                get_required(y_prop, "name", materialBlockName);

                // create a new material property object
                MaterialProperty *matPropBlock = new MaterialProperty(*this, materialBlockName);

                // load and push back
                matPropBlock->load(y_prop);
                materialPropertyVector_.push_back(matPropBlock);

                // push back target names to material propertys
                targetNames_.insert(targetNames_.begin(), matPropBlock->targetNames_.begin(), matPropBlock->targetNames_.end()); 
            }
        }
        else {
            // create a new material property object with default name
            MaterialProperty *matPropBlock = new MaterialProperty(*this, "default");

            // load and push back
            matPropBlock->load(y_material_propertys);
            materialPropertyVector_.push_back(matPropBlock);

            // push back target names to material propertys
            targetNames_.insert(targetNames_.begin(), matPropBlock->targetNames_.begin(), matPropBlock->targetNames_.end()); 
        }
    }
    else {
        throw std::runtime_error("Error: material_properties::load(): 'material_properties' line does not exist");
    }
}
