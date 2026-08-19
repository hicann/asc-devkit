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
#include <mockcpp/mockcpp.hpp>

class tensor_api_cube_copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override { AscendC::SetGCoreType(1); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

namespace {
using namespace asc::te;
constexpr bool enable_relu = false;
constexpr bool enable_channel_split = true;
constexpr l0c_to_l1_trait l0c_tol1_trait = {round_mode::default_round, enable_relu, enable_channel_split};

struct copy_l0c_to_l1_trait_custom {
    using trait_type = l0c_to_l1_trait;
    static constexpr const trait_type value = l0c_tol1_trait;
};

template <typename location_tag, typename pointer_type, typename layout_type>
auto make_tensor_at(pointer_type ptr, const layout_type& layout)
{
    return asc::te::make_tensor(asc::te::make_mem_ptr<location_tag>(ptr), layout);
}

template <typename copy_operation, typename trait_type, typename dst_tensor_type, typename src_tensor_type>
void run_copy_call_paths(const dst_tensor_type& dst, const src_tensor_type& src)
{
    using namespace asc::te;

    auto atom = make_copy(copy_operation{}, trait_type{});
    atom.call(dst, src);

    copy_atom<copy_traits<copy_operation, trait_type>>{}.call(dst, src);
    copy(copy_atom<copy_traits<copy_operation, trait_type>>{}, dst, src);
}

template <typename copy_operation, typename trait_type, typename param_type, typename dst_tensor_type, typename src_tensor_type>
void run_copy_with_param_paths(const dst_tensor_type& dst, const src_tensor_type& src, const param_type& param)
{
    using namespace asc::te;

    auto atom = make_copy(copy_operation{}).with(param);
    atom.call(dst, src);

    auto atom_copy = copy_atom<copy_traits<copy_operation, trait_type>>{}.with(param);
    atom_copy.call(dst, src);

    copy(atom_copy, dst, src);
    copy(copy_atom<copy_traits<copy_operation, trait_type>>{}.with(param), dst, src);
}

} // namespace

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_l1_nz_to_nz_float)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __cbuf__ float dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, float>(m, n));
    auto l1_tensor = make_tensor_at<location::l1>(dst, make_frame_layout<nz_layout_ptn, float>(m, n));

    run_copy_call_paths<copy_l0c_to_l1, l0c_to_l1_trait_default>(l1_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_l1, l0c_to_l1_trait_default>(l1_tensor, l0c_tensor, l0c_to_l1_params{});
    auto atom = make_copy(copy_l0c_to_l1{}, l0c_to_l1_trait_default{});
    copy(atom, l1_tensor, l0c_tensor, make_coord(0, 0), zero_coord, make_shape(16, 16));
    copy(atom.with(l0c_to_l1_params{}), l1_tensor, l0c_tensor,
        zero_coord, make_coord(0, 0), make_shape(16, 16));

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_l1_nz_to_nz_int32)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ int32_t src[m * n] = {0};
    __cbuf__ int32_t dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, int32_t>(m, n));
    auto l1_tensor = make_tensor_at<location::l1>(dst, make_frame_layout<nz_layout_ptn, int32_t>(m, n));

    run_copy_call_paths<copy_l0c_to_l1, l0c_to_l1_trait_default>(l1_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_l1, l0c_to_l1_trait_default>(l1_tensor, l0c_tensor, l0c_to_l1_params{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_l1_nz_to_nz_f32_to_f16)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __cbuf__ half dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, float>(m, n));
    auto l1_tensor = make_tensor_at<location::l1>(dst, make_frame_layout<nz_layout_ptn, half>(m, n));

    run_copy_call_paths<copy_l0c_to_l1, l0c_to_l1_trait_default>(l1_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_l1, l0c_to_l1_trait_default>(l1_tensor, l0c_tensor, l0c_to_l1_params{});

    EXPECT_EQ(dst[0], static_cast<half>(0));
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_l1_nz_to_nz_with_channel_split)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __cbuf__ float dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, float>(m, n));
    auto l1_tensor = make_tensor_at<location::l1>(dst, make_frame_layout<nz_layout_ptn, float>(m, n));

    run_copy_call_paths<copy_l0c_to_l1, copy_l0c_to_l1_trait_custom>(l1_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_l1, copy_l0c_to_l1_trait_custom>(l1_tensor, l0c_tensor, l0c_to_l1_params{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_l1_batch_tensor_quant_coord_shape_compiles)
{
    using namespace asc::te;
    constexpr uint32_t batch = 3;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ int32_t src[batch * m * n] = {0};
    __cbuf__ int8_t dst[batch * m * n] = {0};
    __cbuf__ uint64_t quant[batch * n] = {0};

    auto src_tensor = make_tensor_at<location::l0c>(src,
        make_frame_layout<nz_layout_ptn, layout_trait_default<int32_t, _16>>(batch, m, n));
    auto dst_tensor = make_tensor_at<location::l1>(dst,
        make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t, _16>>(batch, m, n));
    auto quant_tensor = make_tensor_at<location::l1>(quant,
        make_frame_layout<nd_ext_layout_ptn, layout_trait_default<uint64_t>>(batch, 1, n));
    auto atom = make_copy(copy_l0c_to_l1{}, l0c_to_l1_trait_default{});

    if (false) {
        copy(atom, dst_tensor, src_tensor, quant_tensor,
            make_coord(1, make_coord(0, 0)), make_coord(1, make_coord(0, 0)),
            make_shape(1, make_shape(m, n)));
    }
    SUCCEED();
}
