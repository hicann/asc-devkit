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
    copy(atom, dst, src, zero_coord, make_coord(0, 0), make_shape(16, 16));
    copy(dst, src);
    copy(dst, src, zero_coord, make_coord(0, 0), make_shape(16, 16));
}

template <typename copy_operation, typename trait_type, typename dst_tensor_type, typename src_tensor_type>
void run_copy_default_paths(const dst_tensor_type& dst, const src_tensor_type& src)
{
    using namespace asc::te;

    auto atom = copy_atom<copy_traits<copy_operation, trait_type>>{};
    atom.call(dst, src);
    copy(atom, dst, src);
    copy(atom, dst, src, make_coord(0, 0), zero_coord, make_shape(16, 16));
}

__ca__ float* g_batch_dst_base = nullptr;
__cbuf__ float* g_batch_src_base = nullptr;
uint32_t g_batch_call_index = 0;
__ca__ fp4x2_e1m2_t* g_batch_b4_dst_base = nullptr;
__cbuf__ fp4x2_e1m2_t* g_batch_b4_src_base = nullptr;
uint32_t g_batch_b4_call_index = 0;

} // namespace

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0a_routes_to_cube_arch_copy)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[m * n] = {0};
    __ca__ float dst[m * n] = {0};

    auto l1_tensor =
        make_tensor_at<location::l1>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n));
    auto l0a_tensor =
        make_tensor_at<location::l0a>(dst, make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l1_to_l0a, l1_to_l0a_trait_default>(l0a_tensor, l1_tensor);
    run_copy_default_paths<copy_l1_to_l0a, l1_to_l0a_trait_default>(l0a_tensor, l1_tensor);

    EXPECT_EQ(dst[0], 0);
}

