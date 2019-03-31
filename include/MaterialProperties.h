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
class Simulation;
class MaterialProperty;
class YAML::Node;

typedef std::vector<MaterialProperty *> MaterialPropertyVector;

/** Container class to store multiple MaterialProperty objects
 * 
 * Reads the input file and creates a new MaterialProperty object
 * for each property specified.
 */
class MaterialProperties {
public:
    /** Initializes some variables*/
    MaterialProperties(Realm & realm);
    
    /** Deletes all MaterialProperty objects created*/
    ~MaterialProperties();
    
    /** Reads the input file and creates a MaterialProperty object for each
     * property specified. Adds the object to the materialPropertyVector_
     */
    void load(const YAML::Node & node);
    
    Simulation * root();
    Realm * parent();  
    
    /** Ease of access function, an object of MaterialProperties can treated 
     * like an array and its elements accessed
     */
    size_t size() { return materialPropertyVector_.size(); }
    
    /** Ease of access function, an object of MaterialProperties can treated 
     * like an array and its elements accessed
     */
    MaterialProperty *operator[](int i) { return materialPropertyVector_[i];}
    
    Realm & realm_;
    MaterialPropertyVector materialPropertyVector_;
    std::vector<std::string> targetNames_;
};

#endif /* MATERIALPROPERTIES_H */

