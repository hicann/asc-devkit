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
 * \file data_copy_ub.h
 * \brief GM to UB DataCopy best practices
 */

#ifndef DATA_COPY_UB_H
#define DATA_COPY_UB_H

#include "kernel_operator.h"

template <uint32_t m, uint32_t n, uint32_t numBlocks, uint32_t singleCoreM, uint32_t tileM, uint32_t tileN>
class KernelDataCopyPadGm2Ub {
public:
    __aicore__ inline KernelDataCopyPadGm2Ub() {}

    __aicore__ inline void Init(__gm__ half* src)
    {
        srcGlobal.SetGlobalBuffer(src, m * n);
        ubLocal = AscendC::LocalTensor<half>(AscendC::TPosition::VECIN, ubAddr, ubBufferSize);
    }

    __aicore__ inline void Process()
    {
        uint32_t blockIdx = AscendC::GetBlockIdx();
        uint32_t mStart = blockIdx * singleCoreM;

        // 按 tile 粒度切分 M/N 方向，便于比较不同分块大小的搬运效率。
        constexpr uint32_t nLoopCount = (n + tileN - 1) / tileN;
        constexpr uint32_t mLoopCount = singleCoreM / tileM;
        AscendC::DataCopyExtParams copyInParams;
        copyInParams.blockCount = static_cast<uint16_t>(tileM);
        copyInParams.dstStride = 0;
        AscendC::DataCopyPadExtParams<half> padParams{false, 0, 0, 0};

        for (uint32_t mBlockIdx = 0; mBlockIdx < mLoopCount; mBlockIdx++) {
            uint32_t mIdx = mStart + mBlockIdx * tileM;
            // N 方向按 tileN 切分，尾块场景中最后一次 curCols 小于 tileN。
            for (uint32_t nBlockIdx = 0; nBlockIdx < nLoopCount; nBlockIdx++) {
                uint32_t nIdx = nBlockIdx * tileN;
                uint32_t curCols = (nIdx + tileN > n) ? (n - nIdx) : tileN;

                copyInParams.blockLen = static_cast<uint16_t>(curCols * sizeof(half));
                copyInParams.srcStride = static_cast<uint16_t>((n - curCols) * sizeof(half));
                AscendC::DataCopyPad(ubLocal, srcGlobal[mIdx * n + nIdx], copyInParams, padParams);
            }
        }
    }

    __aicore__ inline void ProcessRepeatWhole4Times()
    {
        // 场景 5：整块矩阵沿相同路径连续重复 4 次，用于观察整块重复访问。
        RepeatCopy(0, n, 4);
    }

    __aicore__ inline void ProcessSplitFourAndRepeat4Times()
    {
        static_assert(n % 4 == 0, "n must be divisible by 4");
        constexpr uint32_t quarterN = n / 4;

        // 场景 6：先按 N 方向切成 4 份，每份连续搬 4 次，便于观察分片后的 L2Cache 复用。
        for (uint32_t splitIdx = 0; splitIdx < 4; splitIdx++) {
            RepeatCopy(splitIdx * quarterN, quarterN, 4);
        }
    }

    __aicore__ inline void ProcessFullMatrixSameAddr()
    {
        // 场景 7：所有核按相同顺序访问同一块输入矩阵，容易产生同地址访问冲突。
        FullMatrixCopy(false);
    }

    __aicore__ inline void ProcessFullMatrixOffsetAddr()
    {
        // 场景 8：所有核按块组内轮转顺序访问，和场景 7 对比冲突规避效果。
        FullMatrixCopy(true);
    }

private:
    __aicore__ inline void FullMatrixCopy(bool offsetAddr)
    {
        uint32_t blockIdx = AscendC::GetBlockIdx();
        // 关闭 L2Cache hint，避免把当前场景的访问顺序和缓存策略混在一起。
        srcGlobal.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);

        constexpr uint32_t fullMBlockCount = m / singleCoreM;
        constexpr uint32_t nLoopCount = n / tileN;
        constexpr uint32_t mTileCount = singleCoreM / tileM;
        AscendC::DataCopyExtParams copyInParams{
            static_cast<uint16_t>(tileM), static_cast<uint16_t>(tileN * sizeof(half)),
            static_cast<uint16_t>((n - tileN) * sizeof(half)), 0, 0};
        AscendC::DataCopyPadExtParams<half> padParams{false, 0, 0, 0};

        for (uint32_t mBlockIdx = 0; mBlockIdx < fullMBlockCount; mBlockIdx++) {
            // M 方向先按 numBlocks 切成大块，offsetAddr 为 true 时在每组 numBlocks 个大块内轮转访问顺序。
            uint32_t blockGroupStart = (mBlockIdx / numBlocks) * numBlocks;
            uint32_t curMBlockIdx = offsetAddr ? blockGroupStart + (mBlockIdx + blockIdx) % numBlocks : mBlockIdx;
            uint32_t mStart = curMBlockIdx * singleCoreM;
            for (uint32_t mTileIdx = 0; mTileIdx < mTileCount; mTileIdx++) {
                uint32_t mIdx = mStart + mTileIdx * tileM;
                for (uint32_t nBlockIdx = 0; nBlockIdx < nLoopCount; nBlockIdx++) {
                    uint32_t nIdx = nBlockIdx * tileN;
                    AscendC::DataCopyPad(ubLocal, srcGlobal[mIdx * n + nIdx], copyInParams, padParams);
                }
            }
        }
    }

    __aicore__ inline void RepeatCopy(uint32_t nStart, uint32_t nCount, uint32_t repeatTimes)
    {
        uint32_t blockIdx = AscendC::GetBlockIdx();
        uint32_t mStart = blockIdx * singleCoreM;

        uint32_t nLoopCount = nCount / tileN;
        constexpr uint32_t mLoopCount = singleCoreM / tileM;
        AscendC::DataCopyExtParams copyInParams{
            static_cast<uint16_t>(tileM), static_cast<uint16_t>(tileN * sizeof(half)),
            static_cast<uint16_t>((n - tileN) * sizeof(half)), 0, 0};
        AscendC::DataCopyPadExtParams<half> padParams{false, 0, 0, 0};

        for (uint32_t repeatIdx = 0; repeatIdx < repeatTimes; repeatIdx++) {
            for (uint32_t mBlockIdx = 0; mBlockIdx < mLoopCount; mBlockIdx++) {
                uint32_t mIdx = mStart + mBlockIdx * tileM;
                for (uint32_t nBlockIdx = 0; nBlockIdx < nLoopCount; nBlockIdx++) {
                    uint32_t nIdx = nBlockIdx * tileN;
                    AscendC::DataCopyPad(ubLocal, srcGlobal[mIdx * n + nStart + nIdx], copyInParams, padParams);
                }
            }
        }
    }

    static constexpr uint32_t ubBufferSize = tileM * tileN;
    static constexpr uint64_t ubAddr = 0;
    AscendC::GlobalTensor<half> srcGlobal;
    AscendC::LocalTensor<half> ubLocal;
};

#endif
