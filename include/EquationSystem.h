/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef EQUATIONSYSTEM_H
#define EQUATIONSYSTEM_H

//! Stores a equation system specified in the input file
class EquationSystem {
public:
    EquationSystem();;
    ~EquationSystem();
    
    virtual void register_wall_bc(stk::mesh::Part *part, 
                                  const stk::topology &theTopo, 
                                  const WallBoundaryConditionData &wallBCData) 
    {}
};

#endif /* EQUATIONSYSTEM_H */

