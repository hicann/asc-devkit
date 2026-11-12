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
#include <mockcpp/mockcpp.hpp>

#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/experimental/vector_ops.h"
#include "tensor_api/tensor.h"

namespace {

template <typename... Types>
using void_t = void;

template <typename T, typename = void>
struct has_mask_member : AscendC::Std::false_type {};

template <typename T>
struct has_mask_member<T, void_t<decltype(&T::mask)>> : AscendC::Std::true_type {};

template <typename tensor_type, typename coord_type, typename = void>
struct has_member_reg_load : AscendC::Std::false_type {};

template <typename tensor_type, typename coord_type>
struct has_member_reg_load<
    tensor_type, coord_type,
    void_t<decltype(AscendC::Std::declval<const tensor_type&>()
                        .template load<asc::te::experimental::load_sideband_mode::direct>(
                            AscendC::Std::declval<const coord_type&>()))>> : AscendC::Std::true_type {};

template <
    typename T,
    asc::te::experimental::load_sideband_mode sideband_mode = asc::te::experimental::load_sideband_mode::direct>
__simd_vf__ inline void compile_reg_local_tensor_load_store(__ubuf__ T* dst, __ubuf__ T* src, vector_bool mask)
{
    auto layout = asc::te::make_layout(asc::te::make_shape(64));
    auto dst_mem = asc::te::make_mem_ptr<asc::te::location::ub>(dst);
    auto src_mem = asc::te::make_mem_ptr<asc::te::location::ub>(src);
    using engine_type = asc::te::view_engine<decltype(src_mem)>;
    using layout_type = decltype(layout);
    asc::te::local_tensor<engine_type, layout_type> dst_tensor(dst_mem, layout);
    asc::te::local_tensor<engine_type, layout_type> src_tensor(src_mem, layout);
    using coord_type = decltype(asc::te::make_coord(0));
    using tensor_api_base = typename decltype(src_tensor)::tensor_api_base;
    static_assert(!has_member_reg_load<tensor_api_base, coord_type>::value);
    static_assert(!has_member_reg_load<decltype(src_tensor), coord_type>::value);
    static_assert(AscendC::Std::is_base_of_v<tensor_api_base, decltype(src_tensor)>);

    asc::te::experimental::reg_tensor<bool> mask_reg{mask};
    auto value = asc::te::experimental::load<sideband_mode>(src_tensor, asc::te::make_coord(0)).with_mask(mask_reg);
    static_assert(AscendC::Std::is_same_v<typename decltype(value)::elem_type, T>);
    asc::te::experimental::store(dst_tensor, asc::te::make_coord(0), value);

    auto full_value =
        asc::te::experimental::load(src_tensor, asc::te::make_coord(0)).with_mask(asc::te::experimental::all_mask<T>());
    static_assert(AscendC::Std::is_same_v<typename decltype(full_value)::elem_type, T>);
}

template <typename T>
__simd_vf__ inline void compile_reg_local_tensor_load_store_without_coord(
    __ubuf__ T* dst, __ubuf__ T* src, vector_bool mask)
{
    auto layout = asc::te::make_layout(asc::te::make_shape(64));
    auto dst_tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(dst), layout);
    auto src_tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src), layout);

    auto value = asc::te::experimental::load(src_tensor).with_mask(asc::te::experimental::reg_tensor<bool>{mask});
    static_assert(AscendC::Std::is_same_v<decltype(value), asc::te::experimental::reg_tensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(asc::te::experimental::store(dst_tensor, value)), void>);
    asc::te::experimental::store(dst_tensor, value);

    auto deinterleaved = asc::te::experimental::load<asc::te::experimental::load_sideband_mode::deintlv>(src_tensor);
    static_assert(AscendC::Std::is_same_v<decltype(deinterleaved), asc::te::experimental::reg_pair<T>>);
    asc::te::experimental::reg_tensor<bool> mask_reg{mask};
    deinterleaved.first.with_mask(mask_reg);
    deinterleaved.second.with_mask(mask_reg);
    static_assert(AscendC::Std::is_same_v<
                  decltype(asc::te::experimental::store(dst_tensor, deinterleaved.first, deinterleaved.second)), void>);
    asc::te::experimental::store(dst_tensor, deinterleaved.first, deinterleaved.second);

    auto broadcast = asc::te::experimental::load_broadcast(src_tensor);
    static_assert(AscendC::Std::is_same_v<decltype(broadcast), asc::te::experimental::reg_tensor<T>>);
    (void)broadcast;
}

