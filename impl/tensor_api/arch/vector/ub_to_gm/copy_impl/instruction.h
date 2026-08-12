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
    "impl/tensor_api/arch/vector/ub_to_gm/copy_impl/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_IMPL_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_IMPL_INSTRUCTION_H

#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace asc {
namespace te {

class copy_ub_to_gm_instr {
public:
    template <typename T>
    __aicore__ inline static void
    data_copy(__gm__ T* dst, __ubuf__ T* src, const uint16_t block_count, const uint32_t block_len,
                 const int64_t src_stride, const int64_t dst_stride,
                 const asc_store_l2_cache_mode cache_mode = asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM)
    {
        if ASCEND_IS_AIC {
            return;
        }
        TENSOR_API_DEBUG_CHECK(debug_check_block_limit, block_count, DEBUG_BLOCK_COUNT_MAX, "block_count",
                               "copy_ub_to_gm instruction");
        TENSOR_API_DEBUG_CHECK(debug_check_block_limit, block_len, DEBUG_GM_UB_BLOCK_LEN_MAX, "block_len",
                               "copy_ub_to_gm instruction");
        TENSOR_API_DEBUG_CHECK(debug_check_ub2gm_stride, src_stride, block_len, block_count,
                               "copy_ub_to_gm instruction");
        if constexpr (sizeof(T) == 1) {
            asc_copy_ub2gm_align((__gm__ uint8_t*)dst, (__ubuf__ uint8_t*)src, block_count, block_len, cache_mode,
                                 dst_stride, static_cast<uint32_t>(src_stride));
        } else if constexpr (sizeof(T) == 2) {
            asc_copy_ub2gm_align((__gm__ uint16_t*)dst, (__ubuf__ uint16_t*)src, block_count, block_len, cache_mode,
                                 dst_stride, static_cast<uint32_t>(src_stride));
        } else if constexpr (sizeof(T) == 4 || sizeof(T) == 8) {
            asc_copy_ub2gm_align((__gm__ uint32_t*)dst, (__ubuf__ uint32_t*)src, block_count, block_len, cache_mode,
                                 dst_stride, static_cast<uint32_t>(src_stride));
        } else {
            static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
                          "Unsupported data type size for CopyUbufToGmAlignV2");
        }
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_IMPL_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
