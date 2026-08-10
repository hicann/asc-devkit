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
    "impl/tensor_api/arch/vector/ub_to_ub/copy_impl/data_copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file data_copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_DATA_COPY_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_DATA_COPY_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/arch/vector/utils/copy_utils.h"
#include "impl/tensor_api/arch/vector/ub_to_ub/copy_impl/instruction.h"

namespace asc {
namespace te {

struct copy_ub_to_ub_trait {};

class copy_ub_to_ub_common {
protected:
    template <typename T, typename U>
    __aicore__ inline static void emit_copy(const T& dst, const U& src, uint16_t block_count, uint32_t block_len,
                                            int64_t src_stride, int64_t dst_stride)
    {
        using src_type = typename U::element_type;
        using dst_type = typename T::element_type;

        adjust_b4_copy_params<src_type, dst_type>(block_len, src_stride, dst_stride);

        // Convert from bytes to 32B units for asc_copy_ub2ub
        uint16_t block_len_in32_b = static_cast<uint16_t>(block_len >> 5);
        uint16_t src_stride_in32_b = static_cast<uint16_t>(src_stride >> 5);
        uint16_t dst_stride_in32_b = static_cast<uint16_t>(dst_stride >> 5);

        copy_ub_to_ub_instr::data_copy(dst.data().get(), src.data().get(), block_count, block_len_in32_b,
                                          src_stride_in32_b, dst_stride_in32_b);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_DATA_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
