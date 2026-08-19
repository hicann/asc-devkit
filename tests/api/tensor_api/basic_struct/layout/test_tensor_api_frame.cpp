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

class tensor_api_frame_layout : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_frame_layout, nd_and_dn_group_default_trait)
{
    using namespace asc::te;

    auto nd_layout = make_frame_layout<nd_layout_ptn>(8, 16);
    auto dn_layout = make_frame_layout<dn_layout_ptn>(8, 16);
    auto nd_ext_layout = make_frame_layout<nd_ext_layout_ptn>(8, 16);
    auto dn_ext_layout = make_frame_layout<dn_ext_layout_ptn>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nd_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(nd_layout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(dn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dn_layout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(dn_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(get_shape(nd_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape(nd_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_shape(nd_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride(nd_ext_layout))), 1);

    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(get_shape(dn_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape(dn_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_shape(dn_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape(dn_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride(dn_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride(dn_ext_layout))), 8);
}

TEST_F(tensor_api_frame_layout, batch_frame_layout_default_trait)
{
    using namespace asc::te;

    auto nd_layout = make_frame_layout<nd_layout_ptn>(2, 8, 16);
    auto dn_layout = make_frame_layout<dn_layout_ptn>(2, 8, 16);
    auto nd_ext_layout = make_frame_layout<nd_ext_layout_ptn>(2, 8, 16);
    auto dn_ext_layout = make_frame_layout<dn_ext_layout_ptn>(2, 8, 16);
    auto nz_layout = make_frame_layout<nz_layout_ptn>(2, 32, 64);
    auto zn_layout = make_frame_layout<zn_layout_ptn>(2, 32, 64);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nd_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nd_layout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nd_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride<1>(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride<1>(nd_layout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dn_layout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(dn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(dn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride<1>(dn_layout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride<1>(dn_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nd_ext_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nd_ext_layout)), 128);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape<1>(nd_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape<1>(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride<1>(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride<1>(nd_ext_layout))), 1);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dn_ext_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dn_ext_layout)), 128);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape<1>(dn_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape<1>(dn_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride<1>(dn_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride<1>(dn_ext_layout))), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nz_layout)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(get_shape<1>(nz_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape<1>(nz_layout))), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_shape<1>(nz_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape<1>(nz_layout))), 4);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(get_stride<1>(nz_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride<1>(nz_layout))), 256);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_stride<1>(nz_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride<1>(nz_layout))), 512);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(zn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(zn_layout)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(get_shape<1>(zn_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape<1>(zn_layout))), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_shape<1>(zn_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape<1>(zn_layout))), 4);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(get_stride<1>(zn_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride<1>(zn_layout))), 1024);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_stride<1>(zn_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride<1>(zn_layout))), 256);
}

