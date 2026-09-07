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

struct legacy_copy_call_operation {
    template <typename Trait, const Trait& trait, typename... Args>
    static void Copy(const Args&...)
    {
        called = true;
    }

    static inline bool called = false;
};

struct legacy_mmad_call_operation {
    template <typename Trait, const Trait& trait, typename... Args>
    static void mmad(const Args&...)
    {
        called = true;
    }

    static inline bool called = false;
};

class tensor_api_legacy : public testing::Test {};

TEST_F(tensor_api_legacy, normalize_legacy_enums)
{
    using namespace AscendC::Te;

    EXPECT_EQ(normalize_cache_mode(CacheMode::CACHE_MODE_NORMAL), asc::te::cache_mode::normal);
    EXPECT_EQ(normalize_cache_mode(CacheMode::CACHE_MODE_DISABLE), asc::te::cache_mode::disable);
    EXPECT_EQ(normalize_cache_mode(CacheMode::CACHE_MODE_LAST), asc::te::cache_mode::last);
    EXPECT_EQ(normalize_cache_mode(CacheMode::CACHE_MODE_PERSISTENT), asc::te::cache_mode::persistent);
    EXPECT_EQ(normalize_cache_mode(asc::te::cache_mode::last), asc::te::cache_mode::last);

    EXPECT_EQ(normalize_mmad_type(MmadType::NORMAL), asc::te::mmad_type::normal);
    EXPECT_EQ(normalize_mmad_type(MmadType::MX), asc::te::mmad_type::mx);
    EXPECT_EQ(normalize_mmad_type(asc::te::mmad_type::mx), asc::te::mmad_type::mx);

    EXPECT_EQ(normalize_round_mode(RoundMode::DEFAULT), asc::te::round_mode::default_round);
    EXPECT_EQ(normalize_round_mode(RoundMode::HYBRID), asc::te::round_mode::hybrid);
    EXPECT_EQ(normalize_round_mode(asc::te::round_mode::hybrid), asc::te::round_mode::hybrid);

    EXPECT_EQ(normalize_dual_dst_mode(DUAL_DST_DISABLE), asc::te::dual_dst_mode::disable);
    EXPECT_EQ(normalize_dual_dst_mode(DUAL_DST_SPLIT_M), asc::te::dual_dst_mode::split_m);
    EXPECT_EQ(normalize_dual_dst_mode(DUAL_DST_SPLIT_N), asc::te::dual_dst_mode::split_n);
    EXPECT_EQ(normalize_dual_dst_mode(asc::te::dual_dst_mode::split_n), asc::te::dual_dst_mode::split_n);
}

