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

    void SetUp() override 
    {
        AscendC::SetGCoreType(1);
    }

    void TearDown() override 
    {
        AscendC::SetGCoreType(0);
    }
};

namespace {

uint64_t expected_bt_dst = 0;
__cbuf__ float* expected_bt_src = nullptr;

void copy_cbuf_to_bt_offset_stub(uint64_t dst, __cbuf__ float* src, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)
{
    EXPECT_EQ(dst, expected_bt_dst);
    EXPECT_EQ(src, expected_bt_src);
}

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

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_bt_routes_to_cube_arch_copy)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[m * n] = {0};
    __biasbuf__ float dst[m * n] = {0};

    auto l1_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(m, n));
    auto biastable_tensor = make_tensor_at<location::bias>(dst, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l1_to_biastable, l1_to_biastable_trait_default>(biastable_tensor, l1_tensor);
    run_copy_default_paths<copy_l1_to_biastable, l1_to_biastable_trait_default>(biastable_tensor, l1_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_bt_nd_layout_routes_to_cube_arch_copy)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[m * n] = {0};
    __biasbuf__ float dst[m * n] = {0};

    auto l1_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(m, n));
    auto biastable_tensor = make_tensor_at<location::bias>(dst, make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l1_to_biastable, l1_to_biastable_trait_default>(biastable_tensor, l1_tensor);
    run_copy_default_paths<copy_l1_to_biastable, l1_to_biastable_trait_default>(biastable_tensor, l1_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_bt_offsets)
{
    using namespace asc::te;
    __cbuf__ float src[64] = {0};
    __biasbuf__ float dst[64] = {0};
    auto layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(1, 64);
    auto src_tensor = make_tensor_at<location::l1>(src, layout);
    auto dst_tensor = make_tensor_at<location::bias>(dst, layout);
    expected_bt_dst = reinterpret_cast<uint64_t>(dst + 16);
    expected_bt_src = src + 8;
    MOCKER_CPP(copy_cbuf_to_bt, void(uint64_t, __cbuf__ float*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(&copy_cbuf_to_bt_offset_stub));
    copy(make_copy(copy_l1_to_biastable{}, l1_to_biastable_trait_default{}), dst_tensor, src_tensor,
         make_coord(0, 16), make_coord(0, 8), make_shape(1, 16));
    GlobalMockObject::verify();
}

enum class cube_layout {
    RowMajor,
    NZ,
    ColumnMajor,
    ZN,
    ND
};

enum class prefix {
    gm,
    l1,
    biasbuf,
    fbuf,
};

constexpr int test_fractal_fixed = 16;
constexpr int test_c0_size = 32;
constexpr int test_l1_to_biastable_unit = test_c0_size;          // 64
constexpr int test_c2pipe_to_gm_unit = test_c0_size * 2;      // 128

constexpr int test_ceil_division(int value, int divisor) {
    return (value + divisor - 1) / divisor;
}

// L1 -> BIAS: CopyCbufToBT3501
template<typename data_type, int src_size1, int src_size2, int dst_size1, int dst_size2>
__aicore__ inline void copy_cbuf_to_bt_stub(uint64_t dst, __cbuf__ data_type* src, uint16_t conv_control, uint16_t block_count, uint16_t block_len,
                                uint16_t src_stride, uint16_t dst_stride) {
    EXPECT_EQ(conv_control, 0);
    EXPECT_EQ(block_count, dst_size1);
    EXPECT_EQ(block_len, dst_size2 * sizeof(data_type) / test_l1_to_biastable_unit);
    EXPECT_EQ(src_stride, (src_size2 - dst_size2) * sizeof(data_type) / test_c0_size);
    EXPECT_EQ(dst_stride, (dst_size2 - dst_size2) * sizeof(data_type) / test_l1_to_biastable_unit);
}

// L1 -> BIAS two type: CopyCbufToBT3501
template<typename src_data_type, typename dst_data_type, int src_size1, int src_size2, int dst_size1, int dst_size2>
__aicore__ inline void copy_cbuf_to_bt_two_type_stub(uint64_t dst, __cbuf__ src_data_type* src, uint16_t conv_control, uint16_t block_count, uint16_t block_len,
                                uint16_t src_stride, uint16_t dst_stride) {
    if constexpr (std::is_same_v<src_data_type, half>) {
        EXPECT_EQ(conv_control, 1);
    } else {
        EXPECT_EQ(conv_control, 0);
    }
    EXPECT_EQ(block_count, dst_size1);
    EXPECT_EQ(block_len, dst_size2 * sizeof(src_data_type) / test_l1_to_biastable_unit);
    EXPECT_EQ(src_stride, (src_size2 - dst_size2) * sizeof(src_data_type) / test_c0_size);
    EXPECT_EQ(dst_stride, (dst_size2 - dst_size2) * sizeof(dst_data_type) / test_l1_to_biastable_unit);
}

#define MAKE_LAYOUT_TYPE(fmt) nd_layout_ptn

// create tensor
#define CREATE_TENSOR(data_type, src_size1, src_size2, dst_size1, dst_size2, src_prefix, src_location,                \
                      src_layout_pattern, dst_prefix, dst_location, dst_layout_pattern)                              \
    using namespace asc::te; \
    __##src_prefix##__ data_type src_data[src_size1 * src_size2 * sizeof(data_type)]; \
    __##dst_prefix##__ data_type dst_data[dst_size1 * dst_size2 * sizeof(data_type)]; \
    \
    auto src_iterator = make_mem_ptr<location::l1>(src_data); \
    auto src_layout = make_frame_layout<MAKE_LAYOUT_TYPE(src_layout_pattern), layout_trait_default<data_type>>(src_size1, src_size2); \
    auto src_tensor = make_tensor(src_iterator, src_layout); \
    \
    auto dst_iterator = make_mem_ptr<location::bias>(dst_data); \
    auto dst_layout = make_frame_layout<MAKE_LAYOUT_TYPE(dst_layout_pattern), layout_trait_default<data_type>>(dst_size1, dst_size2); \
    auto dst_tensor = make_tensor(dst_iterator, dst_layout);


// create tensor
#define CREATE_TENSOR_TWO_TYPE(src_data_type, src_size1, src_size2, dst_data_type, dst_size1, dst_size2, src_prefix,  \
                               src_location, src_layout_pattern, dst_prefix, dst_location, dst_layout_pattern)       \
    using namespace asc::te; \
    __##src_prefix##__ src_data_type src_data[src_size1 * src_size2 * sizeof(src_data_type)]; \
    __##dst_prefix##__ dst_data_type dst_data[dst_size1 * dst_size2 * sizeof(dst_data_type)]; \
    \
    auto src_iterator = make_mem_ptr<location::l1>(src_data); \
    auto src_layout = make_frame_layout<MAKE_LAYOUT_TYPE(src_layout_pattern), layout_trait_default<src_data_type>>(src_size1, src_size2); \
    auto src_tensor = make_tensor(src_iterator, src_layout); \
    \
    auto dst_iterator = make_mem_ptr<location::bias>(dst_data); \
    auto dst_layout = make_frame_layout<MAKE_LAYOUT_TYPE(dst_layout_pattern), layout_trait_default<dst_data_type>>(dst_size1, dst_size2); \
    auto dst_tensor = make_tensor(dst_iterator, dst_layout);

