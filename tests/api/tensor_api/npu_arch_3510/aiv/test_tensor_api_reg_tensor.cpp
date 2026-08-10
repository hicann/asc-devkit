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

#include "c_api/stub/cce_stub.h"
#include "tensor_api/tensor.h"

namespace asc {
namespace te {

template <typename T, typename = void>
struct has_mask_member : std::false_type {};

template <typename T>
struct has_mask_member<T, std::void_t<decltype(&T::mask)>> : std::true_type {};

static_assert(std::is_same_v<typename type_get<uint8_t>::type, vector_uint8_t>);
static_assert(std::is_same_v<typename type_get<uint16_t>::type, vector_uint16_t>);
static_assert(std::is_same_v<typename type_get<uint32_t>::type, vector_uint32_t>);
static_assert(std::is_same_v<typename type_get<uint64_t>::type, vector_uint64_t>);
static_assert(std::is_same_v<typename type_get<int8_t>::type, vector_int8_t>);
static_assert(std::is_same_v<typename type_get<int16_t>::type, vector_int16_t>);
static_assert(std::is_same_v<typename type_get<int32_t>::type, vector_int32_t>);
static_assert(std::is_same_v<typename type_get<int64_t>::type, vector_int64_t>);
static_assert(std::is_same_v<typename type_get<half>::type, vector_half>);
static_assert(std::is_same_v<typename type_get<bfloat16_t>::type, vector_bfloat16_t>);
static_assert(std::is_same_v<typename type_get<float>::type, vector_float>);
static_assert(std::is_same_v<typename type_get<hifloat8_t>::type, vector_hifloat8_t>);
static_assert(std::is_same_v<typename type_get<fp8_e4m3fn_t>::type, vector_fp8_e4m3fn_t>);
static_assert(std::is_same_v<typename type_get<fp8_e5m2_t>::type, vector_fp8_e5m2_t>);
static_assert(std::is_same_v<typename type_get<fp8_e8m0_t>::type, vector_fp8_e8m0_t>);
static_assert(std::is_same_v<typename type_get<int4x2_t>::type, vector_int4x2_t>);
static_assert(std::is_same_v<typename type_get<fp4x2_e2m1_t>::type, vector_fp4x2_e2m1_t>);
static_assert(std::is_same_v<typename type_get<fp4x2_e1m2_t>::type, vector_fp4x2_e1m2_t>);
static_assert(std::is_same_v<typename type_get<bool>::type, vector_bool>);
static_assert(std::is_same_v<typename reg_tensor<float>::type, float>);
static_assert(std::is_same_v<typename reg_tensor<float>::reg_type, vector_float>);
static_assert(std::is_same_v<typename reg_tensor<bool>::reg_type, vector_bool>);
static_assert(has_mask_member<reg_tensor<float>>::value);
static_assert(!has_mask_member<reg_tensor<bool>>::value);
static_assert(std::is_same_v<decltype(ones<float>()), reg_tensor<bool>>);

template <typename T, load_sideband_mode sideband_mode = load_sideband_mode::direct>
__aicore__ inline void compile_tensor_reg_load_store(__ubuf__ T* dst, __ubuf__ T* src, vector_bool mask)
{
    auto layout = make_layout(make_shape(64));
    auto dst_tensor = make_tensor(make_mem_ptr<location::ub>(dst), layout);
    auto src_tensor = make_tensor(make_mem_ptr<location::ub>(src), layout);
    reg_tensor<bool> mask_reg{mask};
    auto value = src_tensor.template load<sideband_mode>(make_coord(0)).with_mask(mask_reg);
    static_assert(std::is_same_v<typename decltype(value)::type, T>);
    dst_tensor.store(make_coord(0), value);

    auto full_value = src_tensor.load(make_coord(0)).with_mask(ones<T>());
    static_assert(std::is_same_v<typename decltype(full_value)::type, T>);
}

TEST(test_tensor_api_reg_tensor, holds_register_and_mask)
{
    reg_tensor<float> value{};
    (void)value.reg;
    (void)value.mask;

    auto compile_load_store = &compile_tensor_reg_load_store<float>;
    (void)compile_load_store;
}

} // namespace te
} // namespace asc
