/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_cache_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kernel_operator_cache_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CACHE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_CACHE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_CACHE_IMPL_H

#include <cstdint>
#include "../kernel_log.h"
#include "../kernel_macros.h"

namespace AscendC {

constexpr uint8_t PREFETCH_L2_CACHE_CTL = 3;

template <typename T>
__aicore__ inline void DataCachePreloadImpl(__gm__ uint64_t* src, const T cacheOffset)
{
    static_assert(
        SupportType<T, int16_t, int64_t>(),
        "Failed to check dtype in DataCachePreload, current api support dtype is int16_t / int64_t");
    dc_preload(src, cacheOffset);
}

template <typename T>
__aicore__ inline void MultiDataCachelinePreloadImpl(__gm__ uint64_t* src, const uint32_t cachelineNum)
{
    ASCENDC_DEBUG_ASSERT(
        (cachelineNum > 0 && cachelineNum <= 64),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "cachelineNum must be in range [1, 64], current value: %u", cachelineNum));
    constexpr uint64_t CACHELINE_SIZE = 64;
    const int64_t preloadOffset = static_cast<int64_t>(cachelineNum - 1) * CACHELINE_SIZE;
    dc_preload(src, preloadOffset);
}

__aicore__ inline void PreLoadImpl(void* pc, const int64_t preFetchLen) { preload(pc, preFetchLen); }

__aicore__ inline int64_t GetICachePreloadStatusImpl() { return get_icache_prl_st(); }

__aicore__ inline void PreLoad(const int64_t preFetchLen)
{
    int64_t pc = get_pc() & 0xFFFFFFFFFFFF;
    PreLoadImpl(reinterpret_cast<void*>(pc), preFetchLen);
}

__aicore__ inline void PrefetchStopImpl() { __prefetch_stop(); }

template <typename T>
__aicore__ inline void PrefetchAlignV2Impl(__gm__ T* src, const PrefetchParams& params)
{
    constexpr uint8_t l2CacheCtl = PREFETCH_L2_CACHE_CTL;
    const uint32_t burstNum = params.burstNum;
    const uint32_t burstLen = params.burstLen;
    const uint64_t srcStride = params.srcStride;

    if ASCEND_IS_AIC {
        constexpr bool preAllocation = false;
        if constexpr (sizeof(T) == 4) {
            copy_gm_to_cbuf_align_v2(
                (__cbuf__ uint32_t*)0, (__gm__ uint32_t*)src, 0, burstNum, burstLen, 0, 0, true, preAllocation,
                l2CacheCtl, srcStride, 0);
        } else if constexpr (sizeof(T) == 2) {
            copy_gm_to_cbuf_align_v2(
                (__cbuf__ uint16_t*)0, (__gm__ uint16_t*)src, 0, burstNum, burstLen, 0, 0, true, preAllocation,
                l2CacheCtl, srcStride, 0);
        } else if constexpr (sizeof(T) == 1) {
            copy_gm_to_cbuf_align_v2(
                (__cbuf__ uint8_t*)0, (__gm__ uint8_t*)src, 0, burstNum, burstLen, 0, 0, true, preAllocation,
                l2CacheCtl, srcStride, 0);
        }
    } else if ASCEND_IS_AIV {
        constexpr bool preAllocation = false;
        constexpr bool nonEodCtrl = false;
        if constexpr (sizeof(T) == 4) {
            copy_gm_to_ubuf_align_v2(
                (__ubuf__ uint32_t*)0, (__gm__ uint32_t*)src, 0, burstNum, burstLen, 0, 0, true, preAllocation,
                l2CacheCtl, srcStride, 0, nonEodCtrl);
        } else if constexpr (sizeof(T) == 2) {
            copy_gm_to_ubuf_align_v2(
                (__ubuf__ uint16_t*)0, (__gm__ uint16_t*)src, 0, burstNum, burstLen, 0, 0, true, preAllocation,
                l2CacheCtl, srcStride, 0, nonEodCtrl);
        } else if constexpr (sizeof(T) == 1) {
            copy_gm_to_ubuf_align_v2(
                (__ubuf__ uint8_t*)0, (__gm__ uint8_t*)src, 0, burstNum, burstLen, 0, 0, true, preAllocation,
                l2CacheCtl, srcStride, 0, nonEodCtrl);
        }
    }
}

