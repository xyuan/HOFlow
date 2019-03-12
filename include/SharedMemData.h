/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef SHAREDMEMDATA_H
#define SHAREDMEMDATA_H

#include <stk_mesh/base/BulkData.hpp>

#include <ElemDataRequests.h>
#include <KokkosInterface.h>
#include <SimdInterface.h>

#include <memory>

struct SharedMemData {
    SharedMemData(const TeamHandleType& team,
         const stk::mesh::BulkData& bulk,
         const ElemDataRequests& dataNeededByKernels,
         unsigned nodesPerEntity,
         unsigned rhsSize)
     : simdPrereqData(team, bulk, nodesPerEntity, dataNeededByKernels)
    {
        for(int simdIndex=0; simdIndex<simdLen; ++simdIndex) {
          prereqData[simdIndex] = std::unique_ptr<ScratchViews<double> >(new ScratchViews<double>(team, bulk, nodesPerEntity, dataNeededByKernels));
        }
        simdrhs = get_shmem_view_1D<DoubleType>(team, rhsSize);
        simdlhs = get_shmem_view_2D<DoubleType>(team, rhsSize, rhsSize);
        rhs = get_shmem_view_1D<double>(team, rhsSize);
        lhs = get_shmem_view_2D<double>(team, rhsSize, rhsSize);

        scratchIds = get_int_shmem_view_1D(team, rhsSize);
        sortPermutation = get_int_shmem_view_1D(team, rhsSize);
    }

    const stk::mesh::Entity* elemNodes[simdLen];
    int numSimdElems;
    std::unique_ptr<ScratchViews<double>> prereqData[simdLen];
    ScratchViews<DoubleType> simdPrereqData;
    SharedMemView<DoubleType*> simdrhs;
    SharedMemView<DoubleType**> simdlhs;
    SharedMemView<double*> rhs;
    SharedMemView<double**> lhs;

    SharedMemView<int*> scratchIds;
    SharedMemView<int*> sortPermutation;
};

struct SharedMemData_FaceElem {
    SharedMemData_FaceElem(const TeamHandleType& team,
         const stk::mesh::BulkData& bulk,
         const ElemDataRequests& faceDataNeeded,
         const ElemDataRequests& elemDataNeeded,
         const ScratchMeInfo& meElemInfo,
         unsigned rhsSize)
     : simdFaceViews(team, bulk, meElemInfo.nodesPerFace_, faceDataNeeded),
       simdElemViews(team, bulk, meElemInfo, elemDataNeeded)
    {
        for(int simdIndex=0; simdIndex<simdLen; ++simdIndex) {
          faceViews[simdIndex] = std::unique_ptr<ScratchViews<double> >(new ScratchViews<double>(team, bulk, meElemInfo.nodesPerFace_, faceDataNeeded));
          elemViews[simdIndex] = std::unique_ptr<ScratchViews<double> >(new ScratchViews<double>(team, bulk, meElemInfo, elemDataNeeded));
        }
        simdrhs = get_shmem_view_1D<DoubleType>(team, rhsSize);
        simdlhs = get_shmem_view_2D<DoubleType>(team, rhsSize, rhsSize);
        rhs = get_shmem_view_1D<double>(team, rhsSize);
        lhs = get_shmem_view_2D<double>(team, rhsSize, rhsSize);

        scratchIds = get_int_shmem_view_1D(team, rhsSize);
        sortPermutation = get_int_shmem_view_1D(team, rhsSize);
    }

    const stk::mesh::Entity* connectedNodes[simdLen];
    int numSimdFaces;
    int elemFaceOrdinal;
    std::unique_ptr<ScratchViews<double>> faceViews[simdLen];
    std::unique_ptr<ScratchViews<double>> elemViews[simdLen];
    ScratchViews<DoubleType> simdFaceViews;
    ScratchViews<DoubleType> simdElemViews;
    SharedMemView<DoubleType*> simdrhs;
    SharedMemView<DoubleType**> simdlhs;
    SharedMemView<double*> rhs;
    SharedMemView<double**> lhs;

    SharedMemView<int*> scratchIds;
    SharedMemView<int*> sortPermutation;
};

#endif /* SHAREDMEMDATA_H */

