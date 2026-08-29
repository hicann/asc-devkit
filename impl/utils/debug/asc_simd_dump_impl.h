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
 * \file asc_simd_dump_impl.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_ASC_SIMD_DUMP_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_SIMD_DUMP_IMPL_H

#include "impl/utils/sys_macros.h"

#ifndef ASCENDC_CPU_DEBUG

#include "impl/utils/debug/asc_debug_utils.h"

namespace __asc_simd_vf {
__simd_callee__ inline uint32_t get_reg_dump_u32_count(uint32_t dump_size_bytes)
{
    return (dump_size_bytes + sizeof(uint32_t) - 1) / sizeof(uint32_t);
}

enum class DumpTensorPosition : uint16_t { GM = 0, UB, L1, L0A, L0B, L0C, BIAS, FIXBUF, REG, MAX };

template <DumpTensorPosition dumpPosition, typename T, typename U>
__simd_callee__ inline void set_dump_tlv_info_vf(
    U& src, __ubuf__ DumpTensorTlv* dump_tlv, uint32_t align_dump_len, uint32_t desc, uint32_t dump_size,
    uint16_t block_idx)
{
    dump_tlv->type = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    dump_tlv->length = sizeof(DumpTensorTlv) - sizeof(uint32_t[2]) + align_dump_len;
    if constexpr (dumpPosition == DumpTensorPosition::UB) {
        dump_tlv->tensorAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src));
    } else {
        dump_tlv->tensorAddr = 0U;
    }
    dump_tlv->dataType = static_cast<uint32_t>(get_dump_datatype<T>());
    dump_tlv->desc = desc;
    dump_tlv->blockIdx = block_idx;
    dump_tlv->bufferId = static_cast<uint32_t>(0U);
    dump_tlv->position = static_cast<uint16_t>(dumpPosition);
    dump_tlv->dim = static_cast<uint32_t>(0U);
    for (uint32_t i = 0; i < 8; ++i) {
        dump_tlv->shape[i] = static_cast<uint32_t>(0U);
    }
    dump_tlv->resv1 = static_cast<uint32_t>(0U);
    dump_tlv->dumpSize = dump_size * sizeof(T);
}

template <typename T, typename U>
__simd_callee__ inline void set_dump_tlv_data_ubuf(
    U& src, __ubuf__ DumpTensorTlv* dump_tlv, uint32_t align_dump_len, uint32_t dump_size)
{
    __ubuf__ T* dump_dst_addr = reinterpret_cast<__ubuf__ T*>(dump_tlv + 1);

    for (uint32_t i = 0; i < dump_size; i++) {
        dump_dst_addr[i] = src[i];
    }
}

template <typename T, typename U>
__simd_callee__ inline void set_dump_tlv_data_reg(
    U& src, __ubuf__ DumpTensorTlv* dump_tlv, uint32_t align_dump_len, uint32_t dump_size)
{
    __ubuf__ T* dump_dst_addr = reinterpret_cast<__ubuf__ T*>(dump_tlv + 1);

    const uint32_t count = get_reg_dump_u32_count(dump_tlv->dumpSize);
    vector_align ureg;
    vstus(ureg, count, (vector_u32&)src, (__ubuf__ uint32_t*&)dump_dst_addr, POST_UPDATE);
    vstas(ureg, (__ubuf__ uint32_t*&)dump_dst_addr, 0, POST_UPDATE);
}

template <typename T>
__simd_callee__ inline uint32_t reserve_dump_tlv(__ubuf__ BlockVFBufInfo* block_info, uint32_t dump_size)
{
    if (block_info->flag != 0) {
        return 0;
    }

    constexpr uint16_t data_block_size = 32;
    constexpr uint32_t max_dump_data_len = ASCENDC_SIMD_VF_PRINTF_UBUF_MAX_SIZE > sizeof(DumpTensorTlv) ?
                                               ASCENDC_SIMD_VF_PRINTF_UBUF_MAX_SIZE - sizeof(DumpTensorTlv) :
                                               0;
    if (dump_size > max_dump_data_len / sizeof(T)) {
        block_info->flag = 1;
        return 0;
    }

    const uint32_t align_dump_len = align_up(dump_size * sizeof(T), data_block_size);
    const uint32_t tlv_len = sizeof(DumpTensorTlv) + align_dump_len;
    uint32_t write_len = block_info->writeLen;
    if (tlv_len > block_info->length || write_len > block_info->length) {
        block_info->flag = 1;
        return 0;
    }
    if (write_len + tlv_len > block_info->length) {
        wait_vf_debug_buffer_drained_and_reset(block_info);
    }
    return reserve_debug_tlv(block_info, tlv_len) ? tlv_len : 0;
}

