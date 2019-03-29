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

class AlgorithmDriver {
public:
    /** Initializes some variables*/
    AlgorithmDriver(Realm &realm);
    
    /** Delets all created algorithms*/
    virtual ~AlgorithmDriver();

    /** Work to be done befor algorithm*/
    virtual void pre_work(){};
    
    /** Executes pre_work, all algorithms in algMap and post_work*/
    virtual void execute();
    
    /** Work to be done after algorithm*/
    virtual void post_work(){};

    Realm & realm_;
    std::map<AlgorithmType, Algorithm *> algMap_;
};

#endif /* ALGORITHMDRIVER_H */

