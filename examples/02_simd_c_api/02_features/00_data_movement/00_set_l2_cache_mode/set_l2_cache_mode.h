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
 * \file set_l2_cache_mode.h
 * \brief L2 Cache mode best practices – kernel implementations
 *
 * 本文件包含两部分 kernel 实现：
 * 1. KernelDataCopyPadGm2Ub – GM→UB DataCopy 搬运
 *    用于 Case 1（整块重复搬运）和 Case 2（分片重复搬运）
 * 2. KernelAdd – 矩阵加法 + 双缓冲
 *    用于 Case 3（L2 默认开启）和 Case 4（L2 bypass）
 */

#ifndef SET_L2_CACHE_MODE_H
#define SET_L2_CACHE_MODE_H

#include "c_api/asc_simd.h"

/* !
 * \brief half 向量加法辅助函数（寄存器模式/__simd_vf__）
 * 将 UB 中的数据加载到矢量寄存器中执行加法，再将结果写回 UB。
 */
__simd_vf__ inline void add_vf_half(
    __ubuf__ half* x_local, __ubuf__ half* y_local, __ubuf__ half* z_local, uint32_t data_len)
{
    uint16_t one_rep_size = asc_get_vf_len() / sizeof(half);
    uint16_t repeat_time = (data_len + one_rep_size - 1) / one_rep_size;

    vector_bool vmask;
    vector_half reg_src0;
    vector_half reg_src1;
    vector_half reg_dst;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vmask = asc_update_mask_b16(data_len);
        asc_load(reg_src0, x_local + i * one_rep_size);
        asc_load(reg_src1, y_local + i * one_rep_size);
        asc_add(reg_dst, reg_src0, reg_src1, vmask);
        asc_store(z_local + i * one_rep_size, reg_dst);
    }
}

// ====================================================================
// 第一部分：KernelDataCopyPadGm2Ub（GM→UB 数据搬运）
// 包含整块重复及分片重复两种搬运模式
// ====================================================================

template <uint32_t m, uint32_t n, uint32_t numBlocks, uint32_t singleCoreM, uint32_t tileM, uint32_t tileN>
class KernelDataCopyPadGm2Ub {
public:
    __aicore__ inline KernelDataCopyPadGm2Ub() {}

    __aicore__ inline void Init(__gm__ half* src) { srcGlobal = src; }

    __aicore__ inline void Process()
    {
        __ubuf__ half ubLocal[tileM * tileN];
        processImpl(ubLocal);
    }

    /* !
     * \brief Case 1：整块矩阵沿相同路径连续重复搬运 4 次
     */
    __aicore__ inline void ProcessRepeatWhole4Times()
    {
        __ubuf__ half ubLocal[tileM * tileN];
        RepeatCopyImpl(ubLocal, 0, n, 4);
    }

    /* !
     * \brief Case 2：先按 N 方向切成 4 份，每份连续重复搬运 4 次
     */
    __aicore__ inline void ProcessSplitFourAndRepeat4Times()
    {
        __ubuf__ half ubLocal[tileM * tileN];
        static_assert(n % 4 == 0, "n must be divisible by 4");
        constexpr uint32_t quarterN = n / 4;
        for (uint32_t splitIdx = 0; splitIdx < 4; splitIdx++) {
            RepeatCopyImpl(ubLocal, splitIdx * quarterN, quarterN, 4);
        }
    }

private:
    __aicore__ inline void processImpl(__ubuf__ half* ubLocal)
    {
        uint32_t blockIdx = block_idx;
        uint32_t mStart = blockIdx * singleCoreM;

        constexpr uint32_t nLoopCount = (n + tileN - 1) / tileN;
        constexpr uint32_t mLoopCount = singleCoreM / tileM;

        for (uint32_t mBlockIdx = 0; mBlockIdx < mLoopCount; mBlockIdx++) {
            uint32_t mIdx = mStart + mBlockIdx * tileM;
            for (uint32_t nBlockIdx = 0; nBlockIdx < nLoopCount; nBlockIdx++) {
                uint32_t nIdx = nBlockIdx * tileN;
                uint32_t curCols = (nIdx + tileN > n) ? (n - nIdx) : tileN;
                asc_copy_gm2ub_align(
                    ubLocal, srcGlobal + mIdx * n + nIdx, (uint16_t)tileM, (uint32_t)(curCols * sizeof(half)), 0, 0, 0,
                    asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, (uint64_t)((n - curCols) * sizeof(half)), 0);
            }
        }
    }

