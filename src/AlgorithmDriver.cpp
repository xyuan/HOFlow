/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
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
  // nothing to do
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