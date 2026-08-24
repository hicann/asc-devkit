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
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_HISTOGRAM_COMPUTE
#endif

#ifndef INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_HISTOGRAM_COMPUTE_H
#define INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_HISTOGRAM_COMPUTE_H

#include "tensor_api/arch/vector/experimental/reg_tensor.h"

namespace asc {
namespace te {
namespace experimental {

enum class histogram_half {
    low,
    high,
};

enum class histogram_mode {
    frequency,
    cumulative,
};

struct histogram_options {
    histogram_half half = histogram_half::low;
    histogram_mode mode = histogram_mode::frequency;
};

inline constexpr histogram_options default_histogram_options {};

template <const histogram_options& options = default_histogram_options>
__simd_callee__ inline decltype(auto) histograms(
    const reg_tensor<uint16_t>& dst, const reg_tensor<uint8_t>& src);

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/arch/vector/experimental/histogram_compute_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_HISTOGRAM_COMPUTE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_HISTOGRAM_COMPUTE)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_HISTOGRAM_COMPUTE
#endif
