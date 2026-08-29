/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include "graph/tensor.h"
#include <dlfcn.h>
#define private public
#define protected public
#include "tiling_api.h"
#include "platform_stub.h"
#include "include/adv_api/hccl/internal/hccl_tiling_msg.h"
#include "include/adv_api/hccl/hccl_tiling.h"
#include "include/adv_api/hccl/hccl_common.h"
#include "tiling/platform/platform_ascendc.h"
#include "impl/adv_api/tiling/hccl/hccl_symbol_loader.h"
using namespace ge;
using namespace std;
using namespace optiling;
using namespace AscendC;
using namespace HcclApi;

class TestHcclTiling : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(TestHcclTiling, Mc2CcTilingConfig_normal)
{
    ::Mc2InitTiling initTilingInner;
    ::Mc2CcTiling ccTilingInner;
    string groupName = "test";
    uint32_t opType = 1;
    string algConfig = "fullmesh";
    uint32_t reduceType = 1;
    Mc2CcTilingConfig ccTilingConfig(groupName, opType, algConfig, reduceType);
    EXPECT_EQ(ccTilingConfig.SetDebugMode(1U), EXIT_SUCCESS);
    EXPECT_EQ(ccTilingConfig.SetQueueNum(40U), EXIT_SUCCESS);
    EXPECT_EQ(ccTilingConfig.SetCommBlockNum(48U), EXIT_SUCCESS);
    uint32_t ret = ccTilingConfig.GetTiling(initTilingInner);
    EXPECT_EQ(ret, EXIT_SUCCESS);

    opType = 0;
    EXPECT_NE(ccTilingConfig.SetOpType(opType), EXIT_SUCCESS);
    groupName = "test1";
    EXPECT_EQ(ccTilingConfig.SetGroupName(groupName), EXIT_SUCCESS);
    algConfig = "doublering";
    EXPECT_EQ(ccTilingConfig.SetAlgConfig(algConfig), EXIT_SUCCESS);
    reduceType = 0;
    EXPECT_EQ(ccTilingConfig.SetReduceType(reduceType), EXIT_SUCCESS);
    uint8_t stepSize = 1;
    if (platform_ascendc::PlatformAscendCManager::GetInstance()->GetCurNpuArch() == NpuArch::DAV_2201) {
        EXPECT_EQ(ccTilingConfig.SetStepSize(stepSize), EXIT_SUCCESS);
    } else {
        EXPECT_EQ(ccTilingConfig.SetStepSize(stepSize), EXIT_FAILURE);
    }
    uint8_t skipLocalRankCopy = 1;
    EXPECT_EQ(ccTilingConfig.SetSkipLocalRankCopy(skipLocalRankCopy), EXIT_SUCCESS);
    uint8_t skipBufferWindowCopy = 1;
    EXPECT_EQ(ccTilingConfig.SetSkipBufferWindowCopy(skipBufferWindowCopy), EXIT_SUCCESS);
    uint8_t commEngine = 1;
    EXPECT_EQ(ccTilingConfig.SetCommEngine(commEngine), EXIT_SUCCESS);
    EXPECT_EQ(ccTilingConfig.GetTiling(ccTilingInner), EXIT_SUCCESS);
}

TEST_F(TestHcclTiling, Mc2CcTilingConfig_failed1)
{
    // member variable boundary value validation use case
    ::Mc2CcTiling ccTilingInner;
    string groupName = "test";
    uint32_t opType = 1;
    string algConfig = "fullmesh";
    uint32_t reduceType = 1;
    Mc2CcTilingConfig ccTilingConfig(groupName, opType, algConfig, reduceType);
    EXPECT_NE(ccTilingConfig.SetOpType(static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL)), EXIT_SUCCESS);
    string value129 = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
                      "678901234567890123456789012345678";
    string value128 = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
                      "67890123456789012345678901234567";
    EXPECT_NE(ccTilingConfig.SetGroupName(value129), EXIT_SUCCESS);
    EXPECT_EQ(ccTilingConfig.SetGroupName(value128), EXIT_SUCCESS);
    EXPECT_NE(ccTilingConfig.SetAlgConfig(value129), EXIT_SUCCESS);
    EXPECT_EQ(ccTilingConfig.SetAlgConfig(value128), EXIT_SUCCESS);
    EXPECT_NE(ccTilingConfig.SetReduceType(HCCL_REDUCE_RESERVED), EXIT_SUCCESS);
    EXPECT_NE(ccTilingConfig.SetSkipLocalRankCopy(2), EXIT_SUCCESS);
    EXPECT_NE(ccTilingConfig.SetSkipBufferWindowCopy(3), EXIT_SUCCESS);
    EXPECT_NE(ccTilingConfig.GetTiling(ccTilingInner), EXIT_SUCCESS);
}

