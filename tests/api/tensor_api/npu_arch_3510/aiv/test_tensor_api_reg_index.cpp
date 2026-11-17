/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/experimental/vector_ops.h"
#include "tensor_api/tensor.h"

namespace {

template <typename T>
__aicore__ inline void compile_iota_from_scalar()
{
    auto inc = asc::te::experimental::iota(T{});
    auto dec = asc::te::experimental::iota<asc::te::experimental::iota_order::decrease>(T{});
    static_assert(AscendC::Std::is_same_v<decltype(inc), asc::te::experimental::reg_tensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(dec), asc::te::experimental::reg_tensor<T>>);
    auto full_mask = asc::te::experimental::all_mask<T>();
    EXPECT_EQ(inc.mask, full_mask.reg);
    EXPECT_EQ(dec.mask, full_mask.reg);
}

#define TENSOR_API_IOTA_TEST(DataType) \
    TEST(tensor_api_reg_index_3510, iota_##DataType) { compile_iota_from_scalar<DataType>(); }

TENSOR_API_IOTA_TEST(int8_t)
TENSOR_API_IOTA_TEST(int16_t)
TENSOR_API_IOTA_TEST(int32_t)
TENSOR_API_IOTA_TEST(half)
TENSOR_API_IOTA_TEST(float)

} // namespace
