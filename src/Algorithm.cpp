/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "Algorithm.h"
#include <SupplementalAlgorithm.h>
#include <kernel/Kernel.h>

//! Constructor if part is provided
Algorithm::Algorithm(Realm & realm, stk::mesh::Part * part) :
    realm_(realm)
{
    // push back on partVec
    partVec_.push_back(part);
}

//! Constructor if part vector is provided
Algorithm::Algorithm(Realm & realm, stk::mesh::PartVector & partVec) :
    realm_(realm),
    partVec_(partVec)
{
    // nothing to do
}

Algorithm::~Algorithm()
{
    std::vector<SupplementalAlgorithm *>::iterator ii;
    for( ii=supplementalAlg_.begin(); ii!=supplementalAlg_.end(); ++ii )
        delete *ii;

    std::vector<Kernel*>::iterator ij;
    for (ij = activeKernels_.begin(); ij != activeKernels_.end(); ++ij)
        delete *ij;
}
