/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cstring>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/experimental/vector_ops.h"
#include "tensor_api/tensor.h"

namespace {

template <typename SrcType, typename PackedType, typename UnpackedType = SrcType>
__simd_vf__ inline void compile_reg_data_pack_unpack(vector_bool mask)
{
    asc::te::experimental::reg_tensor<SrcType> src{};
    src.mask = mask;
    auto packed_default = asc::te::experimental::pack(src);
    auto packed_low = asc::te::experimental::pack<asc::te::experimental::high_low_part::low>(src);
    auto packed_high = asc::te::experimental::pack<asc::te::experimental::high_low_part::high>(src);
    static_assert(AscendC::Std::is_same_v<decltype(packed_default), asc::te::experimental::reg_tensor<PackedType>>);
    static_assert(AscendC::Std::is_same_v<decltype(packed_low), asc::te::experimental::reg_tensor<PackedType>>);
    static_assert(AscendC::Std::is_same_v<decltype(packed_high), asc::te::experimental::reg_tensor<PackedType>>);
    auto packed_mask = asc::te::experimental::all_mask<PackedType>();
    EXPECT_EQ(packed_default.mask, packed_mask.reg);
    EXPECT_EQ(packed_low.mask, packed_mask.reg);
    EXPECT_EQ(packed_high.mask, packed_mask.reg);

    packed_default.mask = mask;
    packed_low.mask = mask;
    packed_high.mask = mask;
    auto unpacked_default = asc::te::experimental::unpack(packed_default);
    auto unpacked_low = asc::te::experimental::unpack<asc::te::experimental::high_low_part::low>(packed_low);
    auto unpacked_high = asc::te::experimental::unpack<asc::te::experimental::high_low_part::high>(packed_high);
    static_assert(AscendC::Std::is_same_v<decltype(unpacked_default), asc::te::experimental::reg_tensor<UnpackedType>>);
    static_assert(AscendC::Std::is_same_v<decltype(unpacked_low), asc::te::experimental::reg_tensor<UnpackedType>>);
    static_assert(AscendC::Std::is_same_v<decltype(unpacked_high), asc::te::experimental::reg_tensor<UnpackedType>>);
    auto unpacked_mask = asc::te::experimental::all_mask<UnpackedType>();
    EXPECT_EQ(unpacked_default.mask, unpacked_mask.reg);
    EXPECT_EQ(unpacked_low.mask, unpacked_mask.reg);
    EXPECT_EQ(unpacked_high.mask, unpacked_mask.reg);
}

template <typename SrcType, typename UnpackedType>
__simd_vf__ inline void compile_reg_data_unpack(vector_bool mask)
{
    asc::te::experimental::reg_tensor<SrcType> src{};
    src.mask = mask;
    auto unpacked_low = asc::te::experimental::unpack<asc::te::experimental::high_low_part::low>(src);
    auto unpacked_high = asc::te::experimental::unpack<asc::te::experimental::high_low_part::high>(src);
    static_assert(AscendC::Std::is_same_v<decltype(unpacked_low), asc::te::experimental::reg_tensor<UnpackedType>>);
    static_assert(AscendC::Std::is_same_v<decltype(unpacked_high), asc::te::experimental::reg_tensor<UnpackedType>>);
    auto full_mask = asc::te::experimental::all_mask<UnpackedType>();
    EXPECT_EQ(unpacked_low.mask, full_mask.reg);
    EXPECT_EQ(unpacked_high.mask, full_mask.reg);
}

__simd_vf__ inline void compile_reg_mask_pack_unpack(vector_bool mask)
{
    asc::te::experimental::reg_tensor<bool> src{};
    src.mask = mask;
    src.reg = mask;
    auto packed_default = asc::te::experimental::pack(src);
    auto packed_low = asc::te::experimental::pack<asc::te::experimental::high_low_part::low>(src);
    auto packed_high = asc::te::experimental::pack<asc::te::experimental::high_low_part::high>(src);
    auto unpacked_default = asc::te::experimental::unpack(packed_default);
    auto unpacked_low = asc::te::experimental::unpack<asc::te::experimental::high_low_part::low>(packed_low);
    auto unpacked_high = asc::te::experimental::unpack<asc::te::experimental::high_low_part::high>(packed_high);
    static_assert(AscendC::Std::is_same_v<decltype(packed_default), asc::te::experimental::reg_tensor<bool>>);
    static_assert(AscendC::Std::is_same_v<decltype(packed_low), asc::te::experimental::reg_tensor<bool>>);
    static_assert(AscendC::Std::is_same_v<decltype(packed_high), asc::te::experimental::reg_tensor<bool>>);
    static_assert(AscendC::Std::is_same_v<decltype(unpacked_default), asc::te::experimental::reg_tensor<bool>>);
    static_assert(AscendC::Std::is_same_v<decltype(unpacked_low), asc::te::experimental::reg_tensor<bool>>);
    static_assert(AscendC::Std::is_same_v<decltype(unpacked_high), asc::te::experimental::reg_tensor<bool>>);
    auto full_mask = asc::te::experimental::all_mask<uint8_t>();
    EXPECT_EQ(packed_default.mask, full_mask.reg);
    EXPECT_EQ(packed_low.mask, full_mask.reg);
    EXPECT_EQ(packed_high.mask, full_mask.reg);
    EXPECT_EQ(unpacked_default.mask, full_mask.reg);
    EXPECT_EQ(unpacked_low.mask, full_mask.reg);
    EXPECT_EQ(unpacked_high.mask, full_mask.reg);
}

template <typename T>
vector_bool expected_compress_mask{};

template <typename T>
void compress_stub(
    typename asc::te::experimental::reg_tensor<T>::reg_type& dst,
    typename asc::te::experimental::reg_tensor<T>::reg_type src, vector_bool mask, Literal mode)
{
    EXPECT_EQ(mask, expected_compress_mask<T>);
    EXPECT_EQ(mode, MODE_NO_STORED);
    dst = src;
}

class AivCoreTypeGuard {
public:
    AivCoreTypeGuard() : previous_core_type(g_coreType) { g_coreType = AscendC::AIV_TYPE; }

