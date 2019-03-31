/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef LOCALGRAPHARRAYS_H
#define LOCALGRAPHARRAYS_H

#include <LinearSolverTypes.h>
#include <LinearSolverConfig.h>

#include <LinearSolverTypes.h>

#include <Kokkos_DefaultNode.hpp>
#include <Tpetra_Vector.hpp>
#include <Tpetra_CrsMatrix.hpp>
#include <Teuchos_GlobalMPISession.hpp>
#include <Teuchos_oblackholestream.hpp>

#include <Ifpack2_Factory.hpp>

// Header files defining default types for template parameters.
// These headers must be included after other MueLu/Xpetra headers.
typedef double Scalar;
typedef long GlobalOrdinal;
typedef int LocalOrdinal;
typedef Tpetra::Map<LocalOrdinal, GlobalOrdinal>::node_type Node;
typedef Teuchos::ScalarTraits<Scalar> STS;

const LocalOrdinal INVALID = std::numeric_limits<LocalOrdinal>::max();

/** Helper Class for building the arrays describing the local csr graph,
 * rowPointers and colIndices.
 * 
 * These arrays are passed to the TpetraCrsGraph::setAllIndices method.
 * This helper class is used within the TpetraLinerSystem class.
 */
class LocalGraphArrays {
public:
    LocalGraphArrays(const Kokkos::View<size_t *,HostSpace> & rowLengths);
    ~LocalGraphArrays();
    size_t get_row_length(size_t localRow) const;
    void insertIndices(size_t localRow, size_t numInds, const LocalOrdinal * inds, int numDof);
    static size_t compute_row_pointers(Kokkos::View<size_t *, HostSpace> & rowPtrs, const Kokkos::View<size_t *, HostSpace> & rowLengths);
    
    Kokkos::View<size_t *, HostSpace> rowPointers;
    const size_t * rowPointersData;
    Kokkos::View<LocalOrdinal *, HostSpace> colIndices;
    
private:
    void insert(LocalOrdinal ind, int numDof, LocalOrdinal * insertPoint, LocalOrdinal * rowEnd);
};

#endif /* LOCALGRAPHARRAYS_H */

