/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "hccl_mc2.h"
#include "log.h"
#include "sal.h"
#include "alg_env_config.h"
#include "hccl_inner.h"
#include "param_check.h"
#include "hccl_alloc_ctx_res.h"
#include "op_common.h"

using namespace mc2_ops_hccl;

namespace {
const char* GetMc2OpTypeName(HcclCMDType opType)
{
    switch (opType) {
        case HcclCMDType::HCCL_CMD_ALLGATHER:
            return "AllGather";
        case HcclCMDType::HCCL_CMD_ALLREDUCE:
            return "AllReduce";
        case HcclCMDType::HCCL_CMD_REDUCE_SCATTER:
            return "ReduceScatter";
        case HcclCMDType::HCCL_CMD_ALLTOALL:
            return "AllToAll";
        case HcclCMDType::HCCL_CMD_ALLTOALLV:
            return "AllToAllV";
        default:
            return "Unknown";
    }
}
} // namespace

constexpr uint32_t ALG_CONFIG_SIZE = 128;
struct HcclOpArgs {
    HcclDataType srcDataType;
    HcclDataType dstDataType;
    HcclReduceOp reduceType;
    uint64_t count;
    char algConfig[ALG_CONFIG_SIZE];
    CommEngine commEngine;
    uint64_t reverse;

    void Init()
    {
        srcDataType = HCCL_DATA_TYPE_FP16;
        dstDataType = HCCL_DATA_TYPE_FP16;
        reduceType = HCCL_REDUCE_SUM;
        count = 0;
    }
};

HcclResult HcclKfcAllocOpArgs(void** opArgs)
{
    CHK_PTR_NULL(opArgs);

    HcclOpArgs* opArgsMem = (HcclOpArgs*)malloc(sizeof(HcclOpArgs));
    if (opArgsMem == nullptr) {
        HCCL_ERROR("[HcclKfcAllocOpArgs] malloc HcclOpArgs mem failed, please check.");
        return HCCL_E_INTERNAL;
    }
    opArgsMem->Init();
    *opArgs = opArgsMem;
    HCCL_RUN_INFO("[HcclKfcAllocOpArgs] malloc HcclOpArgs success, please fill mem[%p->%p] in it.", opArgs, *opArgs);

    return HCCL_SUCCESS;
}

HcclResult HcclKfcFreeOpArgs(void* opArgs)
{
    CHK_PTR_NULL(opArgs);

    free(opArgs);
    opArgs = nullptr;

    return HCCL_SUCCESS;
}

HcclResult HcclKfcOpArgsSetSrcDataType(void* opArgs, uint8_t srcDataType)
{
    CHK_PTR_NULL(opArgs);
    CHK_RET(HcomCheckDataType(static_cast<HcclDataType>(srcDataType)));

    HcclOpArgs* opArgsPtr = static_cast<HcclOpArgs*>(opArgs);
    opArgsPtr->srcDataType = static_cast<HcclDataType>(srcDataType);

    return HCCL_SUCCESS;
}

HcclResult HcclKfcOpArgsSetDstDataType(void* opArgs, uint8_t dstDataType)
{
    CHK_PTR_NULL(opArgs);
    CHK_RET(HcomCheckDataType(static_cast<HcclDataType>(dstDataType)));

    HcclOpArgs* opArgsPtr = static_cast<HcclOpArgs*>(opArgs);
    opArgsPtr->dstDataType = static_cast<HcclDataType>(dstDataType);

    return HCCL_SUCCESS;
}

HcclResult HcclKfcOpArgsSetReduceType(void* opArgs, uint32_t reduceType)
{
    CHK_PTR_NULL(opArgs);
    CHK_RET(HcomCheckReductionOp(static_cast<HcclReduceOp>(reduceType)));

    HcclOpArgs* opArgsPtr = static_cast<HcclOpArgs*>(opArgs);
    opArgsPtr->reduceType = static_cast<HcclReduceOp>(reduceType);

    return HCCL_SUCCESS;
}

HcclResult HcclKfcOpArgsSetCount(void* opArgs, uint64_t count)
{
    CHK_PTR_NULL(opArgs);
    if (count > SYS_MAX_COUNT) {
        HCCL_ERROR("[%s] count[%llu] is invalid (bigger than MAX count[%lu])", __func__, count, SYS_MAX_COUNT);
        return HCCL_E_PARA;
    }

    HcclOpArgs* opArgsPtr = static_cast<HcclOpArgs*>(opArgs);
    opArgsPtr->count = count;

    return HCCL_SUCCESS;
}