// L1 to BIAS ND2ND test case
#define DATA_COPY_TEST_L12BIAS_ND2ND(data_type, src_size1, src_size2, dst_size1, dst_size2) \
    TEST_F(tensor_api_cube_copy_3510, test_tensor_api_data_copy_l1_to_bias_nd_to_nd_##data_type##_##src_size1##x##src_size2##_##dst_size1##x##dst_size2) \
    { \
        using namespace asc::te; \
        MOCKER_CPP(copy_cbuf_to_bt, void(uint64_t, __cbuf__ data_type*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)) \
            .times(1) \
            .will(invoke(&copy_cbuf_to_bt_stub<data_type, src_size1, src_size2, dst_size1, dst_size2>)); \
        CREATE_TENSOR(data_type, src_size1, src_size2, dst_size1, dst_size2, cbuf, L1, ND, biasbuf, BIAS, ND) \
        copy(copy_atom<copy_traits<copy_l1_to_biastable, l1_to_biastable_trait_default>>{}, dst_tensor, src_tensor);\
        GlobalMockObject::verify(); \
    }

DATA_COPY_TEST_L12BIAS_ND2ND(float, 1, 64, 1, 64)
DATA_COPY_TEST_L12BIAS_ND2ND(int32_t, 1, 64, 1, 64)

// L1 to BIAS two data type  test case
#define DATA_COPY_TEST_L12BIAS_TWO_TYPE_ND2ND(src_data_type, src_size1, src_size2, dst_data_type, dst_size1, dst_size2) \
    TEST_F(tensor_api_cube_copy_3510, test_tensor_api_data_copy_l1_to_bias_two_type_nd_to_nd_##src_data_type##_##src_size1##x##src_size2##_##dst_data_type##_##dst_size1##x##dst_size2) \
    { \
        using namespace asc::te; \
        MOCKER_CPP(copy_cbuf_to_bt, void(uint64_t, __cbuf__ src_data_type*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)) \
            .times(1) \
            .will(invoke(&copy_cbuf_to_bt_two_type_stub<src_data_type, dst_data_type, src_size1, src_size2, dst_size1, dst_size2>)); \
        CREATE_TENSOR_TWO_TYPE(src_data_type, src_size1, src_size2, dst_data_type, dst_size1, dst_size2, cbuf, L1, ND, biasbuf, BIAS, ND) \
        copy(copy_atom<copy_traits<copy_l1_to_biastable, l1_to_biastable_trait_default>>{}, dst_tensor, src_tensor);\
        GlobalMockObject::verify(); \
    }

DATA_COPY_TEST_L12BIAS_TWO_TYPE_ND2ND(bfloat16_t, 1, 64, float, 1, 64)
DATA_COPY_TEST_L12BIAS_TWO_TYPE_ND2ND(half, 1, 64, float, 1, 64)

// =========================================================================
// Batch ND2ND (l1_to_bt)
//
// layout_type (batch_value, (m_value, n_value)) constructed via make_batch_pattern_layout always emits
// s_b == m_value*n_value. The implementation only keeps the batched branch:
//   conv_control = 0 (same-type) or 1 (half->float)
//   block_count  = batch_value
//   block_len    = ceil_division(m_value*n_value, c0_element<src_type>), rounded up to 2 for fp32/int32
//   src_stride   = (s_b - m_value*n_value) / c0_element<src_type>
//   dst_stride   = ceil_align((s_b - m_value*n_value) / c0_element<dst_type>, 2)
// With s_b == m_value*n_value both strides are 0.
// =========================================================================
template <typename data_type, int batch_value, int m_value, int n_value>
__aicore__ inline void copy_cbuf_to_bt_batch_compact_stub(uint64_t dst, __cbuf__ data_type* src,
                                                          uint16_t conv_control, uint16_t block_count,
                                                          uint16_t block_len, uint16_t src_stride,
                                                          uint16_t dst_stride)
{
    EXPECT_EQ(conv_control, 0);
    EXPECT_EQ(block_count, batch_value);
    uint16_t expect_block_len = test_ceil_division(m_value * n_value * sizeof(data_type), test_c0_size);
    if constexpr (std::is_same_v<data_type, float> || std::is_same_v<data_type, int32_t>) {
        expect_block_len = (expect_block_len + 1) & ~1;
    }
    EXPECT_EQ(block_len, expect_block_len);
    EXPECT_EQ(src_stride, 0);
    EXPECT_EQ(dst_stride, 0);
}

#define DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_COMPACT(data_type, batch_value, m_value, n_value)                                                     \
    TEST_F(tensor_api_cube_copy_3510, test_tensor_api_data_copy_l1_to_bias_batch_nd_to_nd_##data_type##_##batch_value##x##m_value##x##n_value)   \
    {                                                                                                                  \
        using namespace asc::te;                                                                                   \
        MOCKER_CPP(copy_cbuf_to_bt,                                                                                    \
                   void(uint64_t, __cbuf__ data_type*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t))                  \
            .times(1)                                                                                                  \
            .will(invoke(&copy_cbuf_to_bt_batch_compact_stub<data_type, batch_value, m_value, n_value>));                                        \
        __cbuf__ data_type src_data[batch_value * m_value * n_value];                                                                             \
        __biasbuf__ data_type dst_data[batch_value * m_value * n_value];                                                                          \
        auto src_layout = make_frame_layout<nd_layout_ptn, layout_trait_default<data_type>>(                                      \
            static_cast<uint32_t>(batch_value), static_cast<uint32_t>(m_value), static_cast<uint32_t>(n_value));                             \
        auto dst_layout = make_frame_layout<nd_layout_ptn, layout_trait_default<data_type>>(                                      \
            static_cast<uint32_t>(batch_value), static_cast<uint32_t>(m_value), static_cast<uint32_t>(n_value));                             \
        auto src_tensor = make_tensor(make_mem_ptr<location::l1>(src_data), src_layout);                                     \
        auto dst_tensor = make_tensor(make_mem_ptr<location::bias>(dst_data), dst_layout);                                   \
        copy(copy_atom<copy_traits<copy_l1_to_biastable, l1_to_biastable_trait_default>>{}, dst_tensor, src_tensor);                          \
        GlobalMockObject::verify();                                                                                    \
    }

DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_COMPACT(float, 2, 4, 64)
DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_COMPACT(int32_t, 2, 4, 64)
DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_COMPACT(float, 4, 1, 64)

// half -> float conv_control = 1 path
template <int batch_value, int m_value, int n_value>
__aicore__ inline void copy_cbuf_to_bt_batch_compact_half_to_float_stub(uint64_t dst, __cbuf__ half* src,
                                                                     uint16_t conv_control,
                                                                     uint16_t block_count, uint16_t block_len,
                                                                     uint16_t src_stride, uint16_t dst_stride)
{
    EXPECT_EQ(conv_control, 1);
    EXPECT_EQ(block_count, batch_value);
    EXPECT_EQ(block_len, test_ceil_division(m_value * n_value * sizeof(half), test_c0_size));
    EXPECT_EQ(src_stride, 0);
    EXPECT_EQ(dst_stride, 0);
}

#define DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_HALF2FLOAT(batch_value, m_value, n_value)                                                         \
    TEST_F(tensor_api_cube_copy_3510, test_tensor_api_data_copy_l1_to_bias_batch_nd_to_nd_half_to_float_##batch_value##x##m_value##x##n_value) \
    {                                                                                                                  \
        using namespace asc::te;                                                                                   \
        MOCKER_CPP(copy_cbuf_to_bt, void(uint64_t, __cbuf__ half*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t))  \
            .times(1)                                                                                                  \
            .will(invoke(&copy_cbuf_to_bt_batch_compact_half_to_float_stub<batch_value, m_value, n_value>));                                 \
        __cbuf__ half src_data[batch_value * m_value * n_value];                                                                              \
        __biasbuf__ float dst_data[batch_value * m_value * n_value];                                                                          \
        auto src_layout = make_frame_layout<nd_layout_ptn, layout_trait_default<half>>(                                       \
            static_cast<uint32_t>(batch_value), static_cast<uint32_t>(m_value), static_cast<uint32_t>(n_value));                             \
        auto dst_layout = make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(                                      \
            static_cast<uint32_t>(batch_value), static_cast<uint32_t>(m_value), static_cast<uint32_t>(n_value));                             \
        auto src_tensor = make_tensor(make_mem_ptr<location::l1>(src_data), src_layout);                                     \
        auto dst_tensor = make_tensor(make_mem_ptr<location::bias>(dst_data), dst_layout);                                   \
        copy(copy_atom<copy_traits<copy_l1_to_biastable, l1_to_biastable_trait_default>>{}, dst_tensor, src_tensor);                          \
        GlobalMockObject::verify();                                                                                    \
    }

DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_HALF2FLOAT(2, 4, 64)
