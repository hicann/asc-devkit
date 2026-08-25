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

} // namespace

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0b_routes_to_cube_arch_copy)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[m * n] = {0};
    __cb__ float dst[m * n] = {0};

    auto l1_tensor =
        make_tensor_at<location::l1>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n));
    auto l0b_tensor =
        make_tensor_at<location::l0b>(dst, make_frame_layout<zn_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l1_to_l0b, l1_to_l0b_trait_default>(l0b_tensor, l1_tensor);
    run_copy_default_paths<copy_l1_to_l0b, l1_to_l0b_trait_default>(l0b_tensor, l1_tensor);

    EXPECT_EQ(dst[0], 0);
}

template <bool transpose, typename data_type, int expected_m_step, int expected_k_step>
void load_cbuf_to_cb_stub(
    __cb__ data_type* dst, __cbuf__ data_type* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride, bool transposed)
{
    EXPECT_EQ(m_step, expected_m_step);
    EXPECT_EQ(k_step, expected_k_step);
    EXPECT_EQ(transposed, transpose);
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0b_steps)
{
    using namespace asc::te;

    constexpr uint32_t dim = 32;
    __cbuf__ half src[dim * dim] = {0};
    __cb__ half dst[dim * dim] = {0};
    auto layout = make_frame_layout<zn_layout_ptn, layout_trait_default<half>>(dim, dim);
    auto src_tensor = make_tensor_at<location::l1>(src, layout);
    auto dst_tensor = make_tensor_at<location::l0b>(dst, layout);

    MOCKER_CPP(
        load_cbuf_to_cb,
        void(__cb__ half*, __cbuf__ half*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .times(1)
        .will(invoke(&load_cbuf_to_cb_stub<false, half, 1, 1>));

    copy(dst_tensor, src_tensor, make_coord(0, 0), make_coord(0, 16), make_shape(16, 16));
    mockcpp::GlobalMockObject::verify();
}

namespace {

__cb__ fp4x2_e1m2_t* g_batch_b4_l0b_dst_base = nullptr;
__cbuf__ fp4x2_e1m2_t* g_batch_b4_l0b_src_base = nullptr;
uint32_t g_batch_b4_l0b_src_batch_stride = 0;
uint32_t g_batch_b4_l0b_dst_batch_stride = 0;
uint32_t g_batch_b4_l0b_dst_split_stride = 0;
uint32_t g_batch_b4_l0b_call_index = 0;

template <
    typename data_type, int expected_m_step, int expected_k_step, int expected_src_stride, int expected_dst_stride,
    uint32_t expected_split_num>
void load_cbuf_to_cb_batch_b4_split_stub(
    __cb__ data_type* dst, __cbuf__ data_type* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride, bool transposed)
{
    const auto batch_idx = g_batch_b4_l0b_call_index / expected_split_num;
    const auto split_idx = g_batch_b4_l0b_call_index % expected_split_num;
    EXPECT_EQ(
        dst, g_batch_b4_l0b_dst_base + batch_idx * g_batch_b4_l0b_dst_batch_stride +
                 split_idx * g_batch_b4_l0b_dst_split_stride);
    EXPECT_EQ(src, g_batch_b4_l0b_src_base + batch_idx * g_batch_b4_l0b_src_batch_stride);
    EXPECT_EQ(m_start_position, split_idx * expected_m_step);
    EXPECT_EQ(k_start_position, 0);
    EXPECT_EQ(m_step, expected_m_step);
    EXPECT_EQ(k_step, expected_k_step);
    EXPECT_EQ(src_stride, expected_src_stride);
    EXPECT_EQ(dst_stride, expected_dst_stride);
    EXPECT_TRUE(transposed);
    ++g_batch_b4_l0b_call_index;
}

} // namespace

template <typename data_type, uint32_t batch_value, uint32_t m_value, uint32_t n_value>
auto make_batch_zn_layout()
{
    using namespace asc::te;
    constexpr uint32_t c0_value = c0_element<data_type>;

    auto shape = make_shape(
        AscendC::Std::Int<batch_value>{},
        make_shape(
            make_shape(
                AscendC::Std::Int<c0_value>{},
                AscendC::Std::ceil_division(AscendC::Std::Int<m_value>{}, AscendC::Std::Int<c0_value>{})),
            make_shape(
                AscendC::Std::Int<fractal_fixed>{},
                AscendC::Std::ceil_division(AscendC::Std::Int<n_value>{}, AscendC::Std::Int<fractal_fixed>{}))));
    auto stride = make_stride(
        AscendC::Std::Int<m_value * n_value>{},
        make_stride(
            make_stride(
                _1{}, AscendC::Std::Int<c0_value>{} *
                          AscendC::Std::ceil_align(AscendC::Std::Int<n_value>{}, AscendC::Std::Int<fractal_fixed>{})),
            make_stride(AscendC::Std::Int<c0_value>{}, AscendC::Std::Int<c0_value * fractal_fixed>{})));
    return make_pattern_layout<zn_layout_ptn, layout_trait_default<data_type>>(shape, stride);
}

template <typename data_type, uint32_t batch_value, uint32_t m_value, uint32_t n_value>
auto make_batch_nz_layout()
{
    using namespace asc::te;
    constexpr uint32_t c0_value = c0_element<data_type>;

    auto shape = make_shape(
        AscendC::Std::Int<batch_value>{},
        make_shape(
            make_shape(
                AscendC::Std::Int<fractal_fixed>{},
                AscendC::Std::ceil_division(AscendC::Std::Int<m_value>{}, AscendC::Std::Int<fractal_fixed>{})),
            make_shape(
                AscendC::Std::Int<c0_value>{},
                AscendC::Std::ceil_division(AscendC::Std::Int<n_value>{}, AscendC::Std::Int<c0_value>{}))));
    auto stride = make_stride(
        AscendC::Std::Int<m_value * n_value>{},
        make_stride(
            make_stride(AscendC::Std::Int<c0_value>{}, AscendC::Std::Int<c0_value * fractal_fixed>{}),
            make_stride(
                _1{}, AscendC::Std::Int<c0_value>{} *
                          AscendC::Std::ceil_align(AscendC::Std::Int<m_value>{}, AscendC::Std::Int<fractal_fixed>{}))));
    return make_pattern_layout<nz_layout_ptn, layout_trait_default<data_type>>(shape, stride);
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0b_zn_to_zn_batch_routes_to_single_batch_copy)
{
    using namespace asc::te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t n = 16;
    __cbuf__ half src[batch * m * n] = {0};
    __cb__ half dst[batch * m * n] = {0};

    auto src_layout = make_batch_zn_layout<half, batch, m, n>();
    auto dst_layout = make_batch_zn_layout<half, batch, m, n>();
    auto src_tensor = make_tensor(make_mem_ptr<location::l1>(src), src_layout);
    auto dst_tensor = make_tensor(make_mem_ptr<location::l0b>(dst), dst_layout);

    MOCKER_CPP(
        load_cbuf_to_cb,
        void(__cb__ half*, __cbuf__ half*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .expects(once())
        .will(invoke(&load_cbuf_to_cb_stub<false, half, 1, batch>));

    copy(copy_atom<copy_traits<copy_l1_to_l0b, l1_to_l0b_trait_default>>{}, dst_tensor, src_tensor);

    mockcpp::GlobalMockObject::verify();
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0b_nz_to_zn_batch_routes_to_single_batch_copy)
{
    using namespace asc::te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t n = 16;
    __cbuf__ half src[batch * m * n] = {0};
    __cb__ half dst[batch * m * n] = {0};

    auto src_layout = make_batch_nz_layout<half, batch, m, n>();
    auto dst_layout = make_batch_zn_layout<half, batch, m, n>();
    auto src_tensor = make_tensor(make_mem_ptr<location::l1>(src), src_layout);
    auto dst_tensor = make_tensor(make_mem_ptr<location::l0b>(dst), dst_layout);

    MOCKER_CPP(
        load_cbuf_to_cb,
        void(__cb__ half*, __cbuf__ half*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .expects(exactly(batch))
        .will(invoke(&load_cbuf_to_cb_stub<true, half, 1, 1>));

    copy(copy_atom<copy_traits<copy_l1_to_l0b, l1_to_l0b_trait_default>>{}, dst_tensor, src_tensor);

    mockcpp::GlobalMockObject::verify();
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_l0b_batch_nz_to_zn_b8_b4)
{
    using namespace asc::te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 128;
    constexpr uint32_t n = 16;
    constexpr uint32_t src_buffer_size = 8192;
    constexpr uint32_t dst_buffer_size = 2048;
    constexpr uint32_t split_num = 2;
    __cbuf__ fp4x2_e1m2_t src[batch * src_buffer_size];
    __cb__ fp4x2_e1m2_t dst[batch * dst_buffer_size];

    auto src_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<fp4x2_e1m2_t>>(batch, m, n);
    auto dst_layout = make_frame_layout<zn_layout_ptn, layout_trait_default<fp4x2_e1m2_t>>(batch, m, n);
    auto src_tensor = make_tensor(make_mem_ptr<location::l1>(src), src_layout);
    auto dst_tensor = make_tensor(make_mem_ptr<location::l0b>(dst), dst_layout);

    auto first_dst = make_single_batch_sub_tensor(dst_tensor, 0)(make_coord(make_coord(0, 0), make_coord(0, 0)));
    auto second_dst = make_single_batch_sub_tensor(dst_tensor, 1)(make_coord(make_coord(0, 0), make_coord(0, 0)));
    auto split_dst = make_single_batch_sub_tensor(dst_tensor, 0)(make_coord(make_coord(0, 1), make_coord(0, 0)));
    auto first_src = make_single_batch_sub_tensor(src_tensor, 0);
    auto second_src = make_single_batch_sub_tensor(src_tensor, 1);
    g_batch_b4_l0b_dst_base = first_dst.data().get();
    g_batch_b4_l0b_src_base = first_src.data().get();
    g_batch_b4_l0b_dst_batch_stride = second_dst.data().get() - g_batch_b4_l0b_dst_base;
    g_batch_b4_l0b_dst_split_stride = split_dst.data().get() - g_batch_b4_l0b_dst_base;
    g_batch_b4_l0b_src_batch_stride = second_src.data().get() - g_batch_b4_l0b_src_base;
    g_batch_b4_l0b_call_index = 0;

    MOCKER_CPP(
        load_cbuf_to_cb_s4, void(
                                __cb__ fp4x2_e1m2_t*, __cbuf__ fp4x2_e1m2_t*, uint16_t, uint16_t, uint8_t, uint8_t,
                                int16_t, uint16_t, bool))
        .times(batch * split_num)
        .will(invoke(&load_cbuf_to_cb_batch_b4_split_stub<fp4x2_e1m2_t, 4, 1, 8, 1, split_num>));

    copy(copy_atom<copy_traits<copy_l1_to_l0b, l1_to_l0b_trait_default>>{}, dst_tensor, src_tensor);

    EXPECT_EQ(g_batch_b4_l0b_call_index, batch * split_num);
    mockcpp::GlobalMockObject::verify();
}

#define MAKE_LAYOUT_TYPE_IMPL(fmt) MAKE_LAYOUT_TYPE_##fmt
#define MAKE_LAYOUT_TYPE(fmt) MAKE_LAYOUT_TYPE_IMPL(fmt)
#define MAKE_LAYOUT_TYPE_NZ nz_layout_ptn
#define MAKE_LAYOUT_TYPE_ZN zn_layout_ptn
#define TEST_L1_TO_L0B_CONCAT_IMPL_(name, line) name##line
#define TEST_L1_TO_L0B_CONCAT_(name, line) TEST_L1_TO_L0B_CONCAT_IMPL_(name, line)

#define TEST_TENSOR_API_LOAD_DATA(                                                                                    \
    data_type, m_value, n_value, src_format, dst_format, src_pos, dst_pos, src_tag, dst_tag, expected_transpose)      \
    TEST_F(tensor_api_cube_copy_3510, TEST_L1_TO_L0B_CONCAT_(test_load_data_l1_to_l0b_case_, __LINE__))               \
    {                                                                                                                 \
        using namespace asc::te;                                                                                      \
        __##dst_tag##__ data_type dst[m_value * n_value] = {0};                                                       \
        auto dst_iterator = make_mem_ptr<location::l0b>(dst);                                                         \
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
        copy(copy_atom<copy_traits<copy_l1_to_l0b, l1_to_l0b_trait_default>>{}, dst_tensor, src_tensor);              \
                                                                                                                      \
        mockcpp::GlobalMockObject::verify();                                                                          \
    }

#define TEST_TENSOR_API_LOAD_S4_DATA(                                                                            \
    data_type, m_value, n_value, src_format, dst_format, src_pos, dst_pos, src_tag, dst_tag, expected_transpose) \
    TEST_F(tensor_api_cube_copy_3510, TEST_L1_TO_L0B_CONCAT_(test_load_s4_data_l1_to_l0b_case_, __LINE__))       \
    {                                                                                                            \
        using namespace asc::te;                                                                                 \
        __##dst_tag##__ data_type dst[m_value * n_value];                                                        \
        auto dst_iterator = make_mem_ptr<location::l0b>(dst);                                                    \
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
        copy(copy_atom<copy_traits<copy_l1_to_l0b, l1_to_l0b_trait_default>>{}, dst_tensor, src_tensor);         \
                                                                                                                 \
        mockcpp::GlobalMockObject::verify();                                                                     \
    }

// l1 -> l0b ZN2ZN非转置，覆盖所有data_type
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e1m2_t, 64, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e2m1_t, 64, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(bfloat16_t, 16, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(half, 16, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(float, 8, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(int32_t, 8, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(uint32_t, 8, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(int8_t, 32, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(uint8_t, 32, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(int16_t, 16, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(uint16_t, 16, 16, ZN, ZN, L1, L0B, cbuf, cb, false);

// l1 -> l0b NZ2ZN转置，覆盖所有data_type
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e1m2_t, 64, 64, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e2m1_t, 64, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(bfloat16_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(half, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(float, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(int32_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(uint32_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(int8_t, 32, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(int8_t, 32, 32, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(uint8_t, 32, 32, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(int16_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(uint16_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
