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
 * \file asc_aicore_dump_utils.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_5162_ASC_AICORE_DUMP_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_5162_ASC_AICORE_DUMP_UTILS_H

#include "impl/utils/debug/asc_debug_utils.h"
#include "impl/utils/sys_macros.h"
#include "kernel_macros.h"

namespace __asc_aicore {

template <typename T>
__aicore__ inline uint32_t mem_copy_cbuf_to_gm_impl(__gm__ T* dst, __cc__ T* src, const uint32_t& dumpSize)
{
    return 1;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_l1buf_to_gm_impl(__gm__ T* dst, __cbuf__ T* src, const uint32_t alignDumpBytes)
{
    fixp_trans_mode_t transMode = fixp_trans_mode_t::NORMAL_DMA;
    fix_cbuf_to_gm((__gm__ T*)(dst), (__cbuf__ T*)(src), 1, alignDumpBytes, 0, 0, 0, transMode);
    return 1;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_ub_to_gm_impl(__gm__ T* dst, __ubuf__ T* src, const uint32_t& len)
{
    constexpr uint8_t byte_32_align = 32;
    constexpr uint32_t blockCount = 1;
    uint32_t blockLen = len;
    constexpr uint32_t dstStride = 0;
    constexpr uint32_t srcStride = 0;

    uint32_t unitOfBytes = byte_32_align;
    uint32_t burstLen = blockLen * unitOfBytes;
    uint32_t srcStride1 = srcStride * byte_32_align + burstLen;
    srcStride1 = div_ceil(srcStride1, byte_32_align) * byte_32_align;
    uint64_t dstStride1 = dstStride * unitOfBytes + burstLen;
    copy_ubuf_to_gm_align_v2((__gm__ void*)dst, (__ubuf__ void*)src, 0, blockCount, burstLen, dstStride1, srcStride1);
    return 0;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_abuf_to_gm_impl(__gm__ T* dst, __ca__ T* src, const uint32_t alignDumpBytes)
{
    return 1;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_bbuf_to_gm_impl(__gm__ T* dst, __cb__ T* src, const uint32_t alignDumpBytes)
{
    return 1;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_biasbuf_to_gm_impl(__gm__ T* dst, __biasbuf__ T* src, const uint32_t alignDumpBytes)
{
    return 1;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_fbuf_to_gm_impl(__gm__ T* dst, __fbuf__ T* src, const uint32_t alignDumpBytes)
{
    return 1;
}
} // namespace __asc_aicore

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_DUMP_UTILS__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_DUMP_UTILS__
#endif

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_5162_ASC_AICORE_DUMP_UTILS_H
