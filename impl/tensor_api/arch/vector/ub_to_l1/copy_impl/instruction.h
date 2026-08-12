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
    "impl/tensor_api/arch/vector/ub_to_l1/copy_impl/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_L1_COPY_IMPL_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_L1_COPY_IMPL_INSTRUCTION_H

#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace asc {
namespace te {

class copy_ub_to_l1_instr {
public:
    template <typename T>
    __aicore__ inline static void data_copy(__cbuf__ T* dst, __ubuf__ T* src, const uint16_t block_count,
                                                const uint16_t block_len, const uint16_t src_stride,
                                                const uint16_t dst_stride)
    {
        if ASCEND_IS_AIC {
            return;
        }
        TENSOR_API_DEBUG_CHECK(debug_check_block_count, block_count, "block_count", "copy_ub_to_l1 instruction");
        TENSOR_API_DEBUG_CHECK(debug_check_block_len, block_len, DEBUG_BLOCK_LEN_MAX, "copy_ub_to_l1 instruction");

        asc_copy_ub2l1((__cbuf__ void*)dst, (__ubuf__ void*)src, block_count, block_len, src_stride, dst_stride);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_L1_COPY_IMPL_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
