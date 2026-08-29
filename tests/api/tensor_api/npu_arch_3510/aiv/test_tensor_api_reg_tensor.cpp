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
#include "tensor_api/experimental/vector_compute.h"
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

    asc::te::experimental::reg_tensor<T> dst0{mask};
    asc::te::experimental::reg_tensor<T> dst1{mask};
    static_assert(AscendC::Std::is_same_v<decltype(asc::te::experimental::load(src_tensor, dst0, dst1)), void>);
    asc::te::experimental::load(src_tensor, dst0, dst1);
    static_assert(AscendC::Std::is_same_v<decltype(asc::te::experimental::store(dst_tensor, dst0, dst1)), void>);
    asc::te::experimental::store(dst_tensor, dst0, dst1);

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
        asc::te::experimental::reg_tensor<uint8_t> dst0;
        asc::te::experimental::reg_tensor<uint8_t> dst1;
        static_assert(AscendC::Std::is_same_v<
                      decltype(asc::te::experimental::load(src_tensor, asc::te::make_coord(0), dst0, dst1)), void>);
        asc::te::experimental::load<sideband_mode>(src_tensor, asc::te::make_coord(0), dst0, dst1);
        auto mask = asc::te::experimental::all_mask<uint8_t>();
        dst0.with_mask(mask);
        dst1.with_mask(mask);
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

} // namespace
