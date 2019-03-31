/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef REFERENCEPROPERTYDATA_H
#define REFERENCEPROPERTYDATA_H

#include <string>

/** Reference Property Data
 * 
 * @note Seems like this class is not needed anywhere
 *       seriously, can be deleted but safely
 */
class ReferencePropertyData {
public:
    ReferencePropertyData();
    ~ReferencePropertyData();
    
    std::string speciesName_;
};

#endif /* REFERENCEPROPERTYDATA_H */