template <asc::te::experimental::load_sideband_mode sideband_mode>
__simd_vf__ inline void compile_reg_local_tensor_load_mode(__ubuf__ uint8_t* src)
{
    auto layout = asc::te::make_layout(asc::te::make_shape(256));
    auto src_tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src), layout);
    if constexpr (sideband_mode == asc::te::experimental::load_sideband_mode::deintlv) {
        auto value = asc::te::experimental::load<sideband_mode>(src_tensor, asc::te::make_coord(0));
        static_assert(AscendC::Std::is_same_v<decltype(value), asc::te::experimental::reg_pair<uint8_t>>);
        auto mask = asc::te::experimental::all_mask<uint8_t>();
        value.first.with_mask(mask);
        value.second.with_mask(mask);
    } else {
        auto value = asc::te::experimental::load<sideband_mode>(src_tensor, asc::te::make_coord(0));
        static_assert(AscendC::Std::is_same_v<decltype(value), asc::te::experimental::reg_tensor<uint8_t>>);
        value.with_mask(asc::te::experimental::all_mask<uint8_t>());
    }
}

template <asc::te::experimental::broadcast_mode broadcast_mode_value>
__simd_vf__ inline void compile_reg_local_tensor_broadcast(__ubuf__ float* src)
{
    auto layout = asc::te::make_layout(asc::te::make_shape(64));
    auto src_tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src), layout);
    auto value = asc::te::experimental::load_broadcast<broadcast_mode_value>(src_tensor, asc::te::make_coord(0));
    static_assert(AscendC::Std::is_same_v<decltype(value), asc::te::experimental::reg_tensor<float>>);
    value.with_mask(asc::te::experimental::all_mask<float>());
}

template <typename DataType, typename IndexDataType>
__simd_vf__ inline void compile_reg_register_gather(vector_bool mask)
{
    asc::te::experimental::reg_tensor<DataType> src{};
    asc::te::experimental::reg_tensor<IndexDataType> index{};
    src.mask = static_cast<vector_bool>(0x5A5A);
    index.mask = mask;
    auto value = asc::te::experimental::gather(src, index);
    static_assert(AscendC::Std::is_same_v<decltype(value), asc::te::experimental::reg_tensor<DataType>>);
    auto full_mask = asc::te::experimental::all_mask<DataType>();
    EXPECT_EQ(value.mask, full_mask.reg);
}

template <asc::te::experimental::store_sideband_mode sideband_mode>
__simd_vf__ inline void compile_reg_local_tensor_store_mode(__ubuf__ uint32_t* dst, vector_bool mask)
{
    auto layout = asc::te::make_layout(asc::te::make_shape(64));
    auto dst_tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(dst), layout);
    asc::te::experimental::reg_tensor<uint32_t> src0{mask};
    if constexpr (sideband_mode == asc::te::experimental::store_sideband_mode::intlv) {
        asc::te::experimental::reg_tensor<uint32_t> src1{mask};
        static_assert(AscendC::Std::is_same_v<
                      decltype(asc::te::experimental::store(dst_tensor, asc::te::make_coord(0), src0, src1)), void>);
        asc::te::experimental::store<sideband_mode>(dst_tensor, asc::te::make_coord(0), src0, src1);
    } else {
        static_assert(
            AscendC::Std::is_same_v<
                decltype(asc::te::experimental::store<sideband_mode>(dst_tensor, asc::te::make_coord(0), src0)), void>);
        asc::te::experimental::store<sideband_mode>(dst_tensor, asc::te::make_coord(0), src0);
    }
}