TEST_F(tensor_api_frame_layout, batch_frame_layout_trait_forms)
{
    using namespace asc::te;

    auto nd_layout = make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(2, 8, 16);
    auto dn_layout = make_frame_layout<dn_layout_ptn, float>(2, 8, 16);
    auto nd_ext_layout = make_frame_layout<nd_ext_layout_ptn, 8>(2, 8, 16);
    auto dn_ext_layout = make_frame_layout<dn_ext_layout_ptn, layout_trait_default<float>>(2, 8, 16);
    auto nz_layout_with_trait = make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(2, 32, 64);
    auto nz_layout_with_data_type = make_frame_layout<nz_layout_ptn, float>(2, 32, 64);
    auto nz_layout_with_compat_trait = make_frame_layout<nz_layout_ptn, layout_trait<float, _8>>(2, 32, 64);
    auto zn_layout_with_c0 = make_frame_layout<zn_layout_ptn, 8>(2, 32, 64);
    using nz_trait_with_default = get_layout_trait<decltype(nz_layout_with_trait)>;
    using nz_trait_with_data_type = get_layout_trait<decltype(nz_layout_with_data_type)>;
    using nz_compat_trait = get_layout_trait<decltype(nz_layout_with_compat_trait)>;
    static_assert(nz_trait_with_default::c0_element == _8{}, "layout_trait_default<data_type> should only be used to infer c0_value.");
    static_assert(nz_trait_with_data_type::c0_element == _8{}, "Data type argument should only be used to infer c0_value.");
    static_assert(AscendC::Std::is_same_v<nz_compat_trait, layout_trait<float, _8>>,
        "layout_trait<data_type, c0_value> should remain source-compatible.");
    static_assert(nz_compat_trait::c0_element == _8{}, "layout_trait<data_type, c0_value> should use c0_value as layout trait.");

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nd_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nd_layout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dn_layout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nd_ext_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nd_ext_layout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dn_ext_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dn_ext_layout)), 128);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nz_layout_with_trait)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nz_layout_with_trait)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_shape<1>(nz_layout_with_trait))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape<1>(nz_layout_with_trait))), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nz_layout_with_data_type)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nz_layout_with_data_type)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_shape<1>(nz_layout_with_data_type))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape<1>(nz_layout_with_data_type))), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nz_layout_with_compat_trait)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nz_layout_with_compat_trait)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_shape<1>(nz_layout_with_compat_trait))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape<1>(nz_layout_with_compat_trait))), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(zn_layout_with_c0)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(zn_layout_with_c0)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(get_shape<1>(zn_layout_with_c0))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape<1>(zn_layout_with_c0))), 4);
}

