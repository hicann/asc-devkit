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
    "impl/tensor_api/experimental/arch/vector/histogram_compute_impl.h is internal and must not be used directly."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_HISTOGRAM_COMPUTE_IMPL
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_HISTOGRAM_COMPUTE_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_HISTOGRAM_COMPUTE_IMPL_H

#include "impl/tensor_api/experimental/arch/utils/reg_utils.h"

namespace asc {
namespace te {
namespace experimental {

template <const histogram_options& options>
__simd_callee__ inline reg_tensor<uint16_t> histograms(
    const reg_tensor<uint16_t>& dst, const reg_tensor<uint8_t>& src)
{
    static_assert(options.half == histogram_half::low || options.half == histogram_half::high,
        "unsupported histogram half");
    static_assert(options.mode == histogram_mode::frequency || options.mode == histogram_mode::cumulative,
        "unsupported histogram mode");

    reg_tensor<uint16_t> result = dst;
    if constexpr (options.mode == histogram_mode::frequency) {
        if constexpr (options.half == histogram_half::low) {
            asc_frequency_histogram_bin0(result.reg, src.reg, src.mask);
        } else {
            asc_frequency_histogram_bin1(result.reg, src.reg, src.mask);
        }
    } else {
        if constexpr (options.half == histogram_half::low) {
            asc_cumulative_histogram_bin0(result.reg, src.reg, src.mask);
        } else {
            asc_cumulative_histogram_bin1(result.reg, src.reg, src.mask);
        }
    }
    return result;
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_HISTOGRAM_COMPUTE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_HISTOGRAM_COMPUTE_IMPL)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_HISTOGRAM_COMPUTE_IMPL
#endif
