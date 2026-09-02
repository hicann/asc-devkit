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
#include "ccu_assist_pub.h"
#include "hccl_ccu_res.h"
#include "adapter_acl.h"
#include "kfc_server_protocol.h"

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

// 公共逻辑：构造topoTag/ctxTag并校验ccTiling参数
HcclResult BuildTagsAndValidate(
    const void* ccTilingList[], uint32_t tilingNum, const char* commName, u32 rankSize, u32 userRank,
    std::string topoTag[], std::string& ctxTag)
{
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        const Mc2CcTilingInner* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        topoTag[i] = std::to_string(ccTiling->opType) + "_" + std::to_string(ccTiling->srcDataType) + "_" +
                     std::string(commName);
        CHK_RET(HcclCheckTag(topoTag[i].c_str()));
        bool isReduce;
        CHK_RET(CheckIsReduce(ccTiling, &isReduce));
        CHK_RET(CheckDataType(static_cast<HcclDataType>(ccTiling->srcDataType), isReduce));

        if (i == 0) {
            ctxTag = std::string(ccTiling->groupName) + "_" + std::to_string(ccTiling->opType) + "_" +
                     std::string(ccTiling->algConfig) + "_" + std::to_string(ccTiling->commEngine);
        } else {
            ctxTag += "_" + std::to_string(ccTiling->opType) + "_" + std::string(ccTiling->algConfig) + "_" +
                      std::to_string(ccTiling->commEngine);
        }
    }
    CHK_RET(HcomCheckUserRank(rankSize, userRank));
    return HCCL_SUCCESS;
}

bool HcclIsCcuAlgorithmRegistered(uint32_t opType, const char* algName)
{
    if (algName == nullptr || algorithmMap.count(algName) == 0U) {
        return false;
    }
    return CollAlgExecRegistryV2::Instance().IsRegistered(static_cast<HcclCMDType>(opType), algName);
}

bool CheckCcuAlgorithmsRegistered(const void* ccTilingList[], uint32_t tilingNum)
{
    HCCL_INFO("[CheckCcuAlgorithmsRegistered]Start CheckCcuAlgorithmsRegistered!");
    if (tilingNum > 1) {
        HCCL_WARNING("[AllocComResourceByTilingCcu] tilingNum[%u] is not supported in mc2_client.", tilingNum);
        return false;
    }
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        const auto* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        std::string algName;
        if (!GetForcedAlgName(ccTiling, algName)) {
            HCCL_WARNING("[AllocComResourceByTilingCcu] algName[%s] is not supported in mc2_client.", algName.c_str());
            return false;
        }
        if (!HcclIsCcuAlgorithmRegistered(ccTiling->opType, algName.c_str())) {
            HCCL_WARNING(
                "[AllocComResourceByTilingCcu] algorithm[%s] is not registered in mc2_client for opType[%u].",
                algName.c_str(), ccTiling->opType);
            return false;
        }
    }
    return true;
}

// AICPU引擎资源分配流程
HcclResult AllocComResourceByTilingAicpu(
    HcclComm comm, void* stream, void* mc2Tiling, const void* ccTilingList[], uint32_t tilingNum, const char* commName,
    u32 rankSize, u32 userRank, void** opResCtx, std::string& ctxTag)
{
    std::string topoTag[Hccl::MC2_MAX_OP_NUM];
    CHK_RET(BuildTagsAndValidate(ccTilingList, tilingNum, commName, rankSize, userRank, topoTag, ctxTag));

    std::vector<OpParam> opParamVec(tilingNum);
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        CHK_RET(
            GetOpParam(comm, stream, topoTag[i], static_cast<const Mc2CcTilingInner*>(ccTilingList[i]), opParamVec[i]));
    }

    CHK_RET(HcclAllocOpResCtx(comm, ctxTag, opParamVec, mc2Tiling, ccTilingList, opResCtx));

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

    return HCCL_SUCCESS;
}

