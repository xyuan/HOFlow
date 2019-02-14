/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef AUXFUNCTION_H
#define AUXFUNCTION_H

class AuxFunction {
public:
    AuxFunction(const unsigned beginPos, const unsigned endPos);
    ~AuxFunction();
    
protected:
    const unsigned beginPos_;
    const unsigned endPos_;
};

#endif /* AUXFUNCTION_H */

