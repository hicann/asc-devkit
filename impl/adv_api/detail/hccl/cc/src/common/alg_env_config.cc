/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "alg_env_config.h"
#include <mutex>
#include <sstream>
#include <string>
#include <algorithm>

#include "log.h"
#include "adapter_error_manager_pub.h"
#include "mmpa_api.h"
#include "config_log.h"
#include "sal.h"
#include "dtype_common.h"

namespace mc2_ops_hccl {

static std::mutex g_algEnvConfigMutex;
static thread_local AlgEnvConfig g_algEnvConfig;

std::string GetEnv(mmEnvId IdName)
{
    constexpr size_t MAX_ENV_VALUE_SIZE = 1024;
    char envValue[MAX_ENV_VALUE_SIZE] = {0};
    char* mmSysGetEnvValue = envValue;
    MM_SYS_GET_ENV(IdName, mmSysGetEnvValue);
    if (mmSysGetEnvValue != nullptr && mmSysGetEnvValue[0] != '\0') {
        return std::string(mmSysGetEnvValue);
    } else {
        return "EmptyString";
    }
}

static bool IsValidTimeoutFormat(const std::string& str)
{
    if (str.empty())
        return false;

    size_t dotPos = str.find('.');
    size_t pos = 0;

    // 检查小数点前的数字
    while (pos < str.length() && pos != dotPos) {
        if (!std::isdigit(str[pos]))
            return false;
        pos++;
    }

    // 如果有小数点，检查小数部分
    if (dotPos != std::string::npos) {
        if (dotPos == 0 || dotPos == str.length() - 1)
            return false;
        size_t decimalLen = str.length() - dotPos - 1;
        if (decimalLen > 2)
            return false; // 最多2位小数

        for (size_t i = dotPos + 1; i < str.length(); i++) {
            if (!std::isdigit(str[i]))
                return false;
        }
    }

    return true;
}

HcclResult ParseExecTimeout()
{
    std::string execTimeOutEnv = GetEnv(MM_ENV_HCCL_EXEC_TIMEOUT);
    if (execTimeOutEnv == "EmptyString") {
        g_algEnvConfig.execTimeOutSet = false;
        g_algEnvConfig.execTimeout = 0;
        return HCCL_SUCCESS;
    }

    if (!IsValidTimeoutFormat(execTimeOutEnv)) {
        HCCL_WARNING(
            "[ParseExecTimeout] HCCL_EXEC_TIMEOUT[%s] format is invalid, use default.", execTimeOutEnv.c_str());
        g_algEnvConfig.execTimeOutSet = false;
        g_algEnvConfig.execTimeout = 0;
        return HCCL_E_PARA;
    }

    double execTimeOut = 0;
    if (SalStrToDouble(execTimeOutEnv, execTimeOut) != HCCL_SUCCESS) {
        HCCL_WARNING("[ParseExecTimeout] HCCL_EXEC_TIMEOUT[%s] parse failed, use default.", execTimeOutEnv.c_str());
        g_algEnvConfig.execTimeOutSet = false;
        g_algEnvConfig.execTimeout = 0;
        return HCCL_E_PARA;
    }

    g_algEnvConfig.execTimeOutSet = true;
    g_algEnvConfig.execTimeout = execTimeOut;
    return HCCL_SUCCESS;
}

bool GetExternalInputExecTimeout(double& execTimeOut)
{
    std::lock_guard<std::mutex> lock(g_algEnvConfigMutex);
    if (!g_algEnvConfig.execTimeOutSet) {
        return false;
    }

    execTimeOut = g_algEnvConfig.execTimeout;
    return true;
}

/* 入口 */
HcclResult InitEnvConfig()
{
    std::lock_guard<std::mutex> lock(g_algEnvConfigMutex);
    // 解析算子展开模式
    HcclResult ret = ParseOpExpansion();
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>(
            {GetEnv(MM_ENV_HCCL_OP_EXPANSION_MODE), "HCCL_OP_EXPANSION_MODE", "should be \"AI_CPU\""}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Init][EnvVarParam]errNo[0x%016llx] In init env variable param, parse "
            "HCCL_OP_EXPANSION_MODE failed. errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    if (g_algEnvConfig.initialized) {
        return HCCL_SUCCESS;
    }

