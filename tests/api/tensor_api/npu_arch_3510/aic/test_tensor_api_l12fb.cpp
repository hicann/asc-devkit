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

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_fb_routes_to_cube_arch_copy)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ uint64_t src[m * n] = {0};
    __fbuf__ uint64_t dst[m * n] = {0};

    auto l1_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<uint64_t>>(m, n));
    auto fixbuf_tensor = make_tensor_at<location::fixbuf>(dst, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<uint64_t>>(m, n));

    run_copy_call_paths<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(fixbuf_tensor, l1_tensor);
    run_copy_default_paths<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(fixbuf_tensor, l1_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l1_to_fb_nd_layout_routes_to_cube_arch_copy)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ uint64_t src[m * n] = {0};
    __fbuf__ uint64_t dst[m * n] = {0};

    auto l1_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nd_layout_ptn, layout_trait_default<uint64_t>>(m, n));
    auto fixbuf_tensor = make_tensor_at<location::fixbuf>(dst, make_frame_layout<nd_layout_ptn, layout_trait_default<uint64_t>>(m, n));

    run_copy_call_paths<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(fixbuf_tensor, l1_tensor);
    run_copy_default_paths<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(fixbuf_tensor, l1_tensor);

    EXPECT_EQ(dst[0], 0);
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

// L1 -> FIXBUF: CopyCbufToFB3501
template<typename data_type, int src_size1, int src_size2, int dst_size1, int dst_size2>
__aicore__ inline void copy_cbuf_to_fbuf_stub(__fbuf__ void* dst, __cbuf__ void* src, uint16_t block_count, uint16_t block_len,
                                uint16_t src_stride, uint16_t dst_stride) {
    EXPECT_EQ(block_count, dst_size1);
    EXPECT_EQ(block_len, test_ceil_division(dst_size2 * sizeof(data_type), test_c2pipe_to_gm_unit));
    EXPECT_EQ(src_stride, test_ceil_division(src_size2 * sizeof(data_type), test_c0_size));
    EXPECT_EQ(dst_stride, test_ceil_division(dst_size2 * sizeof(data_type), test_c2pipe_to_gm_unit));
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
    auto dst_iterator = make_mem_ptr<location::fixbuf>(dst_data); \
    auto dst_layout = make_frame_layout<MAKE_LAYOUT_TYPE(dst_layout_pattern), layout_trait_default<data_type>>(dst_size1, dst_size2); \
    auto dst_tensor = make_tensor(dst_iterator, dst_layout);

// L1 to FP ND2ND test case
#define DATA_COPY_TEST_L12FB_ND2ND(data_type, src_size1, src_size2, dst_size1, dst_size2) \
    TEST_F(tensor_api_cube_copy_3510, test_tensor_api_data_copy_l1_to_fb_nd_to_nd_##data_type##_##src_size1##x##src_size2##_##dst_size1##x##dst_size2) \
    { \
        using namespace asc::te; \
        MOCKER_CPP(copy_cbuf_to_fbuf, void(__fbuf__ void*, __cbuf__ void*, uint16_t, uint16_t, uint16_t, uint16_t)) \
            .times(1) \
            .will(invoke(&copy_cbuf_to_fbuf_stub<data_type, src_size1, src_size2, dst_size1, dst_size2>)); \
        CREATE_TENSOR(data_type, src_size1, src_size2, dst_size1, dst_size2, cbuf, L1, ND, fbuf, FIXBUF, ND) \
        copy(copy_atom<copy_traits<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>>{}, dst_tensor, src_tensor);\
        GlobalMockObject::verify(); \
    }

DATA_COPY_TEST_L12FB_ND2ND(uint64_t, 1, 64, 1, 64)
