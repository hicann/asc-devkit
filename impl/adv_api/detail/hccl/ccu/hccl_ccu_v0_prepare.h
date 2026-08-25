/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hccl_ccu_v0_prepare.h
 * \brief
 */
#ifndef IMPL_HCCL_CCU_V0_PREEPARE_H
#define IMPL_HCCL_CCU_V0_PREEPARE_H

#include "hccl_ccu_xn_utils.h"
#include "../impl/platform_v310/hccl_ccu_v0_def.h"

namespace AscendC {
template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllToAllV(
    __gm__ CommonPrepareParamCcu* commParam, __gm__ AlltoAllVParamCcu* allToAllVParam)
{
    const uint64_t dataSize = GetHcclDataTypeSize(commParam->dataType);
    xnData_[0] = GetOpId(commParam); // ccu xn0
    uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[1] = (uint64_t)commParam->sendBuf + offset; // ccu xn1
    xnData_[2] = (uint64_t)commParam->recvBuf + offset; // ccu xn2
    xnData_[3] = 0;                                     // 3 is index of xnData
    xnData_[4] = 0;                                     // 4 is index of xnData
    // 按照卡分组，sendSize 、sendOffset、recvSize、recvOffset  以字节为单位 * DataSzie(DataType)
    // ccu xn5
    AssembleHcclMsgExtForCCU(ccuParam_, commParam, allToAllVParam);

    uint64_t loopCount = 8;
    auto dataSlice = ((allToAllVParam->sendCounts[ccuParam_.rankId]) * dataSize) % CCU_MAX_COMM_DATA;
    CalcGoSize(dataSlice, loopCount, CCU_MEMSLICE_SIZE * 8, &xnData_[5]);
    xnData_[9] = reinterpret_cast<uint64_t>(ccuParam_.ccuMsgExt) + CCU_MSG_EXT_RANK_OFFSET * ccuParam_.alltoallvCnt;
    return;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllToAll(
    __gm__ CommonPrepareParamCcu* commParam)
{
    uint64_t dataSize = GetHcclDataTypeSize(commParam->dataType);
    xnData_[0] = GetOpId(commParam); // ccu xn0
    uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[1] = (uint64_t)commParam->sendBuf + offset; // ccu xn1
    xnData_[2] = (uint64_t)commParam->recvBuf + offset; // ccu xn2
    uint64_t sliceSizeAlltoall = commParam->count * dataSize;
    uint64_t strideSize = commParam->strideCount == 0 ? sliceSizeAlltoall : commParam->strideCount * dataSize;
    xnData_[3] = sliceSizeAlltoall;
    xnData_[4] = strideSize;
    xnData_[5] = 0;
    xnData_[6] = strideSize * ccuParam_.rankId;
    uint64_t loopCount = 8;
    CalcGoSize(sliceSizeAlltoall, loopCount, CCU_MEMSLICE_SIZE * 8, &xnData_[7]);
    return;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllToAllVWrite(
    __gm__ CommonPrepareParamCcu* commParam)
{
    xnData_[0] = GetOpId(commParam); // ccu xn0
    uint64_t loopCount = CCU_LOOP_COUNT_ATAVW;

    xnData_[1] = reinterpret_cast<uint64_t>(commParam->sendBuf); // 1 is index of xnData
    xnData_[2] = commParam->wParamExt.sendSizes;                 // 2 is index of xnData
    xnData_[8] = commParam->wParamExt.remoteWinOffset;           // 8 is index of xnData
    uint64_t sliceSize = commParam->count;

    xnData_[3] = commParam->wParamExt.sendOffsets; // 3 is index of xnData
    CalcGoSize(sliceSize, loopCount, CCU_MEMSLICE_SIZE * 8, &xnData_[4]);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllReduce(
    __gm__ CommonPrepareParamCcu* commParam)
{
    const uint64_t dataSize = GetHcclDataTypeSize(commParam->dataType);
    xnData_[0] = GetOpId(commParam); // ccu xn0
    uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[1] = (uint64_t)commParam->sendBuf + offset; // ccu xn1
    xnData_[2] = (uint64_t)commParam->recvBuf + offset; // ccu xn2

    uint64_t tmpCount = commParam->count / ccuParam_.rankNum;
    uint64_t loopCount = CCU_LOOP_COUNT;
    uint64_t sliceCount = (ccuParam_.rankId == ccuParam_.rankNum - 1) ?
                              (commParam->count - (ccuParam_.rankNum - 1) * tmpCount) :
                              tmpCount;
    uint64_t sliceSize = sliceCount * dataSize;

    uint64_t loopSize = loopCount * CCU_MEMSLICE_SIZE;
    uint64_t m = sliceSize / loopSize;
    uint64_t n = (sliceSize - m * loopSize) / CCU_MEMSLICE_SIZE;
    uint64_t p = sliceSize - m * loopSize - n * CCU_MEMSLICE_SIZE;

    xnData_[3] = (commParam->strideCount == 0) ?
                     tmpCount * dataSize * ccuParam_.rankId :
                     (commParam->strideCount * dataSize * ccuParam_.rankId); // 3 is index of xnData
    xnData_[4] = loopSize * m;                                               // 4 is index of xnData
    xnData_[5] = m;                                                          // 5 is index of xnData
    CalcLoopGroupParam(xnData_, m, n, p);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllGather(
    __gm__ CommonPrepareParamCcu* commParam)
{
    xnData_[0] = GetOpId(commParam); // ccu xn0
    auto dataSize = GetHcclDataTypeSize(commParam->dataType);
    uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[1] = (uint64_t)commParam->sendBuf + offset; // ccu xn1
    xnData_[2] = (uint64_t)commParam->recvBuf + offset; // ccu xn2

    uint64_t tmpCount = commParam->count / ccuParam_.rankNum;
    uint64_t loopCount = CCU_LOOP_COUNT;
    uint64_t sliceCount = commParam->count;

    uint64_t sliceSize = sliceCount * dataSize;

    uint64_t loopSize = loopCount * CCU_MEMSLICE_SIZE;
    uint64_t m = sliceSize / loopSize;
    uint64_t n = (sliceSize - m * loopSize) / CCU_MEMSLICE_SIZE;
    uint64_t p = sliceSize - m * loopSize - n * CCU_MEMSLICE_SIZE;

    xnData_[3] = (commParam->strideCount == 0) ?
                     sliceSize * ccuParam_.rankId :
                     (commParam->strideCount * dataSize * ccuParam_.rankId); // 3 is index of xnData
    xnData_[4] = loopSize * m;                                               // 4 is index of xnData
    xnData_[5] = m;                                                          // 5 is index of xnData
    CalcLoopGroupParam(xnData_, m, n, p);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForReduceScatter(
    __gm__ CommonPrepareParamCcu* commParam)
{
    CcuPrepareForAllGather(commParam);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllReduceM2M(
    __gm__ CommonPrepareParamCcu* commParam)
{
    xnData_[0] = GetOpId(commParam); // ccu xn0
    auto dataSize = GetHcclDataTypeSize(commParam->dataType);
    uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[1] = (uint64_t)commParam->sendBuf + offset; // ccu xn1
    xnData_[2] = (uint64_t)commParam->recvBuf + offset; // ccu xn2

    uint64_t loopCount = CCU_LOOP_COUNT_M2M_RE;
    uint64_t tmpCount = (commParam->count + ccuParam_.rankNum - 1) / ccuParam_.rankNum;
    uint64_t sliceCount = (ccuParam_.rankId == ccuParam_.rankNum - 1) ?
                              (commParam->count - (ccuParam_.rankNum - 1) * tmpCount) :
                              tmpCount;
    uint64_t sliceSize = sliceCount * dataSize;

    xnData_[3] = ccuParam_.scratchAddr;
    uint64_t rankSliceOffset =
        ccuParam_.rankId * ((commParam->strideCount == 0) ? sliceSize : (commParam->strideCount * dataSize));
    KERNEL_LOG(
        KERNEL_INFO, "ApiClient CcuPrepareForAllReduceM2M scratchAddr:0x%llx, rankSliceOffset:%d",
        ccuParam_.scratchAddr, rankSliceOffset);
    xnData_[4] = rankSliceOffset;
    xnData_[5] = rankSliceOffset;

    uint64_t normalSliceCount =
        (commParam->count + ccuParam_.rankNum - 1) / ccuParam_.rankNum; // count/rankNum 向上取整
    uint64_t normalSliceSize = normalSliceCount * dataSize;
    uint64_t normalRankCount = commParam->count / normalSliceCount;
    uint64_t lastSliceSize = 0;
    if (normalRankCount == ccuParam_.rankNum) {
        lastSliceSize = (commParam->count * dataSize) - ((normalRankCount - 1) * normalSliceSize);
    } else {
        lastSliceSize = (commParam->count * dataSize) - (normalRankCount * normalSliceSize);
    }
    KERNEL_LOG(
        KERNEL_INFO, "ApiClient CcuPrepareForAllReduceM2M normalSliceSize:%d, lastSliceSize:%d", normalSliceSize,
        lastSliceSize);

    xnData_[6] = normalSliceSize;
    xnData_[7] = lastSliceSize;
    xnData_[8] = ccuParam_.rankId < normalRankCount ? normalSliceSize : lastSliceSize;
    xnData_[9] = ccuParam_.rankId * normalSliceSize;
    xnData_[10] = 0; // input output not equals
    if (ccuParam_.rankId == ccuParam_.rankNum - 1) {
        CalcGoSize(lastSliceSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[11]);
    } else {
        CalcGoSize(normalSliceSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[11]);
    }
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllGatherM2M(
    __gm__ CommonPrepareParamCcu* commParam)
{
    xnData_[0] = GetOpId(commParam); // ccu xn0
    auto dataSize = GetHcclDataTypeSize(commParam->dataType);
    uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[1] = (uint64_t)commParam->sendBuf + offset; // ccu xn1
    xnData_[2] = (uint64_t)commParam->recvBuf + offset; // ccu xn2

    uint64_t loopCount = CCU_LOOP_COUNT_M2M_AG;
    uint64_t sliceCount = commParam->count;
    uint64_t sliceSize = sliceCount * dataSize;

    xnData_[3] = ccuParam_.rankId * ((commParam->strideCount == 0) ? sliceSize : (commParam->strideCount * dataSize));
    xnData_[4] = sliceSize;
    CalcGoSize(sliceSize, loopCount, CCU_MEMSLICE_SIZE * 8, &xnData_[5]);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForReduceScatterM2M(
    __gm__ CommonPrepareParamCcu* commParam)
{
    xnData_[0] = GetOpId(commParam); // ccu xn0
    auto dataSize = GetHcclDataTypeSize(commParam->dataType);
    uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[1] = (uint64_t)commParam->sendBuf + offset; // ccu xn1
    xnData_[2] = (uint64_t)commParam->recvBuf + offset; // ccu xn2

    uint64_t loopCount = CCU_LOOP_COUNT_M2M_RE;
    uint64_t sliceCount = commParam->count;
    uint64_t sliceSize = sliceCount * dataSize;

    xnData_[3] = ccuParam_.scratchAddr;
    uint64_t rankSliceOffset =
        ccuParam_.rankId * ((commParam->strideCount == 0) ? sliceSize : (commParam->strideCount * dataSize));
    KERNEL_LOG(
        KERNEL_INFO, "ApiClient CcuPrepareForReduceScatterM2M scratchAddr:0x%llx, rankSliceOffset:%d",
        ccuParam_.scratchAddr, rankSliceOffset);
    xnData_[4] = rankSliceOffset;
    // xn5/xn6 are outputOffset and repeatStride, which CCU always treats as 0.
    // Set them explicitly to avoid flushing stale values.
    xnData_[5] = 0;
    xnData_[6] = 0;
    xnData_[7] = sliceSize;
    xnData_[8] = UINT64_MAX - 1;
    CalcGoSize(sliceSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[9]);
}
} // namespace AscendC

#endif