// CCU引擎资源分配流程
HcclResult AllocComResourceByTilingCcu(
    HcclComm comm, void* stream, void* mc2Tiling, const void* ccTilingList[], uint32_t tilingNum, const char* commName,
    u32 rankSize, u32 userRank, void** opResCtx, std::string& ctxTag)
{
    HCCL_INFO("[AllocComResourceByTilingCcu]start AllocComResourceByTilingCcu!");
    std::string topoTag[Hccl::MC2_MAX_OP_NUM];
    CHK_RET(BuildTagsAndValidate(ccTilingList, tilingNum, commName, rankSize, userRank, topoTag, ctxTag));
    HCCL_INFO("[AllocComResourceByTilingCcu]BuildTagsAndValidate successfully!");

    // 构建 OpResCtx 基础字段（workspace、XN、CKE等）
    OpResCtx resCtx{};
    CHK_RET(AllocCcuOpResCtx(comm, ctxTag, rankSize, userRank, resCtx));
    HCCL_INFO("[AllocComResourceByTilingCcu]AllocCcuOpResCtx successfully!");
    HCCL_INFO(
        "[AllocComResourceByTilingCcu]allocated: workspace[%p], size[%llu]", (void*)resCtx.workSpace,
        resCtx.workSpaceSize);

    // 逐算子选择算法 + 资源准备（executor->CalcRes + GetAlgResCcu）
    CHK_RET(CcuSelectAlg(comm, stream, topoTag, ccTilingList, tilingNum, mc2Tiling, resCtx));
    HCCL_INFO("[AllocComResourceByTilingCcu]CcuSelectAlg successfully!");

    // 申请OpResCtx硬件内存并写入
    std::string tagOpResCtx = ctxTag + "_opResCtx";
    uint64_t opResCtxSize = sizeof(OpResCtx);
    CHK_RET(GetOrCreateCcuCtx(comm, tagOpResCtx, opResCtxSize, opResCtx));
    aclError aclRet = aclrtMemcpy(*opResCtx, opResCtxSize, &resCtx, opResCtxSize, ACL_MEMCPY_HOST_TO_DEVICE);
    HCCL_INFO(
        "[CCU_DEBUG] opResCtxPtr=%p, *opResCtx=%p, size=%llu ws=0x%llx wsSize=0x%llx xn=0x%llx cke=0x%llx rankId=%llu "
        "rankSize=%llu",
        opResCtx, *opResCtx, opResCtxSize, resCtx.workSpace, resCtx.workSpaceSize, resCtx.xnAddr, resCtx.ckeAddr,
        resCtx.rankId, resCtx.rankSize);
    CHK_RET(aclRet == ACL_ERROR_NONE ? HCCL_SUCCESS : HCCL_E_RUNTIME);
    HCCL_INFO("[AllocComResourceByTilingCcu]end AllocComResourceByTilingCcu!");
    return HCCL_SUCCESS;
}

namespace {
HcclResult HcclAllocComResourceByTilingImpl(HcclComm comm, void* stream, void* mc2Tiling, void** opResCtx)
{
    HCCL_RUN_INFO(
        "[MC2_CLIENT_A5] enter asc-devkit common HcclAllocComResourceByTiling, "
        "comm[%p], stream[%p], tiling[%p].",
        comm, stream, mc2Tiling);
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

    const void* ccTilingList[Hccl::MC2_MAX_OP_NUM];
    uint32_t tilingNum;
    CHK_RET(HcclGetTilingList(mc2Tiling, ccTilingList, tilingNum));

    // 校验commengine
    uint8_t commEngine;
    CHK_RET(ObtainCommEngine(ccTilingList, tilingNum, commEngine));

    // 根据commEngine类型分发到对应的资源分配流程
    std::string ctxTag;
    if (commEngine == static_cast<uint8_t>(OpExecuteConfig::AICPU_TS)) {
        HCCL_INFO("[HcclAllocComResourceByTiling]commEngine == AICPU_TS!");
        CHK_RET(AllocComResourceByTilingAicpu(
            comm, stream, mc2Tiling, ccTilingList, tilingNum, commName, rankSize, userRank, opResCtx, ctxTag));
    } else if (commEngine == static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED)) {
        HCCL_INFO("[HcclAllocComResourceByTiling]commEngine == CCU_SCHED!");
        if (!CheckCcuAlgorithmsRegistered(ccTilingList, tilingNum)) {
            HCCL_INFO("[HcclAllocComResourceByTiling]Current ccu algorithm is not supported in mc2_client.");
            return HCCL_E_ALG_NOT_SUPPORTED;
        }
        CHK_RET(CheckCcuKfcFlow(mc2Tiling, ccTilingList, tilingNum));
        HCCL_INFO("[MC2_DEBUG] before AllocComResourceByTilingCcu.");
        HcclResult ret = AllocComResourceByTilingCcu(
            comm, stream, mc2Tiling, ccTilingList, tilingNum, commName, rankSize, userRank, opResCtx, ctxTag);
        HCCL_INFO("[MC2_DEBUG] after AllocComResourceByTilingCcu, ret[%d].", ret);
        CHK_PRT_RET(
            ret != HCCL_SUCCESS,
            HCCL_ERROR("[MC2_CCU_RESOURCE_ALLOC_FAIL] Failed to allocate CCU resource, please check topo information."),
            ret);
    } else {
        HCCL_ERROR("[%s] unsupported commEngine[%u]", __func__, commEngine);
        return HCCL_E_NOT_SUPPORT;
    }

