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
#include "include/tensor_api/tensor.h"

class tensor_api_vector_copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override { AscendC::SetGCoreType(2); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

namespace {

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
    copy(dst, src);
    copy(dst, src, zero_coord, zero_coord, src.shape());
}

template <typename copy_operation, typename trait_type, typename dst_tensor_type, typename src_tensor_type>
void run_copy_default_paths(const dst_tensor_type& dst, const src_tensor_type& src)
{
    using namespace asc::te;

    auto atom = copy_atom<copy_traits<copy_operation, trait_type>>{};
    atom.call(dst, src);
    copy(atom, dst, src);
}

__cbuf__ void* expected_l1_dst = nullptr;
__ubuf__ void* expected_ub_src = nullptr;
uint16_t expected_block_count = 0;
uint16_t expected_block_len = 0;
uint16_t expected_src_gap = 0;
uint16_t expected_dst_gap = 0;

void copy_ubuf_to_cbuf_stub(
    __cbuf__ void* dst, __ubuf__ void* src, uint8_t sid, uint16_t block_count, uint16_t block_len, uint16_t src_gap,
    uint16_t dst_gap)
{
    EXPECT_EQ(dst, expected_l1_dst);
    EXPECT_EQ(src, expected_ub_src);
    EXPECT_EQ(sid, 0);
    EXPECT_EQ(block_count, expected_block_count);
    EXPECT_EQ(block_len, expected_block_len);
    EXPECT_EQ(src_gap, expected_src_gap);
    EXPECT_EQ(dst_gap, expected_dst_gap);
}

template <typename Call>
void check_copy_params(
    __cbuf__ void* dst, __ubuf__ void* src, uint16_t block_count, uint16_t block_len, uint16_t src_gap,
    uint16_t dst_gap, Call call)
{
    expected_l1_dst = dst;
    expected_ub_src = src;
    expected_block_count = block_count;
    expected_block_len = block_len;
    expected_src_gap = src_gap;
    expected_dst_gap = dst_gap;
    MOCKER_CPP(copy_ubuf_to_cbuf, void(__cbuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_cbuf_stub));
    call();
    GlobalMockObject::verify();
}

} // namespace

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_l1_one_dim)
{
    using namespace asc::te;

    constexpr uint32_t copy_size = 64;
    __ubuf__ int8_t src[copy_size] = {0};
    __cbuf__ int8_t dst[copy_size] = {0};
    auto layout = make_layout(make_shape(copy_size), make_stride(1));
    auto src_tensor = make_tensor_at<location::ub>(src, layout);
    auto dst_tensor = make_tensor_at<location::l1>(dst, layout);

    static_assert(Std::is_same_v<get_layout_pattern<decltype(layout)>, one_dim_layout_ptn>);
    check_copy_params(dst, src, 1, 2, 0, 0, [&] { copy(dst_tensor, src_tensor); });
    check_copy_params(dst + 32, src + 16, 1, 1, 0, 0, [&] {
        copy(dst_tensor, src_tensor, make_coord(32), make_coord(16), make_shape(32));
    });
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_l1_nd_to_nd)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __cbuf__ int8_t dst[m * n] = {0};

    auto ub_tensor =
        make_tensor_at<location::ub>(src, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto l1_tensor =
        make_tensor_at<location::l1>(dst, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);
    run_copy_default_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_l1_nd_layout_to_nd_layout)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __cbuf__ int8_t dst[m * n] = {0};

    auto ub_tensor =
        make_tensor_at<location::ub>(src, make_frame_layout<nd_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto l1_tensor =
        make_tensor_at<location::l1>(dst, make_frame_layout<nd_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);
    run_copy_default_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_l1_dn_to_dn)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __cbuf__ int8_t dst[m * n] = {0};

    auto ub_tensor =
        make_tensor_at<location::ub>(src, make_frame_layout<dn_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto l1_tensor =
        make_tensor_at<location::l1>(dst, make_frame_layout<dn_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);
    run_copy_default_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_l1_dn_layout_to_dn_layout)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __cbuf__ int8_t dst[m * n] = {0};

    auto ub_tensor =
        make_tensor_at<location::ub>(src, make_frame_layout<dn_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto l1_tensor =
        make_tensor_at<location::l1>(dst, make_frame_layout<dn_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);
    run_copy_default_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_l1_nz_to_nz)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __cbuf__ int8_t dst[m * n] = {0};

    auto ub_tensor =
        make_tensor_at<location::ub>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto l1_tensor =
        make_tensor_at<location::l1>(dst, make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);
    run_copy_default_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_l1_zn_to_zn)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __cbuf__ int8_t dst[m * n] = {0};

    auto ub_tensor =
        make_tensor_at<location::ub>(src, make_frame_layout<zn_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto l1_tensor =
        make_tensor_at<location::l1>(dst, make_frame_layout<zn_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);
    run_copy_default_paths<copy_ub_to_l1, ub_to_l1_trait_default>(l1_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_l1_nz_example_params)
{
    using namespace asc::te;
    __ubuf__ int8_t src[32 * 64] = {0};
    __cbuf__ int8_t dst[32 * 64] = {0};
    auto layout = make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(32, 64);
    auto src_tensor = make_tensor_at<location::ub>(src, layout);
    auto dst_tensor = make_tensor_at<location::l1>(dst, layout);
    auto atom = make_copy(copy_ub_to_l1{}, ub_to_l1_trait_default{});

    check_copy_params(dst + layout(make_coord(0, 32)), src, 1, 32, 0, 0, [&] {
        copy(atom, dst_tensor, src_tensor, make_coord(0, 32), make_coord(0, 0), make_shape(32, 32));
    });
    check_copy_params(dst, src, 2, 32, 0, 0, [&] { copy(dst_tensor, src_tensor); });
    check_copy_params(dst + layout(make_coord(16, 0)), src, 2, 16, 16, 16, [&] {
        copy(dst_tensor, src_tensor, make_coord(16, 0), make_coord(0, 0), make_shape(16, 64));
    });
}
