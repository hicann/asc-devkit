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
static_assert(
    KFC_RS_SOLE_NHR_PARAM_NUM <= CCU_USED_XN_NUM, "Sole NHR MultiLink ReduceScatter parameters exceed XN capacity");
static_assert(KFC_CONCURRENT_A2A_PARAM_NUM <= CCU_USED_XN_NUM, "Concurrent AllToAll parameters exceed XN capacity");
static_assert(CCU_USED_XN_NUM <= CCU_MSG_XN_NUM, "KFC loaded parameters exceed the message slot");

__aicore__ inline void CalcPeerOnlyChunkParams(uint64_t sliceSize, uint64_t* tailSize, uint64_t* chunkLoopNum)
{
    const uint64_t fullChunkCount = sliceSize == 0 ? 0 : (sliceSize - 1) / CCU_MAX_COMM_DATA;
    *tailSize = sliceSize - fullChunkCount * CCU_MAX_COMM_DATA;
    const uint64_t chunkCount = sliceSize == 0 ? 0 : fullChunkCount + 1;
    *chunkLoopNum = UINT64_MAX - chunkCount;
}

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

    constexpr uint64_t arScratchSize = 16 * 1024 * 1024;
    constexpr uint64_t minSliceAlign = 128;
    uint64_t chunkSize = arScratchSize / ccuParam_.rankNum / minSliceAlign * minSliceAlign;
    uint64_t mySliceSize = xnData_[8];
    uint64_t fullChunkCount = mySliceSize == 0 ? 0 : (mySliceSize - 1) / chunkSize;
    uint64_t tailSize = mySliceSize - fullChunkCount * chunkSize;
    uint64_t chunkCount = mySliceSize == 0 ? 0 : fullChunkCount + 1;
    xnData_[15] = chunkSize;
    xnData_[16] = tailSize;
    xnData_[17] = UINT64_MAX - chunkCount;
    CalcGoSize(chunkSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[18]);
    CalcGoSize(tailSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[22]);
    KERNEL_LOG(
        KERNEL_INFO, "AR chunk debug: mySlice=0x%llx, chunk=0x%llx, full=0x%llx, tail=0x%llx, loop=0x%llx\n",
        mySliceSize, chunkSize, fullChunkCount, tailSize, xnData_[17]);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForAllGatherM2M(
    __gm__ CommonPrepareParamCcu* commParam)
{
    xnData_[KFC_CONCURRENT_AG_OP_ID] = GetOpId(commParam);
    auto dataSize = GetHcclDataTypeSize(commParam->dataType);
    uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[KFC_CONCURRENT_AG_MESH_INPUT] = (uint64_t)commParam->sendBuf + offset;
    xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT] = (uint64_t)commParam->recvBuf + offset;

    uint64_t loopCount = CCU_LOOP_COUNT_M2M_AG;
    uint64_t sliceCount = commParam->count;
    uint64_t sliceSize = sliceCount * dataSize;

    xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT_OFFSET] =
        ccuParam_.rankId * ((commParam->strideCount == 0) ? sliceSize : (commParam->strideCount * dataSize));
    xnData_[KFC_CONCURRENT_AG_MESH_SLICE_SIZE] = sliceSize;
    CalcGoSize(sliceSize, loopCount, CCU_MEMSLICE_SIZE * 8, &xnData_[KFC_CONCURRENT_AG_MESH_GO_SIZE_0]);
    xnData_[KFC_CONCURRENT_AG_MESH_CURRENT_RANK_SLICE_INPUT_OFFSET] = 0U;
    xnData_[KFC_CONCURRENT_AG_MESH_REPEAT_NUM_INV] = UINT64_MAX - 1U;
    xnData_[KFC_CONCURRENT_AG_MESH_INPUT_REPEAT_STRIDE] = 0U;
    xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT_REPEAT_STRIDE] = 0U;
    xnData_[KFC_CONCURRENT_AG_MESH_LAST_SLICE_SIZE] = sliceSize;
    xnData_[KFC_CONCURRENT_AG_MESH_INPUT_OUTPUT_EQUAL] =
        (xnData_[KFC_CONCURRENT_AG_MESH_INPUT] ==
         xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT] + xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT_OFFSET]) ?
            1U :
            0U;
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
    xnData_[KFC_CONCURRENT_AG_MESH_CURRENT_RANK_SLICE_INPUT_OFFSET] = 0U;
    xnData_[KFC_CONCURRENT_AG_MESH_REPEAT_NUM_INV] = UINT64_MAX - 1U;
    xnData_[KFC_CONCURRENT_AG_MESH_INPUT_REPEAT_STRIDE] = 0U;
    xnData_[KFC_CONCURRENT_AG_MESH_OUTPUT_REPEAT_STRIDE] = 0U;
    xnData_[KFC_CONCURRENT_AG_MESH_LAST_SLICE_SIZE] = meshSize;
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
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForConcurrentAllToAll(
    __gm__ CommonPrepareParamCcu* commParam)
{
    constexpr uint64_t meshBandwidth = 11U;
    constexpr uint64_t totalBandwidth = 21U;
    constexpr uint64_t splitAlignment = 128U;
    const uint64_t dataTypeSize = GetHcclDataTypeSize(commParam->dataType);
    const uint64_t totalSize = commParam->count * dataTypeSize;
    const uint64_t repeatOffset = totalSize * ccuParam_.repeatIndex;
    const uint64_t meshSize = (totalSize * meshBandwidth / totalBandwidth / splitAlignment) * splitAlignment;
    const uint64_t closSize = totalSize - meshSize;
    const uint64_t strideSize = commParam->strideCount == 0U ? totalSize : commParam->strideCount * dataTypeSize;
    const uint64_t inputBase = reinterpret_cast<uint64_t>(commParam->sendBuf) + repeatOffset;
    const uint64_t outputBase = reinterpret_cast<uint64_t>(commParam->recvBuf) + repeatOffset;

    xnData_[KFC_CONCURRENT_A2A_OP_ID] = GetOpId(commParam);
    xnData_[KFC_CONCURRENT_A2A_MESH_INPUT] = inputBase;
    xnData_[KFC_CONCURRENT_A2A_MESH_OUTPUT] = outputBase;
    xnData_[KFC_CONCURRENT_A2A_MESH_SLICE_SIZE] = meshSize;
    xnData_[KFC_CONCURRENT_A2A_MESH_SRC_STRIDE] = strideSize;
    xnData_[KFC_CONCURRENT_A2A_MESH_SRC_OFFSET] = 0U;
    xnData_[KFC_CONCURRENT_A2A_MESH_DST_OFFSET] = strideSize * ccuParam_.rankId;
    CalcGoSize(meshSize, CCU_LOOP_COUNT_M2M_AG, CCU_MEMSLICE_SIZE * 8U, &xnData_[KFC_CONCURRENT_A2A_MESH_GO_SIZE_0]);

    xnData_[KFC_CONCURRENT_A2A_CLOS_INPUT] = inputBase + meshSize;
    xnData_[KFC_CONCURRENT_A2A_CLOS_OUTPUT] = outputBase + meshSize;
    xnData_[KFC_CONCURRENT_A2A_CLOS_SLICE_SIZE] = closSize;
    xnData_[KFC_CONCURRENT_A2A_CLOS_SRC_STRIDE] = strideSize;
    xnData_[KFC_CONCURRENT_A2A_CLOS_SRC_OFFSET] = 0U;
    xnData_[KFC_CONCURRENT_A2A_CLOS_DST_OFFSET] = strideSize * ccuParam_.rankId;
    CalcGoSize(closSize, CCU_LOOP_COUNT_M2M_AG, CCU_MEMSLICE_SIZE * 8U, &xnData_[KFC_CONCURRENT_A2A_CLOS_GO_SIZE_0]);
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
    xnData_[7] = sliceSize;
    xnData_[8] = UINT64_MAX - 1;
    CalcGoSize(sliceSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[9]);
    // KFC-specific parameters at [13..23] (homm template ignores these, uses templateDataParams).
    constexpr uint64_t scratchSize = 64 * 1024 * 1024; // 调试: 16MB->64MB，需与host侧alloc_ctx_res.cc对齐
    constexpr uint64_t minSliceAlign = 128;
    uint64_t chunkSize = scratchSize / ccuParam_.rankNum / minSliceAlign * minSliceAlign;
    uint64_t fullChunkCount = sliceSize == 0 ? 0 : (sliceSize - 1) / chunkSize;
    uint64_t chunkCount = sliceSize == 0 ? 0 : fullChunkCount + 1;
    uint64_t tailSize = sliceSize - fullChunkCount * chunkSize;
    xnData_[13] = chunkSize;
    xnData_[14] = UINT64_MAX - chunkCount;
    xnData_[15] = tailSize;
    CalcGoSize(chunkSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[16]);
    CalcGoSize(tailSize, loopCount, CCU_MEMSLICE_SIZE, &xnData_[20]);
    KERNEL_LOG(
        KERNEL_INFO, "RS chunk debug: slice=0x%llx, chunk=0x%llx, full=0x%llx, tail=0x%llx, loop=0x%llx\n", sliceSize,
        chunkSize, fullChunkCount, xnData_[15], xnData_[14]);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForReduceScatterPeerOnlyM2M(
    __gm__ CommonPrepareParamCcu* commParam)
{
    xnData_[0] = GetOpId(commParam);
    const uint64_t dataSize = GetHcclDataTypeSize(commParam->dataType);
    const uint64_t offset = commParam->count * ccuParam_.repeatIndex * dataSize;
    xnData_[1] = (uint64_t)commParam->sendBuf + offset;
    xnData_[2] = (uint64_t)commParam->recvBuf + offset;
    xnData_[3] = 0;

    const uint64_t sliceSize = commParam->count * dataSize;
    xnData_[4] = ccuParam_.rankId * ((commParam->strideCount == 0) ? sliceSize : (commParam->strideCount * dataSize));
    xnData_[5] = 0;
    xnData_[6] = 0;
    CalcPeerOnlyChunkParams(sliceSize, &xnData_[7], &xnData_[8]);
    KERNEL_LOG(
        KERNEL_INFO,
        "PeerOnly RS prepare: input=0x%llx, output=0x%llx, rankOffset=0x%llx, slice=0x%llx, "
        "chunk=0x%llx, tail=0x%llx, loop=0x%llx\n",
        xnData_[1], xnData_[2], xnData_[4], sliceSize, CCU_MAX_COMM_DATA, xnData_[7], xnData_[8]);
}

// SoleNHRMultiLink（CcuSchedReduceScatterSoleNHRMultiLink）的 AIV prepare：
// 将 hccl InsV2ReduceScatterSoleExecutor::OrchestrateLoop（loopTimes=1 单趟）+
// CcuTempReduceScatterNhrMultiJettyMem2Mem1D::KernelRun 的 host 侧参数计算搬到设备侧。
// 布局见 KfcReduceScatterSoleNhrParamIndex，与 KFC dispatch、kernel 形参逐槽一致。
template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForReduceScatterSoleNhrM2M(
    __gm__ CommonPrepareParamCcu* commParam)
{
    constexpr uint64_t hcclMinSliceAlign = 128U;
    constexpr uint64_t nhrJettyNum = 1U; // 与 hccl CcuTempReduceScatterNhrMultiJettyMem2Mem1D 的 portNum=1 一致
    const uint64_t dataTypeSize = GetHcclDataTypeSize(commParam->dataType);
    const uint64_t sliceSize = commParam->count * dataTypeSize; // 每 rank 输出份
    const uint64_t repeatOffset = sliceSize * ccuParam_.repeatIndex;
    const uint64_t sliceStride = commParam->strideCount == 0U ? sliceSize : commParam->strideCount * dataTypeSize;
    const uint64_t inputBase = reinterpret_cast<uint64_t>(commParam->sendBuf) + repeatOffset;
    const uint64_t outputBase = reinterpret_cast<uint64_t>(commParam->recvBuf) + repeatOffset;
    // hccl 口径：sliceAlignCount = 128 / dataTypeSize（元素数）；portNum=1 时 sliceOneJettySize 不被消费
    const uint64_t sliceAlignCount = hcclMinSliceAlign / dataTypeSize;
    const uint64_t sliceOneJettySize = sliceSize / nhrJettyNum / sliceAlignCount * sliceAlignCount;
    const uint64_t sliceLastJettySize = sliceSize - (nhrJettyNum - 1U) * sliceOneJettySize;

    xnData_[KFC_RS_SOLE_NHR_OP_ID] = GetOpId(commParam);
    xnData_[KFC_RS_SOLE_NHR_INPUT] = inputBase;
    xnData_[KFC_RS_SOLE_NHR_OUTPUT] = outputBase;
    xnData_[KFC_RS_SOLE_NHR_SLICE_SIZE] = sliceSize;
    xnData_[KFC_RS_SOLE_NHR_INPUT_SLICE_STRIDE] = sliceStride;
    xnData_[KFC_RS_SOLE_NHR_SLICE_ONE_JETTY_SIZE] = sliceOneJettySize;
    xnData_[KFC_RS_SOLE_NHR_SLICE_LAST_JETTY_SIZE] = sliceLastJettySize;
    xnData_[KFC_RS_SOLE_NHR_REPEAT_NUM_INV] = UINT64_MAX - 1U; // sole executor repeatNum=1
    xnData_[KFC_RS_SOLE_NHR_INPUT_REPEAT_STRIDE] = 0U;         // sole executor 双 stride=0
    xnData_[KFC_RS_SOLE_NHR_OUTPUT_REPEAT_STRIDE] = 0U;
    KERNEL_LOG(
        KERNEL_INFO, "RS sole-NHR prepare: slice=0x%llx, stride=0x%llx, lastJetty=0x%llx\n", sliceSize, sliceStride,
        sliceLastJettySize);
}
} // namespace AscendC

#endif