TEST_F(TestHcclTiling, Mc2CcTilingConfig_failed2)
{
    // when opType is of the reduce type, reduceType must comply with the range validation cases
    ::Mc2InitTiling initTilingInner;
    string groupName = "test";
    uint32_t opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLREDUCE);
    string algConfig = "fullmesh";
    uint32_t reduceType = HCCL_REDUCE_RESERVED;
    Mc2CcTilingConfig ccTilingConfig(groupName, opType, algConfig, reduceType);
    uint32_t ret = ccTilingConfig.GetTiling(initTilingInner);
    EXPECT_NE(ret, EXIT_SUCCESS);

    // when opType is not of the reduce type, there are not range requirements for reduceType
    EXPECT_EQ(ccTilingConfig.SetOpType(static_cast<uint32_t>(HcclCMDType::HCCL_CMD_SEND)), EXIT_SUCCESS);
    ret = ccTilingConfig.GetTiling(initTilingInner);
    EXPECT_EQ(ret, EXIT_SUCCESS);
}

TEST_F(TestHcclTiling, Mc2CcTilingConfig_failed3)
{
    // do not invoke the initilazation validation test case
    ::Mc2InitTiling initTilingInner;
    ::Mc2CcTiling ccTilingInner;
    string groupName = "test";
    uint32_t opType = 1;
    string algConfig = "fullmesh";
    uint32_t reduceType = 1;
    Mc2CcTilingConfig ccTilingConfig(groupName, opType, algConfig, reduceType);
    EXPECT_NE(ccTilingConfig.GetTiling(ccTilingInner), EXIT_SUCCESS);
}

TEST_F(TestHcclTiling, Mc2CcTilingConfig_SetReduceType_ReduceOp)
{
    const char* groupName = "testGroup";
    uint32_t opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    std::string algConfig = "ReduceScatter=level0:doublering";
    uint32_t reduceType = static_cast<uint32_t>(HcclReduceOp::HCCL_REDUCE_RESERVED);
    uint8_t srcDataType = static_cast<uint32_t>(HcclDataType::HCCL_DATA_TYPE_FP16);
    uint8_t dstDataType = static_cast<uint32_t>(HcclDataType::HCCL_DATA_TYPE_FP16);

    Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType, srcDataType, dstDataType);
    EXPECT_EQ(mc2CcTilingConfig.SetReduceType(HcclReduceOp::HCCL_REDUCE_SUM, srcDataType, dstDataType), EXIT_SUCCESS);

    // invalid dstDataType
    dstDataType = -1;
    EXPECT_EQ(mc2CcTilingConfig.SetReduceType(HcclReduceOp::HCCL_REDUCE_SUM, srcDataType, dstDataType), EXIT_FAILURE);

    // invalid srcDataType
    dstDataType = static_cast<uint32_t>(HcclDataType::HCCL_DATA_TYPE_FP16);
    srcDataType = -1;
    EXPECT_EQ(mc2CcTilingConfig.SetReduceType(HcclReduceOp::HCCL_REDUCE_SUM, srcDataType, dstDataType), EXIT_FAILURE);
}

TEST_F(TestHcclTiling, Mc2CcTilingConfig_SetReduceType_NotReduceOp)
{
    const char* groupName = "testGroup";
    uint32_t opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    std::string algConfig = "AllGather=level0:doublering";
    uint32_t reduceType = static_cast<uint32_t>(HcclReduceOp::HCCL_REDUCE_RESERVED);
    uint8_t srcDataType = static_cast<uint32_t>(HcclDataType::HCCL_DATA_TYPE_FP16);
    uint8_t dstDataType = static_cast<uint32_t>(HcclDataType::HCCL_DATA_TYPE_FP16);

    Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType, srcDataType, dstDataType);
    EXPECT_EQ(mc2CcTilingConfig.SetReduceType(HcclReduceOp::HCCL_REDUCE_SUM, srcDataType, dstDataType), EXIT_SUCCESS);
}

TEST_F(TestHcclTiling, Mc2CcTilingConfig_multiTiling)
{
    ::Mc2InitTiling initTilingInner;
    ::Mc2CcTiling ccTilingInner;
    Mc2CcTilingConfig ccTilingConfig("test", 1, "fullmesh", 1);
    EXPECT_EQ(ccTilingConfig.GetTiling(initTilingInner), EXIT_SUCCESS);
    for (uint32_t i = 0; i < 8; ++i) {
        EXPECT_EQ(ccTilingConfig.GetTiling(ccTilingInner), EXIT_SUCCESS);
    }
    EXPECT_NE(ccTilingConfig.GetTiling(ccTilingInner), EXIT_SUCCESS);
}