TEST_F(tensor_api_frame_layout, nd_and_dn_group_trait_with_type)
{
    using namespace asc::te;

    auto nd_layout = make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(8, 16);
    auto dn_layout = make_frame_layout<dn_layout_ptn, layout_trait_default<float>>(8, 16);
    auto nd_ext_layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(8, 16);
    auto dn_ext_layout = make_frame_layout<dn_ext_layout_ptn, layout_trait_default<float>>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nd_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(nd_layout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(dn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dn_layout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(dn_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape(nd_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride(nd_ext_layout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape(dn_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape(dn_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride(dn_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride(dn_ext_layout))), 8);
}

TEST_F(tensor_api_frame_layout, nd_and_dn_group_trait_with_type_and_c0_element)
{
    using namespace asc::te;

    auto nd_layout = make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(8, 16);
    auto dn_layout = make_frame_layout<dn_layout_ptn, layout_trait_default<float>>(8, 16);
    auto nd_ext_layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(8, 16);
    auto dn_ext_layout = make_frame_layout<dn_ext_layout_ptn, layout_trait_default<float>>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nd_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(nd_layout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(dn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dn_layout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(dn_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape(nd_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride(nd_ext_layout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape(dn_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape(dn_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride(dn_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride(dn_ext_layout))), 8);
}

TEST_F(tensor_api_frame_layout, nd_and_dn_group_integral_constant_trait)
{
    using namespace asc::te;

    auto nd_layout = make_frame_layout<nd_layout_ptn, _16>(8, 16);
    auto dn_layout = make_frame_layout<dn_layout_ptn, _16>(8, 16);
    auto nd_ext_layout = make_frame_layout<nd_ext_layout_ptn, _16>(8, 16);
    auto dn_ext_layout = make_frame_layout<dn_ext_layout_ptn, _16>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nd_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nd_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(nd_layout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(dn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dn_layout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(dn_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape(nd_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride(nd_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride(nd_ext_layout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape(dn_ext_layout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape(dn_ext_layout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_stride(dn_ext_layout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_stride(dn_ext_layout))), 8);
}


TEST_F(tensor_api_frame_layout, scale_group_trait_with_type_and_c0_element)
{
    using namespace asc::te;

    auto and_layout = make_frame_layout<scalea_nd_layout_ptn>(32, 16);
    auto adn_layout = make_frame_layout<scalea_dn_layout_ptn>(32, 16);
    auto bnd_layout = make_frame_layout<scaleb_nd_layout_ptn>(16, 32);
    auto bdn_layout = make_frame_layout<scaleb_dn_layout_ptn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(and_layout)), 32);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(and_layout)), 16);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(adn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(adn_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(bnd_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(bnd_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(bdn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(bdn_layout)), 32);
}

TEST_F(tensor_api_frame_layout, scale_group_integral_constant_trait)
{
    using namespace asc::te;

    auto and_layout = make_frame_layout<scalea_nd_layout_ptn>(32, 16);
    auto adn_layout = make_frame_layout<scalea_dn_layout_ptn>(32, 16);
    auto bnd_layout = make_frame_layout<scaleb_nd_layout_ptn>(16, 32);
    auto bdn_layout = make_frame_layout<scaleb_dn_layout_ptn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(and_layout)), 32);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(and_layout)), 16);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(adn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(adn_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(bnd_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(bnd_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(bdn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(bdn_layout)), 32);
}

TEST_F(tensor_api_frame_layout, other_group_default_trait)
{
    using namespace asc::te;

    auto nz_layout = make_frame_layout<nz_layout_ptn>(32, 64);
    auto zn_layout = make_frame_layout<zn_layout_ptn>(32, 64);
    auto zz_layout = make_frame_layout<zz_layout_ptn>(32, 64);
    auto nn_layout = make_frame_layout<nn_layout_ptn, layout_trait<AscendC::Std::ignore_t, _2>>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nz_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zn_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zz_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nn_layout)), 2);
}

TEST_F(tensor_api_frame_layout, other_group_trait_with_type)
{
    using namespace asc::te;

    auto nz_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(32, 64);
    auto zn_layout = make_frame_layout<zn_layout_ptn, layout_trait_default<float>>(32, 64);
    auto zz_layout = make_frame_layout<zz_layout_ptn, layout_trait_default<float>>(32, 64);
    auto nn_layout = make_frame_layout<nn_layout_ptn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nz_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nz_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zn_layout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zn_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zz_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zz_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nn_layout)), 2);
}


TEST_F(tensor_api_frame_layout, other_group_with_data_type)
{
    using namespace asc::te;

    auto nz_layout = make_frame_layout<nz_layout_ptn, float>(32, 64);
    auto zn_layout = make_frame_layout<zn_layout_ptn, float>(32, 64);
    auto zz_layout = make_frame_layout<zz_layout_ptn, float>(32, 64);
    auto nn_layout = make_frame_layout<nn_layout_ptn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nz_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nz_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zn_layout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zn_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zz_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zz_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nn_layout)), 2);
}


TEST_F(tensor_api_frame_layout, other_group_with_c0_element)
{
    using namespace asc::te;

    auto nz_layout = make_frame_layout<nz_layout_ptn, 8>(32, 64);
    auto zn_layout = make_frame_layout<zn_layout_ptn, 8>(32, 64);
    auto zz_layout = make_frame_layout<zz_layout_ptn, 8>(32, 64);
    auto nn_layout = make_frame_layout<nn_layout_ptn, 2>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nz_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nz_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zn_layout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zn_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zz_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zz_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nn_layout)), 2);
}

TEST_F(tensor_api_frame_layout, other_group_trait_with_type_and_c0_element)
{
    using namespace asc::te;

    auto nz_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(32, 64);
    auto zn_layout = make_frame_layout<zn_layout_ptn, layout_trait_default<float>>(32, 64);
    auto zz_layout = make_frame_layout<zz_layout_ptn, layout_trait_default<float>>(32, 64);
    auto nn_layout = make_frame_layout<nn_layout_ptn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nz_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nz_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zn_layout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zn_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zz_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zz_layout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nn_layout)), 2);
}

