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

namespace AscendC {
namespace Te {

template <typename T, typename = void>
struct has_mask_member : std::false_type {};

template <typename T>
struct has_mask_member<T, std::void_t<decltype(&T::mask)>> : std::true_type {};

static_assert(std::is_same_v<typename asc::te::type_get<uint8_t>::T, vector_uint8_t>);
static_assert(std::is_same_v<typename asc::te::type_get<uint16_t>::T, vector_uint16_t>);
static_assert(std::is_same_v<typename asc::te::type_get<uint32_t>::T, vector_uint32_t>);
static_assert(std::is_same_v<typename asc::te::type_get<uint64_t>::T, vector_uint64_t>);
static_assert(std::is_same_v<typename asc::te::type_get<int8_t>::T, vector_int8_t>);
static_assert(std::is_same_v<typename asc::te::type_get<int16_t>::T, vector_int16_t>);
static_assert(std::is_same_v<typename asc::te::type_get<int32_t>::T, vector_int32_t>);
static_assert(std::is_same_v<typename asc::te::type_get<int64_t>::T, vector_int64_t>);
static_assert(std::is_same_v<typename asc::te::type_get<half>::T, vector_half>);
static_assert(std::is_same_v<typename asc::te::type_get<bfloat16_t>::T, vector_bfloat16_t>);
static_assert(std::is_same_v<typename asc::te::type_get<float>::T, vector_float>);
static_assert(std::is_same_v<typename asc::te::type_get<hifloat8_t>::T, vector_hifloat8_t>);
static_assert(std::is_same_v<typename asc::te::type_get<fp8_e4m3fn_t>::T, vector_fp8_e4m3fn_t>);
static_assert(std::is_same_v<typename asc::te::type_get<fp8_e5m2_t>::T, vector_fp8_e5m2_t>);
static_assert(std::is_same_v<typename asc::te::type_get<fp8_e8m0_t>::T, vector_fp8_e8m0_t>);
static_assert(std::is_same_v<typename asc::te::type_get<int4x2_t>::T, vector_int4x2_t>);
static_assert(std::is_same_v<typename asc::te::type_get<fp4x2_e2m1_t>::T, vector_fp4x2_e2m1_t>);
static_assert(std::is_same_v<typename asc::te::type_get<fp4x2_e1m2_t>::T, vector_fp4x2_e1m2_t>);
static_assert(std::is_same_v<typename asc::te::type_get<bool>::T, vector_bool>);
static_assert(std::is_same_v<typename asc::te::reg_tensor<float>::type, float>);
static_assert(std::is_same_v<typename asc::te::reg_tensor<float>::reg_type, vector_float>);
static_assert(std::is_same_v<typename asc::te::reg_tensor<bool>::reg_type, vector_bool>);
static_assert(has_mask_member<asc::te::reg_tensor<float>>::value);
static_assert(!has_mask_member<asc::te::reg_tensor<bool>>::value);
static_assert(std::is_same_v<decltype(asc::te::ones<float>()), asc::te::reg_tensor<bool>>);

template <typename T, asc::te::load_sideband_mode sidebandMode = asc::te::load_sideband_mode::direct>
__aicore__ inline void CompileTensorRegLoadStore(
    __ubuf__ T* dst, __ubuf__ T* src, vector_bool mask)
{
    auto layout = MakeLayout(MakeShape(64));
    auto dstTensor = MakeTensor(MakeMemPtr<Location::UB>(dst), layout);
    auto srcTensor = MakeTensor(MakeMemPtr<Location::UB>(src), layout);
    asc::te::reg_tensor<bool> maskReg {mask};
    auto value = srcTensor.template load<sidebandMode>(MakeCoord(0)).with_mask(maskReg);
    static_assert(std::is_same_v<typename decltype(value)::type, T>);
    dstTensor.store(MakeCoord(0), value);

    auto fullValue = srcTensor.load(MakeCoord(0)).with_mask(asc::te::ones<T>());
    static_assert(std::is_same_v<typename decltype(fullValue)::type, T>);
}

TEST(TestTensorApiRegTensor, HoldsRegisterAndMask)
{
    asc::te::reg_tensor<float> value {};
    (void)value.reg;
    (void)value.mask;

    auto compileLoadStore = &CompileTensorRegLoadStore<float>;
    (void)compileLoadStore;
}

} // namespace Te
} // namespace AscendC
