/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CopyFieldAlgorithm.h
 * Author: raphi
 *
 * Created on March 3, 2019, 11:33 AM
 */

#ifndef COPYFIELDALGORITHM_H
#define COPYFIELDALGORITHM_H

#include <Algorithm.h>

#include <vector>

#include <stk_mesh/base/Types.hpp>

namespace stk{
namespace mesh{
class Part;
class FieldBase;
class Selector;

typedef std::vector< Part * > PartVector;
}
}

class CopyFieldAlgorithm : public Algorithm
{
public:

  CopyFieldAlgorithm(
    Realm &realm,
    stk::mesh::Part *part,
    stk::mesh::FieldBase * fromField,
    stk::mesh::FieldBase * toField,
    const unsigned beginPos,
    const unsigned endPos,
    const stk::mesh::EntityRank entityRank);
  
  virtual ~CopyFieldAlgorithm() {}
  virtual void execute();

private:
  stk::mesh::FieldBase * fromField_;
  stk::mesh::FieldBase * toField_;

  const unsigned beginPos_;
  const unsigned endPos_;
  const stk::mesh::EntityRank entityRank_;
  
private:
  // make this non-copyable
  CopyFieldAlgorithm(const CopyFieldAlgorithm & other);
  CopyFieldAlgorithm & operator=(const CopyFieldAlgorithm & other);
};

#endif /* COPYFIELDALGORITHM_H */