TEST_F(tensor_api_frame_layout, other_group_integral_constant_trait)
{
    using namespace asc::te;

    auto nz_layout = make_frame_layout<nz_layout_ptn, _16>(32, 64);
    auto zn_layout = make_frame_layout<zn_layout_ptn, _16>(32, 64);
    auto zz_layout = make_frame_layout<zz_layout_ptn, _16>(32, 64);
    auto nn_layout = make_frame_layout<nn_layout_ptn, _2>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nz_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zn_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(zz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(zz_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(zz_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(zz_layout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape<0>(nn_layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<0>(nn_layout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape<1>(nn_layout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape<1>(nn_layout)), 2);
}

TEST_F(tensor_api_frame_layout, scale_layouts_batch_frame_layout)
{
    using namespace asc::te;

    constexpr int batch = 2;
    auto zz = make_frame_layout<zz_layout_ptn, 2>(batch, 32, 64);
    auto nn = make_frame_layout<nn_layout_ptn, 2>(batch, 16, 32);
    auto scalea_nd = make_frame_layout<scalea_nd_layout_ptn, 2>(batch, 32, 16);
    auto scalea_dn = make_frame_layout<scalea_dn_layout_ptn, 2>(batch, 32, 16);
    auto scaleb_nd = make_frame_layout<scaleb_nd_layout_ptn, 2>(batch, 16, 32);
    auto scaleb_dn = make_frame_layout<scaleb_dn_layout_ptn, 2>(batch, 16, 32);

    auto zz_base = make_frame_layout<zz_layout_ptn, 2>(32, 64);
    auto nn_base = make_frame_layout<nn_layout_ptn, 2>(16, 32);
    auto scalea_nd_base = make_frame_layout<scalea_nd_layout_ptn, 2>(32, 16);
    auto scalea_dn_base = make_frame_layout<scalea_dn_layout_ptn, 2>(32, 16);
    auto scaleb_nd_base = make_frame_layout<scaleb_nd_layout_ptn, 2>(16, 32);
    auto scaleb_dn_base = make_frame_layout<scaleb_dn_layout_ptn, 2>(16, 32);

    // Batch dim shape == batch, batch stride == single-matrix Capacity, for every pattern.
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(zz)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(zz)), capacity(zz_base));
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nn)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nn)), capacity(nn_base));
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(scalea_nd)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(scalea_nd)), capacity(scalea_nd_base));
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(scalea_dn)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(scalea_dn)), capacity(scalea_dn_base));
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(scaleb_nd)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(scaleb_nd)), capacity(scaleb_nd_base));
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(scaleb_dn)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(scaleb_dn)), capacity(scaleb_dn_base));

    // Inner fractal shape unchanged vs the non-batch ZZ(32,64,c0=2) layout: ((16,2),(2,32)).
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(get_shape<1>(zz))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(get_shape<1>(zz))), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(get_shape<1>(zz))), 2);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(get_shape<1>(zz))), 32);
}

TEST_F(tensor_api_frame_layout, conv_feature_map_layouts)
{
    using namespace asc::te;

    // NCHW(n_value,C,H,W): row-major contiguous stride (C*H*W, H*W, W, 1).
    auto nchw = make_frame_layout<nchw_layout_ptn>(2, 3, 4, 5);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nchw)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(nchw)), 3);
    EXPECT_EQ(AscendC::Std::get<2>(get_shape(nchw)), 4);
    EXPECT_EQ(AscendC::Std::get<3>(get_shape(nchw)), 5);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nchw)), 60);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(nchw)), 20);
    EXPECT_EQ(AscendC::Std::get<2>(get_stride(nchw)), 5);
    EXPECT_EQ(AscendC::Std::get<3>(get_stride(nchw)), 1);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nchw)>, nchw_layout_ptn>);

    // NHWC(n_value,H,W,C): row-major contiguous stride (H*W*C, W*C, C, 1).
    auto nhwc = make_frame_layout<nhwc_layout_ptn>(2, 4, 5, 3);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nhwc)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(nhwc)), 4);
    EXPECT_EQ(AscendC::Std::get<2>(get_shape(nhwc)), 5);
    EXPECT_EQ(AscendC::Std::get<3>(get_shape(nhwc)), 3);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nhwc)), 60);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(nhwc)), 15);
    EXPECT_EQ(AscendC::Std::get<2>(get_stride(nhwc)), 3);
    EXPECT_EQ(AscendC::Std::get<3>(get_stride(nhwc)), 1);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nhwc)>, nhwc_layout_ptn>);

    // NC1HWC0(n_value,C1,H,W,c0_value): c0_value supplied by caller, row-major contiguous stride.
    auto nc1hwc0 = make_frame_layout<nc1hwc0_layout_ptn>(2, 3, 4, 5, 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nc1hwc0)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(nc1hwc0)), 3);
    EXPECT_EQ(AscendC::Std::get<2>(get_shape(nc1hwc0)), 4);
    EXPECT_EQ(AscendC::Std::get<3>(get_shape(nc1hwc0)), 5);
    EXPECT_EQ(AscendC::Std::get<4>(get_shape(nc1hwc0)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nc1hwc0)), 960);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(nc1hwc0)), 320);
    EXPECT_EQ(AscendC::Std::get<2>(get_stride(nc1hwc0)), 80);
    EXPECT_EQ(AscendC::Std::get<3>(get_stride(nc1hwc0)), 16);
    EXPECT_EQ(AscendC::Std::get<4>(get_stride(nc1hwc0)), 1);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nc1hwc0)>, nc1hwc0_layout_ptn>);
}

