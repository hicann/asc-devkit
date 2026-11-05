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
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_5102_ASC_AICORE_DUMP_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_5102_ASC_AICORE_DUMP_UTILS_H

#include "impl/utils/debug/asc_debug_utils.h"
#include "impl/utils/sys_macros_impl.h"
#include "kernel_macros.h"

namespace __asc_aicore {

template <AscendC::Hardware hardware>
__aicore__ constexpr inline bool is_super_tensor_hardware_supported()
{
    return hardware == AscendC::Hardware::GM || hardware == AscendC::Hardware::UB || hardware == AscendC::Hardware::L0C;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_cbuf_to_gm_impl(__gm__ T* dst, __cc__ T* src, const uint32_t alignDumpBytes)
{
    constexpr int32_t blockCube = 16;
    constexpr int32_t defaultOneBlockSize = 16 * 16 * sizeof(int32_t);
    constexpr int32_t srcBurstLenSizeEle = 16;
    constexpr uint16_t b32ByteSize = 4;

    // copy_matrix_cc_to_gm packs nSize into Xm[15:4], so nSize tops out at 4095 and
    // steps by 16 rows: one copy carries at most 255 fractals of 16x16 B32. Split a
    // larger payload over several copies into the payload this record already
    // reserved, so the record stays a plain DumpTensor the console can print.
    constexpr uint32_t maxCopyBytes = (4095U / blockCube) * blockCube * blockCube * sizeof(float);
    uint16_t burstLen =
        static_cast<uint16_t>(srcBurstLenSizeEle * srcBurstLenSizeEle * sizeof(float) / ASC_ONE_DATABLOCK_SIZE);
    uint16_t m = (burstLen * ASC_ONE_DATABLOCK_SIZE / b32ByteSize) / blockCube;

    for (uint32_t copiedBytes = 0U; copiedBytes < alignDumpBytes;) {
        const uint32_t remaining = alignDumpBytes - copiedBytes;
        const uint32_t chunkBytes = remaining > maxCopyBytes ? maxCopyBytes : remaining;
        uint16_t align = (chunkBytes % defaultOneBlockSize == 0) ? 0 : 1;
        uint16_t countBlks = align + chunkBytes / defaultOneBlockSize;
        uint16_t n = countBlks * blockCube;

        copy_matrix_cc_to_gm(
            reinterpret_cast<__gm__ int32_t*>(dst) + copiedBytes / sizeof(int32_t),
            reinterpret_cast<__cc__ int32_t*>(src) + copiedBytes / sizeof(int32_t), 0, n, m, m * blockCube, m, 0, 0, 0,
            static_cast<uint64_t>(QuantMode_t::NoQuant), static_cast<uint8_t>(false), false, false,
            static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, true, false, false, false);
        copiedBytes += chunkBytes;
    }
    return 0;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_l1buf_to_gm_impl(__gm__ T* dst, __cbuf__ T* src, const uint32_t alignDumpBytes)
{
    return 1;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_ub_to_gm_impl(__gm__ T* dst, __ubuf__ T* src, const uint32_t alignDumpBytes)
{
    constexpr uint8_t byte_32_align = 32;
    constexpr uint32_t blockCount = 1;
    constexpr uint32_t dstStride = 0;
    constexpr uint32_t srcStride = 0;

    // copy_ubuf_to_gm_align_v2 takes lenBurst in bytes.
    const uint32_t burstLen = alignDumpBytes;
    uint32_t srcStride1 = srcStride * byte_32_align + burstLen;
    srcStride1 = div_ceil(srcStride1, byte_32_align) * byte_32_align;
    uint64_t dstStride1 = dstStride * byte_32_align + burstLen;
    copy_ubuf_to_gm_align_v2(
        (__gm__ void*)dst, (__ubuf__ void*)src, 0, blockCount, burstLen, 0, dstStride1, srcStride1);
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

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_5102_ASC_AICORE_DUMP_UTILS_H
