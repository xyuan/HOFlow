/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ALGORITHMDRIVER_H
#define ALGORITHMDRIVER_H

#include<Enums.h>

#include<map>

class Realm;
class Algorithm;

class AlgorithmDriver
{
public:

  AlgorithmDriver(
    Realm &realm);
  virtual ~AlgorithmDriver();

  virtual void pre_work(){};
  virtual void execute();
  virtual void post_work(){};

  Realm & realm_;
  std::map<AlgorithmType, Algorithm *> algMap_;
};

#endif /* ALGORITHMDRIVER_H */

