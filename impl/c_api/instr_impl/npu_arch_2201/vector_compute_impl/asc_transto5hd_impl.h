/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/instr_impl/npu_arch_2201/vector_compute_impl/asc_transto5hd_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_2201_VECTOR_COMPUTE_ASC_TRANSTO5HD_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_2201_VECTOR_COMPUTE_ASC_TRANSTO5HD_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_2201/utils_impl/utils_impl.h"

#define ASC_TRANSTO5HD_B8_CALL(dst_va, src_va, repeat, dst_stride, src_stride, dst_high_half, src_high_half) \
    do {                                                                                                     \
        if (!(dst_high_half)) {                                                                              \
            if (!(src_high_half)) {                                                                          \
                scatter_vnchwconv_b8(dst_va, src_va, repeat, dst_stride, src_stride, false, false);          \
            } else {                                                                                         \
                scatter_vnchwconv_b8(dst_va, src_va, repeat, dst_stride, src_stride, false, true);           \
            }                                                                                                \
        } else if (!(src_high_half)) {                                                                       \
            scatter_vnchwconv_b8(dst_va, src_va, repeat, dst_stride, src_stride, true, false);               \
        } else {                                                                                             \
            scatter_vnchwconv_b8(dst_va, src_va, repeat, dst_stride, src_stride, true, true);                \
        }                                                                                                    \
    } while (0)

__aicore__ inline void asc_transto5hd_b8_impl(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride, bool dst_high_half,
    bool src_high_half)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                ASC_TRANSTO5HD_B8_CALL(VA0, VA2, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            } else if (src == ub_addr8_t::VA4) {
                ASC_TRANSTO5HD_B8_CALL(VA0, VA4, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            } else if (src == ub_addr8_t::VA6) {
                ASC_TRANSTO5HD_B8_CALL(VA0, VA6, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                ASC_TRANSTO5HD_B8_CALL(VA2, VA0, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            } else if (src == ub_addr8_t::VA4) {
                ASC_TRANSTO5HD_B8_CALL(VA2, VA4, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            } else if (src == ub_addr8_t::VA6) {
                ASC_TRANSTO5HD_B8_CALL(VA2, VA6, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                ASC_TRANSTO5HD_B8_CALL(VA4, VA0, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            } else if (src == ub_addr8_t::VA2) {
                ASC_TRANSTO5HD_B8_CALL(VA4, VA2, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            } else if (src == ub_addr8_t::VA6) {
                ASC_TRANSTO5HD_B8_CALL(VA4, VA6, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                ASC_TRANSTO5HD_B8_CALL(VA6, VA0, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            } else if (src == ub_addr8_t::VA2) {
                ASC_TRANSTO5HD_B8_CALL(VA6, VA2, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            } else if (src == ub_addr8_t::VA4) {
                ASC_TRANSTO5HD_B8_CALL(VA6, VA4, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
            }
        }
    }
}

#undef ASC_TRANSTO5HD_B8_CALL

__aicore__ inline void asc_transto5hd_b8_sync_impl(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride, bool dst_high_half,
    bool src_high_half)
{
    asc_transto5hd_b8_impl(dst, src, repeat, dst_stride, src_stride, dst_high_half, src_high_half);
    asc_sync_post_process();
}

__aicore__ inline void asc_transto5hd_b16_impl(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA0, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA0, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA0, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA2, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA2, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA2, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA4, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA4, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA4, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA6, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA6, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA6, VA4, repeat, dst_stride, src_stride);
            }
        }
    }
}

__aicore__ inline void asc_transto5hd_b16_sync_impl(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride)
{
    asc_transto5hd_b16_impl(dst, src, repeat, dst_stride, src_stride);
    asc_sync_post_process();
}

__aicore__ inline void asc_transto5hd_b32_impl(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA0, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA0, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA0, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA2, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA2, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA2, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA4, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA4, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA4, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA6, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA6, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA6, VA4, repeat, dst_stride, src_stride);
            }
        }
    }
}

__aicore__ inline void asc_transto5hd_b32_sync_impl(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride)
{
    asc_transto5hd_b32_impl(dst, src, repeat, dst_stride, src_stride);
    asc_sync_post_process();
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