    // 记录退出日志和性能统计信息
    CHK_RET(LogHcclExit("HcclAllocComResourceByTiling", ctxTag.c_str(), startut));
    return HCCL_SUCCESS;
}
} // namespace

HcclResult __attribute__((visibility("default"))) HcclAllocComResourceByTiling(
    HcclComm comm, void* stream, void* mc2Tiling, void** opResCtx)
{
    return HcclAllocComResourceByTilingImpl(comm, stream, mc2Tiling, opResCtx);
}

extern "C" HcclResult __attribute__((visibility("default"))) HcclAllocComResourceByTilingA5Mc2(
    HcclComm comm, void* stream, void* mc2Tiling, void** opResCtx)
{
    HCCL_RUN_INFO(
        "[MC2_CLIENT_A5_AICPU] enter asc-devkit explicit A5 MC2 resource allocator, "
        "comm[%p], stream[%p], tiling[%p].",
        comm, stream, mc2Tiling);
    return HcclAllocComResourceByTilingImpl(comm, stream, mc2Tiling, opResCtx);
}

namespace {
CcuResult CopyOpResCtxToHost(void* opResCtx, OpResCtx& opResHost)
{
    HCCL_INFO("[CcuKernelLaunch]Obtain OpResCtx.");
    aclError aclRet = aclrtMemcpy(&opResHost, sizeof(OpResCtx), opResCtx, sizeof(OpResCtx), ACL_MEMCPY_DEVICE_TO_HOST);
    CHK_PRT_RET(
        aclRet != ACL_SUCCESS,
        HCCL_ERROR(
            "[CcuKernelLaunch] aclrtMemcpy D2H opResCtx failed, ret[%d], src[%p], size[%zu].", aclRet, opResCtx,
            sizeof(OpResCtx)),
        CCU_E_INTERNAL);
    CHK_PRT_RET(
        opResHost.algInfo[0].opParam == 0U,
        HCCL_ERROR("invalid ccu op resource ctx, opParam[%llu].", opResHost.algInfo[0].opParam), CCU_E_PARA);
    CHK_PRT_RET(
        opResHost.workSpace == 0U || opResHost.workSpaceSize == 0U,
        HCCL_ERROR(
            "invalid ccu op resource ctx, workSpace[%llu], workSpaceSize[%llu].", opResHost.workSpace,
            opResHost.workSpaceSize),
        CCU_E_PARA);
    return CCU_SUCCESS;
}

CcuResult CopyOpParamToHost(const OpResCtx& opResHost, OpParam& opParamHost)
{
    HCCL_INFO("[CcuKernelLaunch]Obtain OpParam.");
    void* opParamDev = reinterpret_cast<void*>(opResHost.algInfo[0].opParam);
    aclError aclRet =
        aclrtMemcpy(&opParamHost, sizeof(OpParam), opParamDev, sizeof(OpParam), ACL_MEMCPY_DEVICE_TO_HOST);
    CHK_PRT_RET(
        aclRet != ACL_SUCCESS,
        HCCL_ERROR(
            "[CcuKernelLaunch] aclrtMemcpy D2H OpParam failed, ret[%d], src[%p], size[%zu].", aclRet, opParamDev,
            sizeof(OpParam)),
        CCU_E_INTERNAL);
    CHK_PRT_RET(
        opParamHost.resCtx == nullptr || opParamHost.ctxSize == 0U,
        HCCL_ERROR("invalid ccu op resource ctx, resCtx[%p], ctxSize[%llu].", opParamHost.resCtx, opParamHost.ctxSize),
        CCU_E_PARA);
    return CCU_SUCCESS;
}

CcuResult LoadResourceCtx(const OpParam& opParamHost, AlgResourceCtxSerializable& resourceCtx)
{
    HCCL_INFO("[CcuKernelLaunch]Obtain resCtx.");
    auto* resCtx = static_cast<char*>(opParamHost.resCtx);
    std::vector<char> seq(opParamHost.ctxSize);
    HCCL_INFO("[CcuKernelLaunch]Start aclrtMemcpy D2H.");
    aclError aclRet =
        aclrtMemcpy(seq.data(), opParamHost.ctxSize, resCtx, opParamHost.ctxSize, ACL_MEMCPY_DEVICE_TO_HOST);
    CHK_PRT_RET(
        aclRet != ACL_SUCCESS,
        HCCL_ERROR(
            "[CcuKernelLaunch] aclrtMemcpy D2H failed, ret[%d], dst[%p], src[%p], size[%llu].", aclRet, seq.data(),
            resCtx, opParamHost.ctxSize),
        CCU_E_INTERNAL);
    HCCL_INFO("[CcuKernelLaunch]Start resourceCtx DeSerialize.");
    resourceCtx.DeSerialize(seq);
    return CCU_SUCCESS;
}

CcuResult GetLaunchMissionNum(const AlgResourceCtxSerializable& resourceCtx, uint32_t& missionNum)
{
    CHK_PRT_RET(resourceCtx.threads.empty(), HCCL_ERROR("empty ccu threads"), CCU_E_PARA);
    CHK_PRT_RET(resourceCtx.ccuKernels.empty(), HCCL_ERROR("empty ccu kernels"), CCU_E_PARA);
    CHK_PRT_RET(
        resourceCtx.kfcServerArgSize != KFC_SERVER_ARG_NUM || resourceCtx.kfcServerArgs.size() < KFC_SERVER_ARG_NUM,
        HCCL_ERROR("invalid kfcServerArgs, kfcServerArgSize[%u].", resourceCtx.kfcServerArgSize), CCU_E_PTR);
    missionNum = static_cast<uint32_t>(resourceCtx.kfcServerArgs[KFC_SERVER_MISSION_NUM_ARG_INDEX]);
    CHK_PRT_RET(
        missionNum == 0U || missionNum > KFC_SERVER_MAX_MISSION_NUM ||
            resourceCtx.kfcServerArgs.size() != missionNum * KFC_SERVER_ARG_NUM,
        HCCL_ERROR("invalid mission layout, missionNum[%u], args[%zu]", missionNum, resourceCtx.kfcServerArgs.size()),
        CCU_E_PARA);
    const uint64_t xnAddr = resourceCtx.kfcServerArgs[KFC_SERVER_XN_ADDR_ARG_INDEX];
    const uint64_t ckeAddr = resourceCtx.kfcServerArgs[KFC_SERVER_CKE_ADDR_ARG_INDEX];
    for (uint32_t missionIndex = 0; missionIndex < missionNum; ++missionIndex) {
        const size_t offset = missionIndex * KFC_SERVER_ARG_NUM;
        CHK_PRT_RET(
            resourceCtx.kfcServerArgs[offset + KFC_SERVER_XN_ADDR_ARG_INDEX] != xnAddr ||
                resourceCtx.kfcServerArgs[offset + KFC_SERVER_CKE_ADDR_ARG_INDEX] != ckeAddr ||
                resourceCtx.kfcServerArgs[offset + KFC_SERVER_MISSION_NUM_ARG_INDEX] != missionNum ||
                resourceCtx.kfcServerArgs[offset + KFC_SERVER_MISSION_INDEX_ARG_INDEX] != missionIndex,
            HCCL_ERROR("inconsistent KFC launch args for mission[%u]", missionIndex), CCU_E_PARA);
    }
    CHK_PRT_RET(
        resourceCtx.threads.size() < missionNum || resourceCtx.ccuKernels.size() < missionNum,
        HCCL_ERROR(
            "insufficient launch handles, missionNum[%u], threads[%zu], kernels[%zu]", missionNum,
            resourceCtx.threads.size(), resourceCtx.ccuKernels.size()),
        CCU_E_PARA);
    for (uint32_t missionIndex = 0; missionIndex < missionNum; ++missionIndex) {
        for (uint32_t previousIndex = 0; previousIndex < missionIndex; ++previousIndex) {
            CHK_PRT_RET(
                resourceCtx.threads[missionIndex] == resourceCtx.threads[previousIndex],
                HCCL_ERROR(
                    "missions[%u] and [%u] use duplicate threadHandle[0x%llx]", previousIndex, missionIndex,
                    static_cast<unsigned long long>(resourceCtx.threads[missionIndex])),
                CCU_E_PARA);
        }
    }
    return CCU_SUCCESS;
}

void LogKernelLaunchArgs(
    const AlgResourceCtxSerializable& resourceCtx, uint32_t missionIndex, ThreadHandle threadHandle,
    CcuKernelHandle kernelHandle)
{
    const size_t offset = missionIndex * KFC_SERVER_ARG_NUM;
    if (resourceCtx.kfcServerArgs.size() >= offset + KFC_SERVER_ARG_NUM) {
        HCCL_INFO(
            "[CcuKernelLaunch] HcommCcuKernelLaunch args: "
            "mission[%u], threadHandle[0x%llx], kernelHandle[0x%llx], argSize[%u], "
            "xnAddr[0x%llx], ckeAddr[0x%llx], dieNum[%llu], missionNum[%llu], "
            "missionIndex[%llu], token[%llu]",
            missionIndex, static_cast<unsigned long long>(threadHandle), static_cast<unsigned long long>(kernelHandle),
            resourceCtx.kfcServerArgSize, static_cast<unsigned long long>(resourceCtx.kfcServerArgs[offset]),
            static_cast<unsigned long long>(resourceCtx.kfcServerArgs[offset + 1]),
            static_cast<unsigned long long>(resourceCtx.kfcServerArgs[offset + 2]),
            static_cast<unsigned long long>(resourceCtx.kfcServerArgs[offset + 3]),
            static_cast<unsigned long long>(resourceCtx.kfcServerArgs[offset + 4]),
            static_cast<unsigned long long>(resourceCtx.kfcServerArgs[offset + 5]));
    } else {
        HCCL_INFO(
            "[CcuKernelLaunch] HcommCcuKernelLaunch args: "
            "threadHandle[0x%llx], kernelHandle[0x%llx], argSize[%u], kfcServerArgsSize[%zu]",
            static_cast<unsigned long long>(threadHandle), static_cast<unsigned long long>(kernelHandle),
            resourceCtx.kfcServerArgSize, resourceCtx.kfcServerArgs.size());
    }
}
} // namespace