template <typename T, typename Index, asc::te::experimental::gather_mode Mode>
__simd_vf__ inline void CompileLoadGather(__ubuf__ T* src)
{
    using RegDataType = asc::te::experimental::detail::get_reg_data_type<T>;
    auto layout = asc::te::make_layout(asc::te::make_shape(64));
    auto src_tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src), layout);
    asc::te::experimental::reg_tensor<Index> index{};
    auto mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl8, uint32_t>();

    auto gathered = asc::te::experimental::load_gather<Mode>(src_tensor, index, mask);
    auto gathered_at = asc::te::experimental::load_gather<Mode>(src_tensor, asc::te::make_coord(0), index, mask);
    static_assert(AscendC::Std::is_same_v<decltype(gathered), asc::te::experimental::reg_tensor<RegDataType>>);
    static_assert(AscendC::Std::is_same_v<decltype(gathered_at), asc::te::experimental::reg_tensor<RegDataType>>);
    EXPECT_EQ(gathered.mask, mask.reg);
    EXPECT_EQ(gathered_at.mask, mask.reg);
}

template <typename T, typename Index>
__simd_vf__ inline void CompileStoreScatter(__ubuf__ T* dst)
{
    auto layout = asc::te::make_layout(asc::te::make_shape(64));
    auto dst_tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(dst), layout);
    asc::te::experimental::reg_tensor<T> src{};
    asc::te::experimental::reg_tensor<Index> index{};

    static_assert(
        AscendC::Std::is_same_v<decltype(asc::te::experimental::store_scatter(dst_tensor, src, index)), void>);
    static_assert(
        AscendC::Std::is_same_v<
            decltype(asc::te::experimental::store_scatter(dst_tensor, asc::te::make_coord(0), src, index)), void>);
    asc::te::experimental::store_scatter(dst_tensor, src, index);
    asc::te::experimental::store_scatter(dst_tensor, asc::te::make_coord(0), src, index);
}

template <typename T>
__simd_vf__ inline void compile_reg_mask(uint32_t remain)
{
    auto vl8_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl8, T>();
    auto every3_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every3, T>();
    auto empty_mask = asc::te::experimental::none_mask<T>();
    auto tail_mask = asc::te::experimental::update_mask<T>(remain);
    static_assert(AscendC::Std::is_same_v<decltype(vl8_mask), asc::te::experimental::reg_tensor<bool>>);
    static_assert(AscendC::Std::is_same_v<decltype(every3_mask), asc::te::experimental::reg_tensor<bool>>);
    (void)vl8_mask;
    (void)every3_mask;
    (void)empty_mask;
    (void)tail_mask;
}

template <typename T>
__simd_vf__ inline void compile_reg_mask_data_reorder(vector_bool src0, vector_bool src1)
{
    asc::te::experimental::reg_tensor<bool> mask0{src0};
    asc::te::experimental::reg_tensor<bool> mask1{src1};
    auto interleaved = asc::te::experimental::interleave<T>(mask0, mask1);
    static_assert(AscendC::Std::is_same_v<decltype(interleaved), asc::te::experimental::reg_pair<bool>>);
    static_assert(AscendC::Std::is_same_v<decltype(interleaved.first), asc::te::experimental::reg_tensor<bool>>);
    static_assert(AscendC::Std::is_same_v<decltype(interleaved.second), asc::te::experimental::reg_tensor<bool>>);

    auto restored = asc::te::experimental::deinterleave<T>(interleaved.first, interleaved.second);
    static_assert(AscendC::Std::is_same_v<decltype(restored), asc::te::experimental::reg_pair<bool>>);
    (void)restored;
}

class TensorApiGatherScatter3510 : public testing::Test {};