TEST_F(tensor_api_frame_layout, conv3d_feature_map_layouts)
{
    using namespace asc::te;

    // NCDHW(n_value,C,D,H,W): row-major contiguous stride (C*D*H*W, D*H*W, H*W, W, 1).
    auto ncdhw = make_frame_layout<ncdhw_layout_ptn>(2, 3, 4, 5, 6);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(ncdhw)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(ncdhw)), 3);
    EXPECT_EQ(AscendC::Std::get<2>(get_shape(ncdhw)), 4);
    EXPECT_EQ(AscendC::Std::get<3>(get_shape(ncdhw)), 5);
    EXPECT_EQ(AscendC::Std::get<4>(get_shape(ncdhw)), 6);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(ncdhw)), 360); // 3*4*5*6
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(ncdhw)), 120); // 4*5*6
    EXPECT_EQ(AscendC::Std::get<2>(get_stride(ncdhw)), 30);  // 5*6
    EXPECT_EQ(AscendC::Std::get<3>(get_stride(ncdhw)), 6);   // 6
    EXPECT_EQ(AscendC::Std::get<4>(get_stride(ncdhw)), 1);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(ncdhw)>, ncdhw_layout_ptn>);

    // NDC1HWC0(n_value,D,C1,H,W,c0_value): c0_value supplied by caller, row-major contiguous stride.
    auto ndc1hwc0 = make_frame_layout<ndc1hwc0_layout_ptn>(2, 4, 3, 5, 6, 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(ndc1hwc0)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(ndc1hwc0)), 4);
    EXPECT_EQ(AscendC::Std::get<2>(get_shape(ndc1hwc0)), 3);
    EXPECT_EQ(AscendC::Std::get<3>(get_shape(ndc1hwc0)), 5);
    EXPECT_EQ(AscendC::Std::get<4>(get_shape(ndc1hwc0)), 6);
    EXPECT_EQ(AscendC::Std::get<5>(get_shape(ndc1hwc0)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(ndc1hwc0)), 5760); // 4*3*5*6*16
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(ndc1hwc0)), 1440); // 3*5*6*16
    EXPECT_EQ(AscendC::Std::get<2>(get_stride(ndc1hwc0)), 480);  // 5*6*16
    EXPECT_EQ(AscendC::Std::get<3>(get_stride(ndc1hwc0)), 96);   // 6*16
    EXPECT_EQ(AscendC::Std::get<4>(get_stride(ndc1hwc0)), 16);   // 16
    EXPECT_EQ(AscendC::Std::get<5>(get_stride(ndc1hwc0)), 1);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(ndc1hwc0)>, ndc1hwc0_layout_ptn>);
}

