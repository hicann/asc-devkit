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

__ubuf__ void* expected_dst = nullptr;
__ubuf__ void* expected_src = nullptr;
uint16_t expected_block_count = 0;
uint16_t expected_block_len = 0;
uint16_t expected_src_gap = 0;
uint16_t expected_dst_gap = 0;

void copy_ubuf_to_ubuf_stub(__ubuf__ void* dst, __ubuf__ void* src, uint8_t sid, uint16_t block_count,
    uint16_t block_len, uint16_t src_gap, uint16_t dst_gap)
{
    EXPECT_EQ(dst, expected_dst);
    EXPECT_EQ(src, expected_src);
    EXPECT_EQ(sid, 0);
    EXPECT_EQ(block_count, expected_block_count);
    EXPECT_EQ(block_len, expected_block_len);
    EXPECT_EQ(src_gap, expected_src_gap);
    EXPECT_EQ(dst_gap, expected_dst_gap);
}

template <typename DstTensor, typename SrcTensor, typename DstCoord, typename SrcCoord, typename CopyShape>
void check_region_copy(const DstTensor& dst, const SrcTensor& src, const DstCoord& dst_coord,
    const SrcCoord& src_coord, const CopyShape& copy_shape, uint16_t block_count, uint16_t block_len,
    uint16_t src_gap, uint16_t dst_gap)
{
    auto resolved_dst = asc::te::resolve_copy_coord(dst.layout(), copy_shape, dst_coord);
    auto resolved_src = asc::te::resolve_copy_coord(src.layout(), copy_shape, src_coord);
    expected_dst = reinterpret_cast<__ubuf__ void*>((dst.data() + dst.layout()(resolved_dst)).get());
    expected_src = reinterpret_cast<__ubuf__ void*>((src.data() + src.layout()(resolved_src)).get());
    expected_block_count = block_count;
    expected_block_len = block_len;
    expected_src_gap = src_gap;
    expected_dst_gap = dst_gap;

    MOCKER_CPP(copy_ubuf_to_ubuf,
        void(__ubuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_ubuf_stub));
    copy(dst, src, dst_coord, src_coord, copy_shape);
    GlobalMockObject::verify();
}

