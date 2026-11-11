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
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

TEST(E6m2VectorDatamove, LoadStoreScatter)
{
    auto* ub = reinterpret_cast<__ubuf__ fp8_e6m2_t*>(32);
    vector_fp8_e6m2_t data{};
    vector_uint16_t index{};
    vector_bool mask{};

    reset_e6m2_vector_datamove_stub_state();
    asc_loadalign(data, ub);
    asc_storealign(ub, data, mask);
    asc_scatter(ub, data, index, mask);

    EXPECT_EQ(g_e6m2_vector_datamove_stub_state.load_count, 1U);
    EXPECT_EQ(g_e6m2_vector_datamove_stub_state.store_count, 1U);
    EXPECT_EQ(g_e6m2_vector_datamove_stub_state.scatter_count, 1U);
}

TEST(E6m2VectorDatamove, AlignedLoads)
{
    auto* ub = reinterpret_cast<__ubuf__ fp8_e6m2_t*>(32);
    vector_fp8_e6m2_t data{};
    vector_fp8_e6m2_t other{};
    vector_bool mask{};
    addr_reg offset{};

    reset_e6m2_vector_datamove_stub_state();
    asc_loadalign(data, ub, 2);
    asc_loadalign(data, ub, offset);
    asc_loadalign(data, ub, 1, 2, mask);
    (void)asc_loadalign(ub);
    asc_loadalign_brc(data, ub);
    asc_loadalign_brc(data, ub, 2);
    asc_loadalign_brc(data, ub, offset);
    asc_loadalign_brc_datablock(data, ub);
    asc_loadalign_brc_datablock(data, ub, 2);
    asc_loadalign_brc_datablock(data, ub, offset);
    (void)asc_loadalign_brc_datablock(ub);
    asc_loadalign_brc_elem(data, ub);
    asc_loadalign_brc_elem(data, ub, 2);
    asc_loadalign_brc_elem(data, ub, offset);
    (void)asc_loadalign_brc_elem(ub);
    asc_loadalign_deintlv(data, other, ub);
    asc_loadalign_deintlv(data, other, ub, 2);
    asc_loadalign_deintlv(data, other, ub, offset);
    asc_loadalign_downsample(data, ub);
    asc_loadalign_downsample(data, ub, 2);
    asc_loadalign_downsample(data, ub, offset);
    (void)asc_loadalign_downsample(ub);
    asc_loadalign_unpack(data, ub);
    asc_loadalign_unpack(data, ub, 2);
    asc_loadalign_unpack(data, ub, offset);
    (void)asc_loadalign_unpack(ub);
    asc_loadalign_unpack4(data, ub);
    asc_loadalign_unpack4(data, ub, 2);
    asc_loadalign_unpack4(data, ub, offset);
    (void)asc_loadalign_unpack4(ub);
    asc_loadalign_upsample(data, ub);
    asc_loadalign_upsample(data, ub, 2);
    asc_loadalign_upsample(data, ub, offset);
    (void)asc_loadalign_upsample(ub);
    (void)asc_loadalign_datablock_strided(ub, 1, 2, mask);

    EXPECT_EQ(g_e6m2_vector_datamove_stub_state.load_count, 35U);
}

TEST(E6m2VectorDatamove, PostupdateLoads)
{
    auto* ub = reinterpret_cast<__ubuf__ fp8_e6m2_t*>(32);
    vector_fp8_e6m2_t data{};
    vector_fp8_e6m2_t other{};
    vector_bool mask{};

    reset_e6m2_vector_datamove_stub_state();
    asc_loadalign_postupdate(data, ub, 2);
    asc_loadalign_postupdate(data, ub, 1, 2, mask);
    asc_loadalign_brc_postupdate(data, ub, 2);
    asc_loadalign_brc_datablock_postupdate(data, ub, 2);
    asc_loadalign_brc_elem_postupdate(data, ub, 2);
    asc_loadalign_deintlv_postupdate(data, other, ub, 2);
    asc_loadalign_downsample_postupdate(data, ub, 2);
    asc_loadalign_unpack_postupdate(data, ub, 2);
    asc_loadalign_unpack4_postupdate(data, ub, 2);
    asc_loadalign_upsample_postupdate(data, ub, 2);

    EXPECT_EQ(g_e6m2_vector_datamove_stub_state.load_count, 10U);
}

