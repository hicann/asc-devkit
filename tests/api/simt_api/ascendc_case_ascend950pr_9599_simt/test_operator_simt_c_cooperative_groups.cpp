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
#include <type_traits>
#include "simt_api/device_functions.h"
#include "simt_api/asc_fp16.h"
#include "simt_api/asc_bf16.h"
#include "utils/debug/asc_assert.h"
#define __NPU_COMPILER_INTERNAL_PURE_SIMT__
#include "simt_api/cooperative_groups.h"
#undef __NPU_COMPILER_INTERNAL_PURE_SIMT__

using namespace std;
using namespace AscendC;
using namespace cooperative_groups;

namespace {
uint64_t* GetGridSyncCounter() { return reinterpret_cast<uint64_t*>(__cce_simt_get_para_base_imp() - 4U * 8U); }

constexpr uint64_t GridSyncPhaseBit = 1ULL << 63;
} // namespace

class CooperativeGroupsTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

class SimtDimGuard {
public:
    SimtDimGuard(
        cce::dim3 blockDimVal, cce::dim3 threadIdxVal, cce::dim3 blockIdxVal = cce::dim3(0u, 0u, 0u),
        cce::dim3 gridDimVal = cce::dim3(1u, 1u, 1u))
        : oldBlockDim_(blockDim), oldThreadIdx_(threadIdx), oldBlockIdx_(blockIdx), oldGridDim_(gridDim)
    {
        blockDim = blockDimVal;
        threadIdx = threadIdxVal;
        blockIdx = blockIdxVal;
        gridDim = gridDimVal;
    }

    ~SimtDimGuard()
    {
        blockDim = oldBlockDim_;
        threadIdx = oldThreadIdx_;
        blockIdx = oldBlockIdx_;
        gridDim = oldGridDim_;
    }

private:
    cce::dim3 oldBlockDim_;
    cce::dim3 oldThreadIdx_;
    cce::dim3 oldBlockIdx_;
    cce::dim3 oldGridDim_;
};

TEST_F(CooperativeGroupsTestsuite, GroupTypeTest)
{
    EXPECT_EQ(static_cast<unsigned int>(group_type::thread_block_type), 0u);
    EXPECT_EQ(static_cast<unsigned int>(group_type::tiled_group_type), 1u);
    EXPECT_EQ(static_cast<unsigned int>(group_type::coalesced_group_type), 2u);
    EXPECT_EQ(static_cast<unsigned int>(group_type::grid_group_type), 3u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupGetTypeTest)
{
    thread_group tg(group_type::thread_block_type);
    EXPECT_EQ(tg.get_type(), group_type::thread_block_type);

    thread_group tg2(group_type::coalesced_group_type);
    EXPECT_EQ(tg2.get_type(), group_type::coalesced_group_type);

    thread_group tg3(group_type::tiled_group_type);
    EXPECT_EQ(tg3.get_type(), group_type::tiled_group_type);
}

TEST_F(CooperativeGroupsTestsuite, GridGroupThisGridTest)
{
    grid_group gg = this_grid();
    EXPECT_EQ(gg.get_type(), group_type::grid_group_type);
    EXPECT_TRUE(gg.is_valid());
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupBaseClassGridGroupDispatchTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(7u, 3u, 1u), cce::dim3(3u, 2u, 1u), cce::dim3(4u, 3u, 2u));
    grid_group gg = this_grid();
    thread_group& tg = gg;

    EXPECT_EQ(tg.get_type(), group_type::grid_group_type);
    EXPECT_EQ(tg.size(), grid_group::size());
    EXPECT_EQ(tg.num_threads(), grid_group::num_threads());
    EXPECT_EQ(tg.thread_rank(), grid_group::thread_rank());
}

TEST_F(CooperativeGroupsTestsuite, GridGroupNumBlocksThreadsAndRank3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(7u, 3u, 1u), cce::dim3(3u, 2u, 1u), cce::dim3(4u, 3u, 2u));

    EXPECT_EQ(grid_group::num_blocks(), 24ull);
    EXPECT_EQ(grid_group::num_threads(), 1536ull);
    EXPECT_EQ(grid_group::size(), 1536ull);
    EXPECT_EQ(grid_group::block_rank(), 23ull);
    EXPECT_EQ(grid_group::thread_rank(), 1535ull);
}

TEST_F(CooperativeGroupsTestsuite, GridGroupDimBlocksAndBlockIndex3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(0u, 0u, 0u), cce::dim3(3u, 2u, 1u), cce::dim3(4u, 3u, 2u));

    dim3 dimBlocks = grid_group::dim_blocks();
    EXPECT_EQ(dimBlocks.x, 4u);
    EXPECT_EQ(dimBlocks.y, 3u);
    EXPECT_EQ(dimBlocks.z, 2u);

    dim3 groupDim = grid_group::group_dim();
    EXPECT_EQ(groupDim.x, 4u);
    EXPECT_EQ(groupDim.y, 3u);
    EXPECT_EQ(groupDim.z, 2u);

    dim3 blockIndex = grid_group::block_index();
    EXPECT_EQ(blockIndex.x, 3u);
    EXPECT_EQ(blockIndex.y, 2u);
    EXPECT_EQ(blockIndex.z, 1u);
}

TEST_F(CooperativeGroupsTestsuite, GridGroupSyncSingleBlockFlipsCounterPhaseTest)
{
    SimtDimGuard guard(cce::dim3(8u, 1u, 1u), cce::dim3(0u, 0u, 0u), cce::dim3(0u, 0u, 0u), cce::dim3(1u, 1u, 1u));
    GetGridSyncCounter()[0] = 123u;

    sync(this_grid());

    EXPECT_EQ(GetGridSyncCounter()[0], GridSyncPhaseBit + 123u);
}

TEST_F(CooperativeGroupsTestsuite, GridGroupSyncBlockZeroReleasesCounterTest)
{
    SimtDimGuard guard(cce::dim3(8u, 1u, 1u), cce::dim3(0u, 0u, 0u), cce::dim3(0u, 0u, 0u), cce::dim3(2u, 1u, 1u));
    GetGridSyncCounter()[0] = 1u;

    this_grid().sync();

    EXPECT_EQ(GetGridSyncCounter()[0], GridSyncPhaseBit);
}

TEST_F(CooperativeGroupsTestsuite, GridGroupSyncNonZeroBlockReleasesCounterTest)
{
    SimtDimGuard guard(cce::dim3(8u, 1u, 1u), cce::dim3(0u, 0u, 0u), cce::dim3(1u, 0u, 0u), cce::dim3(2u, 1u, 1u));
    GetGridSyncCounter()[0] = GridSyncPhaseBit - 1ULL;

    this_grid().sync();

    EXPECT_EQ(GetGridSyncCounter()[0], GridSyncPhaseBit);
}