    // 解析hcclDeterministic,是否为确定性计算
    ret = ParseDeterministic();
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>(
            {GetEnv(MM_ENV_HCCL_DETERMINISTIC), "HCCL_DETERMINISTIC", "should be true ,false or strict"}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Init][EnvVarParam]errNo[0x%016llx] In init env variable param, parse "
            "HCCL_DETERMINISTIC failed. errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    // 解析server内通信方式
    ret = ParseIntraLinkType();
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>(
            {"PCIE enable: " + std::string(GetEnv(MM_ENV_HCCL_INTRA_PCIE_ENABLE)) +
                 " or ROCE enable: " + std::string(GetEnv(MM_ENV_HCCL_INTRA_ROCE_ENABLE)),
             "HCCL_INTRA_PCIE_ENABLE or HCCL_INTRA_ROCE_ENABLE", "0 or 1 (but not both 1)"}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Init][EnvVarParam]errNo[0x%016llx] In init env variable param, parse intra "
            "comm type failed. errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    // 解析Entry日志开关
    ret = ParseEntryLogEnable();
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>({GetEnv(MM_ENV_HCCL_ENTRY_LOG_ENABLE), "HCCL_ENTRY_LOG_ENABLE", "must be 0 or 1"}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Init][EnvVarParam]errNo[0x%016llx] In init env variable param, parse "
            "HCCL_ENTRY_LOG_ENABLE failed. errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    // 解析超节点内节点间链路选择开关
    ret = ParseInterLinkType();
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>(
            {GetEnv(MM_ENV_HCCL_INTER_HCCS_DISABLE), "HCCL_INTER_HCCS_DISABLE", "should be true or false"}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Init][EnvVarParam]errNo[0x%016llx] In init env variable param, parse "
            "HCCL_INTER_HCCS_DISABLE failed. errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    // 解析重执行设置
    ret = ParseRetryEnable();
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>({GetEnv(MM_ENV_HCCL_OP_RETRY_ENABLE), "HCCL_OP_RETRY_ENABLE", "should be 0 or 1"}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Init][EnvVarParam]errNo[0x%016llx] In init env variable param, parse HCCL_OP_RETRY_ENABLE failed. "
            "errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    // 解析执行超时
    ret = ParseExecTimeout();
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>(
            {GetEnv(MM_ENV_HCCL_EXEC_TIMEOUT), "HCCL_EXEC_TIMEOUT", "a non-negative number with up to 2 decimals"}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Init][EnvVarParam]errNo[0x%016llx] In init env variable param, parse HCCL_EXEC_TIMEOUT failed. "
            "errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    // 解析算法配置
    ret = ParseHcclAlgo();
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>(
            {GetEnv(MM_ENV_HCCL_ALGO), "HCCL_ALGO",
             "level0:NA;level1:<algo> or <op0>=level0:NA;level1:<algo0>/<op1>=level0:NA;level1:<algo1>"}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Init][EnvVarParam]errNo[0x%016llx] In init env variable param, parse "
            "hccl algorithm config failed. errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    ret = InitDebugConfigByEnv();
    char* env = nullptr;
    MM_SYS_GET_ENV(MM_ENV_HCCL_DEBUG_CONFIG, env);
    std::string envValue = (env != nullptr) ? std::string(env) : "null";
    RPT_ENV_ERR(
        ret != HCCL_SUCCESS, "EI0001", std::vector<std::string>({"value", "env", "expect"}),
        std::vector<std::string>(
            {envValue, "HCCL_DEBUG_CONFIG", "ALG,TASK,RESOURCE,AIV_OPS_EXC(optionally prefixed with '^')"}));
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InitEnvParam]errNo[0x%016llx] In init environment param, parse "
            "HCCL_DEBUG_CONFIG failed. errorno[%d]",
            HCCL_ERROR_CODE(ret), ret),
        ret);

    g_algEnvConfig.initialized = true;

    return HCCL_SUCCESS;
}

HcclResult ParseHcclAlgo()
{
    std::string hcclAlgo = GetEnv(MM_ENV_HCCL_ALGO);
    if (hcclAlgo != "EmptyString") {
        CHK_RET(SetHcclAlgoConfig(hcclAlgo));
        HCCL_RUN_INFO("HCCL_ALGO set by environment to [%s]", hcclAlgo.c_str());
    } else {
        HCCL_RUN_INFO("HCCL_ALGO is not set");
    }
    return HCCL_SUCCESS;
}

HcclResult SetHcclAlgoConfig(const std::string& hcclAlgo)
{
    std::string algoConfig = hcclAlgo;
    algoConfig.erase(std::remove(algoConfig.begin(), algoConfig.end(), ' '), algoConfig.end());
    if (algoConfig.empty()) {
        HCCL_RUN_INFO("hccl algo config is empty, HCCL use built-in algo selection.");
        return HCCL_SUCCESS;
    }
    std::vector<std::string> algoPerOptype;
    CHK_RET(SplitHcclOpType(algoConfig, algoPerOptype));
    bool anyCommonConfig = false;
    bool anySpecificConfig = false;
    CHK_RET(CheckAlgoConfigValid(algoPerOptype, anyCommonConfig, anySpecificConfig));
    if (anyCommonConfig) {
        CHK_RET(SetCommonAlgType(algoPerOptype));
    } else {
        CHK_RET(SetSpecificAlgType(algoPerOptype));
    }
    return HCCL_SUCCESS;
}

HcclResult ResetAlgEnvConfigInitState()
{
    std::lock_guard<std::mutex> lock(g_algEnvConfigMutex);
    g_algEnvConfig.SetDefaultParams();
    return HCCL_SUCCESS;
}

const std::vector<HcclAlgoType> GetExternalInputHcclAlgoConfig(HcclCMDType opType)
{
    std::lock_guard<std::mutex> lock(g_algEnvConfigMutex);
    return g_algEnvConfig.hcclAlgoConfig[opType];
}

const std::map<HcclCMDType, std::vector<HcclAlgoType>> GetExternalInputHcclAlgoConfigAllType()
{
    std::lock_guard<std::mutex> lock(g_algEnvConfigMutex);
    return g_algEnvConfig.hcclAlgoConfig;
}

HcclResult SetCommonAlgType(std::vector<std::string>& algos)
{
    std::vector<HcclAlgoType> algType;
    CHK_RET(ParseAlgoString("all op type", algos[0], algType));
    for (u32 opType = 0; opType < static_cast<u32>(HcclCMDType::HCCL_CMD_MAX); opType++) {
        g_algEnvConfig.hcclAlgoConfig[static_cast<HcclCMDType>(opType)] = algType;
    }
    return HCCL_SUCCESS;
}