    ~AivCoreTypeGuard() { g_coreType = previous_core_type; }

private:
    int32_t previous_core_type;
};

template <typename T>
__simd_vf__ inline void compile_reg_compress(vector_bool mask)
{
    AivCoreTypeGuard core_type_guard;
    using reg_type = typename asc::te::experimental::reg_tensor<T>::reg_type;
    asc::te::experimental::reg_tensor<T> src{};
    std::memset(&src.reg, 0x12, sizeof(src.reg));
    src.mask = mask;
    expected_compress_mask<T> = mask;
    MOCKER(static_cast<void (*)(reg_type&, reg_type, vector_bool, Literal)>(vsqz))
        .expects(once())
        .will(invoke(compress_stub<T>));
    auto compressed = asc::te::experimental::compress(src);
    static_assert(AscendC::Std::is_same_v<decltype(compressed), asc::te::experimental::reg_tensor<T>>);
    EXPECT_EQ(std::memcmp(&compressed.reg, &src.reg, sizeof(src.reg)), 0);
    EXPECT_EQ(compressed.mask, src.mask);
    GlobalMockObject::verify();
}

template <typename T>
__simd_vf__ inline void compile_reg_expand(vector_bool mask)
{
    asc::te::experimental::reg_tensor<bool> select_mask{};
    select_mask.reg = mask;
    auto expanded = asc::te::experimental::expand<T>(select_mask);
    static_assert(AscendC::Std::is_same_v<decltype(expanded), asc::te::experimental::reg_tensor<T>>);
    auto full_mask = asc::te::experimental::all_mask<T>();
    EXPECT_EQ(expanded.mask, full_mask.reg);
}

TEST(tensor_api_reg_compute_sel_3510, pack_unpack_data)
{
    auto mask = asc::te::experimental::all_mask<uint16_t>();
    compile_reg_data_pack_unpack<uint16_t, uint8_t>(mask.reg);
    compile_reg_data_pack_unpack<uint32_t, uint16_t>(mask.reg);
    compile_reg_data_pack_unpack<int16_t, uint8_t, uint16_t>(mask.reg);
    compile_reg_data_pack_unpack<int32_t, uint16_t, uint32_t>(mask.reg);
    compile_reg_data_unpack<int8_t, int16_t>(mask.reg);
    compile_reg_data_unpack<int16_t, int32_t>(mask.reg);
    compile_reg_data_pack_unpack<uint16_t, uint8_t>(static_cast<vector_bool>(0x5A5A));
    compile_reg_data_pack_unpack<uint32_t, uint16_t>(static_cast<vector_bool>(0x5A5A));
    compile_reg_data_pack_unpack<int16_t, uint8_t, uint16_t>(static_cast<vector_bool>(0x5A5A));
    compile_reg_data_pack_unpack<int32_t, uint16_t, uint32_t>(static_cast<vector_bool>(0x5A5A));
    compile_reg_data_unpack<int8_t, int16_t>(static_cast<vector_bool>(0x5A5A));
    compile_reg_data_unpack<int16_t, int32_t>(static_cast<vector_bool>(0x5A5A));
}

TEST(tensor_api_reg_compute_sel_3510, pack_unpack_mask)
{
    auto mask = asc::te::experimental::all_mask<uint8_t>();
    compile_reg_mask_pack_unpack(mask.reg);
    compile_reg_mask_pack_unpack(static_cast<vector_bool>(0x5A5A));
}

#define TENSOR_API_COMPRESS_TEST(DataType)                                \
    TEST(tensor_api_reg_compute_sel_3510, compress_##DataType)            \
    {                                                                     \
        auto mask = asc::te::experimental::all_mask<DataType>();          \
        compile_reg_compress<DataType>(mask.reg);                         \
        compile_reg_compress<DataType>(static_cast<vector_bool>(0x5A5A)); \
    }

TENSOR_API_COMPRESS_TEST(int8_t)
TENSOR_API_COMPRESS_TEST(uint8_t)
TENSOR_API_COMPRESS_TEST(fp8_e4m3fn_t)
TENSOR_API_COMPRESS_TEST(fp8_e5m2_t)
TENSOR_API_COMPRESS_TEST(fp8_e8m0_t)
TENSOR_API_COMPRESS_TEST(hifloat8_t)
TENSOR_API_COMPRESS_TEST(int16_t)
TENSOR_API_COMPRESS_TEST(uint16_t)
TENSOR_API_COMPRESS_TEST(half)
TENSOR_API_COMPRESS_TEST(bfloat16_t)
TENSOR_API_COMPRESS_TEST(int32_t)
TENSOR_API_COMPRESS_TEST(uint32_t)
TENSOR_API_COMPRESS_TEST(float)

#undef TENSOR_API_COMPRESS_TEST

#define TENSOR_API_EXPAND_TEST(DataType)                                \
    TEST(tensor_api_reg_compute_sel_3510, expand_##DataType)            \
    {                                                                   \
        auto mask = asc::te::experimental::all_mask<DataType>();        \
        compile_reg_expand<DataType>(mask.reg);                         \
        compile_reg_expand<DataType>(static_cast<vector_bool>(0x5A5A)); \
    }

TENSOR_API_EXPAND_TEST(int8_t)
TENSOR_API_EXPAND_TEST(uint8_t)
TENSOR_API_EXPAND_TEST(int16_t)
TENSOR_API_EXPAND_TEST(uint16_t)
TENSOR_API_EXPAND_TEST(int32_t)
TENSOR_API_EXPAND_TEST(uint32_t)

#undef TENSOR_API_EXPAND_TEST

} // namespace
