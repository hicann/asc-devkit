/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This file is a part of the CANN Open Software.
 * Licensed under CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <type_traits>

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/tensor.h"

namespace asc {
namespace te {

static_assert(std::is_same_v<decltype(reduce_sum<float>(reg_tensor<float>{})), reg_tensor<float>>);
static_assert(std::is_same_v<decltype(reduce_max<int16_t>(reg_tensor<int16_t>{})), reg_tensor<int16_t>>);
static_assert(std::is_same_v<decltype(reduce_min<uint32_t>(reg_tensor<uint32_t>{})), reg_tensor<uint32_t>>);

__aicore__ inline void compile_reg_reduction(reg_tensor<float> float_src, reg_tensor<int16_t> int16_src)
{
    auto sum = reduce_sum<float>(float_src);
    auto block_sum = reduce_sum<float, reduce_scope::datablock>(float_src);
    auto pair_sum = reduce_sum<float, reduce_scope::pair>(float_src);
    auto wide_sum = reduce_sum<int32_t>(int16_src);
    auto wide_block_sum = reduce_sum<int32_t, reduce_scope::datablock>(int16_src);
    auto max = reduce_max<float>(float_src);
    auto block_max = reduce_max<float, reduce_scope::datablock>(float_src);
    auto min = reduce_min<float>(float_src);
    auto block_min = reduce_min<float, reduce_scope::datablock>(float_src);

    (void)sum;
    (void)block_sum;
    (void)pair_sum;
    (void)wide_sum;
    (void)wide_block_sum;
    (void)max;
    (void)block_max;
    (void)min;
    (void)block_min;
}

TEST(test_tensor_api_reduction_compute, compiles_supported_scopes)
{
    auto compile_reduction = &compile_reg_reduction;
    (void)compile_reduction;
}

} // namespace te
} // namespace asc
