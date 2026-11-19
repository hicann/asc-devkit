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
 * \file hccl_base.h
 * \brief
 */
#ifndef IMPL_HCCL_BASE_H
#define IMPL_HCCL_BASE_H

#include "hccl_inner_def.h"

namespace AscendC {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#define ASCENDC_HCCL_API_ASSERT(cond, ret, fmt, ...)      \
    do {                                                  \
        if (!(cond)) {                                    \
            KERNEL_LOG(KERNEL_ERROR, fmt, ##__VA_ARGS__); \
            ret;                                          \
        }                                                 \
    } while (0)
#elif defined(ASCENDC_DEBUG)
#define ASCENDC_HCCL_API_ASSERT(cond, ret, fmt, ...)    \
    do {                                                \
        ASCENDC_DEBUG_ASSERT(cond, fmt, ##__VA_ARGS__); \
        if (!(cond)) {                                  \
            ret;                                        \
        }                                               \
    } while (0)
#else
#define ASCENDC_HCCL_API_ASSERT(cond, ret, fmt, ...)
#endif

template <HcclServerType serverType, const auto& config>
class HcclImpl {
public:
    template <bool commit = false>
    __aicore__ inline HcclHandle AllReduce(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, uint8_t repeat = 1)
    {
        return -1;
    }

    template <bool commit = false>
    __aicore__ inline HcclHandle AllGather(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t sendCount, HcclDataType dataType, uint64_t strideCount,
        uint8_t repeat = 1)
    {
        return -1;
    }

    template <bool commit = false>
    __aicore__ inline HcclHandle ReduceScatter(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t recvCount, HcclDataType dataType, HcclReduceOp op,
        uint64_t strideCount, uint8_t repeat = 1)
    {
        return -1;
    }

    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAll(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t dataCount, HcclDataType dataType, uint64_t strideCount = 0,
        uint8_t repeat = 1)
    {
        return -1;
    }

    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAllV(
        GM_ADDR sendBuf, void* sendCounts, void* sdispls, HcclDataType sendType, GM_ADDR recvBuf, void* recvCounts,
        void* rdispls, HcclDataType recvType, uint8_t repeat = 1)
    {
        return -1;
    }

    template <bool commit = false>
    __aicore__ inline HcclHandle BatchWrite(GM_ADDR batchWriteInfo, uint32_t itemNum, uint16_t queueID)
    {
        return -1;
    }

    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAllvWrite(
        GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize)
    {
        return -1;
    }

    __aicore__ inline void Init(GM_ADDR context, __gm__ void* initTiling = nullptr) {}

    __aicore__ inline void InitV2(GM_ADDR context, const void* initTiling) {}

    __aicore__ inline int32_t SetCcTiling(__gm__ void* ccOpTilingData) { return -1; }

    __aicore__ inline int32_t SetCcTilingV2(uint64_t offset) { return -1; }

    __aicore__ inline void Commit(HcclHandle handleId) {}

    __aicore__ inline int32_t Wait(HcclHandle handleId) { return -1; }

    __aicore__ inline int32_t Query(HcclHandle handleId) { return -1; }

    __aicore__ inline void InterHcclGroupSync(int8_t srcGroupID, HcclHandle srcHandleID) {}

    template <ScopeType type = ScopeType::ALL>
    __aicore__ inline void QueueBarrier(uint16_t queueID)
    {}

    template <bool sync = true>
    __aicore__ inline int32_t Iterate(HcclHandle handleId, uint16_t* seqSlices, uint16_t seqSliceLen)
    {
        return -1;
    }

    template <bool sync = true>
    __aicore__ inline void Finalize()
    {}

public:
    __aicore__ inline GM_ADDR GetWindowsInAddr(uint32_t rankId) { return nullptr; }

    __aicore__ inline GM_ADDR GetWindowsOutAddr(uint32_t rankId) { return nullptr; }

    __aicore__ inline uint32_t GetRankId() { return 0; }

    __aicore__ inline uint32_t GetRankDim() { return 0; }

    __aicore__ inline uint16_t GetQueueNum() { return 0; }
};

} // namespace AscendC

#endif