TEST_F(tensor_api_legacy, normalize_legacy_traits)
{
    using namespace AscendC::Te;

    auto legacy_mmad_trait = normalize_mmad_trait(MmadTrait{16, true, true, false, MmadType::MX});
    EXPECT_EQ(legacy_mmad_trait.fm_offset, 16);
    EXPECT_TRUE(legacy_mmad_trait.k_direction_align);
    EXPECT_TRUE(legacy_mmad_trait.init_with_btbuf);
    EXPECT_FALSE(legacy_mmad_trait.disable_gemv);
    EXPECT_EQ(legacy_mmad_trait.mmad_type, asc::te::mmad_type::mx);

    asc::te::mmad_trait snake_mmad_trait{8, false, true, true, asc::te::mmad_type::normal};
    auto normalized_snake_mmad_trait = normalize_mmad_trait(snake_mmad_trait);
    EXPECT_EQ(normalized_snake_mmad_trait.fm_offset, 8);
    EXPECT_TRUE(normalized_snake_mmad_trait.init_with_btbuf);

    auto gm_trait = normalize_copy_trait(CopyL0C2GMTrait{RoundMode::HYBRID, true, false});
    EXPECT_EQ(gm_trait.round_mode, asc::te::round_mode::hybrid);
    EXPECT_TRUE(gm_trait.enable_relu);
    EXPECT_FALSE(gm_trait.enable_channel_split);

    auto ub_trait = normalize_copy_trait(CopyL0C2UBTrait{RoundMode::HYBRID, true, true, DUAL_DST_SPLIT_N});
    EXPECT_EQ(ub_trait.round_mode, asc::te::round_mode::hybrid);
    EXPECT_TRUE(ub_trait.enable_relu);
    EXPECT_TRUE(ub_trait.enable_channel_split);
    EXPECT_EQ(ub_trait.dual_dst_ctl, asc::te::dual_dst_mode::split_n);

    auto l1_trait = normalize_copy_trait(CopyL0C2L1Trait{RoundMode::DEFAULT, false, true});
    EXPECT_EQ(l1_trait.round_mode, asc::te::round_mode::default_round);
    EXPECT_FALSE(l1_trait.enable_relu);
    EXPECT_TRUE(l1_trait.enable_channel_split);

    asc::te::l0c_to_gm_trait snake_copy_trait{asc::te::round_mode::hybrid, false, true};
    auto normalized_snake_copy_trait = normalize_copy_trait(snake_copy_trait);
    EXPECT_EQ(normalized_snake_copy_trait.round_mode, asc::te::round_mode::hybrid);
    EXPECT_TRUE(normalized_snake_copy_trait.enable_channel_split);
}

TEST_F(tensor_api_legacy, normalize_legacy_params)
{
    using namespace AscendC::Te;

    auto legacy_mmad_params = normalize_mmad_params(MmadParams{16, 32, 64, 3, true});
    EXPECT_EQ(legacy_mmad_params.m, 16);
    EXPECT_EQ(legacy_mmad_params.n, 32);
    EXPECT_EQ(legacy_mmad_params.k, 64);
    EXPECT_EQ(legacy_mmad_params.unit_flag, asc::te::unit_flag_mode::enable_update);
    EXPECT_TRUE(legacy_mmad_params.init_with_zero);

    asc::te::mmad_params snake_mmad_params{8, 16, 32, asc::te::unit_flag_mode::enable_keep, false};
    EXPECT_EQ(normalize_mmad_params(snake_mmad_params).unit_flag, asc::te::unit_flag_mode::enable_keep);

    FixpipeParams fixpipe_params{2, true};
    EXPECT_EQ(normalize_l0c_to_gm_params(fixpipe_params).unit_flag, asc::te::unit_flag_mode::enable_keep);
    EXPECT_EQ(normalize_l0c_to_ub_params(fixpipe_params).sub_block_id, 1);
    EXPECT_EQ(normalize_l0c_to_l1_params(fixpipe_params).unit_flag, asc::te::unit_flag_mode::enable_keep);

    asc::te::l0c_to_gm_params snake_gm_params{asc::te::unit_flag_mode::enable_update};
    asc::te::l0c_to_ub_params snake_ub_params{asc::te::unit_flag_mode::enable_update, 1};
    asc::te::l0c_to_l1_params snake_l1_params{asc::te::unit_flag_mode::enable_update};
    EXPECT_EQ(normalize_l0c_to_gm_params(snake_gm_params).unit_flag, asc::te::unit_flag_mode::enable_update);
    EXPECT_EQ(normalize_l0c_to_ub_params(snake_ub_params).sub_block_id, 1);
    EXPECT_EQ(normalize_l0c_to_l1_params(snake_l1_params).unit_flag, asc::te::unit_flag_mode::enable_update);

    CopyGM2UBParams legacy_ub_params{1, 2, false};
    auto normalized_ub_params = normalize_gm_to_ub_params(legacy_ub_params);
    EXPECT_EQ(normalized_ub_params.left_padding_count, 1);
    EXPECT_EQ(normalized_ub_params.right_padding_count, 2);
    EXPECT_FALSE(normalized_ub_params.enable_constant_pad);

    asc::te::gm_to_ub_params snake_vector_params{3, 4, true};
    EXPECT_EQ(normalize_gm_to_ub_params(snake_vector_params).left_padding_count, 3);
}