template <bool transpose, typename data_type, int expected_m_step, int expected_k_step>
void load_cbuf_to_ca_stub(
    __ca__ data_type* dst, __cbuf__ data_type* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride, bool transposed)
{
    EXPECT_EQ(m_step, expected_m_step);
    EXPECT_EQ(k_step, expected_k_step);
    EXPECT_EQ(transposed, transpose);
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0a_steps)
{
    using namespace asc::te;

    constexpr uint32_t dim = 32;
    __cbuf__ half src[dim * dim] = {0};
    __ca__ half dst[dim * dim] = {0};
    auto layout = make_frame_layout<nz_layout_ptn, layout_trait_default<half>>(dim, dim);
    auto src_tensor = make_tensor_at<location::l1>(src, layout);
    auto dst_tensor = make_tensor_at<location::l0a>(dst, layout);

    MOCKER_CPP(
        load_cbuf_to_ca,
        void(__ca__ half*, __cbuf__ half*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .times(1)
        .will(invoke(&load_cbuf_to_ca_stub<false, half, 1, 1>));

    copy(dst_tensor, src_tensor, make_coord(16, 0), make_coord(0, 0), make_shape(16, 16));
    mockcpp::GlobalMockObject::verify();
}

template <
    typename data_type, int expected_m_step, int expected_k_step, int expected_src_stride, int expected_dst_stride>
void load_cbuf_to_ca_batch_stub(
    __ca__ data_type* dst, __cbuf__ data_type* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride, bool transposed)
{
    EXPECT_EQ(dst, g_batch_dst_base);
    EXPECT_EQ(src, g_batch_src_base);
    EXPECT_EQ(m_start_position, 0);
    EXPECT_EQ(k_start_position, 0);
    EXPECT_EQ(m_step, expected_m_step);
    EXPECT_EQ(k_step, expected_k_step);
    EXPECT_EQ(src_stride, expected_src_stride);
    EXPECT_EQ(dst_stride, expected_dst_stride);
    EXPECT_FALSE(transposed);
    ++g_batch_call_index;
}

template <
    typename data_type, int expected_m_step, int expected_k_step, int expected_src_stride, int expected_dst_stride,
    uint32_t expected_batch_stride, bool expected_transpose>
void load_cbuf_to_ca_batch_offset_stub(
    __ca__ data_type* dst, __cbuf__ data_type* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride, bool transposed)
{
    const auto expected_offset = g_batch_call_index * expected_batch_stride;
    EXPECT_EQ(dst, g_batch_dst_base + expected_offset);
    EXPECT_EQ(src, g_batch_src_base + expected_offset);
    EXPECT_EQ(m_start_position, 0);
    EXPECT_EQ(k_start_position, 0);
    EXPECT_EQ(m_step, expected_m_step);
    EXPECT_EQ(k_step, expected_k_step);
    EXPECT_EQ(src_stride, expected_src_stride);
    EXPECT_EQ(dst_stride, expected_dst_stride);
    EXPECT_EQ(transposed, expected_transpose);
    ++g_batch_call_index;
}

template <
    typename data_type, int expected_m_step, int expected_k_step, int expected_src_stride, int expected_dst_stride,
    uint32_t expected_src_batch_stride, uint32_t expected_dst_batch_stride, uint32_t expected_dst_split_stride,
    uint32_t expected_split_num>
void load_cbuf_to_ca_batch_b4_split_stub(
    __ca__ data_type* dst, __cbuf__ data_type* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride, bool transposed)
{
    const auto batch_idx = g_batch_b4_call_index / expected_split_num;
    const auto split_idx = g_batch_b4_call_index % expected_split_num;
    EXPECT_EQ(dst, g_batch_b4_dst_base + batch_idx * expected_dst_batch_stride + split_idx * expected_dst_split_stride);
    EXPECT_EQ(src, g_batch_b4_src_base + batch_idx * expected_src_batch_stride);
    EXPECT_EQ(m_start_position, split_idx * expected_m_step);
    EXPECT_EQ(k_start_position, 0);
    EXPECT_EQ(m_step, expected_m_step);
    EXPECT_EQ(k_step, expected_k_step);
    EXPECT_EQ(src_stride, expected_src_stride);
    EXPECT_EQ(dst_stride, expected_dst_stride);
    EXPECT_TRUE(transposed);
    ++g_batch_b4_call_index;
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0a_batch_nz_to_nz)
{
    using namespace asc::te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[batch * m * n] = {0};
    __ca__ float dst[batch * m * n] = {0};

    auto batch_layout = make_frame_layout<nz_layout_ptn, float>(batch, m, n);
    auto l1_tensor = make_tensor_at<location::l1>(src, batch_layout);
    auto l0a_tensor = make_tensor_at<location::l0a>(dst, batch_layout);

    g_batch_dst_base = dst;
    g_batch_src_base = src;
    g_batch_call_index = 0;

    MOCKER_CPP(
        load_cbuf_to_ca,
        void(__ca__ float*, __cbuf__ float*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .times(1)
        .will(invoke(&load_cbuf_to_ca_batch_stub<float, 2, 8, 2, 2>));

    copy(copy_atom<copy_traits<copy_l1_to_l0a, l1_to_l0a_trait_default>>{}, l0a_tensor, l1_tensor);

    EXPECT_EQ(g_batch_call_index, 1);
    mockcpp::GlobalMockObject::verify();
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0a_batch_zn_to_nz)
{
    using namespace asc::te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    constexpr uint32_t batch_stride = m * n;
    __cbuf__ float src[batch * m * n] = {0};
    __ca__ float dst[batch * m * n] = {0};

    auto src_batch_layout = make_frame_layout<zn_layout_ptn, float>(batch, m, n);
    auto dst_batch_layout = make_frame_layout<nz_layout_ptn, float>(batch, m, n);
    auto l1_tensor = make_tensor_at<location::l1>(src, src_batch_layout);
    auto l0a_tensor = make_tensor_at<location::l0a>(dst, dst_batch_layout);

    g_batch_dst_base = dst;
    g_batch_src_base = src;
    g_batch_call_index = 0;

    MOCKER_CPP(
        load_cbuf_to_ca,
        void(__ca__ float*, __cbuf__ float*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .times(batch)
        .will(invoke(&load_cbuf_to_ca_batch_offset_stub<float, 2, 4, 2, 2, batch_stride, true>));

    copy(copy_atom<copy_traits<copy_l1_to_l0a, l1_to_l0a_trait_default>>{}, l0a_tensor, l1_tensor);

    EXPECT_EQ(g_batch_call_index, batch);
    mockcpp::GlobalMockObject::verify();
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0a_batch_zn_to_nz_b8_b4)
{
    using namespace asc::te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t n = 128;
    constexpr uint32_t src_batch_stride = 4096;
    constexpr uint32_t dst_batch_stride = 1024;
    constexpr uint32_t dst_split_stride = 512;
    constexpr uint32_t split_num = 2;
    __cbuf__ fp4x2_e1m2_t src[batch * src_batch_stride];
    __ca__ fp4x2_e1m2_t dst[batch * dst_batch_stride];

    auto src_batch_layout = make_frame_layout<zn_layout_ptn, layout_trait_default<fp4x2_e1m2_t>>(batch, m, n);
    auto dst_batch_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<fp4x2_e1m2_t>>(batch, m, n);
    auto l1_tensor = make_tensor_at<location::l1>(src, src_batch_layout);
    auto l0a_tensor = make_tensor_at<location::l0a>(dst, dst_batch_layout);

    g_batch_b4_dst_base = dst;
    g_batch_b4_src_base = src;
    g_batch_b4_call_index = 0;

    MOCKER_CPP(
        load_cbuf_to_ca_s4, void(
                                __ca__ fp4x2_e1m2_t*, __cbuf__ fp4x2_e1m2_t*, uint16_t, uint16_t, uint8_t, uint8_t,
                                int16_t, uint16_t, bool))
        .times(batch * split_num)
        .will(invoke(&load_cbuf_to_ca_batch_b4_split_stub<
                     fp4x2_e1m2_t, 4, 1, 8, 1, src_batch_stride, dst_batch_stride, dst_split_stride, split_num>));

    copy(copy_atom<copy_traits<copy_l1_to_l0a, l1_to_l0a_trait_default>>{}, l0a_tensor, l1_tensor);

    EXPECT_EQ(g_batch_b4_call_index, batch * split_num);
    mockcpp::GlobalMockObject::verify();
}

#define MAKE_LAYOUT_TYPE_IMPL(fmt) MAKE_LAYOUT_TYPE_##fmt
#define MAKE_LAYOUT_TYPE(fmt) MAKE_LAYOUT_TYPE_IMPL(fmt)
#define MAKE_LAYOUT_TYPE_NZ nz_layout_ptn
#define MAKE_LAYOUT_TYPE_ZN zn_layout_ptn
#define TEST_L1_TO_L0A_CONCAT_IMPL_(name, line) name##line
#define TEST_L1_TO_L0A_CONCAT_(name, line) TEST_L1_TO_L0A_CONCAT_IMPL_(name, line)

#define TEST_TENSOR_API_LOAD_DATA(                                                                                    \
    data_type, m_value, n_value, src_format, dst_format, src_pos, dst_pos, src_tag, dst_tag, expected_transpose)      \
    TEST_F(tensor_api_cube_copy_3510, TEST_L1_TO_L0A_CONCAT_(test_load_data_l1_to_l0a_case_, __LINE__))               \
    {                                                                                                                 \
        using namespace asc::te;                                                                                      \
        __##dst_tag##__ data_type dst[m_value * n_value] = {0};                                                       \
        auto dst_iterator = make_mem_ptr<location::l0a>(dst);                                                         \
        auto dst_matrix_layout =                                                                                      \
            make_frame_layout<MAKE_LAYOUT_TYPE(dst_format), layout_trait_default<data_type>>(m_value, n_value);       \
        auto dst_tensor = make_tensor(dst_iterator, dst_matrix_layout);                                               \
                                                                                                                      \
        __##src_tag##__ data_type src[m_value * n_value] = {0};                                                       \
        auto src_iterator = make_mem_ptr<location::l1>(src);                                                          \
        auto src_matrix_layout =                                                                                      \
            make_frame_layout<MAKE_LAYOUT_TYPE(src_format), layout_trait_default<data_type>>(m_value, n_value);       \
        auto src_tensor = make_tensor(src_iterator, src_matrix_layout);                                               \
                                                                                                                      \
        constexpr int expected_m_step = (sizeof(data_type) == 1 && expected_transpose) ? 2 : 1;                       \
        constexpr int expected_k_step = (sizeof(data_type) == 4 && expected_transpose) ? 2 : 1;                       \
        MOCKER_CPP(                                                                                                   \
            load_cbuf_to_##dst_tag, void(                                                                             \
                                        __##dst_tag##__ data_type*, __cbuf__ data_type*, uint16_t, uint16_t, uint8_t, \
                                        uint8_t, int16_t, uint16_t, bool))                                            \
            .times(1)                                                                                                 \
            .will(invoke(                                                                                             \
                &load_cbuf_to_##dst_tag##_stub<expected_transpose, data_type, expected_m_step, expected_k_step>));    \
        copy(copy_atom<copy_traits<copy_l1_to_l0a, l1_to_l0a_trait_default>>{}, dst_tensor, src_tensor);              \
                                                                                                                      \
        mockcpp::GlobalMockObject::verify();                                                                          \
    }

#define TEST_TENSOR_API_LOAD_S4_DATA(                                                                            \
    data_type, m_value, n_value, src_format, dst_format, src_pos, dst_pos, src_tag, dst_tag, expected_transpose) \
    TEST_F(tensor_api_cube_copy_3510, TEST_L1_TO_L0A_CONCAT_(test_load_s4_data_l1_to_l0a_case_, __LINE__))       \
    {                                                                                                            \
        using namespace asc::te;                                                                                 \
        __##dst_tag##__ data_type dst[m_value * n_value];                                                        \
        auto dst_iterator = make_mem_ptr<location::l0a>(dst);                                                    \
        auto dst_matrix_layout =                                                                                 \
            make_frame_layout<MAKE_LAYOUT_TYPE(dst_format), layout_trait_default<data_type>>(m_value, n_value);  \
        auto dst_tensor = make_tensor(dst_iterator, dst_matrix_layout);                                          \
                                                                                                                 \
        __##src_tag##__ data_type src[m_value * n_value];                                                        \
        auto src_iterator = make_mem_ptr<location::l1>(src);                                                     \
        auto src_matrix_layout =                                                                                 \
            make_frame_layout<MAKE_LAYOUT_TYPE(src_format), layout_trait_default<data_type>>(m_value, n_value);  \
        auto src_tensor = make_tensor(src_iterator, src_matrix_layout);                                          \
                                                                                                                 \
        copy(copy_atom<copy_traits<copy_l1_to_l0a, l1_to_l0a_trait_default>>{}, dst_tensor, src_tensor);         \
                                                                                                                 \
        mockcpp::GlobalMockObject::verify();                                                                     \
    }

// l1 -> l0a NZ2NZ非转置，覆盖所有data_type
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e1m2_t, 16, 64, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e2m1_t, 16, 64, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(bfloat16_t, 16, 16, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(half, 16, 16, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(float, 16, 8, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(int32_t, 16, 8, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(uint32_t, 16, 8, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(int8_t, 16, 32, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(uint8_t, 16, 32, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(int16_t, 16, 16, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(uint16_t, 16, 16, NZ, NZ, L1, L0A, cbuf, ca, false);

// l1 -> l0a ZN2NZ转置，覆盖所有data_type
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e1m2_t, 64, 64, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e2m1_t, 16, 64, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(bfloat16_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(half, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(float, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(int32_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(uint32_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(int8_t, 16, 32, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(int8_t, 32, 32, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(uint8_t, 32, 32, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(int16_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(uint16_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);

// ================= img2col (L1 NC1HWC0 -> L0A NZ) =================
// The img2col instruction has 17 params, above mockcpp's API-hook arity limit, so this drives the
// full copy path against the empty cce stub and checks it routes/compiles without a crash. The dst
// NZ m_value/k_size (conv-unfolded, ceil-aligned) and the src NC1HWC0 axes exercise LoadDataL12L0AImg2Col::Run.
namespace {
template <typename data_type, size_t c0_value>
auto make_nc1hwc0_layout(int n, int c1, int h, int w)
{
    using namespace asc::te;
    return make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, static_cast<int>(c0_value));
}
} // namespace

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0a_img2_col)
{
    using namespace asc::te;

    // src NC1HWC0: n_value=1, c1=2, height=5, width=5, c0_value=16 -> channel_size = c1*c0_value = 32.
    constexpr int n_value = 1;
    constexpr int c1 = 2;
    constexpr int height = 5;
    constexpr int width = 5;
    constexpr int c0_value = 16;

    // filter 3x3, stride 1, pad 1, dilation 1 -> Ho = Wo = 5, m_value = 25, k_size = 3*3*32 = 288.
    constexpr int filter_h = 3;
    constexpr int filter_w = 3;
    constexpr int m_value = 25;
    constexpr int k_size = filter_h * filter_w * c1 * c0_value; // 288
    constexpr int m_align = 32;                                 // ceil(25, 16) * 16
    constexpr int k_align = 288;                                // already a multiple of c0_value

    __cbuf__ int16_t src[n_value * c1 * height * width * c0_value] = {0};
    __ca__ int16_t dst[m_align * k_align] = {0};

    auto src_tensor = make_tensor(
        make_mem_ptr<location::l1>(src), make_nc1hwc0_layout<int16_t, c0_value>(n_value, c1, height, width));
    auto l0a_tensor = make_tensor_at<location::l0a>(
        dst, make_frame_layout<nz_layout_ptn, layout_trait_default<int16_t>>(m_value, k_size));

    // dst NZ ceil-aligned m_value/k_size, now provided by the caller via img2col_params.
    img2col_params<int16_t> params;
    params.m_extension = static_cast<uint16_t>(m_align);
    params.k_extension = static_cast<uint16_t>(k_align);
    params.filter_w = filter_w;
    params.filter_h = filter_h;
    params.stride_w = 1;
    params.stride_h = 1;
    params.pad_list[0] = params.pad_list[1] = params.pad_list[2] = params.pad_list[3] = 1;

    auto atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    copy(atom.with(params), l0a_tensor, src_tensor);

    EXPECT_EQ(dst[0], 0);
}

// conv3d img2col (L1 NDC1HWC0 -> L0A NZ). The depth axis depth is merged with c1 into the channel
// dimension (channel_size = depth*c1*c0_value), so the caller passes the whole NDC1HWC0 tensor (no per-depth
// slicing). Routes to LoadDataL12L0AImg2Col3D. Drives the full path against the empty cce stub to
// verify it routes/compiles.
TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0a_img2col_3d)
{
    using namespace asc::te;

    // src NDC1HWC0: n_value=1, depth=4, c1=2, height=5, width=5, c0_value=16 -> merged channel_size = depth*c1*c0_value
    // = 128.
    constexpr int n_value = 1;
    constexpr int depth = 4;
    constexpr int c1 = 2;
    constexpr int height = 5;
    constexpr int width = 5;
    constexpr int c0_value = 16;

    // filter 3x3, stride 1, pad 1 -> Ho = Wo = 5, m_value = 25, k_size = 3*3*(depth*c1*c0_value) = 3*3*128 = 1152.
    constexpr int filter_h = 3;
    constexpr int filter_w = 3;
    constexpr int m_value = 25;
    constexpr int k_size = filter_h * filter_w * depth * c1 * c0_value; // 1152
    constexpr int m_align = 32;
    constexpr int k_align = 1152;

    __cbuf__ int16_t src[n_value * depth * c1 * height * width * c0_value] = {0};
    __ca__ int16_t dst[m_align * k_align] = {0};

    auto src_tensor = make_tensor(
        make_mem_ptr<location::l1>(src),
        make_frame_layout<ndc1hwc0_layout_ptn>(n_value, depth, c1, height, width, static_cast<int>(c0_value)));
    auto l0a_tensor = make_tensor_at<location::l0a>(
        dst, make_frame_layout<nz_layout_ptn, layout_trait_default<int16_t>>(m_value, k_size));

    img2col_params<int16_t> params;
    params.m_extension = static_cast<uint16_t>(m_align);
    params.k_extension = static_cast<uint16_t>(k_align);
    params.filter_w = filter_w;
    params.filter_h = filter_h;
    params.stride_w = 1;
    params.stride_h = 1;
    params.pad_list[0] = params.pad_list[1] = params.pad_list[2] = params.pad_list[3] = 1;

    auto atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    // Whole NDC1HWC0 tensor: depth and c1 are merged into the channel axis inside the img2col.
    copy(atom.with(params), l0a_tensor, src_tensor);

    EXPECT_EQ(dst[0], 0);
}