#define LOAD_GATHER_TEST(TestName, DataType, IndexType, Mode)                                             \
    TEST_F(TensorApiGatherScatter3510, LoadGather_##TestName##_##IndexType)                               \
    {                                                                                                     \
        auto compile = &CompileLoadGather<DataType, IndexType, asc::te::experimental::gather_mode::Mode>; \
        (void)compile;                                                                                    \
        SUCCEED();                                                                                        \
    }

#define LOAD_GATHER_ELEMENT_TEST(DataType, IndexType) LOAD_GATHER_TEST(Element_##DataType, DataType, IndexType, element)

#define LOAD_GATHER_DATABLOCK_TEST(DataType) LOAD_GATHER_TEST(DataBlock_##DataType, DataType, uint32_t, data_block)

#define STORE_SCATTER_TEST(DataType, IndexType)                               \
    TEST_F(TensorApiGatherScatter3510, StoreScatter_##DataType##_##IndexType) \
    {                                                                         \
        auto compile = &CompileStoreScatter<DataType, IndexType>;             \
        (void)compile;                                                        \
        SUCCEED();                                                            \
    }

LOAD_GATHER_ELEMENT_TEST(int8_t, uint16_t)
LOAD_GATHER_ELEMENT_TEST(uint8_t, uint16_t)
LOAD_GATHER_ELEMENT_TEST(int16_t, uint16_t)
LOAD_GATHER_ELEMENT_TEST(int16_t, uint32_t)
LOAD_GATHER_ELEMENT_TEST(uint16_t, uint16_t)
LOAD_GATHER_ELEMENT_TEST(uint16_t, uint32_t)
LOAD_GATHER_ELEMENT_TEST(half, uint16_t)
LOAD_GATHER_ELEMENT_TEST(half, uint32_t)
LOAD_GATHER_ELEMENT_TEST(bfloat16_t, uint16_t)
LOAD_GATHER_ELEMENT_TEST(bfloat16_t, uint32_t)
LOAD_GATHER_ELEMENT_TEST(int32_t, uint32_t)
LOAD_GATHER_ELEMENT_TEST(uint32_t, uint32_t)
LOAD_GATHER_ELEMENT_TEST(float, uint32_t)
LOAD_GATHER_ELEMENT_TEST(fp8_e4m3fn_t, uint16_t)
LOAD_GATHER_ELEMENT_TEST(fp8_e5m2_t, uint16_t)
LOAD_GATHER_ELEMENT_TEST(fp8_e8m0_t, uint16_t)
LOAD_GATHER_ELEMENT_TEST(hifloat8_t, uint16_t)

LOAD_GATHER_DATABLOCK_TEST(int8_t)
LOAD_GATHER_DATABLOCK_TEST(uint8_t)
LOAD_GATHER_DATABLOCK_TEST(int16_t)
LOAD_GATHER_DATABLOCK_TEST(uint16_t)
LOAD_GATHER_DATABLOCK_TEST(int32_t)
LOAD_GATHER_DATABLOCK_TEST(uint32_t)
LOAD_GATHER_DATABLOCK_TEST(int64_t)
LOAD_GATHER_DATABLOCK_TEST(uint64_t)
LOAD_GATHER_DATABLOCK_TEST(half)
LOAD_GATHER_DATABLOCK_TEST(float)
LOAD_GATHER_DATABLOCK_TEST(bfloat16_t)
LOAD_GATHER_DATABLOCK_TEST(fp8_e4m3fn_t)
LOAD_GATHER_DATABLOCK_TEST(fp8_e5m2_t)
LOAD_GATHER_DATABLOCK_TEST(fp8_e8m0_t)
LOAD_GATHER_DATABLOCK_TEST(hifloat8_t)
LOAD_GATHER_DATABLOCK_TEST(int4b_t)
LOAD_GATHER_DATABLOCK_TEST(fp4x2_e2m1_t)
LOAD_GATHER_DATABLOCK_TEST(fp4x2_e1m2_t)

STORE_SCATTER_TEST(int8_t, uint16_t)
STORE_SCATTER_TEST(uint8_t, uint16_t)
STORE_SCATTER_TEST(int16_t, uint16_t)
STORE_SCATTER_TEST(uint16_t, uint16_t)
STORE_SCATTER_TEST(half, uint16_t)
STORE_SCATTER_TEST(bfloat16_t, uint16_t)
STORE_SCATTER_TEST(int32_t, uint32_t)
STORE_SCATTER_TEST(uint32_t, uint32_t)
STORE_SCATTER_TEST(float, uint32_t)

TEST(tensor_api_reg_tensor_3510, holds_register_and_mask)
{
    asc::te::experimental::reg_tensor<float> value{};
    (void)value.reg;
    (void)value.mask;

    auto compile_load_store = &compile_reg_local_tensor_load_store<float>;
    auto compile_load_store_without_coord = &compile_reg_local_tensor_load_store_without_coord<uint32_t>;
    (void)compile_load_store;
    (void)compile_load_store_without_coord;

    auto compile_direct = &compile_reg_local_tensor_load_mode<asc::te::experimental::load_sideband_mode::direct>;
    auto compile_deintlv = &compile_reg_local_tensor_load_mode<asc::te::experimental::load_sideband_mode::deintlv>;
    auto compile_downsample =
        &compile_reg_local_tensor_load_mode<asc::te::experimental::load_sideband_mode::downsample>;
    auto compile_unpack = &compile_reg_local_tensor_load_mode<asc::te::experimental::load_sideband_mode::unpack>;
    auto compile_unpack4 = &compile_reg_local_tensor_load_mode<asc::te::experimental::load_sideband_mode::unpack4>;
    auto compile_upsample = &compile_reg_local_tensor_load_mode<asc::te::experimental::load_sideband_mode::upsample>;
    (void)compile_direct;
    (void)compile_deintlv;
    (void)compile_downsample;
    (void)compile_unpack;
    (void)compile_unpack4;
    (void)compile_upsample;

    auto compile_broadcast_elem = &compile_reg_local_tensor_broadcast<asc::te::experimental::broadcast_mode::elem>;
    auto compile_broadcast_datablock =
        &compile_reg_local_tensor_broadcast<asc::te::experimental::broadcast_mode::datablock>;
    auto compile_broadcast_elem2datablock =
        &compile_reg_local_tensor_broadcast<asc::te::experimental::broadcast_mode::elem2datablock>;
    (void)compile_broadcast_elem;
    (void)compile_broadcast_datablock;
    (void)compile_broadcast_elem2datablock;

    auto compile_store_direct =
        &compile_reg_local_tensor_store_mode<asc::te::experimental::store_sideband_mode::direct>;
    auto compile_store_1st =
        &compile_reg_local_tensor_store_mode<asc::te::experimental::store_sideband_mode::store_1st>;
    auto compile_store_intlv = &compile_reg_local_tensor_store_mode<asc::te::experimental::store_sideband_mode::intlv>;
    auto compile_store_pack = &compile_reg_local_tensor_store_mode<asc::te::experimental::store_sideband_mode::pack>;
    auto compile_store_pack_quarter =
        &compile_reg_local_tensor_store_mode<asc::te::experimental::store_sideband_mode::pack_quarter>;
    (void)compile_store_direct;
    (void)compile_store_1st;
    (void)compile_store_intlv;
    (void)compile_store_pack;
    (void)compile_store_pack_quarter;

    auto compile_reg_mask_b8 = &compile_reg_mask<uint8_t>;
    auto compile_reg_mask_b16 = &compile_reg_mask<half>;
    auto compile_reg_mask_b32 = &compile_reg_mask<float>;
    (void)compile_reg_mask_b8;
    (void)compile_reg_mask_b16;
    (void)compile_reg_mask_b32;

    auto compile_reg_mask_data_reorder_b8 = &compile_reg_mask_data_reorder<uint8_t>;
    auto compile_reg_mask_data_reorder_b16 = &compile_reg_mask_data_reorder<half>;
    auto compile_reg_mask_data_reorder_b32 = &compile_reg_mask_data_reorder<float>;
    (void)compile_reg_mask_data_reorder_b8;
    (void)compile_reg_mask_data_reorder_b16;
    (void)compile_reg_mask_data_reorder_b32;
}

class tensor_api_reg_data_move_3510 : public testing::Test {
protected:
    void SetUp() override { g_coreType = AscendC::AIV_TYPE; }

    void TearDown() override
    {
        g_coreType = previous_core_type;
        GlobalMockObject::reset();
    }

private:
    int32_t previous_core_type = g_coreType;
};

struct reg_move_expectation {
    const void* address;
    Literal mode;
    vector_bool predicate;
};

reg_move_expectation expected_reg_move{};

void check_reg_move_args(const void* address, int32_t offset, Literal mode)
{
    EXPECT_EQ(address, expected_reg_move.address);
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(mode, expected_reg_move.mode);
}

void load_mask_stub(vector_bool& dst, __ubuf__ uint32_t* src, int32_t offset, Literal mode)
{
    check_reg_move_args(src, offset, mode);
    dst = expected_reg_move.predicate;
}

void expect_mask_load(__ubuf__ uint32_t* src, Literal mode)
{
    expected_reg_move = {src, mode, static_cast<vector_bool>(*src)};
    MOCKER(static_cast<void (*)(vector_bool&, __ubuf__ uint32_t*, int32_t, Literal)>(plds))
        .expects(once())
        .will(invoke(load_mask_stub));
}

template <asc::te::experimental::load_sideband_mode SidebandMode>
void check_reg_mask_load(Literal mode)
{
    alignas(32) uint32_t data[32]{};
    constexpr int32_t coord = 32 / sizeof(uint32_t);
    data[0] = 1;
    data[coord] = 2;
    auto layout = asc::te::make_layout(asc::te::make_shape(32));
    auto tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(data), layout);

    expect_mask_load(data, mode);
    auto src = asc::te::experimental::load<bool, SidebandMode>(tensor);
    static_assert(AscendC::Std::is_same_v<decltype(src), asc::te::experimental::reg_tensor<bool>>);
    EXPECT_EQ(src.reg, data[0]);
    GlobalMockObject::verify();

    expect_mask_load(data + coord, mode);
    auto src_at = asc::te::experimental::load<bool, SidebandMode>(tensor, asc::te::make_coord(coord));
    static_assert(AscendC::Std::is_same_v<decltype(src_at), asc::te::experimental::reg_tensor<bool>>);
    EXPECT_EQ(src_at.reg, data[coord]);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_reg_data_move_3510, load_mask_direct)
{
    check_reg_mask_load<asc::te::experimental::load_sideband_mode::direct>(Literal::NORM);
}

TEST_F(tensor_api_reg_data_move_3510, load_mask_upsample)
{
    check_reg_mask_load<asc::te::experimental::load_sideband_mode::upsample>(Literal::US);
}

TEST_F(tensor_api_reg_data_move_3510, load_mask_downsample)
{
    check_reg_mask_load<asc::te::experimental::load_sideband_mode::downsample>(Literal::DS);
}

void store_mask_stub(vector_bool src, __ubuf__ uint32_t* dst, int32_t offset, Literal mode)
{
    check_reg_move_args(dst, offset, mode);
    EXPECT_EQ(src, expected_reg_move.predicate);
}

void expect_mask_store(vector_bool src, __ubuf__ uint32_t* dst, Literal mode)
{
    expected_reg_move = {dst, mode, src};
    MOCKER(static_cast<void (*)(vector_bool, __ubuf__ uint32_t*, int32_t, Literal)>(psts))
        .expects(once())
        .will(invoke(store_mask_stub));
}

template <typename DataType, asc::te::experimental::store_sideband_mode SidebandMode>
void check_reg_mask_store(Literal mode)
{
    constexpr int32_t coord = 32 / sizeof(DataType);
    alignas(32) DataType data[2 * coord]{};
    auto layout = asc::te::make_layout(asc::te::make_shape(2 * coord));
    auto tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(data), layout);
    asc::te::experimental::reg_tensor<bool> src{};
    src.reg = vector_bool{1};
    src.mask = vector_bool{0};

    expect_mask_store(src.reg, reinterpret_cast<uint32_t*>(data), mode);
    asc::te::experimental::store<SidebandMode>(tensor, src);
    GlobalMockObject::verify();

    expect_mask_store(src.reg, reinterpret_cast<uint32_t*>(data + coord), mode);
    asc::te::experimental::store<SidebandMode>(tensor, asc::te::make_coord(coord), src);
    GlobalMockObject::verify();
}

