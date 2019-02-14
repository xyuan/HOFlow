/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef AUXFUNCTIONALGORITHM_H
#define AUXFUNCTIONALGORITHM_H

#include <vector>
#include <Algorithm.h>
#include <stk_mesh/base/Types.hpp>

class Realm;
class stk::mesh::Part;
class stk::mesh::FieldBase;
class stk::mesh::Selector;
class AuxFunction;

//typedef std::vector<stk::mesh::Part *> PartVector;

class AuxFunctionAlgorithm : public Algorithm {
public:
    AuxFunctionAlgorithm(Realm & realm,
                         stk::mesh::Part * part,
                         stk::mesh::FieldBase * field,
                         AuxFunction * auxFunction,
                         stk::mesh::EntityRank entityRank);
    virtual ~AuxFunctionAlgorithm();

private:
    stk::mesh::FieldBase * field_;
    AuxFunction * auxFunction_;
    stk::mesh::EntityRank entityRank_;

private:
    // make this non-copyable
    AuxFunctionAlgorithm(const AuxFunctionAlgorithm & other);
    AuxFunctionAlgorithm & operator=(const AuxFunctionAlgorithm & other);
};

#endif /* AUXFUNCTIONALGORITHM_H */

