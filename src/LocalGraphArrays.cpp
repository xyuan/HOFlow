/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "LocalGraphArrays.h"
#include <HOFlowEnv.h>
#include <LinearSolverTypes.h>

#include <stk_util/util/ReportHandler.hpp>

// Tpetra support
#include <BelosLinearProblem.hpp>
#include <BelosMultiVecTraits.hpp>
#include <BelosOperatorTraits.hpp>
#include <BelosSolverFactory.hpp>
#include <BelosSolverManager.hpp>
#include <BelosConfigDefs.hpp>
#include <BelosLinearProblem.hpp>
#include <BelosTpetraAdapter.hpp>

#include <Ifpack2_Factory.hpp>
#include <Kokkos_DefaultNode.hpp>
#include <Kokkos_Serial.hpp>
#include <Teuchos_ArrayRCP.hpp>
#include <Teuchos_DefaultMpiComm.hpp>
#include <Teuchos_OrdinalTraits.hpp>
#include <Tpetra_CrsGraph.hpp>
#include <Tpetra_Export.hpp>
#include <Tpetra_Operator.hpp>
#include <Tpetra_Map.hpp>
#include <Tpetra_MultiVector.hpp>
#include <Tpetra_Vector.hpp>

#include <Teuchos_ParameterXMLFileReader.hpp>

#include <iostream>

LocalGraphArrays::LocalGraphArrays(const Kokkos::View<size_t *,HostSpace> & rowLengths) :
    rowPointers(Kokkos::View<size_t *>(Kokkos::ViewAllocateWithoutInitializing("rowPtrs"),rowLengths.size()+1)),
    rowPointersData(rowPointers.data()),
    colIndices()
{
    size_t nnz = compute_row_pointers(rowPointers, rowLengths);
    colIndices = Kokkos::View<LocalOrdinal*,HostSpace>(Kokkos::ViewAllocateWithoutInitializing("colIndices"), nnz);
    Kokkos::deep_copy(colIndices, INVALID);
}

LocalGraphArrays::~LocalGraphArrays() {
    // nothing to do
}

size_t LocalGraphArrays::get_row_length(size_t localRow) const { 
    return rowPointersData[localRow+1]-rowPointersData[localRow]; 
}

void LocalGraphArrays::insertIndices(size_t localRow, size_t numInds, const LocalOrdinal* inds, int numDof) {
    LocalOrdinal * row = & colIndices(rowPointersData[localRow]);
    size_t rowLen = get_row_length(localRow);
    LocalOrdinal * rowEnd = std::find(row, row+rowLen, INVALID);
    for(size_t i=0; i<numInds; ++i) {
        LocalOrdinal * insertPoint = std::lower_bound(row, rowEnd, inds[i]);
        if (insertPoint <= rowEnd && *insertPoint != inds[i]) {
            insert(inds[i], numDof, insertPoint, rowEnd+numDof);
            rowEnd += numDof;
        }
    }
}

size_t LocalGraphArrays::compute_row_pointers(Kokkos::View<size_t*, HostSpace> & rowPtrs,
                                            const Kokkos::View<size_t *, HostSpace> & rowLengths) 
{
    size_t nnz = 0;
    size_t * rowPtrData = rowPtrs.data();
    const size_t * rowLens = rowLengths.data();
    for(unsigned i=0, iend=rowLengths.size(); i<iend; ++i) {
        rowPtrData[i] = nnz;
        nnz += rowLens[i];
    }
    rowPtrData[rowLengths.size()] = nnz;
    return nnz;
}

void LocalGraphArrays::insert(LocalOrdinal ind, int numDof, LocalOrdinal * insertPoint, LocalOrdinal * rowEnd) {
    for(LocalOrdinal * ptr = rowEnd-1; ptr!= insertPoint; --ptr) {
        *ptr = *(ptr-numDof);
    }
    for(int i=0; i<numDof; ++i) {
        *insertPoint++ = ind+i;
    }
}

