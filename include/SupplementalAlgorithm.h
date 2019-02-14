/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef SUPPLEMENTALALGORITHM_H
#define SUPPLEMENTALALGORITHM_H

class Realm;

class SupplementalAlgorithm {
public:
    SupplementalAlgorithm(Realm & realm);
    ~SupplementalAlgorithm();
    
    Realm & realm_;
};

#endif /* SUPPLEMENTALALGORITHM_H */

