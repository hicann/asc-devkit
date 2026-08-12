/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <algorithm>
#include <limits>
#include <vector>

#include "alg_env_config.h"
#include "cann_host_bridge.h"
#include "executor_v2_base.h"
#include "gtest/gtest.h"
#include "ins_temp_all_to_all_v_mesh_1D.h"

using namespace mc2_ops_hccl;

namespace checker {
namespace {
constexpr u64 ALLTOALL_MULTI_CHANNEL_LIMIT = 150ULL * 1024ULL * 1024ULL;

OpParam MakeAicpuParam(HcclCMDType opType, CommEngine engine = COMM_ENGINE_AICPU_TS)
{
    OpParam param{};
    param.opType = opType;
    param.engine = engine;
    param.opExecuteConfig = OpExecuteConfig::AICPU_TS;
    return param;
}

} // namespace

class ST_ALL_TO_ALL_MESH1D_AICPU_TEST : public ::testing::Test {
protected:
    void SetUp() override { ResetAlgEnvConfigInitState(); }
};

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, route_uses_cann_bridge)
{
    EXPECT_TRUE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALL)));
    EXPECT_TRUE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALLV)));
    EXPECT_TRUE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALL, COMM_ENGINE_AICPU)));
    EXPECT_TRUE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALLV, COMM_ENGINE_AICPU)));

    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALL, COMM_ENGINE_CPU)));
    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALLV, COMM_ENGINE_CPU)));
    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLGATHER)));
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, cann_registry_contains_mesh1d_algorithms)
{
    EXPECT_NE(GetAlgExecViaCann(HCCL_CMD_ALLTOALLV, "InsAlltoAllVMesh1D"), nullptr);
    EXPECT_NE(GetAlgExecViaCann(HCCL_CMD_ALLTOALL, "InsAlltoAllMesh1DSingleChannel"), nullptr);
    EXPECT_NE(GetAlgExecViaCann(HCCL_CMD_ALLTOALL, "InsAlltoAllMesh1D"), nullptr);
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, alltoall_multi_channel_threshold_is_strict_and_overflow_safe)
{
    constexpr u64 rankSize = 3U;
    constexpr u64 dataTypeSize = 1U;
    constexpr u64 countAtLimit = ALLTOALL_MULTI_CHANNEL_LIMIT / rankSize;
    EXPECT_FALSE(ShouldUseMultiChannelForAlltoAll(countAtLimit - 1U, dataTypeSize, rankSize));
    EXPECT_FALSE(ShouldUseMultiChannelForAlltoAll(countAtLimit, dataTypeSize, rankSize));
    EXPECT_TRUE(ShouldUseMultiChannelForAlltoAll(countAtLimit + 1U, dataTypeSize, rankSize));
    EXPECT_TRUE(ShouldUseMultiChannelForAlltoAll(std::numeric_limits<u64>::max(), 16U, 18U));
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, peer_group_rounds_cover_18_ranks)
{
    const std::vector<u32> firstRound = CalcAlltoallMesh1DCommRanks(0U, 18U, 16U, 0U, 17U);
    const std::vector<u32> secondRound = CalcAlltoallMesh1DCommRanks(0U, 18U, 16U, 1U, 1U);
    EXPECT_EQ(firstRound.size(), 16U);
    EXPECT_EQ(std::find(firstRound.begin(), firstRound.end(), 0U), firstRound.end());
    std::vector<u32> sortedFirstRound = firstRound;
    std::sort(sortedFirstRound.begin(), sortedFirstRound.end());
    EXPECT_EQ(std::unique(sortedFirstRound.begin(), sortedFirstRound.end()), sortedFirstRound.end());
    EXPECT_EQ(
        sortedFirstRound, std::vector<u32>({1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U}));
    EXPECT_EQ(secondRound, std::vector<u32>({9U}));
}
} // namespace checker