HcclResult SetSpecificAlgType(std::vector<std::string>& algos)
{
    for (std::string& algConfig : algos) {
        std::size_t found = algConfig.find("=");
        std::string opStringName = algConfig.substr(0, found);
        if (opStringName == "others") {
            std::vector<HcclAlgoType> algType;
            std::string remainAlgoConfig = algConfig.substr(found + 1);
            CHK_RET(ParseAlgoString("others op type", remainAlgoConfig, algType));
            for (u32 opType = 0; opType < static_cast<u32>(HcclCMDType::HCCL_CMD_MAX); opType++) {
                g_algEnvConfig.hcclAlgoConfig[static_cast<HcclCMDType>(opType)] = algType;
            }
        }
    }
    std::map<std::string, HcclCMDType> hcclOpTypeMap = {
        {"broadcast", HcclCMDType::HCCL_CMD_BROADCAST},
        {"allreduce", HcclCMDType::HCCL_CMD_ALLREDUCE},
        {"reduce", HcclCMDType::HCCL_CMD_REDUCE},
        {"send", HcclCMDType::HCCL_CMD_SEND},
        {"receive", HcclCMDType::HCCL_CMD_RECEIVE},
        {"allgather", HcclCMDType::HCCL_CMD_ALLGATHER},
        {"reducescatter", HcclCMDType::HCCL_CMD_REDUCE_SCATTER},
        {"alltoall", HcclCMDType::HCCL_CMD_ALLTOALL},
        {"gather", HcclCMDType::HCCL_CMD_GATHER},
        {"scatter", HcclCMDType::HCCL_CMD_SCATTER},
        {"sendrecv", HcclCMDType::HCCL_CMD_BATCH_SEND_RECV},
    };
    for (std::string& algConfig : algos) {
        std::size_t found = algConfig.find("=");
        std::string opStringName = algConfig.substr(0, found);
        if (hcclOpTypeMap.find(opStringName) != hcclOpTypeMap.end()) {
            HcclCMDType optype = hcclOpTypeMap[opStringName];
            std::string remainAlgoConfig = algConfig.substr(found + 1);
            std::vector<HcclAlgoType> algType;
            CHK_RET(ParseAlgoString(opStringName, remainAlgoConfig, algType));
            if (algType[0] == HcclAlgoType::HCCL_ALGO_TYPE_NULL) {
                HCCL_ERROR("[SetSpecificAlgType] specific config level0 not support null type.");
                return HCCL_E_PARA;
            }
            g_algEnvConfig.hcclAlgoConfig[optype] = algType;
        } else {
            HCCL_ERROR(
                "[SetSpecificAlgType] specific config optype[%s] is invalid, please check", opStringName.c_str());
            return HCCL_E_PARA;
        }
    }
    g_algEnvConfig.hcclAlgoConfig[HcclCMDType::HCCL_CMD_ALLTOALLV] =
        g_algEnvConfig.hcclAlgoConfig[HcclCMDType::HCCL_CMD_ALLTOALL];
    g_algEnvConfig.hcclAlgoConfig[HcclCMDType::HCCL_CMD_ALLTOALLVC] =
        g_algEnvConfig.hcclAlgoConfig[HcclCMDType::HCCL_CMD_ALLTOALL];
    return HCCL_SUCCESS;
}

HcclResult ParserHcclAlgoLevel(const std::string& algoLevel, u32& level, HcclAlgoType& algoType)
{
    std::size_t found = algoLevel.find(":");
    if ((found == 0) || (found == (algoLevel.length() - 1))) {
        HCCL_ERROR("[Parser][HcclAlgoLevel] algo config is invalid.");
        return HCCL_E_PARA;
    }

    std::string orginalLevel = algoLevel.substr(0, found);
    std::string orginalAlgo = algoLevel.substr(found + 1);

    const std::map<std::string, u32> hcclAlgoLevelMap = {
        {"level0", HCCL_ALGO_LEVEL_0},
        {"level1", HCCL_ALGO_LEVEL_1},
        {"level2", HCCL_ALGO_LEVEL_2},
        {"level3", HCCL_ALGO_LEVEL_3}};

    const std::map<std::string, HcclAlgoType> hcclAlgoTypeMap = {
        {"null", HcclAlgoType::HCCL_ALGO_TYPE_NULL},
        {"ring", HcclAlgoType::HCCL_ALGO_TYPE_RING},
        {"pipeline", HcclAlgoType::HCCL_ALGO_TYPE_PIPELINE},
        {"fullmesh", HcclAlgoType::HCCL_ALGO_TYPE_FULLMESH},
        {"H-D_R", HcclAlgoType::HCCL_ALGO_TYPE_HDR},
        {"pairwise", HcclAlgoType::HCCL_ALGO_TYPE_PAIRWISE},
        {"NHR", HcclAlgoType::HCCL_ALGO_TYPE_NHR},
        {"NHR_V1", HcclAlgoType::HCCL_ALGO_TYPE_NHR_V1},
        {"AHC", HcclAlgoType::HCCL_ALGO_TYPE_AHC},
        {"AHC_BROKE", HcclAlgoType::HCCL_ALGO_TYPE_AHC_BROKE},
        {"NB", HcclAlgoType::HCCL_ALGO_TYPE_NB},
        {"NA", HcclAlgoType::HCCL_ALGO_TYPE_NA},
    };

    auto iterAlgoLevel = hcclAlgoLevelMap.find(orginalLevel);
    if (iterAlgoLevel == hcclAlgoLevelMap.end()) {
        HCCL_ERROR("[Parser][HcclAlgoLevel] algo config is invalid, level %s is not supported.", orginalLevel.c_str());
        return HCCL_E_PARA;
    }

    auto iterAlgoType = hcclAlgoTypeMap.find(orginalAlgo);
    if (iterAlgoType == hcclAlgoTypeMap.end()) {
        HCCL_ERROR("[Parser][HcclAlgoLevel] algo config is invalid, algo %s is not supported.", orginalAlgo.c_str());
        return HCCL_E_PARA;
    }

    level = iterAlgoLevel->second;
    algoType = iterAlgoType->second;

    return HCCL_SUCCESS;
}

