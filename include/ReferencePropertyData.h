/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef REFERENCEPROPERTYDATA_H
#define REFERENCEPROPERTYDATA_H

#include <string>

class ReferencePropertyData {
public:
    ReferencePropertyData();
    ~ReferencePropertyData();
    
    std::string speciesName_;
};

#endif /* REFERENCEPROPERTYDATA_H */

