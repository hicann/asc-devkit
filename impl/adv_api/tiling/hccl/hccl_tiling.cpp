/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hccl_tiling.cpp
 * \brief
 */

#include "tiling/platform/platform_ascendc.h"
#include "include/adv_api/hccl/hccl_tiling.h"
#include "include/adv_api/hccl/hccl_common.h"
#include "securec.h"
#include "../../detail/host_log.h"
#include "include/adv_api/hccl/internal/hccl_msg.h"
#include "include/adv_api/hccl/internal/hccl_tiling_msg.h"
#include "tiling/platform/platform_ascendc.h"
#include "hccl_symbol_loader.h"
#include <set>

using namespace std;
using namespace HcclApi;

namespace AscendC {
namespace {

static const std::set<std::string> REGISTERED_CCU_ALGORITHMS = {
    "CcuSchedAllGatherSoleMesh",     "CcuSchedAllGatherConcurMeshNHRMultiLink",
    "CcuSchedReduceScatterSoleMesh", "CcuSchedAllToAllSoleMesh",
    "CcuSchedAllToAllVSoleMesh",     "CcuSchedAllReduceSoleMesh"};

bool IsCcuAlgorithmRegistered(const std::string& algConfig, uint8_t commEngine)
{
    if (commEngine != 5U && commEngine != 6U) {
        return false;
    }
    return REGISTERED_CCU_ALGORITHMS.count(algConfig) != 0U;
}

void PrintMc2InitTiling(const Mc2InitTilingInner& tiling)
{
    TILING_LOG_DEBUG("Mc2InitTiling msg begin.");
    TILING_LOG_DEBUG("Mc2InitTiling msg version:%u", tiling.version);
    TILING_LOG_DEBUG("Mc2InitTiling msg mc2HcommCnt:%u", tiling.mc2HcommCnt);
    for (uint32_t i = 0; i < tiling.mc2HcommCnt; i++) {
        TILING_LOG_DEBUG("Mc2InitTiling msg offset%u:%u", i, tiling.offset[i]);
    }
    TILING_LOG_DEBUG("Mc2InitTiling msg debugMode:%u", tiling.debugMode);
    TILING_LOG_DEBUG("Mc2InitTiling msg preparePosition:%u", tiling.preparePosition);
    TILING_LOG_DEBUG("Mc2InitTiling msg queueNum:%u", tiling.queueNum);
    TILING_LOG_DEBUG("Mc2InitTiling msg commBlockNum:%u", tiling.commBlockNum);
    TILING_LOG_DEBUG("Mc2InitTiling msg devType:%u", tiling.devType);
    TILING_LOG_DEBUG("Mc2InitTiling msg end.");
}

void PrintMc2CcTiling(const Mc2CcTilingInner& tiling)
{
    TILING_LOG_DEBUG("Mc2CcTiling msg begin.");
    TILING_LOG_DEBUG("Mc2CcTiling msg skipLocalRankCopy:%u", tiling.skipLocalRankCopy);
    TILING_LOG_DEBUG("Mc2CcTiling msg skipBufferWindowCopy:%u", tiling.skipBufferWindowCopy);
    TILING_LOG_DEBUG("Mc2CcTiling msg stepSize:%u", tiling.stepSize);
    TILING_LOG_DEBUG("Mc2CcTiling msg version:%u", tiling.version);
    TILING_LOG_DEBUG("Mc2CcTiling msg groupName:%s", tiling.groupName);
    TILING_LOG_DEBUG("Mc2CcTiling msg algConfig:%s", tiling.algConfig);
    TILING_LOG_DEBUG("Mc2CcTiling msg opType:%u", tiling.opType);
    TILING_LOG_DEBUG("Mc2CcTiling msg reduceType:%u", tiling.reduceType);
    TILING_LOG_DEBUG("Mc2CcTiling msg dstDataType:%u", tiling.dstDataType);
    TILING_LOG_DEBUG("Mc2CcTiling msg srcDataType:%u", tiling.srcDataType);
    TILING_LOG_DEBUG("Mc2CcTiling msg commEngine:%u", tiling.commEngine);
    TILING_LOG_DEBUG("Mc2CcTiling msg end.");
}

uint32_t SetDevType(Mc2InitTilingInner* tilingInner)
{
    ASCENDC_HOST_ASSERT(tilingInner != nullptr, return EXIT_FAILURE, "tilingInner must not be nullptr.");
    const char* homePath = std::getenv("ASCEND_HOME_PATH");
    ASCENDC_HOST_ASSERT(
        (homePath != nullptr && homePath[0] != '\0'), return EXIT_FAILURE, "ASCEND_HOME_PATH is not set or empty.");
    std::string pathName(homePath);
    pathName += "/lib64/";
    auto getSocVerFunc =
        HcclSymbolLoader::GetInstance().Load<void (*)(char*, uint32_t)>("libruntime.so", "rtGetSocVersion", pathName);
    ASCENDC_HOST_ASSERT(getSocVerFunc != nullptr, return EXIT_FAILURE, "Failed to get soc version.");
    char socVersion[50];
    getSocVerFunc(socVersion, sizeof(socVersion));
    std::string devType = std::string(socVersion);
    if (devType.find("Ascend910B") != std::string::npos) {
        tilingInner->devType = static_cast<uint8_t>(platform_ascendc::SocVersion::ASCEND910B);
    } else {
        tilingInner->devType = UINT8_MAX;
    }
    return EXIT_SUCCESS;
}

uint32_t UpdateMc2InitTiling(uint64_t initTilingAddr, uint64_t ccTilingAddr)
{
    Mc2InitTilingInner* tilingInner = reinterpret_cast<Mc2InitTilingInner*>(static_cast<uintptr_t>(initTilingAddr));
    ASCENDC_HOST_ASSERT(tilingInner != nullptr, return EXIT_FAILURE, "tilingInner must not be nullptr.");
    uint32_t& cnt = tilingInner->mc2HcommCnt;
    ASCENDC_HOST_ASSERT(
        cnt < MAX_CC_TILING_NUM, return EXIT_FAILURE, "mc2HcommCnt(%u) must be less than or equal %u.", cnt + 1U,
        MAX_CC_TILING_NUM);
    tilingInner->offset[cnt] = static_cast<uint32_t>(ccTilingAddr - initTilingAddr);
    TILING_LOG_INFO(
        "Update Mc2InitTiling, index:%u, offset:%u, initTilingAddr:%#lx, ccTilingAddr:%#lx.", cnt,
        tilingInner->offset[cnt], initTilingAddr, ccTilingAddr);
    ++cnt;
    PrintMc2InitTiling(*tilingInner);
    return EXIT_SUCCESS;
}
} // namespace

Mc2CcTilingConfig::Mc2CcTilingConfig(
    const std::string& groupName, uint32_t opType, const std::string& algConfig, uint32_t reduceType,
    uint8_t dstDataType, uint8_t srcDataType, uint8_t commEngine)
{
    impl_.groupName_ = groupName;
    impl_.opType_ = opType;
    impl_.algConfig_ = algConfig;
    impl_.reduceType_ = reduceType;
    impl_.srcDataType_ = srcDataType;
    impl_.dstDataType_ = dstDataType;
    impl_.commEngine_ = commEngine;
    TILING_LOG_INFO(
        "Init groupName_:%s, opType_:%u, algConfig_:%s, reduceType_:%u, srcDataType_:%u, dstDataType_:%u, "
        "commEngine_:%u.",
        impl_.groupName_.c_str(), impl_.opType_, impl_.algConfig_.c_str(), impl_.reduceType_, impl_.srcDataType_,
        impl_.dstDataType_, impl_.commEngine_);
}

Mc2CcTilingConfig::~Mc2CcTilingConfig() = default;

uint32_t Mc2CcTilingConfig::GetTiling(::Mc2InitTiling& tiling)
{
    // It is not a reduce type or a reduce type, and the reduce type is valid
    const bool reduceFlag =
        (impl_.opType_ == static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLREDUCE) ||
         impl_.opType_ == static_cast<uint8_t>(HcclCMDType::HCCL_CMD_REDUCE) ||
         impl_.opType_ == static_cast<uint8_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER));
    ASCENDC_HOST_ASSERT(
        !reduceFlag || impl_.reduceType_ < HCCL_REDUCE_RESERVED, return EXIT_FAILURE,
        "when opType(%u) is reduce, reduceType must be less than %u.", impl_.opType_,
        static_cast<uint8_t>(HCCL_REDUCE_RESERVED));

    impl_.initTilingAddr_ = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&tiling));
    Mc2InitTilingInner* tilingInner = reinterpret_cast<Mc2InitTilingInner*>(&tiling);
    tilingInner->version = INIT_TILING_VERSION;
    tilingInner->mc2HcommCnt = 0;
    tilingInner->debugMode = impl_.debugMode_;
    tilingInner->preparePosition = 0;
    tilingInner->queueNum = impl_.queueNum_;
    tilingInner->commBlockNum = impl_.commBlockNum_;
    for (uint32_t i = 0; i < MAX_CC_TILING_NUM; i++) {
        tilingInner->offset[i] = 0;
    }
    if (IsCcuAlgorithmRegistered(impl_.algConfig_, impl_.commEngine_)) {
        tilingInner->version = INIT_TILING_CCU_NEW_VERSION;
    }
    ASCENDC_HOST_ASSERT(SetDevType(tilingInner) == EXIT_SUCCESS, return EXIT_FAILURE, "SetDevType failed.");

    TILING_LOG_DEBUG("Mc2InitTiling addr %#lx", impl_.initTilingAddr_);
    PrintMc2InitTiling(*tilingInner);
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::GetTiling(::Mc2CcTiling& tiling)
{
    ASCENDC_HOST_ASSERT(impl_.initTilingAddr_ != 0, return EXIT_FAILURE, "must be set Mc2InitTiling first.");
    ASCENDC_HOST_ASSERT(
        impl_.groupName_.length() <= GROUP_NAME_SIZE, return EXIT_FAILURE,
        "groupName(%s) must be less than or equal %u.", impl_.groupName_.c_str(), GROUP_NAME_SIZE);
    ASCENDC_HOST_ASSERT(
        impl_.algConfig_.length() <= ALG_CONFIG_SIZE, return EXIT_FAILURE,
        "algConfig(%s) must be less than or equal %u.", impl_.algConfig_.c_str(), ALG_CONFIG_SIZE);
    bool isValid = (0 < impl_.opType_) && (impl_.opType_ < static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALL));
    ASCENDC_HOST_ASSERT(
        isValid, return EXIT_FAILURE, "opType(%u) must be less than %u, and not 0.", impl_.opType_,
        static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALL));
    ASCENDC_HOST_ASSERT(
        impl_.reduceType_ < HCCL_REDUCE_RESERVED, return EXIT_FAILURE, "reduceType(%u) must be less than %u.",
        impl_.reduceType_, static_cast<uint8_t>(HCCL_REDUCE_RESERVED));
    ASCENDC_HOST_ASSERT(
        impl_.dstDataType_ < HCCL_DATA_TYPE_RESERVED, return EXIT_FAILURE, "dstDataType(%u) must be less than %u.",
        impl_.dstDataType_, static_cast<uint8_t>(HCCL_DATA_TYPE_RESERVED));
    ASCENDC_HOST_ASSERT(
        impl_.srcDataType_ < HCCL_DATA_TYPE_RESERVED, return EXIT_FAILURE, "srcDataType(%u) must be less than %u.",
        impl_.srcDataType_, static_cast<uint8_t>(HCCL_DATA_TYPE_RESERVED));

    Mc2CcTilingInner* tilingInner = reinterpret_cast<Mc2CcTilingInner*>(&tiling);
    tilingInner->skipLocalRankCopy = impl_.skipLocalRankCopy_;
    tilingInner->skipBufferWindowCopy = impl_.skipBufferWindowCopy_;
    tilingInner->stepSize = impl_.stepSize_;
    tilingInner->version = 1U;
    (void)memset_s(tilingInner->reserved, sizeof(tilingInner->reserved), 0, sizeof(tilingInner->reserved));
    auto ret = strcpy_s(tilingInner->groupName, sizeof(tilingInner->groupName), impl_.groupName_.c_str());
    ASCENDC_HOST_ASSERT(ret == EOK, return EXIT_FAILURE, "groupName(%s) copy failed.", impl_.groupName_.c_str());
    ret = strcpy_s(tilingInner->algConfig, sizeof(tilingInner->algConfig), impl_.algConfig_.c_str());
    ASCENDC_HOST_ASSERT(ret == EOK, return EXIT_FAILURE, "algConfig(%s) copy failed.", impl_.algConfig_.c_str());
    tilingInner->opType = impl_.opType_;
    tilingInner->reduceType = impl_.reduceType_;
    tilingInner->srcDataType = impl_.srcDataType_;
    tilingInner->dstDataType = impl_.dstDataType_;
    tilingInner->commEngine = impl_.commEngine_;
    uint64_t ccTilingAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&tiling));
    ASCENDC_HOST_ASSERT(
        UpdateMc2InitTiling(impl_.initTilingAddr_, ccTilingAddr) == EXIT_SUCCESS, return EXIT_FAILURE,
        "UpdateMc2InitTiling failed.");
    PrintMc2CcTiling(*tilingInner);

    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetOpType(uint32_t opType)
{
    bool isValid = (0 < opType) && (opType < static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALL));
    ASCENDC_HOST_ASSERT(
        isValid, return EXIT_FAILURE, "opType(%u) must be less than %u, and not 0.", opType,
        static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALL));
    impl_.opType_ = opType;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetGroupName(const std::string& groupName)
{
    ASCENDC_HOST_ASSERT(
        groupName.length() <= GROUP_NAME_SIZE, return EXIT_FAILURE, "groupName(%s) must be less than or equal %u.",
        groupName.c_str(), GROUP_NAME_SIZE);
    impl_.groupName_ = groupName;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetAlgConfig(const std::string& algConfig)
{
    ASCENDC_HOST_ASSERT(
        algConfig.length() <= ALG_CONFIG_SIZE, return EXIT_FAILURE, "algConfig(%s) must be less than or equal %u.",
        algConfig.c_str(), ALG_CONFIG_SIZE);
    impl_.algConfig_ = algConfig;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetReduceType(uint32_t reduceType, uint8_t dstDataType, uint8_t srcDataType)
{
    ASCENDC_HOST_ASSERT(
        reduceType < HCCL_REDUCE_RESERVED, return EXIT_FAILURE, "reduceType(%u) must be less than %u.", reduceType,
        static_cast<uint8_t>(HCCL_REDUCE_RESERVED));
    ASCENDC_HOST_ASSERT(
        dstDataType < HCCL_DATA_TYPE_RESERVED, return EXIT_FAILURE, "dstDataType(%u) must be less than %u.",
        dstDataType, static_cast<uint8_t>(HCCL_DATA_TYPE_RESERVED));
    ASCENDC_HOST_ASSERT(
        srcDataType < HCCL_DATA_TYPE_RESERVED, return EXIT_FAILURE, "srcDataType(%u) must be less than %u.",
        srcDataType, static_cast<uint8_t>(HCCL_DATA_TYPE_RESERVED));
    impl_.reduceType_ = reduceType;
    impl_.srcDataType_ = srcDataType;
    impl_.dstDataType_ = dstDataType;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetStepSize(uint8_t stepSize)
{
    auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    auto npuArch = ascendcPlatform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_2201) {
        // This API is only supported on Atlas A3
        impl_.stepSize_ = stepSize;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

uint32_t Mc2CcTilingConfig::SetSkipLocalRankCopy(uint8_t skipLocalRankCopy)
{
    ASCENDC_HOST_ASSERT(
        skipLocalRankCopy <= 1, return EXIT_FAILURE, "skipLocalRankCopy(%u) must be less than or equal 1.",
        skipLocalRankCopy);
    impl_.skipLocalRankCopy_ = skipLocalRankCopy;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetSkipBufferWindowCopy(uint8_t skipBufferWindowCopy)
{
    ASCENDC_HOST_ASSERT(
        skipBufferWindowCopy <= 2, return EXIT_FAILURE, "skipBufferWindowCopy(%u) must be less than or equal 2.",
        skipBufferWindowCopy);
    impl_.skipBufferWindowCopy_ = skipBufferWindowCopy;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetDebugMode(uint8_t debugMode)
{
    bool ret = (1 <= debugMode && debugMode <= 4) || (debugMode >= 250);
    ASCENDC_HOST_ASSERT(ret, return EXIT_FAILURE, "debugMode(%u) only support [1,4] or [250,255].", debugMode);
    impl_.debugMode_ = debugMode;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetQueueNum(uint16_t num)
{
    impl_.queueNum_ = num;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetCommBlockNum(uint16_t num)
{
    impl_.commBlockNum_ = num;
    return EXIT_SUCCESS;
}

uint32_t Mc2CcTilingConfig::SetCommEngine(uint8_t commEngine)
{
    impl_.commEngine_ = commEngine;
    return EXIT_SUCCESS;
}
} // namespace AscendC