HcclResult ParseAlgoString(std::string opName, std::string& algoString, std::vector<HcclAlgoType>& algType)
{
    algType = std::vector<HcclAlgoType>(HCCL_ALGO_LEVEL_NUM, HcclAlgoType::HCCL_ALGO_TYPE_DEFAULT);
    std::vector<std::string> algoLevels;
    HcclResult ret = SplitHcclAlgoLevel(algoString, algoLevels);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[Set][HcclAlgoConfig]hccl algo config[%s] is invalid. "
            "expect: level0:NA;level1:<algo> or <op0>=level0:NA;level1:<algo0>/<op1>=level0:NA;level1:<algo1>",
            algoString.c_str()),
        ret);
    for (auto algoLevel : algoLevels) {
        u32 level = 0;
        HcclAlgoType algo = HcclAlgoType::HCCL_ALGO_TYPE_DEFAULT;
        ret = ParserHcclAlgoLevel(algoLevel, level, algo);
        CHK_PRT_RET(
            ret != HCCL_SUCCESS,
            HCCL_ERROR(
                "[Set][HcclAlgoConfig]hccl algo config[%s] is invalid. "
                "expect: level0:NA;level1:<algo> or <op0>=level0:NA;level1:<algo0>/<op1>=level0:NA;level1:<algo1>",
                algoString.c_str()),
            ret);
        // 检查是否存在重复配置level
        if (algType[level] != HcclAlgoType::HCCL_ALGO_TYPE_DEFAULT) {
            HCCL_ERROR(
                "[Set][HcclAlgoConfig]hccl algo config[%s] is invalid. "
                "expect: level0:NA;level1:<algo> or <op0>=level0:NA;level1:<algo0>/<op1>=level0:NA;level1:<algo1>",
                algoString.c_str());
            return HCCL_E_PARA;
        }
        algType[level] = algo;
    }
    auto level0Iter = HcclAlgoTypeMap.find(algType[HCCL_ALGO_LEVEL_0]);
    auto level1Iter = HcclAlgoTypeMap.find(algType[HCCL_ALGO_LEVEL_1]);
    auto level2Iter = HcclAlgoTypeMap.find(algType[HCCL_ALGO_LEVEL_2]);
    auto level3Iter = HcclAlgoTypeMap.find(algType[HCCL_ALGO_LEVEL_3]);
    HCCL_RUN_INFO(
        "hccl algo op %s config: config level0:%s, level1:%s, level2:%s, level3:%s", opName.c_str(),
        level0Iter->second.c_str(), level1Iter->second.c_str(), level2Iter->second.c_str(), level3Iter->second.c_str());
    return HCCL_SUCCESS;
}

HcclResult SplitHcclOpType(const std::string& algoConfig, std::vector<std::string>& algos)
{
    std::string remainAlgoConfig;
    std::size_t found = algoConfig.find("/");
    if ((found == 0) || (found == (algoConfig.length() - 1))) {
        HCCL_ERROR("[Split][SplitHcclOpType] algo config is invalid.");
        return HCCL_E_PARA;
    } else if (found != std::string::npos) {
        remainAlgoConfig = algoConfig.substr(found + 1);
    }
    algos.push_back(algoConfig.substr(0, found));
    if (!remainAlgoConfig.empty()) {
        CHK_RET(SplitHcclOpType(remainAlgoConfig, algos));
    }
    return HCCL_SUCCESS;
}

