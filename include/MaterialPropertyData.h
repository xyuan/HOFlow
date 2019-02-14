/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef MATERIALPROPERTYDATA_H
#define MATERIALPROPERTYDATA_H

#include <Enums.h>

class MaterialPropertyData {
public:
    MaterialPropertyData();
    ~MaterialPropertyData();
    
    MaterialPropertyType type_;
    double constValue_;
};

#endif /* MATERIALPROPERTYDATA_H */

