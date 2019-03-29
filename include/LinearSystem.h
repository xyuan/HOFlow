/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef LINEARSYSTEM_H
#define LINEARSYSTEM_H

#include <LinearSolverTypes.h>
#include <KokkosInterface.h>

// stk_util
#include <stk_util/parallel/Parallel.hpp>
#include <stk_util/environment/WallTime.hpp>
#include <stk_util/environment/perf_util.hpp>
#include <stk_util/environment/FileUtils.hpp>

// stk_mesh/base/fem
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/FieldParallel.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/GetEntities.hpp>
#include <stk_mesh/base/CoordinateSystems.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Comm.hpp>
#include <stk_mesh/base/CreateEdges.hpp>
#include <stk_mesh/base/SkinBoundary.hpp>
#include <stk_mesh/base/FieldBLAS.hpp>

// stk_io
#include <stk_io/StkMeshIoBroker.hpp>
#include <stk_io/IossBridge.hpp>
#include <stk_io/InputFile.hpp>
#include <Ioss_SubSystem.h>

// stk_util
#include <stk_util/parallel/ParallelReduce.hpp>
#include <Teuchos_RCP.hpp>

#include <Teuchos_GlobalMPISession.hpp>
#include <Teuchos_oblackholestream.hpp>

#include <vector>
#include <string>

class stk::mesh::Entity;
class stk::mesh::FieldBase;
class stk::mesh::Part;

typedef std::vector<stk::mesh::Part *> PartVector;

class EquationSystem;
class Realm;
class LinearSolver;
class TpetraLinearSolver;

/** Abstract class of a linear system that serves as input for the linear solver*/
class LinearSystem {
public:
    LinearSystem(Realm & realm, const unsigned numDof, EquationSystem *eqSys, LinearSolver *linearSolver);
    virtual ~LinearSystem();

    static LinearSystem *create(Realm & realm, const unsigned numDof, EquationSystem *eqSys, LinearSolver *linearSolver);

    // Graph/Matrix Construction
    virtual void buildNodeGraph(const stk::mesh::PartVector & parts)=0; // for nodal assembly (e.g., lumped mass and source)
    virtual void buildFaceToNodeGraph(const stk::mesh::PartVector & parts)=0; // face->node assembly
    virtual void buildEdgeToNodeGraph(const stk::mesh::PartVector & parts)=0; // edge->node assembly
    virtual void buildElemToNodeGraph(const stk::mesh::PartVector & parts)=0; // elem->node assembly
    virtual void buildReducedElemToNodeGraph(const stk::mesh::PartVector & parts)=0; // elem (nearest nodes only)->node assembly
    virtual void buildFaceElemToNodeGraph(const stk::mesh::PartVector & parts)=0; // elem:face->node assembly
    virtual void buildNonConformalNodeGraph(const stk::mesh::PartVector & parts)=0; // nonConformal->elem_node assembly
    virtual void buildOversetNodeGraph(const stk::mesh::PartVector & parts)=0; // overset->elem_node assembly
    virtual void finalizeLinearSystem() = 0;

    /** Process nodes that belong to Dirichlet-type BC
     *
     */
    virtual void buildDirichletNodeGraph(const stk::mesh::PartVector &) {}

    /** Process nodes as belonging to a Dirichlet-type row
     *
     *  See the documentation/implementation of
     *  FixPressureAtNodeAlgorithm for an example of this use case.
     */
    virtual void buildDirichletNodeGraph(const std::vector<stk::mesh::Entity> &) {}

    // Matrix Assembly
    virtual void zeroSystem() = 0;

    virtual void sumInto(unsigned numEntities,
                        const stk::mesh::Entity* entities,
                        const SharedMemView<const double*> & rhs,
                        const SharedMemView<const double**> & lhs,
                        const SharedMemView<int*> & localIds,
                        const SharedMemView<int*> & sortPermutation,
                        const char * trace_tag
                        ) = 0;

    virtual void sumInto(const std::vector<stk::mesh::Entity> & sym_meshobj,
                        std::vector<int> &scratchIds,
                        std::vector<double> &scratchVals,
                        const std::vector<double> & rhs,
                        const std::vector<double> & lhs,
                        const char *trace_tag=0
                        ) = 0;

    virtual void applyDirichletBCs(stk::mesh::FieldBase * solutionField,
                                    stk::mesh::FieldBase * bcValuesField,
                                    const stk::mesh::PartVector & parts,
                                    const unsigned beginPos,
                                    const unsigned endPos) = 0;

    virtual void prepareConstraints(const unsigned beginPos, const unsigned endPos) = 0;

    /** Reset LHS and RHS for the given set of nodes to 0
     *
     *  @param nodeList A list of STK node entities whose rows are zeroed out
     *  @param beginPos Starting index (usually 0)
     *  @param endPos Terminating index (1 for scalar quantities; nDim for vectors)
     */
    virtual void resetRows(std::vector<stk::mesh::Entity> nodeList,
                            const unsigned beginPos,
                            const unsigned endPos) = 0;

    // Solve
    virtual int solve(stk::mesh::FieldBase * linearSolutionField) = 0;
    virtual void loadComplete() = 0;

    virtual void writeToFile(const char * filename, bool useOwned=true) = 0;
    virtual void writeSolutionToFile(const char * filename, bool useOwned=true) = 0;
    unsigned numDof() const { return numDof_; }
    const int & linearSolveIterations() {return linearSolveIterations_; }
    const double & linearResidual() {return linearResidual_; }
    const double & nonLinearResidual() {return nonLinearResidual_; }
    const double & scaledNonLinearResidual() {return scaledNonLinearResidual_; }
    void setNonLinearResidual(const double nlr) { nonLinearResidual_ = nlr; }
    const std::string name() { return eqSysName_; }
    bool & recomputePreconditioner() {return recomputePreconditioner_; }
    bool & reusePreconditioner() {return reusePreconditioner_; }
    double get_timer_precond();
    void zero_timer_precond();

protected:
    virtual void beginLinearSystemConstruction() = 0;
    virtual void checkError(const int err_code, const char * msg) = 0;

    void sync_field(const stk::mesh::FieldBase *field);
    bool debug();

    Realm & realm_;
    EquationSystem * eqSys_;
    bool inConstruction_;
    int writeCounter_;

    const unsigned numDof_;
    const std::string eqSysName_;
    LinearSolver * linearSolver_;
    int linearSolveIterations_;
    double nonLinearResidual_;
    double linearResidual_;
    double firstNonLinearResidual_;
    double scaledNonLinearResidual_;
    bool recomputePreconditioner_;
    bool reusePreconditioner_;                                    

public:
    bool provideOutput_;
};

#endif /* LINEARSYSTEM_H */

