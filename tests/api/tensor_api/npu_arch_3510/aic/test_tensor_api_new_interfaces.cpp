/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express OR IMPLIED.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/tensor.h"

class tensor_api_new_interfaces_aic : public testing::Test {
protected:
    void SetUp() override { AscendC::SetGCoreType(1); }
    void TearDown() override { AscendC::SetGCoreType(0); }
};

namespace {

asc_fill_value_config g_fill_l1_config_capture;

void asc_fill_l1_stub(__cbuf__ half* dst, half value, const asc_fill_value_config& config)
{
    (void)dst;
    (void)value;
    g_fill_l1_config_capture = config;
}

asc_hf32_round_mode g_hf32_round_mode_capture = asc_hf32_round_mode::NEAREST_AWAY;

void asc_set_hf32_round_mode_stub(asc_hf32_round_mode mode) { g_hf32_round_mode_capture = mode; }

int g_mmad_direction_capture = 0;

void asc_set_mmad_direction_m_stub() { g_mmad_direction_capture = 1; }

void asc_set_mmad_direction_n_stub() { g_mmad_direction_capture = 2; }

} // namespace

TEST_F(tensor_api_new_interfaces_aic, fill_l1_forwards_params_to_c_api)
{
    using namespace asc::te;

    constexpr uint32_t length = 32;
    __cbuf__ half dst[length] = {};

    auto tensor = make_tensor(
        make_mem_ptr<location::l1>(dst), make_frame_layout<nd_ext_layout_ptn, layout_trait_default<half>>(1, length));

    fill_params<half> params{};
    params.repeat_times = 2;
    params.block_num = 3;
    params.dst_gap = 4;
    params.init_value = static_cast<half>(5);

    MOCKER_CPP(asc_fill_l1, void(__cbuf__ half*, half, const asc_fill_value_config&))
        .times(1)
        .will(invoke(asc_fill_l1_stub));

    fill(tensor, params);

    GlobalMockObject::verify();
    EXPECT_EQ(g_fill_l1_config_capture.repeat, params.repeat_times);
    EXPECT_EQ(g_fill_l1_config_capture.blk_num, params.block_num);
    EXPECT_EQ(g_fill_l1_config_capture.dst_gap, params.dst_gap);
}

TEST_F(tensor_api_new_interfaces_aic, fill_l1_derives_params_from_layout)
{
    using namespace asc::te;

    constexpr uint32_t length = 32;
    __cbuf__ half dst[length] = {};

    auto tensor = make_tensor(
        make_mem_ptr<location::l1>(dst), make_frame_layout<nd_ext_layout_ptn, layout_trait_default<half>>(1, length));

    MOCKER_CPP(asc_fill_l1, void(__cbuf__ half*, half, const asc_fill_value_config&))
        .times(1)
        .will(invoke(asc_fill_l1_stub));

    fill(tensor, static_cast<half>(5));

    GlobalMockObject::verify();
    EXPECT_EQ(g_fill_l1_config_capture.repeat, 1);
    EXPECT_EQ(g_fill_l1_config_capture.blk_num, 2);
    EXPECT_EQ(g_fill_l1_config_capture.dst_gap, 0);
}

TEST_F(tensor_api_new_interfaces_aic, hf32_interfaces_forward_to_c_api)
{
    MOCKER_CPP(asc_enable_hf32, void(void)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_disable_hf32, void(void)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_set_hf32_round_mode, void(asc_hf32_round_mode)).times(2).will(invoke(asc_set_hf32_round_mode_stub));

    asc::te::enable_hf32();
    asc::te::disable_hf32();
    asc::te::set_hf32_round_mode(asc::te::hf32_round_mode::nearest_even);
    EXPECT_EQ(g_hf32_round_mode_capture, asc_hf32_round_mode::NEAREST_EVEN);
    asc::te::set_hf32_round_mode(asc::te::hf32_round_mode::nearest_away);

    GlobalMockObject::verify();
    EXPECT_EQ(g_hf32_round_mode_capture, asc_hf32_round_mode::NEAREST_AWAY);
}

TEST_F(tensor_api_new_interfaces_aic, set_mmad_direction_forwards_to_c_api)
{
    MOCKER_CPP(asc_set_mmad_direction_m, void(void)).times(1).will(invoke(asc_set_mmad_direction_m_stub));
    MOCKER_CPP(asc_set_mmad_direction_n, void(void)).times(1).will(invoke(asc_set_mmad_direction_n_stub));

    asc::te::set_mmad_direction(asc::te::mmad_direction::m);
    EXPECT_EQ(g_mmad_direction_capture, 1);
    asc::te::set_mmad_direction(asc::te::mmad_direction::n);
    EXPECT_EQ(g_mmad_direction_capture, 2);

    GlobalMockObject::verify();
}
