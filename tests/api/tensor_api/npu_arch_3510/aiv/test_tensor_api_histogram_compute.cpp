/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the license.
 */

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/experimental/vector_compute.h"
#include "tensor_api/tensor.h"

namespace {

constexpr asc::te::experimental::histogram_options frequency_high_options {
    asc::te::experimental::histogram_half::high, asc::te::experimental::histogram_mode::frequency};
constexpr asc::te::experimental::histogram_options cumulative_low_options {
    asc::te::experimental::histogram_half::low, asc::te::experimental::histogram_mode::cumulative};
constexpr asc::te::experimental::histogram_options cumulative_high_options {
    asc::te::experimental::histogram_half::high, asc::te::experimental::histogram_mode::cumulative};

TEST(test_tensor_api_histogram_compute, compile_public_api_and_four_modes)
{
    using dst_type = asc::te::experimental::reg_tensor<uint16_t>;
    using src_type = asc::te::experimental::reg_tensor<uint8_t>;
    static_assert(AscendC::Std::is_same_v<decltype(asc::te::experimental::histograms(
        AscendC::Std::declval<const dst_type&>(), AscendC::Std::declval<const src_type&>())), dst_type>);
    static_assert(AscendC::Std::is_same_v<decltype(asc::te::experimental::histograms<frequency_high_options>(
        AscendC::Std::declval<const dst_type&>(), AscendC::Std::declval<const src_type&>())), dst_type>);
    static_assert(AscendC::Std::is_same_v<decltype(asc::te::experimental::histograms<cumulative_low_options>(
        AscendC::Std::declval<const dst_type&>(), AscendC::Std::declval<const src_type&>())), dst_type>);
    static_assert(AscendC::Std::is_same_v<decltype(asc::te::experimental::histograms<cumulative_high_options>(
        AscendC::Std::declval<const dst_type&>(), AscendC::Std::declval<const src_type&>())), dst_type>);
}

} // namespace