CcuResult CcuKernelLaunch(HcclComm comm, void* opResCtx)
{
    CHK_PRT_RET(comm == nullptr, HCCL_ERROR("[%s] comm is nullptr.", __func__), CCU_E_PTR);
    CHK_PRT_RET(opResCtx == nullptr, HCCL_ERROR("[%s] opResCtx is nullptr.", __func__), CCU_E_PTR);

    // HcclEngineCtxCreate分配的OpResCtx、OpParam和序列化资源均位于device，需逐层拷贝到host。
    OpResCtx opResHost{};
    CcuResult ret = CopyOpResCtxToHost(opResCtx, opResHost);
    if (ret != CCU_SUCCESS) {
        return ret;
    }
    OpParam opParamHost{};
    ret = CopyOpParamToHost(opResHost, opParamHost);
    if (ret != CCU_SUCCESS) {
        return ret;
    }
    AlgResourceCtxSerializable resourceCtx;
    ret = LoadResourceCtx(opParamHost, resourceCtx);
    if (ret != CCU_SUCCESS) {
        return ret;
    }
    uint32_t missionNum = 0;
    ret = GetLaunchMissionNum(resourceCtx, missionNum);
    if (ret != CCU_SUCCESS) {
        return ret;
    }
    HCCL_INFO("[CcuKernelLaunch] start [%u] KFC server missions.", missionNum);
    for (uint32_t missionIndex = 0; missionIndex < missionNum; ++missionIndex) {
        const ThreadHandle threadHandle = resourceCtx.threads[missionIndex];
        const CcuKernelHandle kernelHandle = resourceCtx.ccuKernels[missionIndex];
        CHK_PRT_RET(
            threadHandle == 0 || kernelHandle == 0, HCCL_ERROR("invalid launch handles for mission[%u]", missionIndex),
            CCU_E_PARA);
        LogKernelLaunchArgs(resourceCtx, missionIndex, threadHandle, kernelHandle);
        const void* kfcArgs =
            static_cast<const void*>(resourceCtx.kfcServerArgs.data() + missionIndex * KFC_SERVER_ARG_NUM);
        ret = HcommCcuKernelLaunch(threadHandle, kernelHandle, kfcArgs, resourceCtx.kfcServerArgSize);
        if (ret != CCU_SUCCESS) {
            HCCL_ERROR("[CcuKernelLaunch] mission[%u] launch failed, ret[%d]", missionIndex, ret);
            return ret;
        }
    }
    return CCU_SUCCESS;
}
