/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef CONSTANTBCAUXFUNCTIONALGORITHM_H
#define CONSTANTBCAUXFUNCTIONALGORITHM_H

#include <vector>
#include <Algorithm.h>
#include <FieldTypeDef.h>
#include <stk_mesh/base/Types.hpp>

class Realm;
class stk::mesh::Part;
class stk::mesh::FieldBase;
class stk::mesh::Selector;
class AuxFunction;

/**
 * AuxFunctionAlgorithm implementation for the CFX style boundary conditions
 * 
 * The specified boundary value for the boundary part is applied to the boundary
 * entity field, e.g. edge or face, and not on nodes.
 * @param realm
 * @param part
 * @param field
 * @param entityRank
 * @param bc_value
 */
class ConstantBCAuxFunctionAlgorithm : public Algorithm {
public:
    ConstantBCAuxFunctionAlgorithm(Realm & realm,
                                   stk::mesh::Part * part,
                                   ScalarFieldType * field,
                                   stk::mesh::EntityRank entityRank,
                                   double bc_value);
    virtual ~ConstantBCAuxFunctionAlgorithm();
    virtual void execute();

private:
    ScalarFieldType * field_;
    stk::mesh::EntityRank entityRank_;
    double bcValue_;

private:
    // make this non-copyable
    ConstantBCAuxFunctionAlgorithm(const ConstantBCAuxFunctionAlgorithm & other);
    ConstantBCAuxFunctionAlgorithm & operator=(const ConstantBCAuxFunctionAlgorithm & other);
};

#endif /* CONSTANTBCAUXFUNCTIONALGORITHM_H */

