/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HOFLOWENV_H
#define HOFLOWENV_H

#include<iostream>

class HOFlowEnv {
public:
    HOFlowEnv();
    ~HOFlowEnv();
    static HOFlowEnv & self();
    std::ostream & hoflowOutputP0();
    
    std::ostream *hoflowLogStream_;
};

#endif /* HOFLOWENV_H */

