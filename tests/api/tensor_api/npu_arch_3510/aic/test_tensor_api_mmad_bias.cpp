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

class tensor_api_mmad_with_bias : public testing::Test {
protected:
    void SetUp()
    {
        AscendC::SetGCoreType(1);
    }
    void TearDown()
    {
        AscendC::SetGCoreType(0);
    }
};

using namespace asc::te;

#define MMAD_BIAS_ON_BIAS_INTERFACE_TEST(dst_type, src_type, bias_type, m_value, n_value, k_size)                                 \
    TEST_F(tensor_api_mmad_with_bias, mmad_interface_bias_on_bias_##dst_type##_##src_type##_##bias_type##_##m_value##_##n_value##_##k_size) \
    {                                                                                                              \
        uint8_t a2_buf[256 * 256 * sizeof(src_type)] = {0};                                                        \
        uint8_t b2_buf[256 * 256 * sizeof(src_type)] = {0};                                                        \
        uint8_t c2_buf[256 * 256 * sizeof(dst_type)] = {0};                                                        \
        uint8_t bias_buf[256 * 256 * sizeof(bias_type)] = {0};                                                     \
                                                                                                                   \
        auto a2_addr = reinterpret_cast<__ca__ src_type*>(a2_buf);                                                  \
        auto l0a_tensor = make_tensor(make_mem_ptr<location::l0a>(a2_addr),                                            \
                                    make_frame_layout<nz_layout_ptn, layout_trait_default<src_type>>(m_value, k_size));           \
                                                                                                                   \
        auto b2_addr = reinterpret_cast<__cb__ src_type*>(b2_buf);                                                  \
        auto l0b_tensor = make_tensor(make_mem_ptr<location::l0b>(b2_addr),                                            \
                                    make_frame_layout<zn_layout_ptn, layout_trait_default<src_type>>(k_size, n_value));           \
                                                                                                                   \
        auto c2_addr = reinterpret_cast<__cc__ dst_type*>(c2_buf);                                                  \
        auto l0c_tensor = make_tensor(make_mem_ptr<location::l0c>(c2_addr),                                            \
                                    make_frame_layout<nz_layout_ptn, layout_trait_default<>>(m_value, n_value));                   \
                                                                                                                   \
        auto bias_addr = reinterpret_cast<__biasbuf__ bias_type*>(bias_buf);                                        \
        auto bias_tensor = make_tensor(make_mem_ptr<location::bias>(bias_addr),                                        \
                                     make_frame_layout<nd_layout_ptn, layout_trait_default<bias_type>>(m_value, n_value));         \
                                                                                                                   \
        mmad_params para;                                                                                           \
        para.m = m_value;                                                                                                \
        para.n = n_value;                                                                                                \
        para.k = k_size;                                                                                                \
        para.unit_flag = 0;                                                                                         \
        para.cmatrix_init_val = false;                                                                              \
                                                                                                                   \
        auto mmad_atom = make_mmad(mmad_operation{}, mmad_trait_default{}).with(para);                                 \
        mmad(mmad_atom, l0c_tensor, l0a_tensor, l0b_tensor, bias_tensor);                                               \
        mmad(mmad_atom, l0c_tensor, l0a_tensor, l0b_tensor, bias_tensor);                                               \
        mmad(mmad_atom, l0c_tensor, l0a_tensor, l0b_tensor, bias_tensor);                                               \
        EXPECT_EQ(c2_addr[0], static_cast<dst_type>(0));                                                           \
    }

#define MMAD_BIAS_ON_L0C_INTERFACE_TEST(dst_type, src_type, bias_type, m_value, n_value, k_size)                                  \
    TEST_F(tensor_api_mmad_with_bias, mmad_interface_bias_on_l0c_##dst_type##_##src_type##_##bias_type##_##m_value##_##n_value##_##k_size) \
    {                                                                                                              \
        uint8_t a2_buf[256 * 256 * sizeof(src_type)] = {0};                                                        \
        uint8_t b2_buf[256 * 256 * sizeof(src_type)] = {0};                                                        \
        uint8_t c2_buf[256 * 256 * sizeof(dst_type)] = {0};                                                        \
        uint8_t bias_buf[256 * 256 * sizeof(bias_type)] = {0};                                                     \
                                                                                                                   \
        auto a2_addr = reinterpret_cast<__ca__ src_type*>(a2_buf);                                                  \
        auto l0a_tensor = make_tensor(make_mem_ptr<location::l0a>(a2_addr),                                            \
                                    make_frame_layout<nz_layout_ptn, layout_trait_default<src_type>>(m_value, k_size));           \
                                                                                                                   \
        auto b2_addr = reinterpret_cast<__cb__ src_type*>(b2_buf);                                                  \
        auto l0b_tensor = make_tensor(make_mem_ptr<location::l0b>(b2_addr),                                            \
                                    make_frame_layout<zn_layout_ptn, layout_trait_default<src_type>>(k_size, n_value));           \
                                                                                                                   \
        auto c2_addr = reinterpret_cast<__cc__ dst_type*>(c2_buf);                                                  \
        auto l0c_tensor = make_tensor(make_mem_ptr<location::l0c>(c2_addr),                                            \
                                    make_frame_layout<nz_layout_ptn, layout_trait_default<>>(m_value, n_value));                   \
                                                                                                                   \
        auto bias_addr = reinterpret_cast<__cc__ bias_type*>(bias_buf);                                             \
        auto bias_tensor = make_tensor(make_mem_ptr<location::l0c>(bias_addr),                                         \
                                     make_frame_layout<nz_layout_ptn, layout_trait_default<>>(m_value, n_value));                  \
                                                                                                                   \
        mmad_params para;                                                                                           \
        para.m = m_value;                                                                                                \
        para.n = n_value;                                                                                                \
        para.k = k_size;                                                                                                \
        para.unit_flag = 0;                                                                                         \
        para.cmatrix_init_val = false;                                                                              \
                                                                                                                   \
        auto mmad_atom = make_mmad(mmad_operation{}, mmad_trait_default{}).with(para);                                 \
        mmad(mmad_atom, l0c_tensor, l0a_tensor, l0b_tensor, bias_tensor);                                               \
        mmad(mmad_atom, l0c_tensor, l0a_tensor, l0b_tensor, bias_tensor);                                               \
        EXPECT_EQ(c2_addr[0], static_cast<dst_type>(0));                                                           \
    }

MMAD_BIAS_ON_BIAS_INTERFACE_TEST(float, float, float, 16, 16, 16);
MMAD_BIAS_ON_BIAS_INTERFACE_TEST(float, bfloat16_t, float, 16, 16, 16);
MMAD_BIAS_ON_BIAS_INTERFACE_TEST(float, half, float, 16, 16, 16);
MMAD_BIAS_ON_BIAS_INTERFACE_TEST(int32_t, int8_t, int32_t, 32, 32, 32);

MMAD_BIAS_ON_L0C_INTERFACE_TEST(float, float, float, 16, 16, 16);
MMAD_BIAS_ON_L0C_INTERFACE_TEST(float, bfloat16_t, float, 16, 16, 16);
MMAD_BIAS_ON_L0C_INTERFACE_TEST(float, half, float, 16, 16, 16);
MMAD_BIAS_ON_L0C_INTERFACE_TEST(int32_t, int8_t, int32_t, 32, 32, 32);
