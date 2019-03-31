/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef PROPERTYEVALUATOR_H
#define PROPERTYEVALUATOR_H

#include <stk_mesh/base/Entity.hpp>

/** Abstract class to evaluate a property
 * 
 * @note Seems like this is not needed anywhere
 *       -> can be deleted but safely
 */
class PropertyEvaluator {
public:
    PropertyEvaluator();
    virtual ~PropertyEvaluator();
    
    virtual double execute(double *indVarList, stk::mesh::Entity node = stk::mesh::Entity()) = 0;
};

#endif /* PROPERTYEVALUATOR_H */