TEST_F(CooperativeGroupsTestsuite, GridGroupSyncBlockZeroReleasesCounterForThreeBlocksTest)
{
    SimtDimGuard guard(cce::dim3(8u, 1u, 1u), cce::dim3(0u, 0u, 0u), cce::dim3(0u, 0u, 0u), cce::dim3(3u, 1u, 1u));
    GetGridSyncCounter()[0] = 2u;

    this_grid().sync();

    EXPECT_EQ(GetGridSyncCounter()[0], GridSyncPhaseBit);
}

TEST_F(CooperativeGroupsTestsuite, GridGroupSyncBlockZeroReleasesNextPhaseTest)
{
    SimtDimGuard guard(cce::dim3(8u, 1u, 1u), cce::dim3(0u, 0u, 0u), cce::dim3(0u, 0u, 0u), cce::dim3(2u, 1u, 1u));
    GetGridSyncCounter()[0] = GridSyncPhaseBit + 1ULL;

    this_grid().sync();

    EXPECT_EQ(GetGridSyncCounter()[0], 0u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockThisThreadBlockTest)
{
    thread_block tb = this_thread_block();
    EXPECT_EQ(tb.get_type(), group_type::thread_block_type);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockSyncTest) { thread_block::sync(); }

TEST_F(CooperativeGroupsTestsuite, OuterSyncThreadBlockTest)
{
    thread_block tb = this_thread_block();
    sync(tb);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockThreadRankTest)
{
    unsigned int rank = thread_block::thread_rank();
    EXPECT_GE(rank, 0u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockGroupIndexTest)
{
    dim3 gi = thread_block::group_index();
    EXPECT_GE(gi.x, 0u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockGroupIndex3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(0u, 0u, 0u), cce::dim3(3u, 2u, 1u), cce::dim3(4u, 3u, 2u));
    dim3 gi = thread_block::group_index();
    EXPECT_EQ(gi.x, 3u);
    EXPECT_EQ(gi.y, 2u);
    EXPECT_EQ(gi.z, 1u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockThreadIndexTest)
{
    dim3 ti = thread_block::thread_index();
    EXPECT_GE(ti.x, 0u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockThreadIndex3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(7u, 3u, 1u));
    dim3 ti = thread_block::thread_index();
    EXPECT_EQ(ti.x, 7u);
    EXPECT_EQ(ti.y, 3u);
    EXPECT_EQ(ti.z, 1u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockDimThreadsTest)
{
    dim3 dt = thread_block::dim_threads();
    EXPECT_GT(dt.x, 0u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockDimThreads3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(0u, 0u, 0u));
    dim3 dt = thread_block::dim_threads();
    EXPECT_EQ(dt.x, 8u);
    EXPECT_EQ(dt.y, 4u);
    EXPECT_EQ(dt.z, 2u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockNumThreadsTest)
{
    unsigned int nt = thread_block::num_threads();
    EXPECT_GT(nt, 0u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockNumThreads3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(0u, 0u, 0u));
    EXPECT_EQ(thread_block::num_threads(), 64u);
    EXPECT_EQ(thread_block::size(), 64u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockSizeTest)
{
    unsigned int s = thread_block::size();
    EXPECT_EQ(s, thread_block::num_threads());
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockThreadRank3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(7u, 3u, 1u));
    EXPECT_EQ(thread_block::thread_rank(), 7u + 3u * 8u + 1u * 8u * 4u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockGroupDimTest)
{
    dim3 gd = thread_block::group_dim();
    EXPECT_EQ(gd.x, thread_block::dim_threads().x);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockGroupDim3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(0u, 0u, 0u));
    dim3 gd = thread_block::group_dim();
    EXPECT_EQ(gd.x, 8u);
    EXPECT_EQ(gd.y, 4u);
    EXPECT_EQ(gd.z, 2u);
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupConstructorTest)
{
    coalesced_group cg(0xFFFFFFFF);
    EXPECT_EQ(cg.get_type(), group_type::coalesced_group_type);
    EXPECT_EQ(cg.num_threads(), 32ull);
    EXPECT_EQ(cg.size(), 32ull);
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupConstructorPartialMaskTest)
{
    coalesced_group cg(0x0000FFFF);
    EXPECT_EQ(cg.num_threads(), 16ull);
    EXPECT_EQ(cg.size(), 16ull);
    EXPECT_EQ(cg.meta_group_size(), 1ull);
    EXPECT_EQ(cg.meta_group_rank(), 0ull);
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupConstructorSparseMaskTest)
{
    coalesced_group cg(0x00000055);
    EXPECT_EQ(cg.num_threads(), 4ull);
    EXPECT_EQ(cg.size(), 4ull);
    EXPECT_EQ(cg.meta_group_size(), 1ull);
    EXPECT_EQ(cg.meta_group_rank(), 0ull);
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupSyncTest)
{
    coalesced_group cg(0xFFFFFFFF);
    cg.sync();
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupThreadRankTest)
{
    coalesced_group cg(0xFFFFFFFF);
    unsigned long long rank = cg.thread_rank();
    EXPECT_GE(rank, 0ull);
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupThreadRankUsesMaskAndLaneMaskTest)
{
    coalesced_group cg(0x00000055);
    EXPECT_EQ(
        cg.thread_rank(),
        static_cast<unsigned long long>(__popc(0x00000055 & static_cast<unsigned int>(lanemask_lt()))));
}

namespace {
int32_t AscShflStub(int32_t var, int32_t src_lane, int32_t width)
{
    EXPECT_EQ(var, 7);
    EXPECT_EQ(src_lane, 2);
    EXPECT_EQ(width, 32);
    return 42;
}

class TestTiledGroup : public tiled_group {
public:
    TestTiledGroup(unsigned int numThreads, unsigned int mask, unsigned int metaRank = 0u, unsigned int metaSize = 1u)
        : tiled_group(numThreads)
    {
        _tiled_info.mask = mask;
        _tiled_info.meta_group_rank = metaRank;
        _tiled_info.meta_group_size = metaSize;
    }
};

int32_t AscShflModuloStub(int32_t var, int32_t src_lane, int32_t width)
{
    EXPECT_EQ(var, 11);
    EXPECT_EQ(src_lane, 3);
    EXPECT_EQ(width, 32);
    return 77;
}

int32_t AscTileShflStub(int32_t var, int32_t src_lane, int32_t width)
{
    EXPECT_EQ(var, 13);
    EXPECT_EQ(src_lane, 7);
    EXPECT_EQ(width, 4);
    return 31;
}

int32_t AscTileShflUpStub(int32_t var, uint32_t delta, int32_t width)
{
    EXPECT_EQ(var, 13);
    EXPECT_EQ(delta, 5u);
    EXPECT_EQ(width, 4);
    return 32;
}

int32_t AscTileShflDownStub(int32_t var, uint32_t delta, int32_t width)
{
    EXPECT_EQ(var, 13);
    EXPECT_EQ(delta, 5u);
    EXPECT_EQ(width, 4);
    return 33;
}

int32_t AscTileShflXorStub(int32_t var, int32_t lane_mask, int32_t width)
{
    EXPECT_EQ(var, 13);
    EXPECT_EQ(lane_mask, 7);
    EXPECT_EQ(width, 4);
    return 34;
}
} // namespace

TEST_F(CooperativeGroupsTestsuite, TiledGroupPropertiesTest)
{
    TestTiledGroup tiled(6u, 0x0000003Fu, 2u, 5u);
    EXPECT_EQ(tiled.get_type(), group_type::tiled_group_type);
    EXPECT_EQ(tiled.num_threads(), 6ull);
    EXPECT_EQ(tiled.size(), 6ull);
    EXPECT_EQ(
        tiled.thread_rank(),
        static_cast<unsigned long long>(__popc(0x0000003F & static_cast<unsigned int>(lanemask_lt()))));
    tiled.sync();
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupBaseClassTiledGroupDispatchTest)
{
    TestTiledGroup tiled(6u, 0x0000003Fu);
    thread_group& tg = tiled;
    EXPECT_EQ(tg.get_type(), group_type::tiled_group_type);
    EXPECT_EQ(tg.size(), 6ull);
    EXPECT_EQ(tg.num_threads(), 6ull);
    EXPECT_EQ(tg.thread_rank(), tiled.thread_rank());
    tg.sync();
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupShflInt32Test)
{
    MOCKER_CPP(asc_shfl, int32_t(int32_t, int32_t, int32_t)).times(1).will(invoke(AscShflStub));

    coalesced_group cg(0x1FFFFFFF);
    int32_t result = cg.shfl(7, 2);
    EXPECT_EQ(result, 42);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupShflSrcRankModuloTest)
{
    MOCKER_CPP(asc_shfl, int32_t(int32_t, int32_t, int32_t)).times(1).will(invoke(AscShflModuloStub));

    coalesced_group cg(0x0000000F);
    int32_t result = cg.shfl(11, 7);
    EXPECT_EQ(result, 77);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupPackLanesTest)
{
    coalesced_group cg(0x00000055);
    EXPECT_EQ(cg._packLanes(0x00000000), 0x0u);
    EXPECT_EQ(cg._packLanes(0x00000001), 0x1u);
    EXPECT_EQ(cg._packLanes(0x00000004), 0x2u);
    EXPECT_EQ(cg._packLanes(0x00000010), 0x4u);
    EXPECT_EQ(cg._packLanes(0x00000040), 0x8u);
    EXPECT_EQ(cg._packLanes(0x00000055), 0xFu);
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupMetaGroupSizeRankTest)
{
    coalesced_group cg(0x0000FFFF);
    EXPECT_EQ(cg.meta_group_size(), 1ull);
    EXPECT_EQ(cg.meta_group_rank(), 0ull);
}

TEST_F(CooperativeGroupsTestsuite, CoalescedThreadsTypeTest)
{
    EXPECT_EQ(coalesced_group(0xFFFFFFFF).get_type(), group_type::coalesced_group_type);
}

TEST_F(CooperativeGroupsTestsuite, CoalescedThreadsUsesActiveMaskTest)
{
    MOCKER_CPP(__activemask, uint32_t(void)).times(1).will(returnValue(0x0000000Fu));

    coalesced_group cg = coalesced_threads();
    EXPECT_EQ(cg.get_type(), group_type::coalesced_group_type);
    EXPECT_EQ(cg.num_threads(), 4ull);
    EXPECT_EQ(cg.size(), 4ull);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupVoteFunctionsUseMaskedBallotTest)
{
    MOCKER_CPP(__ballot, uint32_t(int32_t)).times(3).will(returnValue(0x0000000F));

    coalesced_group cg(0x0000000F);
    EXPECT_EQ(cg.any(1), 1);
    EXPECT_EQ(cg.all(1), 1);
    EXPECT_EQ(cg.ballot(1), 0x0000000Fu);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupVoteFunctionsIgnoreOutsideMaskTest)
{
    MOCKER_CPP(__ballot, uint32_t(int32_t)).times(2).will(returnValue(0x00000010));

    coalesced_group cg(0x0000000F);
    EXPECT_EQ(cg.any(1), 0);
    EXPECT_EQ(cg.all(1), 0);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, CoalescedGroupBallotPacksSparseMaskTest)
{
    MOCKER_CPP(__ballot, uint32_t(int32_t)).times(1).will(returnValue(0x00000011));

    coalesced_group cg(0x00000055);
    EXPECT_EQ(cg.ballot(1), 0x5u);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, BinaryPartitionCoalescedGroupAllSamePredTest)
{
    MOCKER_CPP(__ballot, uint32_t(int32_t)).times(1).will(returnValue(0x0000000F));

    coalesced_group cg(0x0000000F);
    coalesced_group result = binary_partition(cg, true);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
    EXPECT_EQ(result.get_mask(), 0x0000000Fu);
    EXPECT_EQ(result.meta_group_rank(), 0ull);
    EXPECT_EQ(result.meta_group_size(), 1ull);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, BinaryPartitionCoalescedGroupSplitTest)
{
    MOCKER_CPP(__ballot, uint32_t(int32_t)).times(1).will(returnValue(0x00000005));

    coalesced_group cg(0x0000000F);
    coalesced_group result = binary_partition(cg, true);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
    EXPECT_EQ(result.get_mask(), 0x00000005u);
    EXPECT_EQ(result.meta_group_rank(), 1ull);
    EXPECT_EQ(result.meta_group_size(), 2ull);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, BinaryPartitionThreadBlockTileUsesTileMaskTest)
{
    MOCKER_CPP(__ballot, uint32_t(int32_t)).times(1).will(returnValue(0x00000005));

    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    coalesced_group result = binary_partition(tile4, true);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
    EXPECT_EQ(result.get_mask(), 0x00000005u);
    EXPECT_EQ(result.meta_group_rank(), 1ull);
    EXPECT_EQ(result.meta_group_size(), 2ull);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflInt32Test)
{
    coalesced_group cg(0xFFFFFFFF);
    int32_t var = 42;
    int32_t result = cg.shfl(var, 0);
    EXPECT_EQ(result, 42);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflUint32Test)
{
    coalesced_group cg(0xFFFFFFFF);
    uint32_t var = 100u;
    uint32_t result = cg.shfl(var, 0);
    EXPECT_EQ(result, 100u);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflInt64Test)
{
    coalesced_group cg(0xFFFFFFFF);
    int64_t var = 12345678901234ll;
    int64_t result = cg.shfl(var, 0);
    EXPECT_EQ(result, 12345678901234ll);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflUint64Test)
{
    coalesced_group cg(0xFFFFFFFF);
    uint64_t var = 9876543210ull;
    uint64_t result = cg.shfl(var, 0);
    EXPECT_EQ(result, 9876543210ull);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflFloatTest)
{
    coalesced_group cg(0xFFFFFFFF);
    float var = 3.14f;
    float result = cg.shfl(var, 0);
    EXPECT_FLOAT_EQ(result, 3.14f);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflHalfTest)
{
    coalesced_group cg(0xFFFFFFFF);
    half var = static_cast<half>(1.5);
    half result = cg.shfl(var, 0);
    EXPECT_EQ(result, static_cast<half>(1.5));
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflHalf2Test)
{
    coalesced_group cg(0xFFFFFFFF);
    half2 var;
    var.x = static_cast<half>(2.5);
    var.y = static_cast<half>(2.5);
    half2 result = cg.shfl(var, 0);
    half2 expected;
    expected.x = static_cast<half>(2.5);
    expected.y = static_cast<half>(2.5);
    EXPECT_EQ(result.x, expected.x);
    EXPECT_EQ(result.y, expected.y);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflUpInt32Test)
{
    coalesced_group cg(0xFFFFFFFF);
    int32_t var = 10;
    int32_t result = cg.shfl_up(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflUpFloatTest)
{
    coalesced_group cg(0xFFFFFFFF);
    float var = 1.0f;
    float result = cg.shfl_up(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflDownInt32Test)
{
    coalesced_group cg(0xFFFFFFFF);
    int32_t var = 10;
    int32_t result = cg.shfl_down(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupShflDownFloatTest)
{
    coalesced_group cg(0xFFFFFFFF);
    float var = 1.0f;
    float result = cg.shfl_down(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupAnyTest)
{
    coalesced_group cg(0xFFFFFFFF);
    int result = cg.any(1);
    EXPECT_EQ(result, 1);
    result = cg.any(0);
    EXPECT_EQ(result, 0);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupAllTest)
{
    coalesced_group cg(0xFFFFFFFF);
    int result = cg.all(1);
    EXPECT_EQ(result, 1);
    result = cg.all(0);
    EXPECT_EQ(result, 0);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupBallotTest)
{
    coalesced_group cg(0xFFFFFFFF);
    unsigned int result = cg.ballot(1);
    EXPECT_EQ(result, 0xFFFFFFFFu);
}

TEST_F(CooperativeGroupsTestsuite, TileHelpersTest)
{
    EXPECT_EQ(tile_helpers<32>::tile_count, 1u);
    EXPECT_EQ(tile_helpers<32>::tile_mask, 0xFFFFFFFFu);
    EXPECT_EQ(tile_helpers<32>::lane_mask, 0x1Fu);
    EXPECT_EQ(tile_helpers<32>::shift_count, 5u);

    EXPECT_EQ(tile_helpers<16>::tile_count, 2u);
    EXPECT_EQ(tile_helpers<16>::tile_mask, 0x0000FFFFu);
    EXPECT_EQ(tile_helpers<16>::lane_mask, 0x0Fu);
    EXPECT_EQ(tile_helpers<16>::shift_count, 4u);

    EXPECT_EQ(tile_helpers<8>::tile_count, 4u);
    EXPECT_EQ(tile_helpers<8>::tile_mask, 0x000000FFu);
    EXPECT_EQ(tile_helpers<8>::lane_mask, 0x07u);
    EXPECT_EQ(tile_helpers<8>::shift_count, 3u);

    EXPECT_EQ(tile_helpers<4>::tile_count, 8u);
    EXPECT_EQ(tile_helpers<4>::tile_mask, 0x0000000Fu);
    EXPECT_EQ(tile_helpers<4>::lane_mask, 0x03u);
    EXPECT_EQ(tile_helpers<4>::shift_count, 2u);

    EXPECT_EQ(tile_helpers<2>::tile_count, 16u);
    EXPECT_EQ(tile_helpers<2>::tile_mask, 0x00000003u);
    EXPECT_EQ(tile_helpers<2>::lane_mask, 0x01u);
    EXPECT_EQ(tile_helpers<2>::shift_count, 1u);

    EXPECT_EQ(tile_helpers<1>::tile_count, 32u);
    EXPECT_EQ(tile_helpers<1>::tile_mask, 0x00000001u);
    EXPECT_EQ(tile_helpers<1>::lane_mask, 0x00u);
    EXPECT_EQ(tile_helpers<1>::shift_count, 0u);
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseNumThreadsSizeTest)
{
    EXPECT_EQ(single_warp_thread_block_tile_base<32>::num_threads(), 32ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<32>::size(), 32ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<16>::num_threads(), 16ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<16>::size(), 16ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<8>::num_threads(), 8ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<8>::size(), 8ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<4>::num_threads(), 4ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<4>::size(), 4ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<2>::num_threads(), 2ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<2>::size(), 2ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<1>::num_threads(), 1ull);
    EXPECT_EQ(single_warp_thread_block_tile_base<1>::size(), 1ull);
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseThreadRankTest)
{
    unsigned long long rank = single_warp_thread_block_tile_base<32>::thread_rank();
    EXPECT_GE(rank, 0ull);
}

TEST_F(CooperativeGroupsTestsuite, ValidThreadBlockTileSizeTraitsTest)
{
    EXPECT_TRUE(_is_valid_single_warp_tile<1>::value);
    EXPECT_TRUE(_is_valid_single_warp_tile<2>::value);
    EXPECT_TRUE(_is_valid_single_warp_tile<4>::value);
    EXPECT_TRUE(_is_valid_single_warp_tile<8>::value);
    EXPECT_TRUE(_is_valid_single_warp_tile<16>::value);
    EXPECT_TRUE(_is_valid_single_warp_tile<32>::value);
    EXPECT_FALSE(_is_valid_single_warp_tile<64>::value);
    EXPECT_FALSE(_is_valid_single_warp_tile<3>::value);

    EXPECT_FALSE(_is_valid_multi_warp_tile<32>::value);
    EXPECT_TRUE(_is_valid_multi_warp_tile<64>::value);
    EXPECT_TRUE(_is_valid_multi_warp_tile<128>::value);
    EXPECT_TRUE(_is_valid_multi_warp_tile<256>::value);
    EXPECT_TRUE(_is_valid_multi_warp_tile<512>::value);
    EXPECT_TRUE(_is_valid_multi_warp_tile<1024>::value);
    EXPECT_TRUE(_is_valid_multi_warp_tile<2048>::value);
    EXPECT_FALSE(_is_valid_multi_warp_tile<3>::value);
    EXPECT_FALSE(_is_valid_multi_warp_tile<4096>::value);
}

TEST_F(CooperativeGroupsTestsuite, MultiWarpThreadBlockTileSizeTraitsTest)
{
    EXPECT_FALSE(_is_multi_warp<32>::value);
    EXPECT_TRUE(_is_multi_warp<64>::value);
    EXPECT_TRUE(_is_multi_warp<128>::value);
    EXPECT_TRUE(_is_multi_warp<256>::value);
    EXPECT_TRUE(_is_multi_warp<512>::value);
    EXPECT_TRUE(_is_multi_warp<1024>::value);
    EXPECT_TRUE(_is_multi_warp<2048>::value);
    EXPECT_FALSE(_is_multi_warp<4096>::value);
}

TEST_F(CooperativeGroupsTestsuite, BlockTileMemoryHostSizeofTest)
{
    constexpr size_t defaultSize = details::multi_warp_scratch::scratch_size_needed(1024);
    constexpr size_t maxSize = details::multi_warp_scratch::scratch_size_needed(2048);

    static_assert(sizeof(block_tile_memory<>) == defaultSize);
    static_assert(sizeof(block_tile_memory<1024>) == defaultSize);
    static_assert(sizeof(block_tile_memory<2048>) == maxSize);
    static_assert(alignof(block_tile_memory<>) == details::multi_warp_scratch::communication_size);

    EXPECT_EQ(sizeof(block_tile_memory<>), defaultSize);
    EXPECT_EQ(sizeof(block_tile_memory<1024>), defaultSize);
    EXPECT_EQ(sizeof(block_tile_memory<2048>), maxSize);
    EXPECT_EQ(defaultSize, 296U);
    EXPECT_EQ(maxSize, 552U);
}

TEST_F(CooperativeGroupsTestsuite, MultiWarpThreadBlockTileMetadataTest)
{
    SimtDimGuard guard(cce::dim3(2048u, 1u, 1u), cce::dim3(130u, 0u, 0u));
    block_tile_memory<2048> memory{};
    thread_block block = this_thread_block(memory);
    auto tile64 = tiled_partition<64>(block);

    EXPECT_NE(block._get_tile_memory(), 0U);
    EXPECT_EQ(tile64.size(), 64ull);
    EXPECT_EQ(tile64.num_threads(), 64ull);
    EXPECT_EQ(tile64.thread_rank(), 2ull);
    EXPECT_EQ(tile64.meta_group_rank(), 2u);
    EXPECT_EQ(tile64.meta_group_size(), 32u);
}

TEST_F(CooperativeGroupsTestsuite, MultiWarpThreadBlockTileSyncLocationMappingTest)
{
    SimtDimGuard guard(cce::dim3(2048u, 1u, 1u), cce::dim3(0u, 0u, 0u));
    block_tile_memory<2048> memory{};
    thread_block block = this_thread_block(memory);
    auto* scratch = reinterpret_cast<details::multi_warp_scratch*>(&memory);

    EXPECT_EQ(
        tiled_partition<64>(block).template get_sync_location<details::tile_memory_type::generic>(),
        &scratch->barriers[0]);
    EXPECT_EQ(
        tiled_partition<128>(block).template get_sync_location<details::tile_memory_type::generic>(),
        &scratch->barriers[1]);
    EXPECT_EQ(
        tiled_partition<256>(block).template get_sync_location<details::tile_memory_type::generic>(),
        &scratch->barriers[2]);
    EXPECT_EQ(
        tiled_partition<512>(block).template get_sync_location<details::tile_memory_type::generic>(),
        &scratch->barriers[3]);
    EXPECT_EQ(
        tiled_partition<1024>(block).template get_sync_location<details::tile_memory_type::generic>(),
        &scratch->barriers[4]);
}

TEST_F(CooperativeGroupsTestsuite, NestedMultiWarpThreadBlockTileMetadataTest)
{
    SimtDimGuard guard(cce::dim3(2048u, 1u, 1u), cce::dim3(700u, 0u, 0u));
    block_tile_memory<2048> memory{};
    thread_block block = this_thread_block(memory);
    auto largeTile = tiled_partition<512>(block);
    auto smallTile = tiled_partition<128>(largeTile);
    thread_block_tile<128, void> smallTileVoid = smallTile;

    EXPECT_EQ(largeTile.thread_rank(), 188ull);
    EXPECT_EQ(largeTile.meta_group_rank(), 1u);
    EXPECT_EQ(largeTile.meta_group_size(), 4u);
    EXPECT_EQ(smallTile.thread_rank(), 60ull);
    EXPECT_EQ(smallTile.meta_group_rank(), 1u);
    EXPECT_EQ(smallTile.meta_group_size(), 4u);
    EXPECT_EQ(smallTileVoid.meta_group_rank(), smallTile.meta_group_rank());
    EXPECT_EQ(smallTileVoid.meta_group_size(), smallTile.meta_group_size());
    EXPECT_NE(smallTileVoid._get_tile_memory(), 0U);
}

TEST_F(CooperativeGroupsTestsuite, MultiWarpThreadBlockTile2048ShflTest)
{
    SimtDimGuard guard(cce::dim3(2048u, 1u, 1u), cce::dim3(0u, 0u, 0u));
    block_tile_memory<2048> memory{};
    thread_block block = this_thread_block(memory);
    auto tile2048 = tiled_partition<2048>(block);

    EXPECT_EQ(tile2048.size(), 2048ull);
    EXPECT_EQ(tile2048.num_threads(), 2048ull);
    EXPECT_EQ(tile2048.meta_group_rank(), 0u);
    EXPECT_EQ(tile2048.meta_group_size(), 1u);
    EXPECT_EQ(tile2048.shfl(123u, 0), 123u);
}

TEST_F(CooperativeGroupsTestsuite, MultiWarpBarrierHelpersTest)
{
    details::multi_warp_scratch::barrier_t barrier{};
    EXPECT_EQ(details::get_group_mask(0u, 2u), 0x00000003u);
    EXPECT_EQ(details::get_group_mask(64u, 2u), 0x0000000Cu);
    EXPECT_EQ(details::get_group_mask(1024u, 32u), 0xFFFFFFFFu);

    barrier.arrived[0] = 0x00000002u;
    details::sync_warps<details::tile_memory_type::generic>(&barrier, 0u, 2u);
    EXPECT_EQ(barrier.arrived[0], 0x00000000u);
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseBuildMaskTest)
{
    EXPECT_EQ(single_warp_thread_block_tile_base<32>::build_mask(), 0xFFFFFFFFu);
    EXPECT_EQ(single_warp_thread_block_tile_base<16>::build_mask(), 0x0000FFFFu);
    EXPECT_EQ(single_warp_thread_block_tile_base<8>::build_mask(), 0x000000FFu);
    EXPECT_EQ(single_warp_thread_block_tile_base<4>::build_mask(), 0x0000000Fu);
    EXPECT_EQ(single_warp_thread_block_tile_base<2>::build_mask(), 0x00000003u);
    EXPECT_EQ(single_warp_thread_block_tile_base<1>::build_mask(), 0x00000001u);
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseSyncTest)
{
    single_warp_thread_block_tile_base<32> tbtb;
    tbtb.sync();
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseShflPassesSrcRankAndWidthTest)
{
    MOCKER_CPP(asc_shfl, int32_t(int32_t, int32_t, int32_t)).times(1).will(invoke(AscTileShflStub));

    single_warp_thread_block_tile_base<4> tile;
    EXPECT_EQ(tile.shfl(13, 7), 31);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseShflUpPassesDeltaAndWidthTest)
{
    MOCKER_CPP(asc_shfl_up, int32_t(int32_t, uint32_t, int32_t)).times(1).will(invoke(AscTileShflUpStub));

    single_warp_thread_block_tile_base<4> tile;
    EXPECT_EQ(tile.shfl_up(13, 5), 32);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseShflDownPassesDeltaAndWidthTest)
{
    MOCKER_CPP(asc_shfl_down, int32_t(int32_t, uint32_t, int32_t)).times(1).will(invoke(AscTileShflDownStub));

    single_warp_thread_block_tile_base<4> tile;
    EXPECT_EQ(tile.shfl_down(13, 5), 33);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseShflXorPassesLaneMaskAndWidthTest)
{
    MOCKER_CPP(asc_shfl_xor, int32_t(int32_t, int32_t, int32_t)).times(1).will(invoke(AscTileShflXorStub));

    single_warp_thread_block_tile_base<4> tile;
    EXPECT_EQ(tile.shfl_xor(13, 7), 34);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, SingleWarpThreadBlockTileBaseVoteFunctionsUseTileMaskTest)
{
    MOCKER_CPP(__ballot, uint32_t(int32_t)).times(3).will(returnValue(0x0000000F));

    single_warp_thread_block_tile_base<4> tile;
    EXPECT_EQ(tile.any(1), 1);
    EXPECT_EQ(tile.all(1), 1);
    EXPECT_EQ(tile.ballot(1), 0x0000000Fu);

    GlobalMockObject::verify();
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_SingleWarpThreadBlockTileBaseShflInt32Test)
{
    single_warp_thread_block_tile_base<32> tbtb;
    int32_t var = 42;
    int32_t result = tbtb.shfl(var, 0);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_SingleWarpThreadBlockTileBaseShflFloatTest)
{
    single_warp_thread_block_tile_base<32> tbtb;
    float var = 3.14f;
    float result = tbtb.shfl(var, 0);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_SingleWarpThreadBlockTileBaseShflUpInt32Test)
{
    single_warp_thread_block_tile_base<32> tbtb;
    int32_t var = 10;
    int32_t result = tbtb.shfl_up(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_SingleWarpThreadBlockTileBaseShflDownInt32Test)
{
    single_warp_thread_block_tile_base<32> tbtb;
    int32_t var = 10;
    int32_t result = tbtb.shfl_down(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_SingleWarpThreadBlockTileBaseShflXorInt32Test)
{
    single_warp_thread_block_tile_base<32> tbtb;
    int32_t var = 10;
    int32_t result = tbtb.shfl_xor(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_SingleWarpThreadBlockTileBaseAnyTest)
{
    single_warp_thread_block_tile_base<32> tbtb;
    int result = tbtb.any(1);
    EXPECT_EQ(result, 1);
    result = tbtb.any(0);
    EXPECT_EQ(result, 0);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_SingleWarpThreadBlockTileBaseAllTest)
{
    single_warp_thread_block_tile_base<32> tbtb;
    int result = tbtb.all(1);
    EXPECT_EQ(result, 1);
    result = tbtb.all(0);
    EXPECT_EQ(result, 0);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_SingleWarpThreadBlockTileBaseBallotTest)
{
    single_warp_thread_block_tile_base<32> tbtb;
    unsigned int result = tbtb.ballot(1);
    EXPECT_NE(result, 0u);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionThreadBlockSize4Test)
{
    thread_block tb = this_thread_block();
    auto tile = tiled_partition<4>(tb);
    EXPECT_EQ(tile.size(), 4ull);
    EXPECT_EQ(tile.num_threads(), 4ull);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionThreadBlockSize1Test)
{
    thread_block tb = this_thread_block();
    auto tile = tiled_partition<1>(tb);
    EXPECT_EQ(tile.size(), 1ull);
    EXPECT_EQ(tile.num_threads(), 1ull);
    EXPECT_EQ(tile.thread_rank(), 0ull);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionThreadBlockSize2Test)
{
    thread_block tb = this_thread_block();
    auto tile = tiled_partition<2>(tb);
    EXPECT_EQ(tile.size(), 2ull);
    EXPECT_EQ(tile.num_threads(), 2ull);
    EXPECT_LT(tile.thread_rank(), tile.size());
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionThreadBlockSize8Test)
{
    thread_block tb = this_thread_block();
    auto tile = tiled_partition<8>(tb);
    EXPECT_EQ(tile.size(), 8ull);
    EXPECT_EQ(tile.num_threads(), 8ull);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionThreadBlockSize16Test)
{
    thread_block tb = this_thread_block();
    auto tile = tiled_partition<16>(tb);
    EXPECT_EQ(tile.size(), 16ull);
    EXPECT_EQ(tile.num_threads(), 16ull);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionThreadBlockSize32Test)
{
    thread_block tb = this_thread_block();
    auto tile = tiled_partition<32>(tb);
    EXPECT_EQ(tile.size(), 32ull);
    EXPECT_EQ(tile.num_threads(), 32ull);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionMetaGroupInfoTest)
{
    thread_block tb = this_thread_block();
    auto tile = tiled_partition<4>(tb);
    unsigned int meta_rank = tile.meta_group_rank();
    unsigned int meta_size = tile.meta_group_size();
    EXPECT_GE(meta_rank, 0u);
    EXPECT_GT(meta_size, 0u);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionThreadGroupTest)
{
    thread_block tb = this_thread_block();
    thread_group tg = tiled_partition(tb, 4);
    EXPECT_EQ(tg.get_type(), group_type::tiled_group_type);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionThreadGroupMetadataTest)
{
    SimtDimGuard guard(cce::dim3(10u, 1u, 1u), cce::dim3(9u, 0u, 0u));
    thread_block tb = this_thread_block();
    thread_group tg = tiled_partition(tb, 4);
    EXPECT_EQ(tg.get_type(), group_type::tiled_group_type);
    EXPECT_TRUE(tg._tiled_info.is_tiled);
    EXPECT_EQ(tg._tiled_info.num_threads, 2u);
    EXPECT_EQ(tg._tiled_info.meta_group_rank, 2u);
    EXPECT_EQ(tg._tiled_info.meta_group_size, 3u);
    EXPECT_EQ(tg.size(), 2ull);
    EXPECT_EQ(tg.num_threads(), 2ull);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionCoalescedGroupTest)
{
    coalesced_group cg(0x0000FFFF);
    coalesced_group result = tiled_partition(cg, 4);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionCoalescedGroupNoSplitWhenTileIsLargeEnoughTest)
{
    coalesced_group cg(0x0000000F);
    coalesced_group result = tiled_partition(cg, 8);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
    EXPECT_EQ(result.num_threads(), 4ull);
    EXPECT_EQ(result.size(), 4ull);
    EXPECT_EQ(result.meta_group_rank(), 0ull);
    EXPECT_EQ(result.meta_group_size(), 1ull);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionAlreadyTiledCoalescedGroupTest)
{
    SimtDimGuard guard(cce::dim3(6u, 1u, 1u), cce::dim3(5u, 0u, 0u));
    thread_block tb = this_thread_block();
    thread_group parent = tiled_partition(tb, 8);

    EXPECT_EQ(parent.get_type(), group_type::tiled_group_type);
    EXPECT_TRUE(parent._tiled_info.is_tiled);
    EXPECT_EQ(parent.num_threads(), 6ull);

    thread_group result = tiled_partition(parent, 4);
    EXPECT_EQ(result.get_type(), group_type::tiled_group_type);
    EXPECT_TRUE(result._tiled_info.is_tiled);
    EXPECT_EQ(result.num_threads(), 2ull);
    EXPECT_EQ(result._tiled_info.meta_group_rank, 1u);
    EXPECT_EQ(result._tiled_info.meta_group_size, 2u);
}

TEST_F(CooperativeGroupsTestsuite, SupportTypeSimtInternelTest)
{
    EXPECT_TRUE((SupportTypeSimtInternel<int32_t, int32_t>));
    EXPECT_TRUE((SupportTypeSimtInternel<uint32_t, int32_t, uint32_t>));
    EXPECT_FALSE((SupportTypeSimtInternel<double, int32_t, uint32_t>));
    EXPECT_TRUE((SupportTypeSimtInternel<float, int32_t, float>));
    EXPECT_TRUE((SupportTypeSimtInternel<half, int32_t, half>));
    EXPECT_TRUE((SupportTypeSimtInternel<half2, int32_t, half2>));
    EXPECT_TRUE((SupportTypeSimtInternel<int64_t, int32_t, int64_t>));
    EXPECT_TRUE((SupportTypeSimtInternel<uint64_t, int32_t, uint64_t>));
}

TEST_F(CooperativeGroupsTestsuite, FnsInternalOffsetZeroTest)
{
    unsigned int mask = 0xFFFFFFFF;
    unsigned int result = __fns_internal(mask, 0, 0);
    EXPECT_EQ(result, 0u);
}

TEST_F(CooperativeGroupsTestsuite, FnsInternalOffsetPositiveTest)
{
    unsigned int mask = 0xFFFFFFFF;
    unsigned int result = __fns_internal(mask, 0, 1);
    EXPECT_EQ(result, 0u);
}

TEST_F(CooperativeGroupsTestsuite, FnsInternalOffsetNegativeTest)
{
    unsigned int mask = 0xFFFFFFFF;
    unsigned int result = __fns_internal(mask, 31, -1);
    EXPECT_EQ(result, 31u);
}

TEST_F(CooperativeGroupsTestsuite, FnsInternalPartialMaskTest)
{
    unsigned int mask = 0x0000FFFF;
    unsigned int result = __fns_internal(mask, 0, 1);
    EXPECT_EQ(result, 0u);
}

TEST_F(CooperativeGroupsTestsuite, FnsInternalNotFoundTest)
{
    unsigned int mask = 0x00000001;
    unsigned int result = __fns_internal(mask, 0, 2);
    EXPECT_EQ(result, static_cast<unsigned int>(-1));
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockTileImplMetaGroupRankSizeTest)
{
    thread_block tb = this_thread_block();
    auto tile = tiled_partition<4>(tb);
    unsigned int meta_rank = tile.meta_group_rank();
    unsigned int meta_size = tile.meta_group_size();
    EXPECT_GE(meta_rank, 0u);
    EXPECT_GT(meta_size, 0u);
    EXPECT_EQ(meta_size, (thread_block::size() + 4 - 1) / 4);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockTileConversionTest)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    thread_block_tile<4, void> tile_void = tile4;
    EXPECT_EQ(tile_void.size(), 4u);
}

TEST_F(CooperativeGroupsTestsuite, NestedThreadBlockTileConversionTest)
{
    thread_block tb = this_thread_block();
    auto tile32 = tiled_partition<32>(tb);
    auto tile16 = tiled_partition<16>(tile32);
    auto tile8 = tiled_partition<8>(tile16);
    thread_block_tile<16, void> tile16Void = tile16;
    thread_block_tile<8, void> tile8Void = tile8;

    EXPECT_EQ(tile16.size(), 16ull);
    EXPECT_EQ(tile8.size(), 8ull);
    EXPECT_EQ(tile16Void.size(), 16ull);
    EXPECT_EQ(tile8Void.size(), 8ull);
    EXPECT_EQ(tile16.meta_group_size(), 2u);
    EXPECT_EQ(tile8.meta_group_size(), 2u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupBaseClassThreadBlockTest)
{
    thread_block tb = this_thread_block();
    thread_group& tg = tb;
    EXPECT_EQ(tg.size(), thread_block::size());
    EXPECT_GT(tg.size(), 0ull);
    EXPECT_EQ(tg.num_threads(), thread_block::num_threads());
    EXPECT_EQ(tg.num_threads(), tg.size());
    EXPECT_EQ(tg.thread_rank(), thread_block::thread_rank());
    EXPECT_GE(tg.thread_rank(), 0ull);
    EXPECT_LT(tg.thread_rank(), tg.size());
    EXPECT_EQ(tg.get_type(), group_type::thread_block_type);
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupBaseClassTiledGroupDefaultDispatchTest)
{
    TestTiledGroup tiled(4u, 0x0000000Fu);
    thread_group& tg = tiled;
    EXPECT_EQ(tg.get_type(), group_type::tiled_group_type);
    EXPECT_EQ(tg.size(), 4ull);
    EXPECT_EQ(tg.num_threads(), 4ull);
    EXPECT_EQ(tg.thread_rank(), tiled.thread_rank());
    tg.sync();
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupBaseClassSyncFromThreadBlockTest)
{
    thread_block tb = this_thread_block();
    thread_group& tg = tb;
    tg.sync();
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupBaseClassCoalescedGroupTest)
{
    coalesced_group cg(0xFFFFFFFF);
    thread_group& tg = cg;
    EXPECT_EQ(tg.size(), cg.size());
    EXPECT_EQ(tg.size(), 32ull);
    EXPECT_EQ(tg.num_threads(), cg.num_threads());
    EXPECT_EQ(tg.num_threads(), 32ull);
    EXPECT_EQ(tg.thread_rank(), cg.thread_rank());
    EXPECT_EQ(tg.get_type(), group_type::coalesced_group_type);
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupBaseClassSyncFromCoalescedGroupTest)
{
    coalesced_group cg(0xFFFFFFFF);
    thread_group& tg = cg;
    tg.sync();
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupBaseClassPartialCoalescedGroupTest)
{
    coalesced_group cg(0x0000FFFF);
    thread_group& tg = cg;
    EXPECT_EQ(tg.size(), 16ull);
    EXPECT_EQ(tg.num_threads(), 16ull);
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupFromTiledPartitionSizeTest)
{
    thread_block tb = this_thread_block();
    thread_group tg = tiled_partition(tb, 4);
    EXPECT_GT(tg.size(), 0ull);
    EXPECT_LE(tg.size(), 32ull);
    EXPECT_EQ(tg.num_threads(), tg.size());
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupFromTiledPartitionThreadRankTest)
{
    thread_block tb = this_thread_block();
    thread_group tg = tiled_partition(tb, 4);
    unsigned long long rank = tg.thread_rank();
    EXPECT_EQ(
        rank, static_cast<unsigned long long>(__popc(tg._tiled_info.mask & static_cast<unsigned int>(lanemask_lt()))));
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupFromTiledPartitionSyncTest)
{
    thread_block tb = this_thread_block();
    thread_group tg = tiled_partition(tb, 4);
    tg.sync();
}

TEST_F(CooperativeGroupsTestsuite, ThreadGroupFromTiledPartitionGetTypeTest)
{
    thread_block tb = this_thread_block();
    thread_group tg = tiled_partition(tb, 4);
    EXPECT_EQ(tg.get_type(), group_type::tiled_group_type);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedThreadsTest)
{
    coalesced_group cg = coalesced_threads();
    EXPECT_EQ(cg.get_type(), group_type::coalesced_group_type);
    EXPECT_GT(cg.num_threads(), 0ull);
    EXPECT_EQ(cg.num_threads(), cg.size());
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_BinaryPartitionCoalescedGroupAllTrueTest)
{
    coalesced_group cg(0xFFFFFFFF);
    coalesced_group result = binary_partition(cg, true);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_BinaryPartitionCoalescedGroupAllFalseTest)
{
    coalesced_group cg(0xFFFFFFFF);
    coalesced_group result = binary_partition(cg, false);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_BinaryPartitionCoalescedGroupMixedTest)
{
    coalesced_group cg(0xFFFFFFFF);
    bool pred = (thread_block::thread_rank() % 2 == 0);
    coalesced_group result = binary_partition(cg, pred);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_BinaryPartitionThreadBlockTileTest)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    bool pred = (tile4.thread_rank() % 2 == 0);
    coalesced_group result = binary_partition(tile4, pred);
    EXPECT_EQ(result.get_type(), group_type::coalesced_group_type);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockTileConstructorTest)
{
    thread_block tb = this_thread_block();
    thread_block_tile<4, thread_block> tile4(tb);
    EXPECT_EQ(tile4.size(), 4u);
    EXPECT_EQ(tile4.num_threads(), 4u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockTileImplVoidConstructorCopiesMetadataTest)
{
    thread_block_tile_impl<4, void> tile4(2u, 8u);
    EXPECT_EQ(tile4.get_type(), group_type::tiled_group_type);
    EXPECT_EQ(tile4.size(), 4u);
    EXPECT_EQ(tile4.num_threads(), 4u);
    EXPECT_EQ(tile4.meta_group_rank(), 2u);
    EXPECT_EQ(tile4.meta_group_size(), 8u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockTileFromThreadBlockTest)
{
    thread_block tb = this_thread_block();
    auto tile8 = tiled_partition<8>(tb);
    EXPECT_EQ(tile8.size(), 8u);
    EXPECT_EQ(tile8.num_threads(), 8u);
    EXPECT_GE(tile8.thread_rank(), 0u);
    EXPECT_LT(tile8.thread_rank(), tile8.size());
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_ThreadBlockTileShflInt32Test)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    int32_t var = 42;
    int32_t result = tile4.shfl(var, 0);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_ThreadBlockTileShflUpInt32Test)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    int32_t var = 10;
    int32_t result = tile4.shfl_up(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_ThreadBlockTileShflDownInt32Test)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    int32_t var = 10;
    int32_t result = tile4.shfl_down(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_ThreadBlockTileShflXorInt32Test)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    int32_t var = 10;
    int32_t result = tile4.shfl_xor(var, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_ThreadBlockTileAnyTest)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    int result = tile4.any(1);
    EXPECT_EQ(result, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_ThreadBlockTileAllTest)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    int result = tile4.all(1);
    EXPECT_EQ(result, 1);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_ThreadBlockTileBallotTest)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    unsigned int result = tile4.ballot(1);
    EXPECT_NE(result, 0u);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionFromThreadBlockTileTest)
{
    thread_block tb = this_thread_block();
    auto tile8 = tiled_partition<8>(tb);
    auto tile4 = tiled_partition<4>(tile8);
    EXPECT_EQ(tile4.size(), 4ull);
    EXPECT_EQ(tile4.num_threads(), 4ull);
    EXPECT_GE(tile4.thread_rank(), 0ull);
}

TEST_F(CooperativeGroupsTestsuite, TiledPartitionFromThreadBlockTileSize2Test)
{
    thread_block tb = this_thread_block();
    auto tile8 = tiled_partition<8>(tb);
    auto tile2 = tiled_partition<2>(tile8);
    EXPECT_EQ(tile2.size(), 2ull);
    EXPECT_EQ(tile2.num_threads(), 2ull);
    EXPECT_LT(tile2.thread_rank(), tile2.size());
}

TEST_F(CooperativeGroupsTestsuite, MultiLevelTiledPartitionMetaGroupInfoTest)
{
    thread_block tb = this_thread_block();
    auto tile8 = tiled_partition<8>(tb);
    auto tile4 = tiled_partition<4>(tile8);
    unsigned int meta_rank = tile4.meta_group_rank();
    unsigned int meta_size = tile4.meta_group_size();
    EXPECT_GE(meta_rank, 0u);
    EXPECT_GT(meta_size, 0u);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockTileSyncTest)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    tile4.sync();
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_CoalescedGroupTiledPartitionPartialMaskTest)
{
    coalesced_group cg(0x0000FFFF);
    coalesced_group tile = tiled_partition(cg, 4);
    EXPECT_GT(tile.num_threads(), 0ull);
    EXPECT_LE(tile.num_threads(), 16ull);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockTileMetaGroupRankSizeFromBlockTest)
{
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    unsigned int meta_rank = tile4.meta_group_rank();
    unsigned int meta_size = tile4.meta_group_size();
    EXPECT_EQ(meta_rank, thread_block::thread_rank() / 4);
    EXPECT_EQ(meta_size, (thread_block::size() + 3) / 4);
}

TEST_F(CooperativeGroupsTestsuite, ThreadBlockTileMetaGroupRankSizeFromBlock3DTest)
{
    SimtDimGuard guard(cce::dim3(8u, 4u, 2u), cce::dim3(7u, 3u, 1u));
    thread_block tb = this_thread_block();
    auto tile4 = tiled_partition<4>(tb);
    EXPECT_EQ(tile4.meta_group_rank(), thread_block::thread_rank() / 4);
    EXPECT_EQ(tile4.meta_group_size(), (thread_block::size() + 3) / 4);
}

TEST_F(CooperativeGroupsTestsuite, DISABLED_ThreadGroupTiledPartitionFromCoalescedGroupTest)
{
    coalesced_group cg(0xFFFFFFFF);
    thread_group tg = tiled_partition(static_cast<const thread_group&>(cg), 4);
    EXPECT_EQ(tg.get_type(), group_type::coalesced_group_type);
    EXPECT_GT(tg.num_threads(), 0ull);
}