class TestHcclTilingCcuKfc : public TestHcclTiling {
protected:
    void ExpectInitTilingVersion(
        uint32_t opType, const std::string& algConfig, uint8_t commEngine, uint32_t expectedVersion)
    {
        ::Mc2InitTiling initTiling{};
        Mc2CcTilingConfig tilingConfig("test", opType, algConfig, 0, 0, 0, commEngine);

        ASSERT_EQ(tilingConfig.GetTiling(initTiling), EXIT_SUCCESS);
        const auto* tilingInner = reinterpret_cast<const Mc2InitTilingInner*>(&initTiling);
        EXPECT_EQ(tilingInner->version, expectedVersion);
    }
};

TEST_F(TestHcclTilingCcuKfc, AllToAllVCcuSchedUsesCcuNewVersion)
{
    ExpectInitTilingVersion(
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV), "CcuSchedAllToAllVSoleMesh", 6U,
        INIT_TILING_CCU_NEW_VERSION);
}

TEST_F(TestHcclTilingCcuKfc, AllToAllCcuSchedUsesCcuNewVersion)
{
    ExpectInitTilingVersion(
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLTOALL), "CcuSchedAllToAllSoleMesh", 6U,
        INIT_TILING_CCU_NEW_VERSION);
}

TEST_F(TestHcclTilingCcuKfc, UnsupportedCommEngineUsesDefaultVersion)
{
    ExpectInitTilingVersion(
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER), "CcuAllGatherMesh1DMem2Mem", 4U, INIT_TILING_VERSION);
}

TEST_F(TestHcclTilingCcuKfc, UnregisteredAlgorithmUsesDefaultVersion)
{
    ExpectInitTilingVersion(
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER), "CcuUnregisteredAlgorithm", 6U, INIT_TILING_VERSION);
}

TEST_F(TestHcclTilingCcuKfc, AllGatherCcuMsUsesCcuNewVersion)
{
    ExpectInitTilingVersion(
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER), "CcuAllGatherMesh1DMem2Mem", 5U,
        INIT_TILING_CCU_NEW_VERSION);
}

TEST_F(TestHcclTilingCcuKfc, AllGatherCcuSchedUsesCcuNewVersion)
{
    ExpectInitTilingVersion(
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER), "CcuAllGatherMesh1DMem2Mem", 6U,
        INIT_TILING_CCU_NEW_VERSION);
}

TEST_F(TestHcclTilingCcuKfc, ReduceScatterCcuMsUsesCcuNewVersion)
{
    ExpectInitTilingVersion(
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER), "CcuSchedReduceScatterSoleMesh", 5U,
        INIT_TILING_CCU_NEW_VERSION);
}

TEST_F(TestHcclTilingCcuKfc, ReduceScatterCcuSchedUsesCcuNewVersion)
{
    ExpectInitTilingVersion(
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER), "CcuSchedReduceScatterSoleMesh", 6U,
        INIT_TILING_CCU_NEW_VERSION);
}

// ============================================================================
// 以下用例覆盖 commit aaa3d79e5 的改动:
// 修复 hccl_tiling.cpp 中不安全的动态库加载。
//   1. HcclSymbolLoader::Load 新增 pathName 参数,通过 realpath 校验 so 的真实
//      路径后再 dlopen,非法/不存在的路径返回 nullptr。
//   2. SetDevType 通过 ASCEND_HOME_PATH 环境变量拼接 /lib64/ 作为加载路径。
// ============================================================================

class TestHcclSymbolLoader : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    // 保存并在用例结束后恢复 ASCEND_HOME_PATH,避免污染其他用例。
    void SetUp() override
    {
        const char* env = std::getenv("ASCEND_HOME_PATH");
        hasOldEnv_ = (env != nullptr);
        if (hasOldEnv_) {
            oldEnv_ = env;
        }
    }
    void TearDown() override
    {
        if (hasOldEnv_) {
            setenv("ASCEND_HOME_PATH", oldEnv_.c_str(), 1);
        } else {
            unsetenv("ASCEND_HOME_PATH");
        }
    }

    bool hasOldEnv_ = false;
    std::string oldEnv_;
};

// Load 在 pathName 为合法目录、so 真实存在时,能够成功加载到符号。
TEST_F(TestHcclSymbolLoader, Load_validPath_success)
{
    const char* homePath = std::getenv("ASCEND_HOME_PATH");
    ASSERT_NE(homePath, nullptr);
    std::string pathName = std::string(homePath) + "/lib64/";

    auto func =
        HcclSymbolLoader::GetInstance().Load<void (*)(char*, uint32_t)>("libruntime.so", "rtGetSocVersion", pathName);
    EXPECT_NE(func, nullptr);
}

