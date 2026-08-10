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
#include "include/tensor_api/tensor.h"

namespace {

template <typename data_type>
struct is_tensor_api_global_tensor : AscendC::Std::false_type {};

template <typename engine_type, typename layout_type>
struct is_tensor_api_global_tensor<asc::te::global_tensor<engine_type, layout_type>>
    : AscendC::Std::true_type {};

template <typename data_type>
constexpr bool is_tensor_api_global_tensor_v =
    is_tensor_api_global_tensor<AscendC::Std::remove_cvref_t<data_type>>::value;

template <typename data_type>
struct is_tensor_api_local_tensor : AscendC::Std::false_type {};

template <typename engine_type, typename layout_type>
struct is_tensor_api_local_tensor<asc::te::local_tensor<engine_type, layout_type>>
    : AscendC::Std::true_type {};

template <typename data_type>
constexpr bool is_tensor_api_local_tensor_v =
    is_tensor_api_local_tensor<AscendC::Std::remove_cvref_t<data_type>>::value;

} // namespace

class tensor_api_tensor_cache_mode : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_tensor_cache_mode, set_l2_cache_hint)
{
    using namespace asc::te;

    constexpr uint32_t tile_length = 8;
    __gm__ float data[tile_length] = {0, 1, 2, 3, 4, 5, 6, 7};
    auto ptr = make_mem_ptr<location::gm>(data);
    auto tensor = make_tensor(ptr, make_shape(_2{}, _2{}, _2{}),
                             make_stride(_4{}, _2{}, _1{}));

    tensor.set_l2_cache_hint(cache_mode::disable);
    EXPECT_EQ(tensor.engine().get_cache_mode(), static_cast<uint8_t>(cache_mode::disable));

    tensor.set_l2_cache_hint(cache_mode::normal);
    EXPECT_EQ(tensor.engine().get_cache_mode(), static_cast<uint8_t>(cache_mode::normal));

    tensor.set_l2_cache_hint(cache_mode::last);
    EXPECT_EQ(tensor.engine().get_cache_mode(), static_cast<uint8_t>(cache_mode::last));

    tensor.set_l2_cache_hint(cache_mode::persistent);
    EXPECT_EQ(tensor.engine().get_cache_mode(), static_cast<uint8_t>(cache_mode::persistent));
}

TEST_F(tensor_api_tensor_cache_mode, slice_coord_tensor_can_set_l2_cache_hint)
{
    using namespace asc::te;

    constexpr uint32_t m = 8;
    constexpr uint32_t n = 16;
    __gm__ float gm_data[m * n] = {0};

    auto layout = make_frame_layout<nd_layout_ptn>(m, n);
    auto gm_tensor = make_tensor(make_mem_ptr(gm_data), layout);

    auto coord_tensor = gm_tensor(make_coord(2, 4));

    static_assert(is_tensor_api_global_tensor_v<decltype(coord_tensor)>);

    coord_tensor.set_l2_cache_hint(cache_mode::disable);
    EXPECT_EQ(coord_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::disable));

    coord_tensor.set_l2_cache_hint(cache_mode::normal);
    EXPECT_EQ(coord_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::normal));
}

TEST_F(tensor_api_tensor_cache_mode, slice_tensor_can_set_l2_cache_hint)
{
    using namespace asc::te;

    constexpr uint32_t m = 8;
    constexpr uint32_t n = 16;
    __gm__ float gm_data[m * n] = {0};

    auto layout = make_frame_layout<nd_layout_ptn>(m, n);
    auto gm_tensor = make_tensor(make_mem_ptr(gm_data), layout);

    auto slice_tensor = gm_tensor.slice(make_coord(2, 4), make_shape(4, 8));

    static_assert(is_tensor_api_global_tensor_v<decltype(slice_tensor)>);

    slice_tensor.set_l2_cache_hint(cache_mode::last);
    EXPECT_EQ(slice_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::last));

    slice_tensor.set_l2_cache_hint(cache_mode::persistent);
    EXPECT_EQ(slice_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::persistent));
}

TEST_F(tensor_api_tensor_cache_mode, slice_tensor_inherits_cache_mode)
{
    using namespace asc::te;

    constexpr uint32_t m = 8;
    constexpr uint32_t n = 16;
    __gm__ float gm_data[m * n] = {0};

    auto layout = make_frame_layout<nd_layout_ptn>(m, n);
    auto gm_tensor = make_tensor(make_mem_ptr(gm_data), layout);

    gm_tensor.set_l2_cache_hint(cache_mode::disable);
    EXPECT_EQ(gm_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::disable));

    auto coord_tensor = gm_tensor(make_coord(2, 4));
    EXPECT_EQ(coord_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::disable));

    auto slice_tensor = gm_tensor.slice(make_coord(2, 4), make_shape(4, 8));
    EXPECT_EQ(slice_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::disable));

    coord_tensor.set_l2_cache_hint(cache_mode::normal);
    EXPECT_EQ(coord_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::normal));

    EXPECT_EQ(gm_tensor.engine().get_cache_mode(),
              static_cast<uint8_t>(cache_mode::disable));
}

TEST_F(tensor_api_tensor_cache_mode, slice_tensor_still_global_tensor)
{
    using namespace asc::te;

    constexpr uint32_t m = 8;
    constexpr uint32_t n = 16;
    __gm__ float gm_data[m * n] = {0};

    auto layout = make_frame_layout<nd_layout_ptn>(m, n);
    auto gm_tensor = make_tensor(make_mem_ptr(gm_data), layout);

    static_assert(is_tensor_api_global_tensor_v<decltype(gm_tensor)>);

    auto coord_tensor = gm_tensor(make_coord(2, 4));
    static_assert(is_tensor_api_global_tensor_v<decltype(coord_tensor)>);
    static_assert(!is_tensor_api_local_tensor_v<decltype(coord_tensor)>);

    auto slice_tensor = gm_tensor.slice(make_coord(2, 4), make_shape(4, 8));
    static_assert(is_tensor_api_global_tensor_v<decltype(slice_tensor)>);
    static_assert(!is_tensor_api_local_tensor_v<decltype(slice_tensor)>);

    gm_tensor.set_l2_cache_hint(cache_mode::disable);
    coord_tensor.set_l2_cache_hint(cache_mode::normal);
    slice_tensor.set_l2_cache_hint(cache_mode::persistent);

    EXPECT_TRUE(true);
}
