/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef FIELDTYPEDEF_H
#define FIELDTYPEDEF_H

#include <stk_mesh/base/FieldBase.hpp>
#include <stk_mesh/base/CoordinateSystems.hpp>

// define scalar field typedef
typedef stk::mesh::Field<double>  ScalarFieldType;
typedef stk::mesh::Field<stk::mesh::EntityId> GlobalIdFieldType;
typedef stk::mesh::Field<int>  ScalarIntFieldType;

// define vector field typedef; however, what is the value of Cartesian?
typedef stk::mesh::Field<double, stk::mesh::Cartesian>  VectorFieldType;

// define generic
typedef stk::mesh::Field<double, stk::mesh::SimpleArrayTag>  GenericFieldType;

// field type for local ids
typedef unsigned LocalId;
typedef stk::mesh::Field<LocalId>  LocalIdFieldType;

#endif /* FIELDTYPEDEF_H */

