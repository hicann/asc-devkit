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
    "impl/tensor_api/arch/vector/gm_to_ub/copy_impl/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_INSTRUCTION_H

#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace asc {
namespace te {

class copy_gm_to_ub_instr {
public:
    template <typename T>
    __aicore__ inline static void
    data_copy(__ubuf__ T* dst, __gm__ T* src, const uint16_t block_count, const uint32_t block_len,
                 const uint8_t left_padding_count, const uint8_t right_padding_count,
                 const bool enable_constant_pad, const asc_load_l2_cache_mode cache_mode,
                 const int64_t src_stride, const int64_t dst_stride)
    {
        if ASCEND_IS_AIC {
            return;
        }

        if constexpr (sizeof(T) == 1) {
            asc_copy_gm2ub_align((__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, block_count, block_len,
                                 left_padding_count, right_padding_count, enable_constant_pad, cache_mode, src_stride,
                                 dst_stride);
        } else if constexpr (sizeof(T) == 2) {
            asc_copy_gm2ub_align((__ubuf__ uint16_t*)dst, (__gm__ uint16_t*)src, block_count, block_len,
                                 left_padding_count, right_padding_count, enable_constant_pad, cache_mode, src_stride,
                                 dst_stride);
        } else if constexpr (sizeof(T) == 4) {
            asc_copy_gm2ub_align((__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, block_count, block_len,
                                 left_padding_count, right_padding_count, enable_constant_pad, cache_mode, src_stride,
                                 dst_stride);
        } else if constexpr (sizeof(T) == 8) {
            asc_copy_gm2ub_align((__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, block_count, block_len,
                                 left_padding_count, right_padding_count, enable_constant_pad, cache_mode, src_stride,
                                 dst_stride);
        } else {
            static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
                          "Unsupported data type size for CopyGmToUbufAlignV2");
        }
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
