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
 * \file asc_debug_utils_impl.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_5102_ASC_DEBUG_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_5102_ASC_DEBUG_UTILS_H

#include "impl/utils/sys_macros.h"
#include "impl/utils/debug/asc_debug_utils.h"

namespace __asc_aicore {
__aicore__ inline void asc_entire_dcci_impl(__gm__ uint64_t* ptr)
{
    dcci(ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

__aicore__ inline uint64_t asc_debug_get_system_cycle_impl()
{
    uint64_t sysCnt = 0;
    asm volatile("MOV %0, SYS_CNT\n" : "+l"(sysCnt));
    return sysCnt;
}

template <uint64_t timeoutCycle>
__aicore__ inline void ringbuf_wait_rts_sync_impl()
{
    const uint64_t firstTimeStamp = asc_debug_get_system_cycle_impl();
    while (static_cast<uint64_t>(asc_debug_get_system_cycle_impl()) - firstTimeStamp < timeoutCycle) {
        // Wait for RTS sync
    }
}

__aicore__ inline uint32_t asc_debug_get_core_idx_impl()
{
    constexpr uint32_t dumpCoreNums = 108;
    return get_coreid() % dumpCoreNums;
}

__aicore__ inline int64_t get_task_ration() { return get_subblockdim(); }

__aicore__ inline uint64_t asc_debug_get_block_idx_impl() { return get_block_idx(); }

__aicore__ inline void sync_all_impl() { pipe_barrier(pipe_t::PIPE_ALL); }

template <typename T>
__aicore__ constexpr inline DumpTensorDataType get_dump_datatype_impl()
{
    if constexpr (std::is_same<T, bool>::value) {
        return DumpTensorDataType::ACL_BOOL;
    } else if (std::is_same<T, uint8_t>::value) {
        return DumpTensorDataType::ACL_UINT8;
    } else if (std::is_same<T, int8_t>::value) {
        return DumpTensorDataType::ACL_INT8;
    } else if (std::is_same<T, int16_t>::value) {
        return DumpTensorDataType::ACL_INT16;
    } else if (std::is_same<T, uint16_t>::value) {
        return DumpTensorDataType::ACL_UINT16;
    } else if (std::is_same<T, int32_t>::value) {
        return DumpTensorDataType::ACL_INT32;
    } else if (std::is_same<T, uint32_t>::value) {
        return DumpTensorDataType::ACL_UINT32;
    } else if (std::is_same<T, uint64_t>::value) {
        return DumpTensorDataType::ACL_UINT64;
    } else if (std::is_same<T, int64_t>::value) {
        return DumpTensorDataType::ACL_INT64;
    } else if (std::is_same<T, float>::value) {
        return DumpTensorDataType::ACL_FLOAT;
    } else if (std::is_same<T, half>::value) {
        return DumpTensorDataType::ACL_FLOAT16;
    } else if (std::is_same<T, bfloat16_t>::value) {
        return DumpTensorDataType::ACL_BF16;
    } else if (std::is_same<T, hifloat8_t>::value) {
        return DumpTensorDataType::ACL_HIFLOAT8;
    } else if (std::is_same<T, float8_e5m2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E5M2;
    } else if (std::is_same<T, float8_e4m3_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E4M3FN;
    } else if (std::is_same<T, float8_e8m0_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E8M0;
    } else if (std::is_same<T, float4_e2m1x2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT4_E2M1;
    } else if (std::is_same<T, float4_e1m2x2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT4_E1M2;
    } else {
        return DumpTensorDataType::ACL_MAX;
    }
}

} // namespace __asc_aicore
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_DEBUG_UTILS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_DEBUG_UTILS_IMPL__
#endif

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_2201_ASC_DEBUG_UTILS_H