template <typename T>
__aicore__ inline void PrefetchND2NZImpl(__gm__ T* src, const PrefetchToNZParams<PrefetchLayout::ROW_MAJOR>& params)
{
    if ASCEND_IS_AIC {
        constexpr uint8_t l2CacheCtl = PREFETCH_L2_CACHE_CTL;

        uint64_t mte2NzPara = static_cast<uint64_t>(0) << 48 | static_cast<uint64_t>(0) << 32 |
                              static_cast<uint64_t>(0) << 16 | static_cast<uint64_t>(params.matrixNum);
        set_mte2_nz_para(mte2NzPara);

        const uint64_t loop1SrcStride = static_cast<uint64_t>(params.srcDValue) * sizeof(T);
        const uint64_t loop4SrcStride = static_cast<uint64_t>(params.srcMatrixStride) * sizeof(T);

        constexpr bool preAllocation = false;
        if constexpr (sizeof(T) == 1) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ int8_t*)0, (__gm__ int8_t*)src, 0, loop1SrcStride, l2CacheCtl, params.nValue, params.dValue,
                loop4SrcStride, false, preAllocation);
        } else if constexpr (sizeof(T) == 2) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ half*)0, (__gm__ half*)src, 0, loop1SrcStride, l2CacheCtl, params.nValue, params.dValue,
                loop4SrcStride, false, preAllocation);
        } else if constexpr (sizeof(T) == 4) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ float*)0, (__gm__ float*)src, 0, loop1SrcStride, l2CacheCtl, params.nValue, params.dValue,
                loop4SrcStride, false, preAllocation);
        }
    }
}

template <typename T>
__aicore__ inline void PrefetchDN2NZImpl(__gm__ T* src, const PrefetchToNZParams<PrefetchLayout::COLUMN_MAJOR>& params)
{
    if ASCEND_IS_AIC {
        constexpr uint8_t l2CacheCtl = PREFETCH_L2_CACHE_CTL;

        uint64_t mte2NzPara = static_cast<uint64_t>(0) << 48 | static_cast<uint64_t>(0) << 32 |
                              static_cast<uint64_t>(0) << 16 | static_cast<uint64_t>(params.matrixNum);
        set_mte2_nz_para(mte2NzPara);

        const uint64_t loop1SrcStride = static_cast<uint64_t>(params.srcDValue) * sizeof(T);
        const uint64_t loop4SrcStride = static_cast<uint64_t>(params.srcMatrixStride) * sizeof(T);

        constexpr bool preAllocation = false;
        if constexpr (sizeof(T) == 1) {
            copy_gm_to_cbuf_multi_dn2nz(
                (__cbuf__ int8_t*)0, (__gm__ int8_t*)src, 0, loop1SrcStride, l2CacheCtl, params.nValue, params.dValue,
                loop4SrcStride, false, preAllocation);
        } else if constexpr (sizeof(T) == 2) {
            copy_gm_to_cbuf_multi_dn2nz(
                (__cbuf__ half*)0, (__gm__ half*)src, 0, loop1SrcStride, l2CacheCtl, params.nValue, params.dValue,
                loop4SrcStride, false, preAllocation);
        } else if constexpr (sizeof(T) == 4) {
            copy_gm_to_cbuf_multi_dn2nz(
                (__cbuf__ float*)0, (__gm__ float*)src, 0, loop1SrcStride, l2CacheCtl, params.nValue, params.dValue,
                loop4SrcStride, false, preAllocation);
        }
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_CACHE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CACHE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CACHE_IMPL_H__
#endif
