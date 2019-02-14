/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <Realm.h>

class Realm;
class SupplementalAlgorithm;
class stk::mesh::Part;

typedef std::vector<stk::mesh::Part *> PartVector;

//! Abstract class for various algorithms
class Algorithm {
public:
    Algorithm(Realm & realm, stk::mesh::Part * part);
    Algorithm(Realm & realm, stk::mesh::PartVector & partVec);
    virtual ~Algorithm();
//    virtual void execute() = 0;
//    virtual void pre_work() {}
    
    Realm & realm_;
    PartVector partVec_;
    std::vector<SupplementalAlgorithm *> supplementalAlg_;
    //std::vector<Kernel *> activeKernels_;
};

#endif /* ALGORITHM_H */

