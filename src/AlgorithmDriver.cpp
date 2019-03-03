/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AlgorithmDriver.cpp
 * Author: raphi
 * 
 * Created on March 3, 2019, 10:19 AM
 */

#include "AlgorithmDriver.h"

#include <Algorithm.h>
#include <Enums.h>

class Realm;

//==========================================================================
// Class Definition
//==========================================================================
// AlgorithmDriver - Drives algorithms
//==========================================================================
//--------------------------------------------------------------------------
//-------- constructor -----------------------------------------------------
//--------------------------------------------------------------------------
AlgorithmDriver::AlgorithmDriver(
  Realm &realm)
  : realm_(realm)
{
  // does nothing
}

//--------------------------------------------------------------------------
//-------- destructor ------------------------------------------------------
//--------------------------------------------------------------------------
AlgorithmDriver::~AlgorithmDriver()
{
  std::map<AlgorithmType, Algorithm * >::iterator ii;
  for( ii=algMap_.begin(); ii!=algMap_.end(); ++ii ) {
    Algorithm *theAlg = ii->second;
    delete theAlg;
  }
}

//--------------------------------------------------------------------------
//-------- execute ---------------------------------------------------------
//--------------------------------------------------------------------------
void
AlgorithmDriver::execute()
{
  pre_work();

  // assemble
  std::map<AlgorithmType, Algorithm *>::iterator it;
  for ( it = algMap_.begin(); it != algMap_.end(); ++it ) {
    it->second->execute();
  }

  post_work();

}