TEST(E6m2VectorDatamove, UnalignedLoads)
{
    auto* ub = reinterpret_cast<__ubuf__ fp8_e6m2_t*>(32);
    vector_fp8_e6m2_t data{};
    vector_load_unalign align{};
    addr_reg offset{};

    reset_e6m2_vector_datamove_stub_state();
    asc_loadunalign_pre(align, ub);
    asc_loadunalign_pre(align, ub, offset);
    asc_loadunalign(data, align, ub);
    asc_loadunalign_postupdate(data, align, ub, 2);
    asc_loadunalign_postupdate(data, align, ub, offset, 2);

    EXPECT_EQ(g_e6m2_vector_datamove_stub_state.load_count, 5U);
}

TEST(E6m2VectorDatamove, AlignedStores)
{
    auto* ub = reinterpret_cast<__ubuf__ fp8_e6m2_t*>(32);
    vector_fp8_e6m2_t data{};
    vector_bool mask{};
    addr_reg offset{};

    reset_e6m2_vector_datamove_stub_state();
    asc_storealign(ub, data, mask);
    asc_storealign(ub, data, 2, mask);
    asc_storealign(ub, data, offset, mask);
    asc_storealign(ub, data, 1, 2, mask);
    asc_storealign_postupdate(ub, data, 1, 2, mask);
    asc_storealign_1st(ub, data);
    asc_storealign_1st(ub, data, 2);
    asc_storealign_1st(ub, data, offset);
    asc_storealign_1st_postupdate(ub, data, 2);
    asc_storealign_intlv(ub, data, data);
    asc_storealign_intlv(ub, data, data, 2);
    asc_storealign_intlv(ub, data, data, offset);

    EXPECT_EQ(g_e6m2_vector_datamove_stub_state.store_count, 12U);
}

TEST(E6m2VectorDatamove, UnalignedStores)
{
    auto* ub = reinterpret_cast<__ubuf__ fp8_e6m2_t*>(32);
    vector_fp8_e6m2_t data{};
    vector_store_unalign align{};
    addr_reg offset{};

    reset_e6m2_vector_datamove_stub_state();
    asc_storeunalign(ub, align, data, 2);
    asc_storeunalign_postupdate(ub, align, data, 2);
    asc_storeunalign_postupdate(ub, align, data, offset);
    asc_storeunalign_post(ub, align, 2);
    asc_storeunalign_post(ub, align, offset);
    asc_storeunalign_post_postupdate(ub, align, 2);

    EXPECT_EQ(g_e6m2_vector_datamove_stub_state.store_count, 6U);
}

TEST(E6m2VectorDatamove, AlignedCopies)
{
    auto* ub = reinterpret_cast<__ubuf__ fp8_e6m2_t*>(32);
    auto* gm = reinterpret_cast<__gm__ fp8_e6m2_t*>(64);

    reset_e6m2_vector_datamove_stub_state();
    reset_copy_gm_to_ubuf_align_v2_stub_state();
    reset_copy_ubuf_to_gm_align_v2_stub_state();
    asc_copy_gm2ub_align(ub, gm, 3, 64, 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 4, 5);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.call_count, 1U);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.src_stride, 4U);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.dst_stride, 5U);
    asc_copy_gm2ub_align(ub, gm, 64);
    asc_copy_ub2gm_align(gm, ub, 3, 64, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 4, 5);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.call_count, 1U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.dst_stride, 4U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.src_stride, 5U);
    asc_copy_ub2gm_align(gm, ub, 64);

    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.call_count, 2U);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.src_stride, 0U);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.dst_stride, 0U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.call_count, 2U);
}
