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
 * \file asc_printf_simt_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_SIMT_IMPL__
#warning "impl/utils/debug/asc_printf_simt_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "utils/debug/asc_printf.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_UTILS_DEBUG_ASC_PRINTF_SIMT_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_PRINTF_SIMT_IMPL_H

#include "impl/utils/sys_macros.h"

#ifndef __SIMT_DEVICE_FUNCTIONS_DECL__
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#define __SIMT_DEVICE_FUNCTIONS_DECL__ __aicore__
#else
#define __SIMT_DEVICE_FUNCTIONS_DECL__ __simt_callee__
#endif
#endif

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#define __simt_gm__ __gm__
#else
#include "__clang_cce_simt_atomic.h"
#define __simt_gm__
#endif

inline __gm__ uint8_t* __simt_gm__ g_sysSimtPrintFifoSpace = nullptr;

namespace __asc_simt_vf {

constexpr uint32_t BYTE_ALGIN = 8;
constexpr uint16_t MAGIC = 0xAE86;

enum class DumpType : uint32_t {
    DUMP_DEFAULT = 0,
    DUMP_SCALAR,
    DUMP_TENSOR,
    DUMP_SHAPE,
    DUMP_ASSERT,
    DUMP_META,
    DUMP_TIME_STAMP,
    DUMP_SIMT,
    DUMP_BUFI,
    DUMP_BUFO,
    DUMP_SKIP,
    DUMP_SIMT_ASSERT = 0xF0E00F0E,
    DUMP_SIMT_PRINTF = 0xF0F00F0F,
    DUMP_WAIT = 0xF0A55A0F,
};

struct BlockRingBufInfo {
    uint32_t length = 0U;      // total size per block (include head and r/w info)
    uint32_t coreId = 0U;      // current core id
    uint32_t blockNum = 0U;    // total core num
    uint32_t ringBufLen = 0U;  // fifo buff size (print tlv storage)
    uint16_t magic = 0U;       // magic number
    uint16_t flag = 0U;        // 0: simd, 1: simt
    uint32_t rsv = 0U;         // reserve
    uint64_t ringBufAddr = 0U; // start addr of fifo buff
    uint32_t resvMem[6];       // reserved
};

struct RingBufWriteInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFI); // DumpType = DUMP_BUFI
    uint32_t length = 0U;                                       // u64 + u64
    uint64_t bufOffset = 0U;                                    // the offset of write addr relative to ringBufAddr
    uint64_t packIdx = 0U;                                      // print pack counter
};

struct RingBufReadInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFO); // DumpType = DUMP_BUFO
    uint32_t length = 0U;                                       // u64 + u64
    uint64_t bufOffset = 0U;                                    // the offset of read addr relative to ringBufAddr
    uint64_t resv = 0U;
};

struct PrintTlvInfoHead {
    uint32_t type = 0U;
    uint32_t length = 0U;
    uint32_t blockIdx[3] = {0U};
    uint32_t threadIdx[3] = {0U};
    uint32_t resv[4];        // reserved
    uint64_t fmtOffset = 0U; // offset of fmt string from the start of fmtOffset addr
};

union TlvHeadToBytes {
    PrintTlvInfoHead tlv_head;
    uint8_t bytes[56];
};

union TypeToByte8 {
    int64_t value_s64;
    uint64_t value_u64;
    float value_f32;
    double value_f64;
    uint8_t bytes[8];
};

union TypeToByte4 {
    uint32_t value;
    uint8_t bytes[4];
};

struct BaseTlv {
    uint16_t type;
    uint16_t len;
};

struct BinaryMetaAscFeature {
    BaseTlv head;
    uint32_t feature;
};

#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void enable_printf()
{
    static const struct BinaryMetaAscFeature __asc_feature_print__
        __attribute__((used, section(".ascend.meta"))) = {4, 4, 4};
}