template <DumpTensorPosition dumpPosition, typename T, typename U>
__simd_callee__ inline void asc_dump_impl(U& src, uint32_t desc, uint32_t dump_size)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    __ubuf__ BlockVFBufInfo* block_info = get_printf_ubuf_addr(0);
    const uint32_t tlv_len = reserve_dump_tlv<T>(block_info, dump_size);
    if (tlv_len == 0) {
        return;
    }
    const uint32_t align_dump_len = tlv_len - sizeof(DumpTensorTlv);

    __ubuf__ DumpTensorTlv* dump_tlv =
        (__ubuf__ DumpTensorTlv*)((__ubuf__ uint8_t*)(block_info->buffer) + block_info->writeLen);
    set_dump_tlv_info_vf<dumpPosition, T>(src, dump_tlv, align_dump_len, desc, dump_size, block_info->blockIdx);
    if constexpr (dumpPosition == DumpTensorPosition::REG) {
        set_dump_tlv_data_reg<T>(src, dump_tlv, align_dump_len, dump_size);
    } else {
        set_dump_tlv_data_ubuf<T>(src, dump_tlv, align_dump_len, dump_size);
    }

    block_info->magic = ASCENDC_SIMD_VF_MAGIC_NUMBER;
    block_info->writeLen += tlv_len;
#endif
}

template <typename T, typename U>
__ASC_USE_RESERVED_UBUF__(
    3510, "asc_dump_reg is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_diagnostics();
    asc_dump_impl<DumpTensorPosition::REG, T>(input, desc, dump_size);
#endif
}

template <typename T>
__ASC_USE_RESERVED_UBUF__(
    3510, "asc_dump_ubuf is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_diagnostics();
    asc_dump_impl<DumpTensorPosition::UB, T>(input, desc, dump_size);
#endif
}

template <typename T, typename U>
__ASC_USE_RESERVED_UBUF__(3510, "asc_dump is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_diagnostics();
    asc_dump_impl<DumpTensorPosition::REG, T>(input, desc, dump_size);
#endif
}

template <typename T>
__ASC_USE_RESERVED_UBUF__(3510, "asc_dump is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_diagnostics();
    asc_dump_impl<DumpTensorPosition::UB, T>(input, desc, dump_size);
#endif
}
} // namespace __asc_simd_vf

#else

#include <cassert>

namespace __asc_simd_vf {
template <typename T, typename U>
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size)
{
    (void)input;
    (void)desc;
    (void)dump_size;
    assert(false && "asc_dump_reg is not supported in cpu mode.");
}

template <typename T>
__simd_callee__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    (void)input;
    (void)desc;
    (void)dump_size;
    assert(false && "asc_dump_ubuf is not supported in cpu mode.");
}

template <typename T, typename U>
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size)
{
    (void)input;
    (void)desc;
    (void)dump_size;
    assert(false && "asc_dump is not supported in cpu mode.");
}

template <typename T>
__simd_callee__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    (void)input;
    (void)desc;
    (void)dump_size;
    assert(false && "asc_dump is not supported in cpu mode.");
}
} // namespace __asc_simd_vf

#if __NPU_ARCH__ == 3510
namespace __asc_aicore {
// CPU debug exposes unqualified debug APIs through this namespace; bridge the SIMD-only register overloads.
template <typename T, typename U>
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size)
{
    __asc_simd_vf::asc_dump_reg<T>(input, desc, dump_size);
}

template <typename T, typename U>
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size)
{
    __asc_simd_vf::asc_dump<T>(input, desc, dump_size);
}
} // namespace __asc_aicore
#endif

#endif // ASCENDC_CPU_DEBUG

#endif // IMPL_UTILS_DEBUG_ASC_SIMD_DUMP_IMPL_H