HcclResult HcclKfcOpArgsSetAlgConfig(void* opArgs, char* algConfig)
{
    CHK_PTR_NULL(opArgs);
    CHK_PTR_NULL(algConfig);

    HcclOpArgs* opArgsPtr = static_cast<HcclOpArgs*>(opArgs);
    s32 ret = strcpy_s(opArgsPtr->algConfig, ALG_CONFIG_SIZE, algConfig);
    if (ret != EOK) {
        HCCL_ERROR("[%s] strcpy_s algConfig failed, ret[%d]", __func__, ret);
        return HCCL_E_PARA;
    }

    return HCCL_SUCCESS;
}

HcclResult HcclKfcOpArgsSetCommEngine(void* opArgs, uint8_t commEngine)
{
    CHK_PTR_NULL(opArgs);
    // A3只支持AICPU和AIV场景
    if (commEngine != COMM_ENGINE_AICPU && commEngine != COMM_ENGINE_AIV) {
        HCCL_ERROR("[%s] commEngine[%u] not supported", __func__, commEngine);
        return HCCL_E_NOT_SUPPORT;
    }

    HcclOpArgs* opArgsPtr = static_cast<HcclOpArgs*>(opArgs);
    opArgsPtr->commEngine = static_cast<CommEngine>(commEngine);

    return HCCL_SUCCESS;
}

HcclResult HcclCreateOpResCtx(HcclComm comm, uint8_t opType, void* opArgs, void** opResCtx)
{
    CHK_PTR_NULL(comm);
    CHK_PTR_NULL(opArgs);
    CHK_PTR_NULL(opResCtx);
    if (opType >= static_cast<uint8_t>(HcclCMDType::HCCL_CMD_MAX)) {
        HCCL_ERROR("[%s] invalid opType[%u]", __func__, opType);
        return HCCL_E_PARA;
    }

    CHK_RET(InitEnvConfig());

    HcclOpArgs* opArgsPtr = static_cast<HcclOpArgs*>(opArgs);
    if (GetExternalInputHcclEnableEntryLog()) {
        HCCL_RUN_INFO(
            "Entry-HcclKfcCreateOpResCtx, opType[%u], opArgs[%p], srcDataType[%u], dstDataType[%u], reduceType[%u], "
            "count[%llu], algConfig[%s], commEngine[%u], opResCtx[%p]",
            opType, opArgs, opArgsPtr->srcDataType, opArgsPtr->dstDataType, opArgsPtr->reduceType, opArgsPtr->count,
            opArgsPtr->algConfig, opArgsPtr->commEngine, opResCtx);
    }

    CHK_RET(HcclCreateOpResCtxInner(
        comm, opType, opArgsPtr->srcDataType, opArgsPtr->dstDataType, opArgsPtr->reduceType, opArgsPtr->count,
        opArgsPtr->algConfig, opArgsPtr->commEngine, opResCtx));

    return HCCL_SUCCESS;
}

