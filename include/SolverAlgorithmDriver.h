/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef SOLVERALGORITHMDRIVER_H
#define SOLVERALGORITHMDRIVER_H

#include<AlgorithmDriver.h>
#include<Enums.h>

#include<map>

class Realm;
class SolverAlgorithm;

/** Container class that stores solver algorithms
 * 
 * The class contains one or multiple solver algorithms.
 * The execute() methods executes all of them between
 * the pre_work() and post_work()
 */
class SolverAlgorithmDriver : public AlgorithmDriver {
public:
    /** Initializes some variables*/
    SolverAlgorithmDriver(Realm &realm);
    
    /** Deletes all algorithms stored*/
    virtual ~SolverAlgorithmDriver();

    /** Calls the initialize_connectivity() method of all
     * algorithms stored
     */
    virtual void initialize_connectivity();
    
    /** Work that has to be done bevor the actual solver algorithm*/
    virtual void pre_work();
    
    /** Executes all algorithms stored between pre_work() and post_work()*/
    virtual void execute();
    
    /** Work that has to be done after the actual solver algorithm*/
    virtual void post_work();

    // different types of algorithms... interior/flux; constraints and dirichlet
    std::map<std::string, SolverAlgorithm *> solverAlgorithmMap_;
    std::map<AlgorithmType, SolverAlgorithm *> solverAlgMap_;
    std::map<AlgorithmType, SolverAlgorithm *> solverConstraintAlgMap_;
    std::map<AlgorithmType, SolverAlgorithm *> solverDirichAlgMap_;
};

#endif /* SOLVERALGORITHMDRIVER_H */

