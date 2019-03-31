/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef MATERIALPROPERTYDATA_H
#define MATERIALPROPERTYDATA_H

#include <Enums.h>

/** Material property data container
 * 
 * For the constant property inmpelented so far
 * not so interesting. Can become more advanced
 * with property tables and functions.
 */
class MaterialPropertyData {
public:
    MaterialPropertyData();
    ~MaterialPropertyData();
    
    MaterialPropertyType type_;
    double constValue_;
};

#endif /* MATERIALPROPERTYDATA_H */

