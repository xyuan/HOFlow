/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef MATERIALPROPERTIES_H
#define MATERIALPROPERTIES_H

//! Stores one or multiple material properties (objects of the class MaterialProperty)
class MaterialProperties {
public:
    MaterialProperties();
    ~MaterialProperties();
    void load(const YAML::Node & node);
};

#endif /* MATERIALPROPERTIES_H */

