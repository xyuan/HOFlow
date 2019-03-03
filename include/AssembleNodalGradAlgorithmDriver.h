/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ASSEMBLENODALGRADALGORITHMDRIVER_H
#define ASSEMBLENODALGRADALGORITHMDRIVER_H

#include <AlgorithmDriver.h>
#include <string>

class Realm;

class AssembleNodalGradAlgorithmDriver : public AlgorithmDriver
{
public:

  AssembleNodalGradAlgorithmDriver(
    Realm &realm,
    const std::string & scalarQName,
    const std::string & dqdxName);
  ~AssembleNodalGradAlgorithmDriver();

  void pre_work();
  void post_work();

  const std::string scalarQName_;
  const std::string dqdxName_;
  
};

#endif /* ASSEMBLENODALGRADALGORITHMDRIVER_H */

