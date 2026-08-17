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
#include <utility>

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/tensor.h"

namespace {

using namespace asc::te;

template <typename T, typename = void>
struct has_mask_member : std::false_type {};

template <typename T>
struct has_mask_member<T, std::void_t<decltype(&T::mask)>> : std::true_type {};

template <typename tensor_type, typename coord_type, typename = void>
struct has_reg_load : std::false_type {};

template <typename tensor_type, typename coord_type>
struct has_reg_load<
    tensor_type, coord_type,
    std::void_t<decltype(std::declval<const tensor_type&>().template load<load_sideband_mode::direct>(
        std::declval<const coord_type&>()))>> : std::true_type {};

template <typename T, load_sideband_mode sideband_mode = load_sideband_mode::direct>
__simd_vf__ inline void compile_reg_local_tensor_load_store(
    __ubuf__ T* dst, __ubuf__ T* src, vector_bool mask)
{
    auto layout = make_layout(make_shape(64));
    auto dst_mem = make_mem_ptr<location::ub>(dst);
    auto src_mem = make_mem_ptr<location::ub>(src);
    using engine_type = view_engine<decltype(src_mem)>;
    using layout_type = decltype(layout);
    local_tensor<engine_type, layout_type> dst_tensor(dst_mem, layout);
    local_tensor<engine_type, layout_type> src_tensor(src_mem, layout);
    using coord_type = decltype(make_coord(0));
    using tensor_api_base = typename decltype(src_tensor)::tensor_api_base;
    static_assert(!has_reg_load<tensor_api_base, coord_type>::value);
    static_assert(has_reg_load<decltype(src_tensor), coord_type>::value);
    static_assert(std::is_base_of_v<tensor_api_base, decltype(src_tensor)>);

    reg_tensor<bool> mask_reg {mask};
    auto value = src_tensor.template load<sideband_mode>(make_coord(0)).with_mask(mask_reg);
    static_assert(std::is_same_v<typename decltype(value)::type, T>);
    dst_tensor.store(make_coord(0), value);

    auto full_value = src_tensor.load(make_coord(0)).with_mask(all_mask<T>());
    static_assert(std::is_same_v<typename decltype(full_value)::type, T>);

    reg_tensor<T> assigned {mask};
    auto& assigned_ref = (assigned = static_cast<T>(1));
    static_assert(std::is_same_v<decltype(assigned_ref), reg_tensor<T>&>);
}

template <load_sideband_mode sideband_mode>
__simd_vf__ inline void compile_reg_local_tensor_load_mode(__ubuf__ uint8_t* src)
{
    auto layout = make_layout(make_shape(256));
    auto src_tensor = make_tensor(make_mem_ptr<location::ub>(src), layout);
    if constexpr (sideband_mode == load_sideband_mode::deintlv) {
        reg_tensor<uint8_t> dst0;
        reg_tensor<uint8_t> dst1;
        static_assert(std::is_same_v<decltype(src_tensor.load(make_coord(0), dst0, dst1)), void>);
        src_tensor.template load<sideband_mode>(make_coord(0), dst0, dst1);
        auto mask = all_mask<uint8_t>();
        dst0.with_mask(mask);
        dst1.with_mask(mask);
    } else {
        auto value = src_tensor.template load<sideband_mode>(make_coord(0));
        static_assert(std::is_same_v<decltype(value), reg_tensor<uint8_t>>);
        value.with_mask(all_mask<uint8_t>());
    }
}

template <broadcast_mode broadcast_mode_value>
__simd_vf__ inline void compile_reg_local_tensor_broadcast(__ubuf__ float* src)
{
    auto layout = make_layout(make_shape(64));
    auto src_tensor = make_tensor(make_mem_ptr<location::ub>(src), layout);
    auto value = src_tensor.template load_broadcast<broadcast_mode_value>(make_coord(0));
    static_assert(std::is_same_v<decltype(value), reg_tensor<float>>);
    value.with_mask(all_mask<float>());
}

template <store_sideband_mode sideband_mode>
__simd_vf__ inline void compile_reg_local_tensor_store_mode(__ubuf__ uint32_t* dst, vector_bool mask)
{
    auto layout = make_layout(make_shape(64));
    auto dst_tensor = make_tensor(make_mem_ptr<location::ub>(dst), layout);
    reg_tensor<uint32_t> src0 {mask};
    if constexpr (sideband_mode == store_sideband_mode::intlv) {
        reg_tensor<uint32_t> src1 {mask};
        static_assert(std::is_same_v<decltype(dst_tensor.store(make_coord(0), src0, src1)), void>);
        dst_tensor.template store<sideband_mode>(make_coord(0), src0, src1);
    } else {
        static_assert(
            std::is_same_v<decltype(dst_tensor.template store<sideband_mode>(make_coord(0), src0)), void>);
        dst_tensor.template store<sideband_mode>(make_coord(0), src0);
    }
}

template <typename T>
__simd_vf__ inline void compile_reg_mask(uint32_t remain)
{
    auto vl8_mask = make_mask<mask_pattern::vl8, T>();
    auto every3_mask = make_mask<mask_pattern::every3, T>();
    auto empty_mask = none_mask<T>();
    auto tail_mask = update_mask<T>(remain);
    static_assert(std::is_same_v<decltype(vl8_mask), reg_tensor<bool>>);
    static_assert(std::is_same_v<decltype(every3_mask), reg_tensor<bool>>);
    (void)vl8_mask;
    (void)every3_mask;
    (void)empty_mask;
    (void)tail_mask;
}

template <typename T>
__simd_vf__ inline void compile_reg_mask_data_reorder(vector_bool src0, vector_bool src1)
{
    reg_tensor<bool> mask0 {src0};
    reg_tensor<bool> mask1 {src1};
    auto interleaved = interleave<T>(mask0, mask1);
    static_assert(std::is_same_v<decltype(interleaved), reg_pair<bool>>);
    static_assert(std::is_same_v<decltype(interleaved.first), reg_tensor<bool>>);
    static_assert(std::is_same_v<decltype(interleaved.second), reg_tensor<bool>>);

    auto restored = deinterleave<T>(interleaved.first, interleaved.second);
    static_assert(std::is_same_v<decltype(restored), reg_pair<bool>>);
    (void)restored;
}

TEST(tensor_api_reg_tensor_3510, holds_register_and_mask)
{
    reg_tensor<float> value {};
    (void)value.reg;
    (void)value.mask;

    auto compile_load_store = &compile_reg_local_tensor_load_store<float>;
    (void)compile_load_store;

    auto compile_direct = &compile_reg_local_tensor_load_mode<load_sideband_mode::direct>;
    auto compile_deintlv = &compile_reg_local_tensor_load_mode<load_sideband_mode::deintlv>;
    auto compile_downsample = &compile_reg_local_tensor_load_mode<load_sideband_mode::downsample>;
    auto compile_unpack = &compile_reg_local_tensor_load_mode<load_sideband_mode::unpack>;
    auto compile_unpack4 = &compile_reg_local_tensor_load_mode<load_sideband_mode::unpack4>;
    auto compile_upsample = &compile_reg_local_tensor_load_mode<load_sideband_mode::upsample>;
    (void)compile_direct;
    (void)compile_deintlv;
    (void)compile_downsample;
    (void)compile_unpack;
    (void)compile_unpack4;
    (void)compile_upsample;

    auto compile_broadcast_elem = &compile_reg_local_tensor_broadcast<broadcast_mode::elem>;
    auto compile_broadcast_datablock = &compile_reg_local_tensor_broadcast<broadcast_mode::datablock>;
    auto compile_broadcast_elem2datablock =
        &compile_reg_local_tensor_broadcast<broadcast_mode::elem2datablock>;
    (void)compile_broadcast_elem;
    (void)compile_broadcast_datablock;
    (void)compile_broadcast_elem2datablock;

    auto compile_store_direct = &compile_reg_local_tensor_store_mode<store_sideband_mode::direct>;
    auto compile_store_1st = &compile_reg_local_tensor_store_mode<store_sideband_mode::store_1st>;
    auto compile_store_intlv = &compile_reg_local_tensor_store_mode<store_sideband_mode::intlv>;
    auto compile_store_pack = &compile_reg_local_tensor_store_mode<store_sideband_mode::pack>;
    auto compile_store_pack_quarter = &compile_reg_local_tensor_store_mode<store_sideband_mode::pack_quarter>;
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
