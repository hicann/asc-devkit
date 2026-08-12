/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_ALG_ENV_CONFIG_H
#define HCCL_ALG_ENV_CONFIG_H

#include <vector>
#include <map>
#include <hccl/hccl_types.h>
#include "hccl/base.h"
#include "alg_type.h"
#include "hccl_common.h"
namespace mc2_ops_hccl {

constexpr u32 MAX_LEN_OF_DIGIT_ENV = 10; // 数字环境变量最大长度

constexpr u32 HCCL_RETRY_ENABLE_LEVEL_0 = 0;   // HCCL 重执行层级0
constexpr u32 HCCL_RETRY_ENABLE_LEVEL_1 = 1;   // HCCL 重执行层级1
constexpr u32 HCCL_RETRY_ENABLE_LEVEL_2 = 2;   // HCCL 重执行层级2
constexpr u32 HCCL_RETRY_ENABLE_LEVEL_NUM = 3; // HCCL 重执行层级最多3级

enum class DeterministicEnableLevel {
    DETERMINISTIC_DISABLE = 0, // 不支持确定性
    DETERMINISTIC_ENABLE,      // 支持确定性，不支持规约保序
    DETERMINISTIC_STRICT       // 支持确定性以及规约保序
};

struct AlgEnvConfig {
    // 初始化标识
    bool initialized;

    bool interHccsDisable;
    bool enableEntryLog;
    u32 intraRoceSwitch; // server内的通信方式 与intraPcieSwitch组合使用，默认为0
    u8 hcclDeterministic;
    bool aicpuUnfold;
    uint8_t aicpuCacheEnable;
    bool aivMode;
    bool aivOnlyMode;
    bool ccuMSMode;
    bool ccuSchedMode;
    bool enableFfts;
    bool execTimeOutSet;
    double execTimeout;
    bool hcclRetryConfig[HCCL_RETRY_ENABLE_LEVEL_NUM];
    std::map<HcclCMDType, std::vector<HcclAlgoType>> hcclAlgoConfig;

    AlgEnvConfig() { SetDefaultParams(); }
    void SetDefaultParams()
    {
        initialized = false;
        interHccsDisable = false;
        enableEntryLog = false;
        intraRoceSwitch = 0; // server内的通信方式 与intraPcieSwitch组合使用，默认为0
        hcclDeterministic = static_cast<u8>(
            DeterministicEnableLevel::
                DETERMINISTIC_DISABLE); // 确定性配置 0：不支持；1：支持确定性不支持规约保序；2：支持确定性&规约保序
        enableFfts = true;
        aicpuCacheEnable = 1; // 默认开启aicpu cache (只有当aicpuUnfold为true时才生效)
        aivOnlyMode = false;
        execTimeOutSet = false;
        execTimeout = 0;
        // 环境变量参数
        for (u32 opType = 0; opType < static_cast<u32>(HcclCMDType::HCCL_CMD_MAX); opType++) {
            hcclAlgoConfig[static_cast<HcclCMDType>(opType)] =
                std::vector<HcclAlgoType>(HCCL_ALGO_LEVEL_NUM, HcclAlgoType::HCCL_ALGO_TYPE_DEFAULT);
        }
    }
};

const std::map<HcclAlgoType, std::string> HcclAlgoTypeMap = {
    {HcclAlgoType::HCCL_ALGO_TYPE_DEFAULT, "default"},
    {HcclAlgoType::HCCL_ALGO_TYPE_RING, "ring"},
    {HcclAlgoType::HCCL_ALGO_TYPE_PIPELINE, "pipeline"},
    {HcclAlgoType::HCCL_ALGO_TYPE_FULLMESH, "fullmesh"},
    {HcclAlgoType::HCCL_ALGO_TYPE_HDR, "H-D_R"},
    {HcclAlgoType::HCCL_ALGO_TYPE_PAIRWISE, "pairwise"},
    {HcclAlgoType::HCCL_ALGO_TYPE_NHR, "NHR"},
    {HcclAlgoType::HCCL_ALGO_TYPE_NHR_V1, "NHR_V1"},
    {HcclAlgoType::HCCL_ALGO_TYPE_AHC, "AHC"},
    {HcclAlgoType::HCCL_ALGO_TYPE_AHC_BROKE, "AHC_BROKE"},
    {HcclAlgoType::HCCL_ALGO_TYPE_NB, "NB"},
    {HcclAlgoType::HCCL_ALGO_TYPE_NULL, "null"},
    {HcclAlgoType::HCCL_ALGO_TYPE_NA, "NA"},
};

HcclResult InitEnvConfig();

HcclResult ParseHcclAlgo();

HcclResult SetHcclAlgoConfig(const std::string& hcclAlgo);

HcclResult ResetAlgEnvConfigInitState();

const std::vector<HcclAlgoType> GetExternalInputHcclAlgoConfig(HcclCMDType opType = HcclCMDType::HCCL_CMD_ALL);

HcclResult SetCommonAlgType(std::vector<std::string>& algos);

HcclResult SetSpecificAlgType(std::vector<std::string>& algos);

HcclResult ParserHcclAlgoLevel(const std::string& algoLevel, u32& level, HcclAlgoType& algoType);

HcclResult ParseAlgoString(std::string opName, std::string& algoString, std::vector<HcclAlgoType>& algType);

HcclResult SplitHcclOpType(const std::string& algoConfig, std::vector<std::string>& algos);

HcclResult CheckAlgoConfigValid(std::vector<std::string>& algos, bool& anyCommonConfig, bool& anySpecificConfig);

HcclResult SplitHcclAlgoLevel(const std::string& algoConfig, std::vector<std::string>& algos);

HcclResult ParseIntraLinkType();

HcclResult ParseDeterministic();

HcclResult ParseEntryLogEnable();

HcclResult ParseInterLinkType();

HcclResult ParseOpExpansion();

HcclResult ParseExecTimeout();

HcclResult SplitHcclRetryEnable(const std::string& retryConfig, std::vector<std::string>& retryEnables);

HcclResult CollectRetryEnableFromConfig(const std::vector<std::string>& retryEnables);

HcclResult ParseRetryEnable();

const u32& GetExternalInputIntraRoceSwitch();

const bool& GetExternalInputHcclAicpuUnfold();

const uint8_t& GetExternalInputHcclAicpuCacheEnable();

const bool& GetExternalInputHcclAivMode();

const bool& GetExternalInputHcclAivOnlyMode();

const bool& GetExternalInputHcclCcuMSMode();

const bool& GetExternalInputHcclCcuSchedMode();

const bool& GetExternalInputInterHccsDisable();

const bool& GetExternalInputIntraServerRetryEnable();

const bool& GetExternalInputInterServerRetryEnable();

const bool& GetExternalInputInterSuperPodRetryEnable();

const bool& GetExternalInputHcclEnableEntryLog();

const std::map<HcclCMDType, std::vector<HcclAlgoType>> GetExternalInputHcclAlgoConfigAllType();

bool GetExternalInputExecTimeout(double& execTimeOut);

bool RunIndependentOpExpansion(DevType deviceType);
} // namespace mc2_ops_hccl

#endif // HCCL_ALG_ENV_CONFIG_H