// Load 命中缓存:同一 so + func 第二次加载走 symbolMap_ 缓存分支,返回相同指针。
TEST_F(TestHcclSymbolLoader, Load_cached_returnsSamePointer)
{
    const char* homePath = std::getenv("ASCEND_HOME_PATH");
    ASSERT_NE(homePath, nullptr);
    std::string pathName = std::string(homePath) + "/lib64/";

    auto func1 =
        HcclSymbolLoader::GetInstance().Load<void (*)(char*, uint32_t)>("libruntime.so", "rtGetSocVersion", pathName);
    auto func2 =
        HcclSymbolLoader::GetInstance().Load<void (*)(char*, uint32_t)>("libruntime.so", "rtGetSocVersion", pathName);
    EXPECT_NE(func1, nullptr);
    EXPECT_EQ(func1, func2);
}

// Load 的核心安全修复:pathName 拼接出的 so 路径不存在时,realpath 失败,返回 nullptr。
TEST_F(TestHcclSymbolLoader, Load_invalidPath_returnsNull)
{
    std::string pathName = "/path/that/does/not/exist/";
    auto func = HcclSymbolLoader::GetInstance().Load<void (*)(char*, uint32_t)>(
        "libruntime_not_exist.so", "rtGetSocVersion", pathName);
    EXPECT_EQ(func, nullptr);
}

// Load 的路径校验同样拦截空路径形式的不安全加载。
TEST_F(TestHcclSymbolLoader, Load_emptyPath_returnsNull)
{
    std::string pathName = "";
    auto func = HcclSymbolLoader::GetInstance().Load<void (*)(char*, uint32_t)>(
        "libruntime_not_exist.so", "rtGetSocVersion", pathName);
    EXPECT_EQ(func, nullptr);
}

// so 存在但符号不存在时,dlsym 失败返回 nullptr。
TEST_F(TestHcclSymbolLoader, Load_invalidSymbol_returnsNull)
{
    const char* homePath = std::getenv("ASCEND_HOME_PATH");
    ASSERT_NE(homePath, nullptr);
    std::string pathName = std::string(homePath) + "/lib64/";

    auto func = HcclSymbolLoader::GetInstance().Load<void (*)(char*, uint32_t)>(
        "libruntime.so", "rtSymbolDefinitelyNotExist", pathName);
    EXPECT_EQ(func, nullptr);
}

// SetDevType 经由 GetTiling(Mc2InitTiling) 调用:ASCEND_HOME_PATH 已正确设置时成功。
TEST_F(TestHcclSymbolLoader, GetInitTiling_withValidAscendHome_success)
{
    const char* homePath = std::getenv("ASCEND_HOME_PATH");
    ASSERT_NE(homePath, nullptr);

    ::Mc2InitTiling initTilingInner;
    Mc2CcTilingConfig ccTilingConfig("test", 1, "fullmesh", 1);
    EXPECT_EQ(ccTilingConfig.GetTiling(initTilingInner), EXIT_SUCCESS);
}

// SetDevType 的环境变量校验:ASCEND_HOME_PATH 未设置时,getenv 返回 nullptr。
TEST_F(TestHcclSymbolLoader, GetInitTiling_ascendHomeUnset_failure)
{
    unsetenv("ASCEND_HOME_PATH");
    ASSERT_EQ(std::getenv("ASCEND_HOME_PATH"), nullptr);

    ::Mc2InitTiling initTilingInner;
    Mc2CcTilingConfig ccTilingConfig("test", 1, "fullmesh", 1);
    EXPECT_EQ(ccTilingConfig.GetTiling(initTilingInner), EXIT_FAILURE);
}

// SetDevType 的环境变量校验:ASCEND_HOME_PATH 为空字符串时,getenv 返回非空指针但首字符为 '\0'。
TEST_F(TestHcclSymbolLoader, GetInitTiling_ascendHomeEmpty_failure)
{
    setenv("ASCEND_HOME_PATH", "", 1);
    const char* homePath = std::getenv("ASCEND_HOME_PATH");
    ASSERT_NE(homePath, nullptr);
    ASSERT_EQ(homePath[0], '\0');

    ::Mc2InitTiling initTilingInner;
    Mc2CcTilingConfig ccTilingConfig("test", 1, "fullmesh", 1);
    EXPECT_EQ(ccTilingConfig.GetTiling(initTilingInner), EXIT_FAILURE);
}
