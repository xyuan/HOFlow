/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AlgorithmDriver.h
 * Author: raphi
 *
 * Created on March 3, 2019, 10:19 AM
 */

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

