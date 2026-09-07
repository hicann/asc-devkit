/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file l2_cache_kernel.h
 * \brief Workload parameters and device processing shared by both traversal orders.
 */

#ifndef ADD_L2_CACHE_SPLIT_L2_CACHE_KERNEL_H
#define ADD_L2_CACHE_SPLIT_L2_CACHE_KERNEL_H

#include "kernel_operator.h"
#include "tiling/platform/platform_ascendc.h"

namespace AddL2Cache {

constexpr uint32_t vectorCoreCount = 20;
constexpr uint32_t computeRoundCount = 2;

// The negative example scans the whole 384MB range. The positive example derives four 96MB L2 batches
// from the 100MB effective working-set threshold, so repeated reads stay closer to the first access.
constexpr uint32_t totalByteSize = 384 * 1024 * 1024;
constexpr uint32_t L2_TILE_THRESHOLD = 100 * 1024 * 1024;
constexpr uint32_t l2BatchCount = (totalByteSize + L2_TILE_THRESHOLD - 1) / L2_TILE_THRESHOLD;
constexpr uint32_t l2BatchSizeBytes = totalByteSize / l2BatchCount;
constexpr uint32_t totalElementCount = totalByteSize / sizeof(half);
constexpr uint32_t l2BatchElementCount = l2BatchSizeBytes / sizeof(half);

// Use 32-byte DataBlocks as the core distribution unit so every DataCopy length remains aligned.
constexpr uint32_t dataBlockSizeBytes = 32;
constexpr uint32_t elementsPerDataBlock = dataBlockSizeBytes / sizeof(half);

constexpr uint32_t MAX_DATA_COPY_LEN = 16384;
constexpr uint32_t logicalGmToUbBytesPerKernel = totalByteSize * computeRoundCount;
constexpr uint32_t logicalUbToGmBytesPerKernel = logicalGmToUbBytesPerKernel;

class KernelAdd {
public:
    __aicore__ inline void Init(__gm__ uint8_t* data)
    {
        dataGm_.SetGlobalBuffer(reinterpret_cast<__gm__ half*>(data), totalElementCount);
        inputLocal_ = AscendC::LocalTensor<half>(AscendC::TPosition::VECIN, inputUbOffsetBytes, MAX_DATA_COPY_LEN);
        outputLocal_ = AscendC::LocalTensor<half>(AscendC::TPosition::VECOUT, outputUbOffsetBytes, MAX_DATA_COPY_LEN);
    }

    __aicore__ inline void ProcessL2Batch(uint32_t l2BatchIdx)
    {
        // Positive path: only one 96MB L2 batch is active before the next round reuses it.
        ProcessDataRange(static_cast<int64_t>(l2BatchIdx) * l2BatchElementCount, l2BatchElementCount);
    }

    __aicore__ inline void ProcessWholeData()
    {
        // Negative path: one round treats the full 384MB range as a single L2 working set.
        ProcessDataRange(0, totalElementCount);
    }

    __aicore__ inline void SyncAllCores() { AscendC::SyncAll(); }

private:
    static constexpr uint32_t inputUbOffsetBytes = 0;
    static constexpr uint32_t outputUbOffsetBytes = MAX_DATA_COPY_LEN * sizeof(half);

    __aicore__ inline void ProcessDataRange(int64_t rangeOffsetInData, uint32_t rangeElementNum)
    {
        const uint32_t coreIdx = AscendC::GetBlockIdx();
        const uint32_t rangeDataBlockNum = rangeElementNum / elementsPerDataBlock;
        // Distribute aligned DataBlocks evenly. The first cores take one extra block when the division has
        // a remainder, which keeps each core's GM range contiguous and non-overlapping.
        const uint32_t baseDataBlocksPerCore = rangeDataBlockNum / vectorCoreCount;
        const uint32_t coresWithExtraDataBlock = rangeDataBlockNum % vectorCoreCount;
        const uint32_t extraDataBlockNumBeforeCore =
            coreIdx < coresWithExtraDataBlock ? coreIdx : coresWithExtraDataBlock;
        const uint32_t coreDataBlockNum = baseDataBlocksPerCore + (coreIdx < coresWithExtraDataBlock ? 1U : 0U);
        const uint32_t coreDataBlockOffset = coreIdx * baseDataBlocksPerCore + extraDataBlockNumBeforeCore;
        const uint32_t coreElementNum = coreDataBlockNum * elementsPerDataBlock;
        const int64_t coreOffsetInData =
            rangeOffsetInData + static_cast<int64_t>(coreDataBlockOffset) * elementsPerDataBlock;
        ProcessCoreRange(coreOffsetInData, coreElementNum);
    }

    __aicore__ inline void ProcessCoreRange(int64_t coreOffsetInData, uint32_t coreElementNum)
    {
        // UB tiling only fits each core's local buffer. It is not the L2 Cache split granularity.
        const uint32_t ubTileNum = AscendC::Ceil(coreElementNum, MAX_DATA_COPY_LEN);
        for (uint32_t ubTileIdx = 0; ubTileIdx < ubTileNum; ++ubTileIdx) {
            const uint32_t processedElementNum = ubTileIdx * MAX_DATA_COPY_LEN;
            const uint32_t remainingElementNum = coreElementNum - processedElementNum;
            const uint32_t tileElementNum =
                remainingElementNum > MAX_DATA_COPY_LEN ? MAX_DATA_COPY_LEN : remainingElementNum;
            const int64_t gmElementOffset = coreOffsetInData + processedElementNum;

            // Reuse one event id for the steady-state single-buffer pipeline: wait for the previous tile's
            // V->MTE2 dependency, load the current tile, then release Vector compute.
            if (ubTileIdx != 0) {
                AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
            }
            AscendC::DataCopy(inputLocal_, dataGm_[gmElementOffset], tileElementNum);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

            // Do not overwrite the output UB region until the previous store has consumed it.
            if (ubTileIdx != 0) {
                AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
            }
            AscendC::Adds(outputLocal_, inputLocal_, half(1.0), tileElementNum);
            if (ubTileIdx != ubTileNum - 1) {
                AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
            }

            AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::DataCopy(dataGm_[gmElementOffset], outputLocal_, tileElementNum);
            // The last tile has no following load or compute stage, so it leaves no outstanding event.
            if (ubTileIdx != ubTileNum - 1) {
                AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
            }
        }
    }

private:
    AscendC::GlobalTensor<half> dataGm_;
    AscendC::LocalTensor<half> inputLocal_;
    AscendC::LocalTensor<half> outputLocal_;
};

} // namespace AddL2Cache

#endif
