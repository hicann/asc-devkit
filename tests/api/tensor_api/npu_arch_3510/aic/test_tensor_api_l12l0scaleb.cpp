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

class tensor_api_cube_copy_l1_to_l0scaleb_3510 : public testing::Test {
protected:
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
    return asc::te::make_tensor(asc::te::make_mem_ptr<location_tag, fp8_e8m0_t>(reinterpret_cast<uint64_t>(ptr) / 16), layout);
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

uint64_t g_expected_mx_dst_addr = 0;
__cbuf__ void* g_expected_mx_src = nullptr;

// Batched scale L1->L0 now copies one matrix per batch in a loop (see commit "scale batch layout"),
// so the instruction fires once per batch with the single-matrix step values, and the dst/src
// addresses advance by the batch layout's per-batch stride each call. With the compact
// make_batch_pattern_layout below, that stride is the single-matrix capacity (NN m=32,n=16,c0=2 -> 512),
// applied equally to dst and src. Single-matrix m_step(x_step)=ceil(n/16)=1, y_step=m/2=16.
constexpr uint32_t k_scaleb_batch = 3;
constexpr uint32_t k_scalebm = 32;
constexpr uint32_t k_scalebn = 16;
constexpr uint32_t k_scaleb_batch_stride_elem = 512;
uint32_t g_scaleb_call_idx = 0;

void load_cbuf_to_cb_mx_batch_stub(uint64_t dst, __cbuf__ void* src, uint16_t x_start_pos, uint16_t y_start_pos,
    uint8_t x_step, uint8_t y_step, uint16_t src_stride, uint16_t dst_stride)
{
    EXPECT_EQ(dst, g_expected_mx_dst_addr + g_scaleb_call_idx * k_scaleb_batch_stride_elem);
    EXPECT_EQ(src, reinterpret_cast<__cbuf__ void*>(
        reinterpret_cast<fp8_e8m0_t*>(g_expected_mx_src) + g_scaleb_call_idx * k_scaleb_batch_stride_elem));
    EXPECT_EQ(x_start_pos, 0);
    EXPECT_EQ(y_start_pos, 0);
    EXPECT_EQ(x_step, 1);
    EXPECT_EQ(y_step, 16);
    EXPECT_EQ(src_stride, 16);
    EXPECT_EQ(dst_stride, 16);
    ++g_scaleb_call_idx;
}

void load_cbuf_to_cb_mx_stride_stub(uint64_t dst, __cbuf__ void* src, uint16_t x_start_pos,
    uint16_t y_start_pos, uint8_t x_step, uint8_t y_step, uint16_t src_stride, uint16_t dst_stride)
{
    EXPECT_EQ(dst, g_expected_mx_dst_addr);
    EXPECT_EQ(src, g_expected_mx_src);
    EXPECT_EQ(x_start_pos, 0);
    EXPECT_EQ(y_start_pos, 0);
    EXPECT_EQ(x_step, 1);
    EXPECT_EQ(y_step, 2);
    EXPECT_EQ(src_stride, 2);
    EXPECT_EQ(dst_stride, 2);
}

} // namespace

TEST_F(tensor_api_cube_copy_l1_to_l0scaleb_3510, copy_l1_to_l0scaleb_routes_to_cube_arch_copy)
{
    using namespace asc::te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 16;
    __cbuf__ fp8_e8m0_t src[m * n] = {0};
    __cb__ fp8_e8m0_t dst[m * n] = {0};

    auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(src), make_frame_layout<nn_layout_ptn, AscendC::Std::Int<2>>(m, n));
    auto dst_ptr = make_mem_ptr<location::l0scaleb, fp8_e8m0_t>((reinterpret_cast<uint64_t>(dst)) / 16);
    auto l0b_tensor = make_tensor(dst_ptr, make_frame_layout<nn_layout_ptn, AscendC::Std::Int<2>>(m, n));

    run_copy_call_paths<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>(l0b_tensor, l1_tensor);
    run_copy_default_paths<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>(l0b_tensor, l1_tensor);

    EXPECT_EQ(dst[0], static_cast<fp8_e8m0_t>(0));
}

TEST_F(tensor_api_cube_copy_l1_to_l0scaleb_3510, copy_l1_to_l0scaleb_batch)
{
    using namespace asc::te;

    constexpr uint32_t batch = k_scaleb_batch;
    constexpr uint32_t m = k_scalebm;
    constexpr uint32_t n = k_scalebn;
    __cbuf__ fp8_e8m0_t src[batch * m * n] = {0};
    __cb__ fp8_e8m0_t dst[batch * m * n] = {0};

    auto base_layout = make_frame_layout<nn_layout_ptn, AscendC::Std::Int<2>>(m, n);
    using layout_trait_type = get_layout_trait<decltype(base_layout)>;
    auto batch_layout = make_batch_pattern_layout<nn_layout_ptn, layout_trait_type>(batch, base_layout);
    auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(src), batch_layout);
    auto dst_ptr = make_mem_ptr<location::l0scaleb, fp8_e8m0_t>((reinterpret_cast<uint64_t>(dst)) / 16);
    auto l0b_tensor = make_tensor(dst_ptr, batch_layout);

    g_expected_mx_dst_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
    g_expected_mx_src = reinterpret_cast<__cbuf__ void*>(src);
    g_scaleb_call_idx = 0;

    MOCKER_CPP(load_cbuf_to_cb_mx,
        void(uint64_t, __cbuf__ void*, uint16_t, uint16_t, uint8_t, uint8_t, uint16_t, uint16_t))
        .times(batch)
        .will(invoke(&load_cbuf_to_cb_mx_batch_stub));

    copy(copy_atom<copy_traits<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>>{}, l0b_tensor, l1_tensor);

    EXPECT_EQ(g_scaleb_call_idx, batch);
    mockcpp::GlobalMockObject::verify();
}

TEST_F(tensor_api_cube_copy_l1_to_l0scaleb_3510, copy_l1_to_l0scaleb_stride)
{
    using namespace asc::te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 32;
    __cbuf__ fp8_e8m0_t src[m * n] = {0};
    __cb__ fp8_e8m0_t dst[m * n] = {0};
    auto layout = make_frame_layout<nn_layout_ptn, AscendC::Std::Int<2>>(m, n);
    auto src_tensor = make_tensor(make_mem_ptr<location::l1>(src), layout);
    auto dst_ptr = make_mem_ptr<location::l0scaleb, fp8_e8m0_t>(reinterpret_cast<uint64_t>(dst) / 16);
    auto dst_tensor = make_tensor(dst_ptr, layout);

    g_expected_mx_dst_addr = reinterpret_cast<uint64_t>(dst) / 16;
    g_expected_mx_src = reinterpret_cast<__cbuf__ void*>(src + layout(make_coord(0, 16)));
    MOCKER_CPP(load_cbuf_to_cb_mx,
        void(uint64_t, __cbuf__ void*, uint16_t, uint16_t, uint8_t, uint8_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(&load_cbuf_to_cb_mx_stride_stub));

    copy(dst_tensor, src_tensor, make_coord(0, 0), make_coord(0, 16), make_shape(4, 16));
    mockcpp::GlobalMockObject::verify();
}