// make_frame_layout(batch0, ..., batch_n, row, col): the batch axes are flat -- they sit side by side in
// the outermost tuple with the base (row, col) block as the last element, giving rank batch_num + 1:
//   (batch0, batch1, (row, col))
// Batch strides are row-major over the base block: the last batch axis steps by the base capacity and
// each axis to its left multiplies in the extents to its right.
TEST_F(tensor_api_frame_layout, multi_batch_frame_layout_two_batches)
{
    using namespace asc::te;

    // ND base (8,16): capacity 128. Flat shape (2, 3, (8,16)), strides (384, 128, (16,1)).
    auto layout = make_frame_layout<nd_layout_ptn>(2, 3, 8, 16);

    static_assert(decltype(layout)::rank_size == 3, "two batch axes + base block are flat");

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(layout)), 3);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(layout)), 384); // 3 * 128
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(layout)), 128); // base capacity 8*16

    // Last element is the plain ND base block, untouched.
    auto inner_shape = AscendC::Std::get<2>(get_shape(layout));
    auto inner_stride = AscendC::Std::get<2>(get_stride(layout));
    EXPECT_EQ(AscendC::Std::get<0>(inner_shape), 8);
    EXPECT_EQ(AscendC::Std::get<1>(inner_shape), 16);
    EXPECT_EQ(AscendC::Std::get<0>(inner_stride), 16);
    EXPECT_EQ(AscendC::Std::get<1>(inner_stride), 1);

    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(layout)>, nd_layout_ptn>);
}

TEST_F(tensor_api_frame_layout, multi_batch_frame_layout_three_batches)
{
    using namespace asc::te;

    // base (8,16) capacity 128. Flat shape (2, 3, 4, (8,16)), strides (1536, 512, 128, (16,1)).
    auto layout = make_frame_layout<nd_layout_ptn>(2, 3, 4, 8, 16);

    static_assert(decltype(layout)::rank_size == 4, "three batch axes + base block are flat");

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(layout)), 3);
    EXPECT_EQ(AscendC::Std::get<2>(get_shape(layout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(layout)), 1536); // 3 * 4 * 128
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(layout)), 512);  // 4 * 128
    EXPECT_EQ(AscendC::Std::get<2>(get_stride(layout)), 128);  // base capacity
}

// Flat batch axes with a fractal base: the last element stays the untouched NZ block
// ((row0,row1),(col0,col1)), so the result is (batch0, batch1, ((16,2),(16,4))).
TEST_F(tensor_api_frame_layout, multi_batch_frame_layout_fractal)
{
    using namespace asc::te;

    // NZ(32,64) with default trait: single-matrix capacity 2048 (see batch_frame_layout_default_trait).
    auto layout = make_frame_layout<nz_layout_ptn>(2, 3, 32, 64);

    static_assert(decltype(layout)::rank_size == 3);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(layout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(layout)), 3);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(layout)), 6144); // 3 * 2048
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(layout)), 2048); // base capacity

    // Base block keeps the NZ fractal nesting ((16, m_value/16), (c0_value, n_value/c0_value)).
    auto base_shape = AscendC::Std::get<2>(get_shape(layout));
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(base_shape)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(base_shape)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(base_shape)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(base_shape)), 4);

    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(layout)>, nz_layout_ptn>);
}

// Regression guard: adding the multi-batch overload must not change the existing arities -- 3 args is
// still single-batch, and the conv feature-map patterns keep their fixed positional meaning (their
// 4/5-arg Make is not a batch form).
TEST_F(tensor_api_frame_layout, multi_batch_frame_layout_keeps_existing_arities)
{
    using namespace asc::te;

    // 3 args = single batch, unchanged.
    auto single = make_frame_layout<nd_layout_ptn>(2, 8, 16);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(single)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(single)), 128);

    // 4 args on NCHW stays (n_value, C, H, W), not (batch0, batch1, row, col).
    auto nchw_keep = make_frame_layout<nchw_layout_ptn>(2, 3, 4, 5);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(nchw_keep)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(nchw_keep)), 3);
    EXPECT_EQ(AscendC::Std::get<2>(get_shape(nchw_keep)), 4);
    EXPECT_EQ(AscendC::Std::get<3>(get_shape(nchw_keep)), 5);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(nchw_keep)), 60);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nchw_keep)>, nchw_layout_ptn>);

    // 5 args on NC1HWC0 stays (n_value, C1, H, W, c0_value).
    auto nc1hwc0_keep = make_frame_layout<nc1hwc0_layout_ptn>(2, 3, 4, 5, 16);
    EXPECT_EQ(AscendC::Std::get<4>(get_shape(nc1hwc0_keep)), 16);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nc1hwc0_keep)>, nc1hwc0_layout_ptn>);
}