HcclResult __attribute__((visibility("default"))) HcclAllocComResourceByTiling(
    HcclComm comm, void* stream, void* mc2Tiling, void** opResCtx)
{
    HCCL_RUN_INFO(
        "[MC2_CLIENT_A5_AICPU] enter asc-devkit common HcclAllocComResourceByTiling, "
        "comm[%p], stream[%p], tiling[%p].",
        comm, stream, mc2Tiling);
    HCCL_INFO("Start to run execute HcclAllocComResourceByTiling");
    // 记录开始时间，用于性能统计
    HcclUs startut = TIME_NOW();
    // 获取设备类型
    DevType deviceType = DevType::DEV_TYPE_COUNT;
    CHK_RET(hrtGetDeviceType(deviceType));

    // 检查设备类型是否支持新流程，950或910_95支持新流程，其他设备走老流程
    if (deviceType != DevType::DEV_TYPE_950) {
        HCCL_ERROR("[%s] invalid deviceType[%u]", __func__, deviceType);
        return HCCL_E_NOT_SUPPORT;
    }

    // 初始化环境变量配置，解析HCCL相关的环境变量
    // 包括算子展开模式、确定性计算、通信方式、日志开关等配置
    CHK_RET(InitEnvConfig());

    // 检查输入参数的合法性（comm、sendBuf、recvBuf、stream不能为空）
    CHK_RET(CheckInputParam(comm, mc2Tiling, stream));

    // 获取通信域中的rank数量
    u32 rankSize = INVALID_VALUE_RANKSIZE;
    CHK_RET(HcclGetRankSize(comm, &rankSize));

    // 获取当前rank的ID
    u32 userRank = INVALID_VALUE_RANKID;
    CHK_RET(HcclGetRankId(comm, &userRank));

    // 获取通信域名称
    char commName[COMM_INDENTIFIER_MAX_LENGTH];
    CHK_RET(HcclGetCommName(comm, commName));

    const void* ccTilingList[MAX_CC_TILING_NUM];
    uint32_t tilingNum;
    CHK_RET(HcclGetTilingList(mc2Tiling, ccTilingList, tilingNum));

    // 校验commengine
    CHK_RET(CheckCommEngine(ccTilingList, tilingNum));

    // 构造操作标签，用于日志、错误追踪、topo资源管理
    // topoTag = ccTilingList->opType + commName
    // ctxTag = ccTilingList->groupName + "_" + ccTilingList[0]->opType + "_" + ccTilingList[0]->algConfig + "_" +
    // ccTilingList[0]->commEngine
    // ctxTag不再统一管理资源，而是根据每个资源opParam、WorkSpace、OpResCtx继续组成tag申请资源
    std::string topoTag[MAX_CC_TILING_NUM];
    std::string ctxTag;
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        const Mc2CcTilingInner* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        topoTag[i] = std::to_string(ccTiling->opType) + "_" + std::to_string(ccTiling->srcDataType) + "_" +
                     std::string(commName);
        // 检查标签的合法性
        CHK_RET(HcclCheckTag(topoTag[i].c_str()));
        // 检查是否为reduce类型
        bool isReduce;
        CHK_RET(CheckIsReduce(ccTiling, &isReduce));
        // 检查数据类型的合法性
        CHK_RET(CheckDataType(static_cast<HcclDataType>(ccTiling->srcDataType), isReduce));

        if (i == 0) {
            ctxTag = std::string(ccTiling->groupName) + "_" + std::to_string(ccTiling->opType) + "_" +
                     std::string(ccTiling->algConfig) + "_" + std::to_string(ccTiling->commEngine);
        } else {
            ctxTag += "_" + std::to_string(ccTiling->opType) + "_" + std::string(ccTiling->algConfig) + "_" +
                      std::to_string(ccTiling->commEngine);
        }
    }

    // TODO:记录接口入口日志，包含所有关键参数信息

    // 检查userRank是否在有效范围内
    CHK_RET(HcomCheckUserRank(rankSize, userRank));

    std::vector<OpParam> opParamVec(tilingNum);
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        // TODO: 根据topoTag[i] 获取opParam[i]的参数
        CHK_RET(
            GetOpParam(comm, stream, topoTag[i], static_cast<const Mc2CcTilingInner*>(ccTilingList[i]), opParamVec[i]));
    }

    // TODO: 根据ctxTag 申请通信资源 ，并返回OpResCtx的地址
    CHK_RET(HcclAllocOpResCtx(comm, ctxTag, opParamVec, mc2Tiling, ccTilingList, opResCtx));

    // 记录退出日志和性能统计信息
    CHK_RET(LogHcclExit("HcclAllocComResourceByTiling", ctxTag.c_str(), startut));

    for (uint32_t i = 0U; i < tilingNum; ++i) {
        const Mc2CcTilingInner* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        const OpParam& opParam = opParamVec[i];
        const HcclDataType srcDataType = static_cast<HcclDataType>(ccTiling->srcDataType);
        const HcclDataType dstDataType = static_cast<HcclDataType>(ccTiling->dstDataType);
        const std::string srcDataTypeName = GetDataTypeEnumStr(srcDataType);
        const std::string dstDataTypeName = GetDataTypeEnumStr(dstDataType);
        HCCL_RUN_INFO(
            "[MC2_ALG_INFO] rank[%u], group[%s], opType[%s](%u), algName[%s], "
            "srcDataType[%s](%u), dstDataType[%s](%u), engine[%u].",
            userRank, ccTiling->groupName, GetMc2OpTypeName(opParam.opType), static_cast<uint32_t>(opParam.opType),
            opParam.algName, srcDataTypeName.c_str(), static_cast<uint32_t>(srcDataType), dstDataTypeName.c_str(),
            static_cast<uint32_t>(dstDataType), static_cast<uint32_t>(opParam.engine));
    }

    HCCL_INFO("End to run execute HcclAllocComResourceByTiling");

    return HCCL_SUCCESS;
}

extern "C" HcclResult __attribute__((visibility("default"))) HcclAllocComResourceByTilingA5Mc2(
    HcclComm comm, void* stream, void* mc2Tiling, void** opResCtx)
{
    HCCL_RUN_INFO(
        "[MC2_CLIENT_A5_AICPU] enter asc-devkit explicit A5 MC2 resource allocator, "
        "comm[%p], stream[%p], tiling[%p].",
        comm, stream, mc2Tiling);
    return HcclAllocComResourceByTiling(comm, stream, mc2Tiling, opResCtx);
}
