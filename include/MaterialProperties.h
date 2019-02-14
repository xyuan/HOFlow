/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef MATERIALPROPERTIES_H
#define MATERIALPROPERTIES_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>

class Realm;
class MaterialProperty;
class YAML::Node;

typedef std::vector<MaterialProperty *> MaterialPropertyVector;

//! Stores one or multiple material properties (objects of the class MaterialProperty)
class MaterialProperties {
public:
    MaterialProperties(Realm & realm);
    ~MaterialProperties();
    void load(const YAML::Node & node);
    
    //! Ease of access function, an object of MaterialProperties can treated like an array and its elements accessed
    size_t size() { return materialPropertyVector_.size(); }
    
    //! Ease of access function, an object of MaterialProperties can treated like an array and its elements accessed
    MaterialProperty *operator[](int i) { return materialPropertyVector_[i];}
    
    Realm & realm_;
    MaterialPropertyVector materialPropertyVector_;
    std::vector<std::string> targetNames_;
};

#endif /* MATERIALPROPERTIES_H */