template <typename DstTensor, typename SrcTensor>
void check_whole_copy(const DstTensor& dst, const SrcTensor& src, uint16_t block_count, uint16_t block_len,
    uint16_t src_gap, uint16_t dst_gap)
{
    expected_dst = reinterpret_cast<__ubuf__ void*>(dst.data().get());
    expected_src = reinterpret_cast<__ubuf__ void*>(src.data().get());
    expected_block_count = block_count;
    expected_block_len = block_len;
    expected_src_gap = src_gap;
    expected_dst_gap = dst_gap;

    MOCKER_CPP(copy_ubuf_to_ubuf,
        void(__ubuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_ubuf_stub));
    copy(dst, src);
    GlobalMockObject::verify();
}

} // namespace

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_nd_to_nd)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto src_tensor = make_tensor_at<location::ub>(src, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto dst_tensor = make_tensor_at<location::ub>(dst, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);
    run_copy_default_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_nd_layout_to_nd_layout)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto src_tensor = make_tensor_at<location::ub>(src, make_frame_layout<nd_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto dst_tensor = make_tensor_at<location::ub>(dst, make_frame_layout<nd_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);
    run_copy_default_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_nd_whole_params)
{
    using namespace asc::te;
    __ubuf__ int8_t src[64 * 64] = {0};
    __ubuf__ int8_t dst[64 * 64] = {0};
    auto layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(64, 64);
    auto src_tensor = make_tensor_at<location::ub>(src, layout);
    auto dst_tensor = make_tensor_at<location::ub>(dst, layout);

    expected_dst = dst;
    expected_src = src;
    expected_block_count = 64;
    expected_block_len = 2;
    expected_src_gap = 0;
    expected_dst_gap = 0;
    MOCKER_CPP(copy_ubuf_to_ubuf,
        void(__ubuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_ubuf_stub));

    copy(dst_tensor, src_tensor);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_dn_to_dn)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto src_tensor = make_tensor_at<location::ub>(src, make_frame_layout<dn_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto dst_tensor = make_tensor_at<location::ub>(dst, make_frame_layout<dn_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);
    run_copy_default_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_dn_layout_to_dn_layout)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto src_tensor = make_tensor_at<location::ub>(src, make_frame_layout<dn_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto dst_tensor = make_tensor_at<location::ub>(dst, make_frame_layout<dn_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);
    run_copy_default_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_nz_to_nz)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto src_tensor = make_tensor_at<location::ub>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto dst_tensor = make_tensor_at<location::ub>(dst, make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);
    run_copy_default_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_zn_to_zn)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto src_tensor = make_tensor_at<location::ub>(src, make_frame_layout<zn_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto dst_tensor = make_tensor_at<location::ub>(dst, make_frame_layout<zn_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);
    run_copy_default_paths<copy_ub_to_ub, ub_to_ub_trait_default>(dst_tensor, src_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_nd_region_params)
{
    using namespace asc::te;
    __ubuf__ int8_t src[64 * 64] = {0};
    __ubuf__ int8_t dst[64 * 64] = {0};
    auto layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(64, 64);
    auto src_tensor = make_tensor_at<location::ub>(src, layout);
    auto dst_tensor = make_tensor_at<location::ub>(dst, layout);

    check_region_copy(dst_tensor, src_tensor, make_coord(3, 16), make_coord(5, 0), make_shape(2, 32),
        2, 1, 1, 1);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_dn_region_params)
{
    using namespace asc::te;
    __ubuf__ int8_t src[64 * 64] = {0};
    __ubuf__ int8_t dst[64 * 64] = {0};
    auto layout = make_frame_layout<dn_ext_layout_ptn, layout_trait_default<int8_t>>(64, 64);
    auto src_tensor = make_tensor_at<location::ub>(src, layout);
    auto dst_tensor = make_tensor_at<location::ub>(dst, layout);

    check_region_copy(dst_tensor, src_tensor, make_coord(16, 3), make_coord(0, 5), make_shape(32, 2),
        2, 1, 1, 1);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_nz_region_params)
{
    using namespace asc::te;
    __ubuf__ int8_t src[64 * 64] = {0};
    __ubuf__ int8_t dst[64 * 64] = {0};
    auto layout = make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(64, 64);
    auto src_tensor = make_tensor_at<location::ub>(src, layout);
    auto dst_tensor = make_tensor_at<location::ub>(dst, layout);

    check_region_copy(dst_tensor, src_tensor, make_coord(16, 32), make_coord(0, 0), make_shape(32, 32),
        1, 32, 32, 32);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_zn_region_params)
{
    using namespace asc::te;
    __ubuf__ int8_t src[64 * 64] = {0};
    __ubuf__ int8_t dst[64 * 64] = {0};
    auto layout = make_frame_layout<zn_layout_ptn, layout_trait_default<int8_t>>(64, 64);
    auto src_tensor = make_tensor_at<location::ub>(src, layout);
    auto dst_tensor = make_tensor_at<location::ub>(dst, layout);

    check_region_copy(dst_tensor, src_tensor, make_coord(32, 16), make_coord(0, 0), make_shape(32, 32),
        1, 32, 32, 32);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_ub_whole_shape_matches_whole)
{
    using namespace asc::te;
    __ubuf__ int8_t src[64 * 64] = {0};
    __ubuf__ int8_t dst[64 * 64] = {0};

    auto check = [&](const auto& layout, uint16_t block_count, uint16_t block_len, uint16_t gap) {
        auto src_tensor = make_tensor_at<location::ub>(src, layout);
        auto dst_tensor = make_tensor_at<location::ub>(dst, layout);
        check_whole_copy(dst_tensor, src_tensor, block_count, block_len, gap, gap);
        check_region_copy(dst_tensor, src_tensor, zero_coord, zero_coord, src_tensor.shape(), block_count,
            block_len, gap, gap);
    };

    check(make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(64, 64), 64, 2, 0);
    check(make_frame_layout<dn_ext_layout_ptn, layout_trait_default<int8_t>>(64, 64), 64, 2, 0);
    check(make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(64, 64), 2, 64, 0);
    check(make_frame_layout<zn_layout_ptn, layout_trait_default<int8_t>>(64, 64), 2, 64, 0);
}
