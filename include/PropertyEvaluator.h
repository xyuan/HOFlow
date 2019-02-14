/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef PROPERTYEVALUATOR_H
#define PROPERTYEVALUATOR_H

//! Abstract class for various properties
class PropertyEvaluator {
public:
    PropertyEvaluator();
    ~PropertyEvaluator();
    
//    virtual double execute(double *indVarList, stk::mesh::Entity node = stk::mesh::Entity()) = 0;
};

#endif /* PROPERTYEVALUATOR_H */