template <uint32_t count = 1>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void __internal_nop()
{
#pragma unroll 8
    for (uint32_t i = 0; i < count; ++i) {
        __nop();
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t get_string_length(const __simt_gm__ char* s)
{
    uint32_t i = 0;
    while (*(s + i) != '\0') {
        i++;
    }
    return i + 1;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t get_print_args_len(uint32_t& args_num) { return 0; }

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t get_print_args_len(uint32_t& args_num, Args&&... args);

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t get_print_args_len_impl(
    uint32_t& args_num, __simt_gm__ const char* s, Args&&... args)
{
    constexpr uint32_t param_size = sizeof(uint64_t);
    const uint32_t str_len = get_string_length(s);
    args_num += 1;
    return param_size + str_len + get_print_args_len(args_num, args...);
}

template <typename T, typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t get_print_args_len_impl(uint32_t& args_num, T scalar, Args&&... args)
{
    constexpr uint32_t param_size = sizeof(uint64_t);
    args_num += 1;
    return param_size + get_print_args_len(args_num, args...);
}

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t get_print_args_len(uint32_t& args_num, Args&&... args)
{
    return get_print_args_len_impl(args_num, args...);
}

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t get_print_tlv_len(
    uint32_t& args_num, __simt_gm__ const char* fmt, Args&&... args)
{
    constexpr uint32_t print_info_len = sizeof(PrintTlvInfoHead);
    const uint32_t fmt_len = get_string_length(fmt);
    const uint32_t args_len = get_print_args_len(args_num, args...);
    return print_info_len + args_len + fmt_len;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline __simt_gm__ RingBufReadInfo* get_ring_buf_read_info(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info)
{
    __simt_gm__ uint8_t* block_head = reinterpret_cast<__simt_gm__ uint8_t*>(block_ring_buf_info);

    return reinterpret_cast<__simt_gm__ RingBufReadInfo*>(block_head + sizeof(BlockRingBufInfo));
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline __simt_gm__ RingBufWriteInfo* get_ring_buf_write_info(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info)
{
    __simt_gm__ uint8_t* ring_buf_addr = reinterpret_cast<__simt_gm__ uint8_t*>(block_ring_buf_info->ringBufAddr);

    return reinterpret_cast<__simt_gm__ RingBufWriteInfo*>(ring_buf_addr + block_ring_buf_info->ringBufLen);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void ring_buffer_wait(__simt_gm__ RingBufReadInfo* read_info, uint64_t end_offset)
{
    constexpr uint32_t nop_count = 2048; // reduce the pressure of L1 cache
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
    volatile uint64_t tmp =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(&read_info->bufOffset);
    while (end_offset >= tmp) {
        tmp = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(&read_info->bufOffset);
        __internal_nop<nop_count>();
    }
#else
    while (end_offset > *reinterpret_cast<volatile uint64_t*>(&read_info->bufOffset)) {
        __internal_nop<nop_count>();
    }
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t check_and_wait_ring_buf_space(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t& tlv_len)
{
    __simt_gm__ RingBufReadInfo* read_info = get_ring_buf_read_info(block_ring_buf_info);
    __simt_gm__ RingBufWriteInfo* write_info = get_ring_buf_write_info(block_ring_buf_info);

    uint64_t start_offset = 0;
    uint32_t active_thread = __activemask();
    tlv_len = __reduce_max(tlv_len);
    uint64_t total_len = __popc(active_thread) * tlv_len;
    int32_t lane_id = __ffs(static_cast<int32_t>(active_thread)) - 1;
    if (laneid() == lane_id) {
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
        start_offset = atomicAdd(&write_info->bufOffset, total_len);
#else
        start_offset = __atomic_add(&write_info->bufOffset, total_len);
#endif
        uint64_t end_offset = start_offset + total_len;
        volatile __simt_gm__ RingBufReadInfo* read_info_v =
            reinterpret_cast<volatile __simt_gm__ RingBufReadInfo*>(read_info);
        if (end_offset > read_info_v->bufOffset + block_ring_buf_info->ringBufLen) {
            ring_buffer_wait(read_info, end_offset - block_ring_buf_info->ringBufLen);
        }
    }
    uint32_t idx = __popc(active_thread & lanemask_lt());
    start_offset = __shfl(start_offset, lane_id, warpSize) + tlv_len * idx;
    return start_offset;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void write_ring_buf_tlv_head(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t& write_ptr, uint32_t tlv_len, uint32_t args_num)
{
    if (write_ptr + sizeof(PrintTlvInfoHead) > block_ring_buf_info->ringBufLen) {
        volatile __simt_gm__ uint8_t* data_ptr =
            reinterpret_cast<volatile __simt_gm__ uint8_t*>(block_ring_buf_info->ringBufAddr);
        TlvHeadToBytes tmp{
            .tlv_head = {
                static_cast<uint32_t>(DumpType::DUMP_WAIT),
                static_cast<uint32_t>(tlv_len - (sizeof(uint32_t) * 2)),
                {static_cast<uint32_t>(blockIdx.x), static_cast<uint32_t>(blockIdx.y),
                 static_cast<uint32_t>(blockIdx.z)},
                {static_cast<uint32_t>(threadIdx.x), static_cast<uint32_t>(threadIdx.y),
                 static_cast<uint32_t>(threadIdx.z)},
                {0},
                (args_num + 1) * sizeof(uint64_t)}};

        uint32_t part1_len = block_ring_buf_info->ringBufLen - write_ptr;
        for (int32_t index = 0; index < part1_len; index++) {
            data_ptr[write_ptr + index] = tmp.bytes[index];
        }
        uint32_t part2_len = sizeof(PrintTlvInfoHead) - part1_len;
        for (int32_t index = 0; index < part2_len; index++) {
            data_ptr[index] = tmp.bytes[part1_len + index];
        }
        write_ptr = part2_len;
    } else {
        volatile __simt_gm__ PrintTlvInfoHead* print_tlv =
            reinterpret_cast<volatile __simt_gm__ PrintTlvInfoHead*>(block_ring_buf_info->ringBufAddr + write_ptr);
        print_tlv->type = static_cast<uint32_t>(DumpType::DUMP_WAIT);
        print_tlv->length = static_cast<uint32_t>(tlv_len - (sizeof(uint32_t) * 2)); // 2: exclude type and length
        print_tlv->blockIdx[0] = blockIdx.x;
        print_tlv->blockIdx[1] = blockIdx.y;
        print_tlv->blockIdx[2] = blockIdx.z;
        print_tlv->threadIdx[0] = threadIdx.x;
        print_tlv->threadIdx[1] = threadIdx.y;
        print_tlv->threadIdx[2] = threadIdx.z;
        print_tlv->resv[0] = 0;
        print_tlv->resv[1] = 0;
        print_tlv->resv[2] = 0;
        print_tlv->resv[3] = 0;
        print_tlv->fmtOffset = (args_num + 1) * sizeof(uint64_t);
        write_ptr = write_ptr + sizeof(PrintTlvInfoHead);
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t write_string(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, __simt_gm__ const char* str, uint32_t write_ptr)
{
    write_ptr = write_ptr % block_ring_buf_info->ringBufLen;
    uint32_t str_len = get_string_length(str);
    volatile __simt_gm__ char* data_ptr =
        reinterpret_cast<volatile __simt_gm__ char*>(block_ring_buf_info->ringBufAddr);
    if (write_ptr + str_len > block_ring_buf_info->ringBufLen) {
        uint32_t part1_len = block_ring_buf_info->ringBufLen - write_ptr;
        for (int32_t index = 0; index < part1_len; index++) {
            data_ptr[write_ptr + index] = str[index];
        }
        uint32_t part2_len = str_len - part1_len;
        for (int32_t index = 0; index < part2_len; index++) {
            data_ptr[index] = str[part1_len + index];
        }
    } else {
#pragma unroll
        for (int32_t i = 0; i < str_len; i++) {
            data_ptr[write_ptr + i] = str[i];
        }
    }
    return str_len;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float bf16_to_float(bfloat16_t x)
{
    union Data {
        bfloat16_t bf;
        unsigned int i;
    };
    union Data d = {.bf = x};
    unsigned int u = d.i << 16;
    union Data2 {
        float f;
        unsigned int i;
    };
    union Data2 d2 = {.i = u};
    return d2.f;
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void write_scalar(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t write_ptr, T scalar)
{
    TypeToByte8 tmp{0};
    if constexpr (std::is_same_v<T, half> || std::is_same_v<T, float>) {
        tmp.value_f32 = static_cast<float>(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        tmp.value_s64 = static_cast<int64_t>(scalar);
    } else if constexpr (std::is_unsigned<T>::value) {
        tmp.value_u64 = static_cast<uint64_t>(scalar);
    } else if constexpr (std::is_same_v<T, bfloat16_t>) {
        tmp.value_f32 = bf16_to_float(scalar);
    } else if constexpr (std::is_pointer<T>::value) {
        tmp.value_u64 = (uintptr_t)scalar;
    } else if constexpr (std::is_enum<T>::value) {
        tmp.value_u64 = static_cast<uint64_t>(scalar);
    }

    write_ptr = write_ptr % block_ring_buf_info->ringBufLen;
    volatile __simt_gm__ uint64_t* data_ptr =
        reinterpret_cast<volatile __simt_gm__ uint64_t*>(block_ring_buf_info->ringBufAddr + write_ptr);
    *data_ptr = tmp.value_u64;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void set_param(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t start_offset, uint32_t param_idx, uint32_t str_offset)
{
    return;
}

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void set_param(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t start_offset, uint32_t param_idx, uint32_t str_offset,
    Args&&... args);

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void set_param_impl(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t start_offset, uint32_t param_idx, uint32_t str_offset,
    __simt_gm__ const char* s, Args&&... args)
{
    uint32_t param_offset = sizeof(uint64_t) * param_idx;
    write_scalar(block_ring_buf_info, start_offset + param_offset, str_offset - param_offset);
    uint32_t str_len = write_string(block_ring_buf_info, s, start_offset + str_offset);
    set_param(block_ring_buf_info, start_offset, param_idx + 1, str_offset + str_len, args...);
}

template <typename T, typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void set_param_impl(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t start_offset, uint32_t param_idx, uint32_t str_offset,
    T scalar, Args&&... args)
{
    uint32_t param_offset = sizeof(uint64_t) * param_idx;
    write_scalar(block_ring_buf_info, start_offset + param_offset, scalar);
    set_param(block_ring_buf_info, start_offset, param_idx + 1, str_offset, args...);
}

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void set_param(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t start_offset, uint32_t param_idx, uint32_t str_offset,
    Args&&... args)
{
    set_param_impl(block_ring_buf_info, start_offset, param_idx, str_offset, args...);
}

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void write_ring_buf_tlv_data(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint32_t start_offset, uint32_t args_num,
    __simt_gm__ const char* fmt, Args&&... args)
{
    uint32_t fmt_offset = args_num * sizeof(uint64_t);
    uint32_t str_len = write_string(block_ring_buf_info, fmt, start_offset + fmt_offset);
    set_param(block_ring_buf_info, start_offset, 0, fmt_offset + str_len, args...);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void write_finish(
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info, uint64_t write_ptr, DumpType print_type)
{
    __simt_gm__ RingBufWriteInfo* write_info = get_ring_buf_write_info(block_ring_buf_info);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
    atomicExch(
        reinterpret_cast<__simt_gm__ uint32_t*>(block_ring_buf_info->ringBufAddr + write_ptr),
        static_cast<uint32_t>(print_type));
    atomicAdd(&write_info->packIdx, 1);
#else
    __atomic_exch(
        reinterpret_cast<__simt_gm__ uint32_t*>(block_ring_buf_info->ringBufAddr + write_ptr),
        static_cast<uint32_t>(print_type));
    __atomic_add(&write_info->packIdx, 1);
#endif
}
#endif

template <typename... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void simt_printf_impl(
    DumpType print_type, __simt_gm__ const char* fmt, Args&&... args)
{
#ifndef ASCENDC_CPU_DEBUG
    enable_printf();
    if (g_sysSimtPrintFifoSpace == nullptr) {
        return;
    }
    __simt_gm__ BlockRingBufInfo* block_ring_buf_info =
        reinterpret_cast<__simt_gm__ BlockRingBufInfo*>(g_sysSimtPrintFifoSpace);
    if (block_ring_buf_info->magic != MAGIC) {
        return;
    }

    uint32_t args_num = 0;
    uint32_t tlv_len = get_print_tlv_len(args_num, fmt, args...);
    tlv_len = (tlv_len + BYTE_ALGIN - 1) & (~(BYTE_ALGIN - 1)); // 8 byte align
    uint64_t start_offset = check_and_wait_ring_buf_space(block_ring_buf_info, tlv_len);
    start_offset = start_offset % block_ring_buf_info->ringBufLen;

    uint32_t write_ptr = static_cast<uint32_t>(start_offset);
    write_ring_buf_tlv_head(block_ring_buf_info, write_ptr, tlv_len, args_num);
    write_ring_buf_tlv_data(block_ring_buf_info, write_ptr, args_num, fmt, args...);

    __threadfence();
    write_finish(block_ring_buf_info, start_offset, print_type);

    if (print_type == DumpType::DUMP_SIMT_ASSERT) {
        __sync_workitems();
    }
#endif
}

#ifndef __CHECK_FEATURE_AT_PRECOMPILE

template <class... Args>
static __attribute__((noinline)) __SIMT_DEVICE_FUNCTIONS_DECL__ void printf(const __simt_gm__ char* fmt, Args&&... args)
{
#if !defined(ASCENDC_DUMP) || (ASCENDC_DUMP != 0)
    simt_printf_impl(DumpType::DUMP_SIMT_PRINTF, fmt, args...);
#endif
}

#endif

} // namespace __asc_simt_vf

#endif // IMPL_UTILS_DEBUG_ASC_PRINTF_SIMT_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_SIMT_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_SIMT_IMPL__
#endif