#define TENSOR_API_REG_MASK_STORE_TEST(DataType, SidebandMode, Mode)                                    \
    TEST_F(tensor_api_reg_data_move_3510, store_mask_##SidebandMode##_##DataType)                       \
    {                                                                                                   \
        check_reg_mask_store<DataType, asc::te::experimental::store_sideband_mode::SidebandMode>(Mode); \
    }

TENSOR_API_REG_MASK_STORE_TEST(uint8_t, direct, Literal::NORM)
TENSOR_API_REG_MASK_STORE_TEST(uint16_t, direct, Literal::NORM)
TENSOR_API_REG_MASK_STORE_TEST(uint32_t, direct, Literal::NORM)
TENSOR_API_REG_MASK_STORE_TEST(uint8_t, pack, Literal::PK)
TENSOR_API_REG_MASK_STORE_TEST(uint16_t, pack, Literal::PK)
TENSOR_API_REG_MASK_STORE_TEST(uint32_t, pack, Literal::PK)

#undef TENSOR_API_REG_MASK_STORE_TEST

void load_vector_stub(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset, Literal mode)
{
    check_reg_move_args(src, offset, mode);
    dst = {};
}

void store_vector_stub(vector_uint16_t, __ubuf__ uint16_t* dst, int32_t offset, Literal mode, vector_bool mask)
{
    check_reg_move_args(dst, offset, mode);
    EXPECT_EQ(mask, expected_reg_move.predicate);
}

void expect_vector_store(__ubuf__ uint16_t* dst, Literal mode, vector_bool mask)
{
    expected_reg_move = {dst, mode, mask};
    MOCKER(static_cast<void (*)(vector_uint16_t, __ubuf__ uint16_t*, int32_t, Literal, vector_bool)>(vsts))
        .expects(once())
        .will(invoke(store_vector_stub));
}

TEST_F(tensor_api_reg_data_move_3510, load_and_store_vector)
{
    alignas(32) uint16_t data[128]{};
    auto layout = asc::te::make_layout(asc::te::make_shape(128));
    auto tensor = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(data), layout);

    expected_reg_move = {data, Literal::NORM, {}};
    MOCKER(static_cast<void (*)(vector_uint16_t&, __ubuf__ uint16_t*, int32_t, Literal)>(vlds))
        .expects(exactly(2))
        .will(invoke(load_vector_stub));
    auto src = asc::te::experimental::load(tensor);
    auto src_at = asc::te::experimental::load(tensor, asc::te::make_coord(0));
    GlobalMockObject::verify();

    src.mask = vector_bool{1};
    src_at.mask = vector_bool{2};

    expect_vector_store(data, Literal::NORM_B16, src.mask);
    asc::te::experimental::store(tensor, src);
    GlobalMockObject::verify();

    expect_vector_store(data, Literal::PK_B16, src_at.mask);
    asc::te::experimental::store<asc::te::experimental::store_sideband_mode::pack>(tensor, src_at);
    GlobalMockObject::verify();
}