    __aicore__ inline void RepeatCopyImpl(
        __ubuf__ half* ubLocal, uint32_t nStart, uint32_t nCount, uint32_t repeatTimes)
    {
        uint32_t blockIdx = block_idx;
        uint32_t mStart = blockIdx * singleCoreM;

        uint32_t nLoopCount = nCount / tileN;
        constexpr uint32_t mLoopCount = singleCoreM / tileM;

        for (uint32_t repeatIdx = 0; repeatIdx < repeatTimes; repeatIdx++) {
            for (uint32_t mBlockIdx = 0; mBlockIdx < mLoopCount; mBlockIdx++) {
                uint32_t mIdx = mStart + mBlockIdx * tileM;
                for (uint32_t nBlockIdx = 0; nBlockIdx < nLoopCount; nBlockIdx++) {
                    uint32_t nIdx = nBlockIdx * tileN;
                    asc_copy_gm2ub_align(
                        ubLocal, srcGlobal + mIdx * n + nStart + nIdx, (uint16_t)tileM,
                        (uint32_t)(tileN * sizeof(half)), 0, 0, 0, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM,
                        (uint64_t)((n - tileN) * sizeof(half)), 0);
                }
            }
        }
    }

    __gm__ half* srcGlobal;
};

// ====================================================================
// 第二部分：KernelAdd（矩阵加法 Kernel）
// 包含双缓冲版本（Case 3）及双缓冲+L2 bypass 版本（Case 4）
// ====================================================================

template <uint32_t totalM, uint32_t totalN, uint32_t splitM, uint32_t splitN, uint32_t dataCopyLen>
class KernelAdd {
public:
    __aicore__ inline KernelAdd() = default;
    __aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
    {
        static_assert(splitN == 1, "This optimized sample expects splitN == 1 for contiguous row-wise DataCopy.");
        uint32_t blockIdx = block_idx;
        uint32_t blockIdxM = blockIdx;
        constexpr uint32_t baseCoreM = totalM / splitM;
        constexpr uint32_t remainderM = totalM % splitM;
        uint32_t actualCoreM;
        uint32_t startM;
        if (blockIdxM < remainderM) {
            actualCoreM = baseCoreM + 1;
            startM = blockIdxM * actualCoreM;
        } else {
            actualCoreM = baseCoreM;
            startM = remainderM * (baseCoreM + 1) + (blockIdxM - remainderM) * baseCoreM;
        }
        this->singleCoreM = actualCoreM;
        this->singleCoreN = totalN;
        this->totalElementsPerCore = this->singleCoreM * this->singleCoreN;
        xGm = reinterpret_cast<__gm__ half*>(x) + startM * totalN;
        yGm = reinterpret_cast<__gm__ half*>(y) + startM * totalN;
        zGm = reinterpret_cast<__gm__ half*>(z) + startM * totalN;
    }

