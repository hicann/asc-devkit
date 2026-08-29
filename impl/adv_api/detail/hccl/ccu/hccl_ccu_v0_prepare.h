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
#include "../common/kfc_server_message.h"
#include "../impl/platform_v310/hccl_ccu_v0_def.h"

namespace AscendC {
using namespace HcclKfcProtocol;
static_assert(KFC_CONCURRENT_AG_PARAM_NUM <= CCU_USED_XN_NUM, "Concurrent AllGather parameters exceed XN capacity");
static_assert(CCU_USED_XN_NUM <= CCU_MSG_XN_NUM, "KFC loaded parameters exceed the message slot");

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllToAllV(
    __gm__ CommonPrepareParamCcu* commParam, __gm__ AlltoAllVParamCcu* allToAllVParam)
{
    const uint64_t dataSize = GetHcclDataTypeSize(commParam->dataType);
    xnData_[0] = GetOpId(commParam); // ccu xn0
    KERNEL_LOG(
        KERNEL_INFO, "ApiClient CcuPrepareForAllToAllV count:%llu, sendBuf:0x%llx, recvBuf:0x%llx, dataType:%d",
        (unsigned long long)commParam->count, (unsigned long long)(uint64_t)commParam->sendBuf,
        (unsigned long long)(uint64_t)commParam->recvBuf, static_cast<int>(commParam->dataType));
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
    KERNEL_LOG(
        KERNEL_INFO,
        "ApiClient CcuPrepareForAllToAllV xn0(opId):0x%llx, xn1(sendBuf+off):0x%llx, xn2(recvBuf+off):0x%llx, "
        "xn5(goSize):0x%llx, xn9(msgExt):0x%llx",
        (unsigned long long)xnData_[0], (unsigned long long)xnData_[1], (unsigned long long)xnData_[2],
        (unsigned long long)xnData_[5], (unsigned long long)xnData_[9]);
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

    // Chunking params for large-shape path: partition mySliceSize into fixed chunkSize iterations
    // so that rankSize * chunkSize never exceeds the fixed 16 MiB scratch buffer.
    constexpr uint64_t arScratchSize = 16 * 1024 * 1024;
    constexpr uint64_t minSliceAlign = 128;
    uint64_t chunkSize = arScratchSize / ccuParam_.rankNum / minSliceAlign * minSliceAlign;
    uint64_t mySliceSize = xnData_[8];
    uint64_t fullChunkCount = mySliceSize == 0 ? 0 : (mySliceSize - 1) / chunkSize;
    uint64_t tailSize = mySliceSize - fullChunkCount * chunkSize;
    xnData_[15] = chunkSize;
    xnData_[16] = tailSize;
    xnData_[17] = UINT64_MAX - fullChunkCount;
    KERNEL_LOG(
        KERNEL_INFO, "AR chunk debug: mySlice=0x%llx, chunk=0x%llx, full=0x%llx, tail=0x%llx, loop=0x%llx\n",
        mySliceSize, chunkSize, fullChunkCount, tailSize, xnData_[17]);
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
    xnData_[9] = (xnData_[1] == xnData_[2] + xnData_[3]) ? 1U : 0U;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForConcurrentAllGatherM2M(
    __gm__ CommonPrepareParamCcu* commParam)
{
    constexpr uint64_t meshBandwidth = 11U;
    constexpr uint64_t totalBandwidth = 21U;
    constexpr uint64_t splitAlignment = 128U;
    constexpr uint64_t nhrJettyNum = 4U;
    const uint64_t dataTypeSize = DATA_TYPE_MAP[static_cast<uint64_t>(commParam->dataType)];
    const uint64_t totalSize = commParam->count * dataTypeSize;
    const uint64_t repeatOffset = totalSize * ccuParam_.repeatIndex;
    const uint64_t meshSize = (totalSize * meshBandwidth / totalBandwidth / splitAlignment) * splitAlignment;
    const uint64_t nhrSize = totalSize - meshSize;
    const uint64_t nhrSliceSizePerJetty = (nhrSize / nhrJettyNum / splitAlignment) * splitAlignment;
    const uint64_t nhrLastSliceSizePerJetty = nhrSize - nhrSliceSizePerJetty * (nhrJettyNum - 1U);
    const uint64_t outputStride = commParam->strideCount == 0U ? totalSize : commParam->strideCount * dataTypeSize;
    const uint64_t inputBase = reinterpret_cast<uint64_t>(commParam->sendBuf) + repeatOffset;
    const uint64_t outputBase = reinterpret_cast<uint64_t>(commParam->recvBuf) + repeatOffset;

    xnData_[KFC_CONCURRENT_AG_OP_ID] = GetOpId(commParam);
    xnData_[KFC_CONCURRENT_AG_MESH_INPUT] = inputBase;
    xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT] = outputBase;
    xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT_OFFSET] = outputStride * ccuParam_.rankId;
    xnData_[KFC_CONCURRENT_AG_MESH_SLICE_SIZE] = meshSize;
    CalcGoSize(meshSize, CCU_LOOP_COUNT_M2M_AG, CCU_MEMSLICE_SIZE * 8U, &xnData_[KFC_CONCURRENT_AG_MESH_GO_SIZE_0]);
    xnData_[KFC_CONCURRENT_AG_MESH_INPUT_OUTPUT_EQUAL] =
        (inputBase == outputBase + xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT_OFFSET]) ? 1U : 0U;

    xnData_[KFC_CONCURRENT_AG_NHR_INPUT] = inputBase + meshSize;
    xnData_[KFC_CONCURRENT_AG_NHR_OUTPUT] = outputBase + meshSize;
    xnData_[KFC_CONCURRENT_AG_NHR_SLICE_SIZE] = nhrSize;
    xnData_[KFC_CONCURRENT_AG_NHR_SLICE_SIZE_PER_JETTY] = nhrSliceSizePerJetty;
    xnData_[KFC_CONCURRENT_AG_NHR_LAST_SLICE_SIZE_PER_JETTY] = nhrLastSliceSizePerJetty;
    xnData_[KFC_CONCURRENT_AG_NHR_REPEAT_NUM_INV] = UINT64_MAX - 1U;
    xnData_[KFC_CONCURRENT_AG_NHR_INPUT_SLICE_STRIDE] = 0U;
    xnData_[KFC_CONCURRENT_AG_NHR_OUTPUT_SLICE_STRIDE] = outputStride;
    xnData_[KFC_CONCURRENT_AG_NHR_INPUT_REPEAT_STRIDE] = 0U;
    xnData_[KFC_CONCURRENT_AG_NHR_OUTPUT_REPEAT_STRIDE] = 0U;
    xnData_[KFC_CONCURRENT_AG_NHR_INPUT_OUTPUT_EQUAL] =
        (xnData_[KFC_CONCURRENT_AG_NHR_INPUT] ==
         xnData_[KFC_CONCURRENT_AG_NHR_OUTPUT] + outputStride * ccuParam_.rankId) ?
            1U :
            0U;
    CalcGoSize(nhrSize, CCU_LOOP_COUNT_M2M_AG, CCU_MEMSLICE_SIZE * 8U, &xnData_[KFC_CONCURRENT_AG_NHR_GO_SIZE_0]);
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
    xnData_[5] = 0;
    xnData_[6] = 0;
    // XN7/XN8 carry the tail size and inverse total chunk count.
    constexpr uint64_t scratchSize = 16 * 1024 * 1024;
    constexpr uint64_t minSliceAlign = 128;
    uint64_t chunkSize = scratchSize / ccuParam_.rankNum / minSliceAlign * minSliceAlign;
    uint64_t fullChunkCount = sliceSize == 0 ? 0 : (sliceSize - 1) / chunkSize;
    uint64_t chunkCount = sliceSize == 0 ? 0 : fullChunkCount + 1;
    xnData_[7] = sliceSize - fullChunkCount * chunkSize;
    xnData_[8] = UINT64_MAX - chunkCount;
    KERNEL_LOG(
        KERNEL_INFO, "RS chunk debug: slice=0x%llx, chunk=0x%llx, full=0x%llx, tail=0x%llx, loop=0x%llx\n", sliceSize,
        chunkSize, fullChunkCount, xnData_[7], xnData_[8]);
    CalcGoSize(sliceSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[9]);
}
} // namespace AscendC

#endif
