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
 * \file hccl_ccu_xn_utils.h
 * \brief
 */
#ifndef IMPL_HCCL_CCU_XN_UTILS_H
#define IMPL_HCCL_CCU_XN_UTILS_H

#include "../common/hccl_inner_def.h"
#include "../common/hccl_utils.h"

namespace AscendC {

constexpr uint64_t ALGO_TYPE_MASK = 0x7f;
constexpr uint64_t OUT_DATA_TYPE_MASK = 0x7f;
constexpr uint64_t REDUCE_TYPE_MASK = 0x7f;
constexpr uint64_t DATA_TYPE_MASK = 0x7f;
constexpr uint64_t COMM_TYPE_MASK = 0x7f;

constexpr uint64_t ALGO_TYPE_SHIFT = 32;
constexpr uint64_t OUT_DATA_TYPE_SHIFT = 24;
constexpr uint64_t REDUCE_TYPE_SHIFT = 16;
constexpr uint64_t DATA_TYPE_SHIFT = 8;
constexpr uint64_t COMM_TYPE_SHIFT = 0;

constexpr int CCU_PARAM_INDEX = 2;

constexpr uint64_t REPEAT_NUM_SHIFT = 55;
constexpr uint64_t REPEAT_LOOP_INDEX_SHIFT = 48;
constexpr uint64_t TOTAL_LOOP_NUM_SHIFT = 41;
constexpr uint64_t MASK = 0x7f;

__aicore__ inline uint64_t GetOpId(__gm__ CommonPrepareParamCcu* commParam)
{
    if (commParam->commType.msgType == ControlMsgType::HCCL_CMD_FINALIZE) {
        return 0xffffffffffffffff;
    }

    uint64_t algoType = 0U;
    uint64_t commType = static_cast<uint64_t>(commParam->commType.prepareType);
    uint64_t dataType = static_cast<uint64_t>(commParam->dataType);
    uint64_t outDataType = static_cast<uint64_t>(commParam->dstDataType);

    bool isReduceType = (commParam->commType.prepareType == HcclCMDType::HCCL_CMD_REDUCE) ||
                        (commParam->commType.prepareType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER) ||
                        (commParam->commType.prepareType == HcclCMDType::HCCL_CMD_ALLREDUCE);

    uint64_t reduceType = isReduceType ? static_cast<uint64_t>(commParam->op) : 0U;

    KERNEL_LOG(
        KERNEL_INFO, "ApiClient GetOpId algoType:%d outDataType:%d, reduceType:%d, dataType:%d, commType:%d", algoType,
        outDataType, reduceType, dataType, commType);

    return ((algoType & ALGO_TYPE_MASK) << ALGO_TYPE_SHIFT) |
           ((outDataType & OUT_DATA_TYPE_MASK) << OUT_DATA_TYPE_SHIFT) |
           ((reduceType & REDUCE_TYPE_MASK) << REDUCE_TYPE_SHIFT) | ((dataType & DATA_TYPE_MASK) << DATA_TYPE_SHIFT) |
           (commType & COMM_TYPE_MASK);
}

__aicore__ inline void AssembleHcclMsgExtForCCU(
    CcuPrepareParam& ccuParam, __gm__ CommonPrepareParamCcu* commParam, __gm__ AlltoAllVParamCcu* allToAllVParam)
{
    if (!IsValidCcuRankNum(ccuParam.rankNum)) {
        KERNEL_LOG(
            KERNEL_ERROR, "Assemble CCU AlltoAllV message failed, rank num %u is outside the supported range [1, %u].",
            ccuParam.rankNum, HCCL_CCU_MAX_RANK_NUM);
        return;
    }
    __gm__ CCUMsgExt* ccuMsgExt = reinterpret_cast<__gm__ CCUMsgExt*>(
        reinterpret_cast<uint64_t>(ccuParam.ccuMsgExt) + CCU_MSG_EXT_RANK_OFFSET * ccuParam.alltoallvCnt);
    FlushDataCache(ccuMsgExt);
    uint64_t dataSize = GetHcclDataTypeSize(commParam->dataType);

    KERNEL_LOG(
        KERNEL_INFO, "ApiClient AssembleHcclMsgExtForCCU ccuParam.ccuMsgExt:0x%llx, ccuMsgExt:0x%llx",
        reinterpret_cast<uint64_t>(ccuParam.ccuMsgExt), reinterpret_cast<uint64_t>(ccuMsgExt));

    for (uint32_t i = 0U; i < ccuParam.rankNum; ++i) {
        uint64_t sendSize = allToAllVParam->sendCounts[i] * dataSize;
        ccuMsgExt[i].sendSize = sendSize % CCU_MAX_COMM_DATA;
        ccuMsgExt[i].sendOffset = allToAllVParam->sdispls[i] * dataSize + sendSize * ccuParam.repeatIndex;
        ccuMsgExt[i].recvOffset =
            allToAllVParam->rdispls[i] * dataSize + (allToAllVParam->recvCounts[i] * dataSize) * ccuParam.repeatIndex;
        KERNEL_LOG(
            KERNEL_INFO, "ApiClient ccuMsgExt rankIndex:%u, sendSize:%d, sendOffset:%d, recvOffset:%d", i,
            ccuMsgExt[i].sendSize, ccuMsgExt[i].sendOffset, ccuMsgExt[i].recvOffset);
    }

    uint32_t tmpCnt = (sizeof(CCUMsgExt) * ccuParam.rankNum) / MAX_DCCI_CNT;
    uint32_t copyCnt = (sizeof(CCUMsgExt) * ccuParam.rankNum) % MAX_DCCI_CNT ? tmpCnt + 1 : tmpCnt;
    KERNEL_LOG(KERNEL_INFO, "ApiClient AssembleHcclMsgExtForCCU tmpCnt:%d, copyCnt:%d", tmpCnt, copyCnt);

    GlobalTensor<int64_t> globalHcclMsgArea;
    uint64_t tmpSize = 0;
    for (uint32_t i = 0U; i < copyCnt; ++i) {
        FlushDataCache(globalHcclMsgArea, (GM_ADDR)(reinterpret_cast<uint64_t>(ccuMsgExt) + tmpSize));
        tmpSize += MAX_DCCI_CNT;
    }
}

__aicore__ inline uint64_t GetParallelParameters(uint64_t repeatNum, uint64_t repeatLoopIndex, uint64_t totalLoopNum)
{
    return ((repeatNum & MASK) << REPEAT_NUM_SHIFT) | ((repeatLoopIndex & MASK) << REPEAT_LOOP_INDEX_SHIFT) |
           ((totalLoopNum & MASK) << TOTAL_LOOP_NUM_SHIFT);
}

__aicore__ inline void CalcLoopGroupParam(uint64_t* xnData, uint64_t m, uint64_t n, uint64_t p)
{
    if (n == 0 && p == 0) {
        // 数据量为loopSize的整数倍，跳过LoopGroup1
        xnData[6] = 0; // ccu xn6
        xnData[7] = 0; // ccu xn7
    } else if (n != 0 && p == 0) {
        // 数据量为256K * m + CCU_MEMSLICE_SIZE * n
        xnData[6] = GetParallelParameters(n - 1, 0, 1); // ccu xn6
        xnData[7] = CCU_MEMSLICE_SIZE;                  // ccu xn7
    } else if (n == 0 && p != 0) {
        // 数据量为loopSize * m + p
        xnData[6] = GetParallelParameters(0, 0, 1); // ccu xn6
        xnData[7] = p;                              // ccu xn7
    } else {
        // 数据量为loopSize * m + CCU_MEMSLICE_SIZE * n + p
        xnData[6] = GetParallelParameters(n - 1, 1, CCU_PARAM_INDEX); // ccu xn6
        xnData[7] = p;                                                // ccu xn7
    }
}

__aicore__ inline void CalcGoSize(uint64_t sliceSize, uint64_t loopCount, uint64_t ccuMemsliceSize, uint64_t* goSize)
{
    uint64_t loopSize = loopCount * ccuMemsliceSize;
    uint64_t m = sliceSize / loopSize;
    uint64_t n = (sliceSize - m * loopSize) / ccuMemsliceSize;
    uint64_t p = sliceSize - m * loopSize - n * ccuMemsliceSize;
    KERNEL_LOG(KERNEL_INFO, "ApiClient CalcGoSize loopSize:%d, m:%d, n:%d, p:%d", loopSize, m, n, p);

    goSize[0] = loopSize * m;
    goSize[1] = m;
    if (n == 0 && p == 0) {
        // 数据量为loopSize的整数倍，跳过LoopGroup1
        goSize[2] = 0;
        goSize[3] = 0;
    } else if (n != 0 && p == 0) {
        // 数据量为256K * m + ccuMemsliceSize * n
        goSize[2] = GetParallelParameters(n - 1, 0, 1);
        goSize[3] = ccuMemsliceSize;
    } else if (n == 0 && p != 0) {
        // 数据量为loopSize * m + p
        goSize[2] = GetParallelParameters(0, 0, 1);
        goSize[3] = p;
    } else {
        // 数据量为loopSize * m + ccuMemsliceSize * n + p
        goSize[2] = GetParallelParameters(n - 1, 1, CCU_PARAM_INDEX);
        goSize[3] = p;
    }
}
} // namespace AscendC

#endif
