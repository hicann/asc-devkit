/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning                                                                                                               \
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_INSTRUCTION_H

#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace asc {
namespace te {

struct copy_gm_to_l1_trait {};

template <typename T>
__aicore__ inline void set_mte2_nz_para(const T& para)
{
    asc_set_gm2l1_nz_para(para);
}

class copy_gm_to_l1_align_v2_instr {
public:
    template <typename T>
    __aicore__ inline static void data_copy(__cbuf__ T* dst, __gm__ T* src, uint32_t block_count,
                                                            uint32_t block_len, uint8_t left_padding_cnt,
                                                            uint8_t right_padding_cnt, uint8_t cache_mode,
                                                            uint64_t src_stride, uint32_t dst_stride)
    {
        if ASCEND_IS_AIV {
            return;
        }
        if constexpr (sizeof(T) == sizeof(int8_t)) {
            asc_copy_gm2l1_align((__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, block_count, block_len,
                                 left_padding_cnt, right_padding_cnt, true, cache_mode, src_stride, dst_stride);
        } else if constexpr (sizeof(T) == sizeof(half)) {
            asc_copy_gm2l1_align((__cbuf__ half*)dst, (__gm__ half*)src, block_count, block_len,
                                 left_padding_cnt, right_padding_cnt, true, cache_mode, src_stride, dst_stride);
        } else if constexpr (sizeof(T) == sizeof(float)) {
            asc_copy_gm2l1_align((__cbuf__ float*)dst, (__gm__ float*)src, block_count, block_len,
                                 left_padding_cnt, right_padding_cnt, true, cache_mode, src_stride, dst_stride);
        } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
            asc_copy_gm2l1_align((__cbuf__ uint32_t*)dst, (__gm__ uint32_t*)src, block_count, block_len,
                                 left_padding_cnt, right_padding_cnt, true, cache_mode, src_stride, dst_stride);
        }
    }
};

class copy_gm_to_l1_multi_nd2nz_instr {
public:
    template <typename T>
    __aicore__ inline static void data_copy(__cbuf__ T* dst, __gm__ T* src, uint16_t nd_num,
                                                               uint16_t loop2_dst_stride, uint16_t loop3_dst_stride,
                                                               uint16_t loop4_dst_stride, uint64_t loop1_src_stride,
                                                               uint8_t cache_mode, uint16_t n_value, uint32_t d_value,
                                                               uint64_t loop4_src_stride, bool enable_small_c0)
    {
        if ASCEND_IS_AIV {
            return;
        }
        uint64_t mte2_nz_para = static_cast<uint64_t>(loop4_dst_stride) << 48; // MTE2_NZ_PARA[63:48]
        mte2_nz_para |= static_cast<uint64_t>(loop3_dst_stride) << 32;         // MTE2_NZ_PARA[47:32]
        mte2_nz_para |= static_cast<uint64_t>(loop2_dst_stride) << 16;         // MTE2_NZ_PARA[31:16]
        mte2_nz_para |= static_cast<uint64_t>(nd_num);                         // MTE2_NZ_PARA[15:0]
        set_mte2_nz_para(mte2_nz_para);
        if constexpr (sizeof(T) == sizeof(int8_t)) {
            asc_copy_gm2l1_nd2nz((__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, loop1_src_stride, cache_mode,
                                 n_value, d_value, loop4_src_stride, enable_small_c0);
        } else if constexpr (sizeof(T) == sizeof(half)) {
            asc_copy_gm2l1_nd2nz((__cbuf__ half*)dst, (__gm__ half*)src, loop1_src_stride, cache_mode,
                                 n_value, d_value, loop4_src_stride, enable_small_c0);
        } else if constexpr (sizeof(T) == sizeof(float)) {
            asc_copy_gm2l1_nd2nz((__cbuf__ float*)dst, (__gm__ float*)src, loop1_src_stride, cache_mode,
                                 n_value, d_value, loop4_src_stride, enable_small_c0);
        }
    }
};

class copy_gm_to_l1_multi_dn2nz_instr {
public:
    template <typename T>
    __aicore__ inline static void data_copy(__cbuf__ T* dst, __gm__ T* src, uint16_t dn_num,
                                                               uint16_t loop2_dst_stride, uint16_t loop3_dst_stride,
                                                               uint16_t loop4_dst_stride, uint64_t loop1_src_stride,
                                                               uint8_t cache_mode, uint16_t n_value, uint32_t d_value,
                                                               uint64_t loop4_src_stride, bool enable_small_c0)
    {
        if ASCEND_IS_AIV {
            return;
        }

        uint64_t mte2_nz_para = static_cast<uint64_t>(loop4_dst_stride) << 48; // MTE2_NZ_PARA[63:48]
        mte2_nz_para |= static_cast<uint64_t>(loop3_dst_stride) << 32;         // MTE2_NZ_PARA[47:32]
        mte2_nz_para |= static_cast<uint64_t>(loop2_dst_stride) << 16;         // MTE2_NZ_PARA[31:16]
        mte2_nz_para |= static_cast<uint64_t>(dn_num);                         // MTE2_NZ_PARA[15:0]
        set_mte2_nz_para(mte2_nz_para);
        if constexpr (sizeof(T) == sizeof(int8_t)) {
            asc_copy_gm2l1_dn2nz((__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, loop1_src_stride, cache_mode,
                                 n_value, d_value, loop4_src_stride, enable_small_c0);
        } else if constexpr (sizeof(T) == sizeof(half)) {
            asc_copy_gm2l1_dn2nz((__cbuf__ half*)dst, (__gm__ half*)src, loop1_src_stride, cache_mode,
                                 n_value, d_value, loop4_src_stride, enable_small_c0);
        } else if constexpr (sizeof(T) == sizeof(float)) {
            asc_copy_gm2l1_dn2nz((__cbuf__ float*)dst, (__gm__ float*)src, loop1_src_stride, cache_mode,
                                 n_value, d_value, loop4_src_stride, enable_small_c0);
        }
    }
};

} // namespace te
} // namespace asc


#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