// 新的逐算法的配置和原有的统一配置只可使用一种，发现同时存在时报错
HcclResult CheckAlgoConfigValid(std::vector<std::string>& algos, bool& anyCommonConfig, bool& anySpecificConfig)
{
    for (std::string& algConfig : algos) {
        std::size_t found = algConfig.find("=");
        if ((found == 0) || (found == (algConfig.length() - 1))) {
            HCCL_ERROR("[Split][CheckAlgoConfigValid] algo config is invalid.");
            return HCCL_E_PARA;
        } else if (found != std::string::npos) {
            anySpecificConfig = true;
        } else {
            anyCommonConfig = true;
        }
    }
    if (anyCommonConfig && anySpecificConfig) {
        HCCL_ERROR("[CheckAlgoConfigValid]should not set both algo config way");
        return HCCL_E_PARA;
    }
    if (anyCommonConfig && algos.size() > 1) {
        HCCL_ERROR("[CheckAlgoConfigValid]should only set one common config");
        return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

HcclResult SplitHcclAlgoLevel(const std::string& algoConfig, std::vector<std::string>& algos)
{
    std::string remainAlgoConfig;
    std::size_t found = algoConfig.find(";");
    if ((found == 0) || (found == (algoConfig.length() - 1))) {
        HCCL_ERROR("[Split][HcclAlgoLevel] algo config is invalid.");
        return HCCL_E_PARA;
    } else if (found != std::string::npos) {
        remainAlgoConfig = algoConfig.substr(found + 1);
    } else {
        // 最后一组配置,剩余的字符串为空
    }
    algos.push_back(algoConfig.substr(0, found));

    if (algos.size() > HCCL_ALGO_LEVEL_NUM) {
        HCCL_ERROR("[Split][HcclAlgoLevel] algo config is invalid. algo level is more than %u.", HCCL_ALGO_LEVEL_NUM);
        return HCCL_E_PARA;
    }
    if (!remainAlgoConfig.empty()) {
        CHK_RET(SplitHcclAlgoLevel(remainAlgoConfig, algos));
    }

    return HCCL_SUCCESS;
}

bool CheckEnvLen(const char* envStr, u32 envMaxLen)
{
    // 校验环境变量长度
    u32 envLen = strnlen(envStr, envMaxLen + 1);
    if (envLen == (envMaxLen + 1)) {
        HCCL_ERROR(
            "[Check][EnvLen]errNo[0x%016llx] env len is invalid, len is %u", HCCL_ERROR_CODE(HCCL_E_PARA), envLen);
        return false;
    }
    return true;
}

HcclResult GetIntraLinkTypeDigit(std::string& intraCommStr, u32& intraCommDig)
{
    CHK_RET(IsAllDigit(intraCommStr.c_str()));
    CHK_RET(SalStrToULong(intraCommStr.c_str(), HCCL_BASE_DECIMAL, intraCommDig));

    if ((intraCommDig != 0) && (intraCommDig != 1)) { // 判断转换后的数字是否为0或1
        HCCL_ERROR("[Get][IntraLinkTypeDigit]environmental digit variable error, intraCommDig[%u]", intraCommDig);
        return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

HcclResult ParseInterLinkType()
{
    std::string interHccsDisableEnv = GetEnv(MM_ENV_HCCL_INTER_HCCS_DISABLE);
    if (interHccsDisableEnv == "EmptyString") {
        HCCL_RUN_INFO(
            "HCCL_INTER_HCCS_DISABLE is not set, default value is %s.",
            g_algEnvConfig.interHccsDisable ? "TRUE" : "FALSE");
        return HCCL_SUCCESS;
    }
    std::transform(interHccsDisableEnv.begin(), interHccsDisableEnv.end(), interHccsDisableEnv.begin(), ::toupper);
    if ("TRUE" == interHccsDisableEnv) {
        g_algEnvConfig.interHccsDisable = true;
    } else if ("FALSE" == interHccsDisableEnv) {
        g_algEnvConfig.interHccsDisable = false;
    } else {
        HCCL_ERROR("HCCL_INTER_HCCS_DISABLE %s is invalid, expect true or false.", interHccsDisableEnv.c_str());
        return HCCL_E_PARA;
    }
    HCCL_RUN_INFO(
        "environmental variable HCCL_INTER_HCCS_DISABLE is set to [%s], interHccsDisable[%d]",
        interHccsDisableEnv.c_str(), g_algEnvConfig.interHccsDisable);
    return HCCL_SUCCESS;
}

HcclResult ParseIntraLinkType()
{
    std::string intraPcieEnv = GetEnv(MM_ENV_HCCL_INTRA_PCIE_ENABLE);
    std::string intraRoceEnv = GetEnv(MM_ENV_HCCL_INTRA_ROCE_ENABLE);

    u32 intraPcie = 1; // 保存pcie环境变量的解析数字
    u32 intraRoce = 0; // 保存roce环境变量的解析数字

    // 两个通信域环境变量均未设置，默认走pcie
    if (intraPcieEnv == "EmptyString" && intraRoceEnv == "EmptyString") {
        HCCL_RUN_INFO(
            "HCCL_INTRA_PCIE_ENABLE set by default to [%u], HCCL_INTRA_ROCE_ENABLE set by default to [%u]", intraPcie,
            intraRoce);
        return HCCL_SUCCESS;
    }

    if (intraPcieEnv != "EmptyString") { // 解析HCCL_INTRA_PCIE_ENABLE为数字
        // 校验环境变量长度
        bool isEnvLenValid = CheckEnvLen(intraPcieEnv.c_str(), MAX_LEN_OF_DIGIT_ENV);
        CHK_PRT_RET(
            !isEnvLenValid,
            HCCL_ERROR(
                "[Parse][IntraLinkType]errNo[0x%016llx] Invalid INTRA_PCIE_ENABLE env len, len is bigger than "
                "[%u]. errorno[%d]",
                HCCL_ERROR_CODE(HCCL_E_PARA), MAX_LEN_OF_DIGIT_ENV, HCCL_E_PARA),
            HCCL_E_PARA);
        std::string intraPcieStr(intraPcieEnv);
        CHK_RET(GetIntraLinkTypeDigit(intraPcieStr, intraPcie));
    }

    if (intraRoceEnv != "EmptyString") { // 解析HCCL_INTRA_ROCE_ENABLE为数字
        // 校验环境变量长度
        bool isEnvLenValid = CheckEnvLen(intraRoceEnv.c_str(), MAX_LEN_OF_DIGIT_ENV);
        CHK_PRT_RET(
            !isEnvLenValid,
            HCCL_ERROR(
                "[Parse][IntraLinkType]errNo[0x%016llx] Invalid INTRA_ROCE_ENABLE env len, len is bigger than "
                "[%u]. errorno[%d]",
                HCCL_ERROR_CODE(HCCL_E_PARA), MAX_LEN_OF_DIGIT_ENV, HCCL_E_PARA),
            HCCL_E_PARA);
        std::string intraRoceStr(intraRoceEnv);
        CHK_RET(GetIntraLinkTypeDigit(intraRoceStr, intraRoce));
    }

    // 只配置了roce的环境变量
    if (intraPcieEnv == "EmptyString" && intraRoceEnv != "EmptyString") {
        if (intraRoce == 0) { // roce环境变量值为0，报错
            HCCL_ERROR("[Parse][IntraLinkType]only set HCCL_INTRA_ROCE_ENABLE, and the val is zero, pls set "
                       "HCCL_INTRA_PCIE_ENABLE");
            return HCCL_E_PARA;
        } else { // roce环境变量值为1，走roce
            intraPcie = 0;
        }
        HCCL_RUN_INFO(
            "HCCL_INTRA_PCIE_ENABLE set by environment to [%u], "
            "HCCL_INTRA_ROCE_ENABLE set by environment to [%u]",
            intraPcie, intraRoce);
    }

    // 只配置了pcie的环境变量
    if (intraPcieEnv != "EmptyString" && intraRoceEnv == "EmptyString") {
        if (intraPcie == 0) { // pcie环境变量值为0，报错
            HCCL_ERROR("[Parse][IntraLinkType]only set HCCL_INTRA_PCIE_ENABLE, and the val is zero, pls set "
                       "HCCL_INTRA_ROCE_ENABLE");
            return HCCL_E_PARA;
        }
        HCCL_RUN_INFO(
            "HCCL_INTRA_PCIE_ENABLE set by environment to [%u], "
            "HCCL_INTRA_ROCE_ENABLE set by default to [%u]",
            intraPcie, intraRoce);
    }

    // pcie和roce环境变量同时配置且不相等
    if (intraPcieEnv != "EmptyString" && intraRoceEnv != "EmptyString") {
        if ((intraPcie == 0 && intraRoce == 1) || (intraPcie == 1 && intraRoce == 0)) {
            HCCL_RUN_INFO(
                "HCCL_INTRA_PCIE_ENABLE set by environment to [%u], "
                "HCCL_INTRA_ROCE_ENABLE set by environment to [%u]",
                intraPcie, intraRoce);
        }
    }

    // pcie和roce环境变量同时配置且相等
    if ((intraPcie ^ intraRoce) == 0) {
        if (intraPcie == 1) { // 同时为1，暂不支持，报错
            HCCL_ERROR(
                "[Parse][IntraLinkType] Enabling intra Pcie and intra Roce at the same time is not supported now.");
            return HCCL_E_PARA;
        } else { // 同时为0，走pcie
            HCCL_WARNING("Pcie and Roce Env both set to zero at the same time, intra comm is default Pcie");
            intraPcie = 1;
        }
        HCCL_RUN_INFO(
            "HCCL_INTRA_PCIE_ENABLE set by environment to [%u], "
            "HCCL_INTRA_ROCE_ENABLE set by environment to [%u]",
            intraPcie, intraRoce);
    }

    g_algEnvConfig.intraRoceSwitch = intraRoce;
    return HCCL_SUCCESS;
}

HcclResult ParseEntryLogEnable()
{
    std::string enableEntryLogEnv = GetEnv(MM_ENV_HCCL_ENTRY_LOG_ENABLE);
    if (enableEntryLogEnv == "EmptyString") {
        HCCL_RUN_INFO("HCCL_ENTRY_LOG_ENABLE set by default to [0]");
        return HCCL_SUCCESS;
    }
    if (enableEntryLogEnv != "0" && enableEntryLogEnv != "1") {
        HCCL_ERROR(
            "[Parser][EntryLogEnable]environmental variable HCCL_ENTRY_LOG_ENABLE [%s] is invalid, set by "
            "default to [0]",
            enableEntryLogEnv.c_str());
        return HCCL_E_PARA;
    }
    g_algEnvConfig.enableEntryLog = false;
    if (enableEntryLogEnv == "1") {
        g_algEnvConfig.enableEntryLog = true;
    }
    HCCL_RUN_INFO("HCCL_ENTRY_LOG_ENABLE set by environment to [%u]", g_algEnvConfig.enableEntryLog);
    return HCCL_SUCCESS;
}

HcclResult ParseOpExpansion()
{
    const std::string& opExpansionModeEnv = GetEnv(MM_ENV_HCCL_OP_EXPANSION_MODE);
    g_algEnvConfig.aicpuUnfold = false;
    g_algEnvConfig.aivMode = false;
    g_algEnvConfig.aivOnlyMode = false;
    g_algEnvConfig.ccuMSMode = false;
    g_algEnvConfig.ccuSchedMode = false;

    if (opExpansionModeEnv == "CCU_MS") {
        g_algEnvConfig.ccuMSMode = true;
        return HCCL_SUCCESS;
    }

    if (opExpansionModeEnv == "CCU_SCHED") {
        g_algEnvConfig.ccuSchedMode = true;
        return HCCL_SUCCESS;
    }

    DevType deviceType;
    CHK_RET(hrtGetDeviceType(deviceType));
    // 910_93默认打开AICPU展开
    if (deviceType == DevType::DEV_TYPE_910_93) {
        g_algEnvConfig.aicpuUnfold = true;
    }

    if (opExpansionModeEnv == "EmptyString") {
        HCCL_RUN_INFO(
            "HCCL_OP_EXPANSION_MODE is not set, aicpuUnfold is [%u], aivMode is [%u]", g_algEnvConfig.aicpuUnfold,
            g_algEnvConfig.aivMode);
        return HCCL_SUCCESS;
    }

    if (opExpansionModeEnv == "AI_CPU") {
        if (deviceType == DevType::DEV_TYPE_910) {
            HCCL_WARNING("910 do not support AICPU unfold.");
        } else {
            g_algEnvConfig.aicpuUnfold = true;
        }
    } else if (opExpansionModeEnv == "AIV") {
        if (g_algEnvConfig.hcclDeterministic == true) {
            HCCL_WARNING("Deterministic do not support aiv");
        }
        g_algEnvConfig.aivMode = true;
    } else if (opExpansionModeEnv == "AIV_ONLY") {
        if (g_algEnvConfig.hcclDeterministic == true) {
            HCCL_WARNING("Deterministic do not support aiv only");
        }
        g_algEnvConfig.aivMode = true;
        g_algEnvConfig.aivOnlyMode = true;
    } else if (opExpansionModeEnv == "HOST") {
        g_algEnvConfig.aivMode = false;
        g_algEnvConfig.aicpuUnfold = false;
    } else if (opExpansionModeEnv == "HOST_TS") {
        if (deviceType == DevType::DEV_TYPE_910B) {
            g_algEnvConfig.enableFfts = false;
        } else {
            HCCL_WARNING("deviceType[%u] do not support HOST_TS", deviceType);
        }
    } else if (opExpansionModeEnv == "AICPU_CacheDisable") {
        if (deviceType == DevType::DEV_TYPE_910) {
            HCCL_WARNING("910 do not support AICPU unfold.");
        } else {
            g_algEnvConfig.aicpuUnfold = true;
            g_algEnvConfig.aicpuCacheEnable = 0; // Disable aicpu cache
        }
    } else {
        HCCL_ERROR(
            "HCCL_OP_EXPANSION_MODE is set to [%s], which is incorrect. Please check", opExpansionModeEnv.c_str());
        return HCCL_E_PARA;
    }
    HCCL_INFO(
        "environmental variable HCCL_OP_EXPANSION_MODE is [%s], aicpuUnfold[%u], aivMode[%u], enableFfts[%u]",
        opExpansionModeEnv.c_str(), g_algEnvConfig.aicpuUnfold, g_algEnvConfig.aivMode, g_algEnvConfig.enableFfts);
    return HCCL_SUCCESS;
}

HcclResult SplitHcclRetryEnable(const std::string& retryConfig, std::vector<std::string>& retryEnables)
{
    std::string remainRetryConfig;
    std::size_t found = retryConfig.find(",");
    if ((found == 0) || (found == (retryConfig.length() - 1))) {
        HCCL_ERROR("[SplitHcclRetryEnable] algo config is invalid.");
        return HCCL_E_PARA;
    } else if (found != std::string::npos) {
        remainRetryConfig = retryConfig.substr(found + 1);
    } else {
        // 最后一组配置,剩余的字符串为空
    }
    retryEnables.push_back(retryConfig.substr(0, found));

    if (retryEnables.size() > HCCL_RETRY_ENABLE_LEVEL_NUM) {
        HCCL_ERROR(
            "[SplitHcclRetryEnable] retryEnable config is invalid. retryEnable level is more than %u.",
            HCCL_RETRY_ENABLE_LEVEL_NUM);
        return HCCL_E_PARA;
    }
    if (!remainRetryConfig.empty()) {
        CHK_RET(SplitHcclRetryEnable(remainRetryConfig, retryEnables));
    }
    return HCCL_SUCCESS;
}

HcclResult CollectRetryEnableFromConfig(const std::vector<std::string>& retryEnables)
{
    const std::map<std::string, u32> hcclRetryLevelMap = {
        {"L0", HCCL_RETRY_ENABLE_LEVEL_0}, {"L1", HCCL_RETRY_ENABLE_LEVEL_1}, {"L2", HCCL_RETRY_ENABLE_LEVEL_2}};

    std::map<std::string, u32> countHcclRetryLevelMap = {{"L0", 0}, {"L1", 0}, {"L2", 0}};

    const std::map<std::string, bool> hcclRetryEnableMap = {{"0", false}, {"1", true}};
    for (auto retryEnableLevel : retryEnables) {
        u32 level = 0;
        bool retryEnable = false;
        std::size_t found = retryEnableLevel.find(":");
        if ((found == 0) || (found == (retryEnableLevel.length() - 1))) {
            HCCL_ERROR("[CollectRetryEnableFromConfig] Hccl retryEnableLevel is invalid.");
            return HCCL_E_PARA;
        }
        std::string orginalLevel = retryEnableLevel.substr(0, found);
        std::string orginalRetryEnable = retryEnableLevel.substr(found + 1);
        if (orginalLevel == "L0") {
            HCCL_RUN_WARNING("[CollectRetryEnableFromConfig] L0 config does not take effect");
        }
        // 检查是否存在重复配置level
        auto iterCountRetryLevel = countHcclRetryLevelMap.find(orginalLevel);
        if (iterCountRetryLevel == countHcclRetryLevelMap.end()) {
            HCCL_ERROR(
                "[CollectRetryEnableFromConfig] Retry config is invalid, level %s is not supported.",
                orginalLevel.c_str());
            return HCCL_E_PARA;
        }
        if (countHcclRetryLevelMap[orginalLevel] == 1) {
            HCCL_ERROR(
                "[CollectRetryEnableFromConfig] Retry config level[%s] is repeated, expect: L1:0, L2:0",
                orginalLevel.c_str());
            return HCCL_E_PARA;
        }
        countHcclRetryLevelMap[orginalLevel] += 1;
        // 获取level和对应的retryEnable，并赋值给g_algEnvConfig.hcclRetryConfig
        auto iterRetryLevel = hcclRetryLevelMap.find(orginalLevel);
        if (iterRetryLevel == hcclRetryLevelMap.end()) {
            HCCL_ERROR(
                "[CollectRetryEnableFromConfig] Retry config is invalid, level %s is not supported.",
                orginalLevel.c_str());
            return HCCL_E_PARA;
        }
        auto iterRetryEnable = hcclRetryEnableMap.find(orginalRetryEnable);
        if (iterRetryEnable == hcclRetryEnableMap.end()) {
            HCCL_ERROR(
                "[CollectRetryEnableFromConfig] Retry config is invalid, retryEnable %s is not supported.",
                orginalRetryEnable.c_str());
            return HCCL_E_PARA;
        }
        level = iterRetryLevel->second;
        retryEnable = iterRetryEnable->second;
        g_algEnvConfig.hcclRetryConfig[level] = retryEnable;
    }
    return HCCL_SUCCESS;
}

HcclResult ParseRetryEnable()
{
    // 默认都设置成false
    for (u32 level = 0; level < HCCL_RETRY_ENABLE_LEVEL_NUM; ++level) {
        g_algEnvConfig.hcclRetryConfig[level] = false;
    }
    std::string hcclRetryEnable = GetEnv(MM_ENV_HCCL_OP_RETRY_ENABLE);
    if (hcclRetryEnable == "EmptyString") {
        HCCL_RUN_INFO(
            "[ParseRetryEnable] HCCL_OP_RETRY_ENABLE is not set. The retryEnable of all levels is set to false.");
        return HCCL_SUCCESS;
    }
    // 去除空格
    std::string retryConfig = hcclRetryEnable;
    retryConfig.erase(std::remove(retryConfig.begin(), retryConfig.end(), ' '), retryConfig.end());

    if (retryConfig.empty()) {
        HCCL_RUN_INFO("[ParseRetryEnable] Hccl retry config is empty. The retryEnable of all levels is set to false.");
        return HCCL_SUCCESS;
    }

    std::vector<std::string> retryEnables;
    HcclResult ret = SplitHcclRetryEnable(retryConfig, retryEnables);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[CollectRetryEnableFromConfig] Hccl retry config[%s] is invalid. "
            "expect: L1:0, L2:0",
            retryConfig.c_str()),
        ret);

    CHK_RET(CollectRetryEnableFromConfig(retryEnables));
    HCCL_RUN_INFO("[ParseRetryEnable] HCCL_OP_RETRY_ENABLE set by environment variable to [%s].", retryConfig.c_str());
    return HCCL_SUCCESS;
}

HcclResult ParseDeterministic()
{
    std::string hcclDeterministicEnv = GetEnv(MM_ENV_HCCL_DETERMINISTIC);
    if (hcclDeterministicEnv == "EmptyString") {
        HCCL_RUN_INFO("HCCL_DETERMINISTIC set by default to [false]");
        return HCCL_SUCCESS;
    }

    std::transform(hcclDeterministicEnv.begin(), hcclDeterministicEnv.end(), hcclDeterministicEnv.begin(), ::toupper);
    if (hcclDeterministicEnv != "STRICT" && hcclDeterministicEnv != "TRUE" && hcclDeterministicEnv != "FALSE") {
        HCCL_ERROR("HCCL_DETERMINISTIC is set to [%s], which is incorrect. Please check", hcclDeterministicEnv.c_str());
        return HCCL_E_PARA;
    }
    if (hcclDeterministicEnv == "STRICT") {
        // 规约保序场景（严格的确定性计算，在确定性的基础上强保证规约顺序一致）
        DevType deviceType;
        CHK_RET(hrtGetDeviceType(deviceType));
        if (deviceType != DevType::DEV_TYPE_910B && deviceType != DevType::DEV_TYPE_910_93) {
            // 规约保序仅支持A2 A3场景
            HCCL_ERROR(
                "HCCL_DETERMINISTIC is set to [%s], Reduce order preservation is not supported for "
                "deviceType[%d], please check",
                hcclDeterministicEnv.c_str(), deviceType);
            return HCCL_E_NOT_SUPPORT;
        }
        g_algEnvConfig.hcclDeterministic = static_cast<u8>(DeterministicEnableLevel::DETERMINISTIC_STRICT);
    } else if (hcclDeterministicEnv == "TRUE") {
        // 确定性计算场景（不保证规约保序）
        g_algEnvConfig.hcclDeterministic = static_cast<u8>(DeterministicEnableLevel::DETERMINISTIC_ENABLE);
    } else {
        g_algEnvConfig.hcclDeterministic = static_cast<u8>(DeterministicEnableLevel::DETERMINISTIC_DISABLE);
    }
    HCCL_RUN_INFO(
        "HCCL_DETERMINISTIC set by environment to [%s], hcclDeterministic[%u]", hcclDeterministicEnv.c_str(),
        g_algEnvConfig.hcclDeterministic);
    return HCCL_SUCCESS;
}

const u32& GetExternalInputIntraRoceSwitch() { return g_algEnvConfig.intraRoceSwitch; }

const bool& GetExternalInputHcclAicpuUnfold()
{
    std::lock_guard<std::mutex> lock(g_algEnvConfigMutex);
    return g_algEnvConfig.aicpuUnfold;
}

const uint8_t& GetExternalInputHcclAicpuCacheEnable() { return g_algEnvConfig.aicpuCacheEnable; }

const bool& GetExternalInputHcclAivMode() { return g_algEnvConfig.aivMode; }

const bool& GetExternalInputHcclAivOnlyMode() { return g_algEnvConfig.aivOnlyMode; }

const bool& GetExternalInputHcclCcuMSMode() { return g_algEnvConfig.ccuMSMode; }

const bool& GetExternalInputHcclCcuSchedMode() { return g_algEnvConfig.ccuSchedMode; }

const bool& GetExternalInputInterHccsDisable() { return g_algEnvConfig.interHccsDisable; }

const bool& GetExternalInputIntraServerRetryEnable()
{
    return g_algEnvConfig.hcclRetryConfig[HCCL_RETRY_ENABLE_LEVEL_0];
}

const bool& GetExternalInputInterServerRetryEnable()
{
    return g_algEnvConfig.hcclRetryConfig[HCCL_RETRY_ENABLE_LEVEL_1];
}

const bool& GetExternalInputInterSuperPodRetryEnable()
{
    return g_algEnvConfig.hcclRetryConfig[HCCL_RETRY_ENABLE_LEVEL_2];
}

const bool& GetExternalInputHcclEnableEntryLog() { return g_algEnvConfig.enableEntryLog; }

bool RunIndependentOpExpansion(DevType deviceType)
{
    std::string opExpansionModeEnv = GetEnv(MM_ENV_HCCL_OP_EXPANSION_MODE);
    if (deviceType == DevType::DEV_TYPE_910_93) {
        return opExpansionModeEnv == "AI_CPU" || opExpansionModeEnv == "HOST_TS" || opExpansionModeEnv == "EmptyString";
    }

#ifdef MACRO_DEV_TYPE_NEW
    if (deviceType == DevType::DEV_TYPE_950) {
#else
    if (deviceType == DevType::DEV_TYPE_910_95) {
#endif
        return opExpansionModeEnv == "AI_CPU" || opExpansionModeEnv == "HOST_TS" ||
               opExpansionModeEnv == "EmptyString" || opExpansionModeEnv == "AIV" || opExpansionModeEnv == "AIV_ONLY" ||
               opExpansionModeEnv == "CCU_SCHED" || opExpansionModeEnv == "CCU_MS";
    }

    // HOST_TS为Host展开
    if (deviceType == DevType::DEV_TYPE_910B) {
        return opExpansionModeEnv == "HOST_TS" || opExpansionModeEnv == "HOST";
    }
    return false;
}
} // namespace mc2_ops_hccl