#define TENSOR_API_REG_GATHER_TEST(DataType, IndexType)             \
    TEST(tensor_api_reg_tensor_3510, gather_register_##DataType)    \
    {                                                               \
        auto mask = asc::te::experimental::all_mask<IndexType>();   \
        compile_reg_register_gather<DataType, IndexType>(mask.reg); \
    }

TENSOR_API_REG_GATHER_TEST(int8_t, uint8_t)
TENSOR_API_REG_GATHER_TEST(uint8_t, uint8_t)
TENSOR_API_REG_GATHER_TEST(fp8_e4m3fn_t, uint8_t)
TENSOR_API_REG_GATHER_TEST(fp8_e5m2_t, uint8_t)
TENSOR_API_REG_GATHER_TEST(fp8_e8m0_t, uint8_t)
TENSOR_API_REG_GATHER_TEST(hifloat8_t, uint8_t)
TENSOR_API_REG_GATHER_TEST(int16_t, uint16_t)
TENSOR_API_REG_GATHER_TEST(uint16_t, uint16_t)
TENSOR_API_REG_GATHER_TEST(half, uint16_t)
TENSOR_API_REG_GATHER_TEST(bfloat16_t, uint16_t)
TENSOR_API_REG_GATHER_TEST(int32_t, uint32_t)
TENSOR_API_REG_GATHER_TEST(uint32_t, uint32_t)
TENSOR_API_REG_GATHER_TEST(float, uint32_t)

#undef TENSOR_API_REG_GATHER_TEST

} // namespace
