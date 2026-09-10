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
 * \file asc_simd_printf_impl.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_ASC_SIMD_PRINTF_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_SIMD_PRINTF_IMPL_H

#include "impl/utils/sys_macros.h"

#ifndef ASCENDC_CPU_DEBUG
#include "impl/utils/debug/asc_debug_utils.h"
#include "impl/utils/debug/npu_arch_3510/asc_type_conversion_utils.h"

namespace __asc_simd_vf {
template <typename T>
__simd_callee__ inline void set_scalar_param_vf_impl(__ubuf__ uint8_t* param_addr, uint32_t param_idx, T scalar)
{
    __ubuf__ uint64_t* scalar_addr = (__ubuf__ uint64_t*)param_addr + param_idx;
    *scalar_addr = 0;

    if constexpr (is_same_in_list<T, half, float>()) {
        *((__ubuf__ float*)scalar_addr) = static_cast<float>(scalar);
    } else if constexpr (is_same_in_list<T, double>()) {
        *((__ubuf__ double*)scalar_addr) = static_cast<double>(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        *((__ubuf__ int64_t*)scalar_addr) = static_cast<int64_t>(scalar);
    } else if constexpr (std::is_unsigned<T>::value) {
        *((__ubuf__ uint64_t*)scalar_addr) = static_cast<uint64_t>(scalar);
    } else if constexpr (is_same_in_list<T, bfloat16_t, float8_e5m2_t, float8_e8m0_t, float8_e4m3_t, hifloat8_t>()) {
        *((__ubuf__ float*)scalar_addr) = to_float(scalar);
    } else if constexpr (std::is_pointer<T>::value) {
        *((__ubuf__ uint64_t*)scalar_addr) = (uintptr_t)scalar;
    } else if constexpr (std::is_enum<T>::value) {
        *((__ubuf__ uint64_t*)scalar_addr) = static_cast<uint64_t>(scalar);
    }
}

__simd_callee__ constexpr uint32_t align_print_tlv_len(const uint32_t data_len)
{
    constexpr uint32_t align_bytes = 8;
    return ((data_len + (align_bytes - 1)) & ~(align_bytes - 1)) + align_bytes;
}

template <typename T>
__simd_callee__ inline void set_scalar_param_vf(__ubuf__ uint8_t* param_addr, uint32_t param_idx, T scalar)
{
    set_scalar_param_vf_impl(param_addr, param_idx, scalar);
}

__simd_callee__ inline void set_string_param_vf(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, __ubuf__ const char* s, uint32_t& offset)
{
    __ubuf__ uint64_t* string_addr = reinterpret_cast<__ubuf__ uint64_t*>(param_addr) + param_idx;
    __ubuf__ uint8_t* dst_str_addr = param_addr + offset;

    *string_addr = static_cast<uint64_t>(offset - sizeof(uint64_t) * param_idx);

    uint32_t str_len = get_cstring_len_vf(s);
    for (uint32_t i = 0; i < str_len; i++) {
        *(dst_str_addr + i) = *(s + i);
    }
    offset += str_len;
}

__simd_callee__ inline void set_param_vf(__ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset)
{
    (void)param_addr;
    (void)param_idx;
    (void)offset;
    return;
}

template <typename... Args>
__simd_callee__ inline void set_param_vf(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset, Args&&... args);

template <typename... Args>
__simd_callee__ inline void set_param_vf_impl(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset, __ubuf__ const char* s, Args&&... args)
{
    set_string_param_vf(param_addr, param_idx, s, offset);
    set_param_vf(param_addr, param_idx + 1, offset, args...);
}

template <typename T, typename... Args>
__simd_callee__ inline void set_param_vf_impl(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset, T scalar, Args&&... args)
{
    set_scalar_param_vf(param_addr, param_idx, scalar);
    set_param_vf(param_addr, param_idx + 1, offset, args...);
}

template <typename... Args>
__simd_callee__ inline void set_param_vf(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset, Args&&... args)
{
    set_param_vf_impl(param_addr, param_idx, offset, args...);
}

__simd_callee__ inline uint32_t get_args_len_vf(uint32_t& args_num)
{
    (void)args_num;
    return 0;
}

template <typename... Args>
__simd_callee__ inline uint32_t get_args_len_vf(uint32_t& args_num, Args&&... args);

template <typename... Args>
__simd_callee__ inline uint32_t get_args_len_vf_impl(uint32_t& args_num, __ubuf__ const char* s, Args&&... args)
{
    constexpr uint32_t param_size = sizeof(uint64_t);
    const uint32_t str_len = get_cstring_len_vf(s);
    args_num += 1;
    return param_size + str_len + get_args_len_vf(args_num, args...);
}

template <typename T, typename... Args>
__simd_callee__ inline uint32_t get_args_len_vf_impl(uint32_t& args_num, T scalar, Args&&... args)
{
    constexpr uint32_t param_size = sizeof(uint64_t);
    args_num += 1;
    return param_size + get_args_len_vf(args_num, args...);
}

template <typename... Args>
__simd_callee__ inline uint32_t get_args_len_vf(uint32_t& args_num, Args&&... args)
{
    return get_args_len_vf_impl(args_num, args...);
}

template <typename... Args>
__simd_callee__ inline uint32_t get_print_tlv_len_simd(
    uint32_t& args_num, __ubuf__ const char* fmt, __ubuf__ const char* dump_head, Args&&... args)
{
    constexpr uint32_t print_info_len = sizeof(PrintTlv);
    const uint32_t args_len = get_args_len_vf(args_num, args...);
    const uint32_t fmt_len = get_cstring_len_vf(fmt);
    const uint32_t dump_head_len = get_cstring_len_vf(dump_head) - 1;
    return align_print_tlv_len(print_info_len + args_len + fmt_len + dump_head_len);
}

__simd_callee__ inline void set_print_tlv_info_vf(
    DumpType debug_type, __ubuf__ PrintTlv* print_tlv, const uint32_t& tlv_len, const uint32_t& args_num,
    uint16_t block_idx)
{
    print_tlv->type = static_cast<uint32_t>(debug_type);
    print_tlv->length = tlv_len - sizeof(uint32_t[2]);
    print_tlv->blockIdx = block_idx;
    print_tlv->resv = static_cast<uint32_t>(0U);
    print_tlv->fmtOffset = (args_num + 1) * sizeof(uint64_t);
}

__simd_callee__ inline void copy_fmt_to_ubuf(__ubuf__ uint8_t* dst, __ubuf__ const char* src, uint32_t len)
{
    for (uint32_t i = 0; i < len; i += 2) {
        dst[i] = src[i];
        dst[i + 1] = src[i + 1];
    }
}

template <typename... Args>
__simd_callee__ inline void set_print_tlv_data_vf(
    __ubuf__ PrintTlv* print_tlv, __ubuf__ const char* fmt, __ubuf__ const char* dump_head, Args&&... args)
{
    const uint32_t dump_head_len = get_cstring_len_vf(dump_head) - 1;
    const uint32_t fmt_len = get_cstring_len_vf(fmt);
    __ubuf__ uint8_t* param_addr = reinterpret_cast<__ubuf__ uint8_t*>(print_tlv + 1);
    __ubuf__ uint8_t* dump_head_addr = param_addr + print_tlv->fmtOffset - sizeof(uint64_t);
    __ubuf__ uint8_t* fmt_addr = dump_head_addr + dump_head_len;

    copy_fmt_to_ubuf(dump_head_addr, dump_head, dump_head_len);
    copy_fmt_to_ubuf(fmt_addr, fmt, fmt_len);

    uint32_t str_param_offset = print_tlv->fmtOffset + dump_head_len + fmt_len;
    set_param_vf(param_addr, 0, str_param_offset, args...);
}

template <class... Args>
__simd_callee__ inline void scalar_printf_impl(
    DumpType debug_type, __ubuf__ const char* fmt, __ubuf__ const char* dump_head, Args&&... args)
{
    __ubuf__ BlockVFBufInfo* block_info = get_printf_ubuf_addr(0);
    if (block_info->flag != 0) {
        return;
    }

    uint32_t args_num = 0;
    const uint32_t tlv_len = get_print_tlv_len_simd(args_num, fmt, dump_head, args...);
    uint32_t write_len = block_info->writeLen;

    if (tlv_len > block_info->length || write_len > block_info->length) {
        block_info->flag = 1;
        return;
    }

    if (write_len + tlv_len > block_info->length) {
        wait_vf_debug_buffer_drained_and_reset(block_info);
        write_len = 0;
    }

    if (!reserve_debug_tlv(block_info, tlv_len)) {
        return;
    }

    __ubuf__ PrintTlv* print_tlv =
        reinterpret_cast<__ubuf__ PrintTlv*>((__ubuf__ uint8_t*)(block_info->buffer) + write_len);
    set_print_tlv_info_vf(debug_type, print_tlv, tlv_len, args_num, block_info->blockIdx);
    set_print_tlv_data_vf(print_tlv, fmt, dump_head, args...);

    block_info->magic = ASCENDC_SIMD_VF_MAGIC_NUMBER;
    block_info->writeLen = write_len + tlv_len;
}

template <class... Args>
__simd_callee__ inline void printf_impl(__ubuf__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_diagnostics();
    scalar_printf_impl(DumpType::DUMP_SCALAR, fmt, "", args...);
#endif
}

template <class... Args>
__simd_callee__ inline void printf_impl_assert(__ubuf__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_assert();
    scalar_printf_impl(DumpType::DUMP_ASSERT, fmt, "", args...);
#endif
}

template <class... Args>
__simd_callee__ inline void printf_impl_assert_msg(
    __ubuf__ const char* assertion, __ubuf__ const char* file, unsigned int line, __ubuf__ const char* function,
    __ubuf__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_assert();
    scalar_printf_impl(
        DumpType::DUMP_ASSERT, fmt, "[ASSERT] %s:%u: %s: Assertion '%s' failed. ", file, line, function, assertion,
        args...);
#endif
}

template <class... Args>
__ASC_USE_RESERVED_UBUF__(3510, "printf is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void printf(__ubuf__ const char* fmt, Args&&... args)
{
    printf_impl(fmt, args...);
}
} // namespace __asc_simd_vf

#else

#include <cstdio>

namespace __asc_simd_vf {
template <class... Args>
__simd_callee__ inline void printf_impl(__ubuf__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    std::printf(fmt, args...);
#endif
}

template <class... Args>
__simd_callee__ inline void printf(__ubuf__ const char* fmt, Args&&... args)
{
    printf_impl(fmt, args...);
}
} // namespace __asc_simd_vf

#endif // ASCENDC_CPU_DEBUG

#endif // IMPL_UTILS_DEBUG_ASC_SIMD_PRINTF_IMPL_H