TEST_F(tensor_api_legacy, normalize_legacy_img2col_params)
{
    using namespace AscendC::Te;

    Img2ColParams<float> params;
    params.mExtension = 16;
    params.kExtension = 32;
    params.mStartPt = 2;
    params.kStartPt = 4;
    params.padList[0] = 1;
    params.padList[1] = 2;
    params.padList[2] = 3;
    params.padList[3] = 4;
    params.strideW = 2;
    params.strideH = 3;
    params.filterW = 5;
    params.filterH = 7;
    params.dilationFilterW = 2;
    params.dilationFilterH = 4;
    params.filterSizeW = true;
    params.filterSizeH = true;
    params.transpose = true;
    params.fMatrixCtrl = true;
    params.padValue = 1.5F;

    auto normalized = normalize_img2col_params(params);
    EXPECT_EQ(normalized.m_extension, 16);
    EXPECT_EQ(normalized.k_extension, 32);
    EXPECT_EQ(normalized.m_start_pos, 2);
    EXPECT_EQ(normalized.k_start_pos, 4);
    EXPECT_EQ(normalized.pad_list[0], 1);
    EXPECT_EQ(normalized.pad_list[1], 2);
    EXPECT_EQ(normalized.pad_list[2], 3);
    EXPECT_EQ(normalized.pad_list[3], 4);
    EXPECT_EQ(normalized.stride_w, 2);
    EXPECT_EQ(normalized.stride_h, 3);
    EXPECT_EQ(normalized.filter_w, 5);
    EXPECT_EQ(normalized.filter_h, 7);
    EXPECT_EQ(normalized.dilation_filter_w, 2);
    EXPECT_EQ(normalized.dilation_filter_h, 4);
    EXPECT_TRUE(normalized.enable_filter_w_extend);
    EXPECT_TRUE(normalized.enable_filter_h_extend);
    EXPECT_TRUE(normalized.enable_transpose);
    EXPECT_TRUE(normalized.enable_f_matrix_ctrl);
    EXPECT_FLOAT_EQ(normalized.pad_value, 1.5F);
}

TEST_F(tensor_api_legacy, bind_legacy_atom_params)
{
    using namespace AscendC::Te;

    auto copy_atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{3});
    EXPECT_EQ(copy_atom.params.unit_flag, asc::te::unit_flag_mode::enable_update);

    auto img2col_atom = MakeCopy(CopyL12L0A{}).with(Img2ColParams<float>{});
    EXPECT_EQ(img2col_atom.params.stride_w, 1);
    EXPECT_EQ(img2col_atom.params.stride_h, 1);

    auto mmad_atom = MakeMmad(MmadOperation{}).with(MmadParams{16, 32, 64, 2, true});
    EXPECT_EQ(mmad_atom.params.m, 16);
    EXPECT_EQ(mmad_atom.params.unit_flag, asc::te::unit_flag_mode::enable_keep);
    EXPECT_TRUE(mmad_atom.params.init_with_zero);
}

TEST_F(tensor_api_legacy, call_legacy_copy_atom)
{
    using namespace AscendC::Te;
    using copy_traits_type = CopyTraits<legacy_copy_call_operation, asc::te::gm_to_l1_trait_default>;

    legacy_copy_call_operation::called = false;
    CopyAtom<copy_traits_type>{}.Call();

    EXPECT_TRUE(legacy_copy_call_operation::called);
}

TEST_F(tensor_api_legacy, call_legacy_mmad_atom)
{
    using namespace AscendC::Te;
    using mmad_traits_type = MmadTraits<legacy_mmad_call_operation, asc::te::mmad_trait_default>;

    legacy_mmad_call_operation::called = false;
    MmadAtom<mmad_traits_type>{}.Call();

    EXPECT_TRUE(legacy_mmad_call_operation::called);
}