    /* !
     * \brief Case 3：Add 双缓冲 + L2 Cache 默认开启
     *
     * 设计意图：采用双缓冲（Ping-Pong）技术实现数据搬运与向量计算的流水线并行。
     * L2 Cache 默认开启时，MTE2 将数据搬运到 UB 的同时会写入 L2 Cache，
     * 但 Add 的输入 x、y 均为流式数据（每个元素只读一次），写入 L2 是纯浪费的。
     * 与 Case 4 对比，展示 L2 bypass 对流式数据的优化效果。
     */
    __aicore__ inline void ProcessDoubleBuffer()
    {
        ProcessDoubleBufferImpl(asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    }

    /* !
     * \brief Case 4：Add 双缓冲 + L2 Cache DISABLE（L2 bypass）
     *
     * 设计意图：流式数据通过 L2 bypass 跳过 L2 Cache 写回，节省带宽。
     */
    __aicore__ inline void ProcessDoubleBufferL2Bypass()
    {
        ProcessDoubleBufferImpl(asc_load_l2_cache_mode::NOTALLOC_KEEP);
    }

private:
    static constexpr uint32_t max_data_copy_len = 20992;

    __aicore__ inline uint32_t GetTotalBlocks() const { return (totalElementsPerCore + dataCopyLen - 1) / dataCopyLen; }

    /* !
     * \brief 双缓冲核心流水线
     * \param l2CacheMode
     */
    __aicore__ inline void ProcessDoubleBufferImpl(asc_load_l2_cache_mode l2CacheMode)
    {
        constexpr uint32_t totalBufSize = 6 * max_data_copy_len;
        __ubuf__ half ubBuf[totalBufSize];

        /* Ping 偏移 */
        __ubuf__ half* xPing = ubBuf;
        __ubuf__ half* yPing = ubBuf + max_data_copy_len;
        __ubuf__ half* zPing = ubBuf + 2 * max_data_copy_len;

        /* Pong 偏移 */
        __ubuf__ half* xPong = ubBuf + 3 * max_data_copy_len;
        __ubuf__ half* yPong = ubBuf + 4 * max_data_copy_len;
        __ubuf__ half* zPong = ubBuf + 5 * max_data_copy_len;

        uint32_t totalBlocks = GetTotalBlocks();
        uint32_t actualDataCopyLen = dataCopyLen;

        asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0);
        asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID1);
        asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
        asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID1);

        for (uint32_t loopIdx = 0; loopIdx < totalBlocks; loopIdx++) {
            uint32_t startElement = loopIdx * actualDataCopyLen;
            uint32_t remainElements = totalElementsPerCore - startElement;
            uint32_t curLen = remainElements > actualDataCopyLen ? actualDataCopyLen : remainElements;

            event_t eventID = ((loopIdx & 1) == 0 ? EVENT_ID0 : EVENT_ID1);

            __ubuf__ half* xLocal = ((loopIdx & 1) == 0 ? xPing : xPong);
            __ubuf__ half* yLocal = ((loopIdx & 1) == 0 ? yPing : yPong);
            __ubuf__ half* zLocal = ((loopIdx & 1) == 0 ? zPing : zPong);

            asc_sync_wait(PIPE_V, PIPE_MTE2, eventID);

            asc_copy_gm2ub_align(xLocal, xGm + startElement, 1, curLen * sizeof(half), 0, 0, 0, l2CacheMode, 0, 0);
            asc_copy_gm2ub_align(yLocal, yGm + startElement, 1, curLen * sizeof(half), 0, 0, 0, l2CacheMode, 0, 0);

            asc_sync_notify(PIPE_MTE2, PIPE_V, eventID);
            asc_sync_wait(PIPE_MTE2, PIPE_V, eventID);

            asc_sync_wait(PIPE_MTE3, PIPE_V, eventID);

            add_vf_half(xLocal, yLocal, zLocal, curLen);

            asc_sync_notify(PIPE_V, PIPE_MTE2, eventID);

            asc_sync_notify(PIPE_V, PIPE_MTE3, eventID);
            asc_sync_wait(PIPE_V, PIPE_MTE3, eventID);

            asc_copy_ub2gm_align(
                zGm + startElement, zLocal, 1, curLen * sizeof(half), asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0,
                0);

            asc_sync_notify(PIPE_MTE3, PIPE_V, eventID);
        }

        asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0);
        asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID1);
        asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
        asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID1);
    }

    __gm__ half* xGm;
    __gm__ half* yGm;
    __gm__ half* zGm;
    uint32_t singleCoreM;
    uint32_t singleCoreN;
    uint32_t totalElementsPerCore;
};

#endif // SET_L2_CACHE_MODE_H
