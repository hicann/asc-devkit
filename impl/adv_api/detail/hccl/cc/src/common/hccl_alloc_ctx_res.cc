/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "hccl_alloc_ctx_res.h"

using namespace mc2_ops_hccl;
using namespace hcomm::CcuRep;

std::unordered_map<HcclCMDType, OpParamPrepareFunc> opParamPrepareFuncMap = {
    {HcclCMDType::HCCL_CMD_ALLGATHER, PrepareParamForAllGather},
    {HcclCMDType::HCCL_CMD_ALLREDUCE, PrepareParamForAllReduce},
    {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, PrepareParamForReduceScatter},
    {HcclCMDType::HCCL_CMD_ALLTOALL, PrepareParamForAlltoAll},
    {HcclCMDType::HCCL_CMD_ALLTOALLV, PrepareParamForAlltoAllV},
};

CommEngine OpExecuteConfigToCommEngine(uint8_t commEngine)
{
    if (static_cast<OpExecuteConfig>(commEngine) == OpExecuteConfig::AICPU_TS) {
        return COMM_ENGINE_AICPU;
    } else if (static_cast<OpExecuteConfig>(commEngine) == OpExecuteConfig::CCU_SCHED) {
        return COMM_ENGINE_CCU;
    }
    return COMM_ENGINE_RESERVED;
}

HcclResult CheckInputParam(const HcclComm comm, const void* mc2Tiling, const aclrtStream stream)
{
    // 检查comm是否为空指针
    RPT_INPUT_ERR(
        comm == nullptr, "EI0003", std::vector<std::string>({"ccl_op", "value", "parameter", "expect"}),
        std::vector<std::string>({"HcclAllocComResourceByTiling", "nullptr", "comm", "non-null pointer"}));
    CHK_PTR_NULL(comm);

    // 检查sendBuf是否为空指针
    RPT_INPUT_ERR(
        mc2Tiling == nullptr, "EI0003", std::vector<std::string>({"ccl_op", "value", "parameter", "expect"}),
        std::vector<std::string>({"HcclAllocComResourceByTiling", "nullptr", "mc2Tiling", "non-null pointer"}));
    CHK_PTR_NULL(mc2Tiling);

    // 检查stream是否为空指针
    RPT_INPUT_ERR(
        stream == nullptr, "EI0003", std::vector<std::string>({"ccl_op", "value", "parameter", "expect"}),
        std::vector<std::string>({"HcclAllocComResourceByTiling", "nullptr", "stream", "non-null pointer"}));
    CHK_PTR_NULL(stream);

    return HCCL_SUCCESS;
}

HcclResult HcclGetTilingList(const void* mc2Tiling, const void* p[], uint32_t& cnt)
{
    const u32* versionPtr = static_cast<const u32*>(mc2Tiling);
    const u32 version = *(versionPtr++);
    CHK_PRT_RET(version < MC2_TILING_VERSION, HCCL_ERROR("Invalid tiling version %u.", version), HCCL_E_PARA);

    cnt = *(versionPtr++);
    CHK_PRT_RET(cnt > MAX_HCOM_NUM, HCCL_ERROR("Invalid hcom tiling number %u.", cnt), HCCL_E_PARA);

    u64 serverCfgAddr = reinterpret_cast<u64>(versionPtr) + sizeof(Mc2ServerCfg);
    for (uint32_t i = 0U; i < MAX_CC_TILING_NUM; ++i) {
        p[i] = reinterpret_cast<const void*>(reinterpret_cast<const u8*>(mc2Tiling) + versionPtr[i]);
    }
    HCCL_INFO("HcclGetTilingList version[%u] cnt[%u]", version, cnt);
    return HCCL_SUCCESS;
}

HcclResult CheckIsReduce(const Mc2CcTilingInner* ccTiling, bool* isReduce)
{
    if (ccTiling->opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER || ccTiling->opType == HcclCMDType::HCCL_CMD_REDUCE ||
        ccTiling->opType == HcclCMDType::HCCL_CMD_ALLREDUCE) {
        *isReduce = true;
    } else {
        *isReduce = false;
    }
    return HCCL_SUCCESS;
}

HcclResult CheckCommEngine(const void* ccTilingList[], uint32_t tilingNum)
{
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        const Mc2CcTilingInner* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        if (ccTiling->commEngine != static_cast<uint8_t>(COMM_ENGINE_AICPU) &&
            ccTiling->commEngine != static_cast<uint8_t>(COMM_ENGINE_CPU)) {
            HCCL_ERROR("Invalid commEngine %u.", ccTiling->commEngine);
            return HCCL_E_NOT_SUPPORT;
        }
    }
    return HCCL_SUCCESS;
}

HcclResult ObtainCommEngine(const void* ccTilingList[], uint32_t tilingNum, uint8_t& commEngine)
{
    commEngine = static_cast<uint8_t>(OpExecuteConfig::DEFAULT);
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        const Mc2CcTilingInner* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        if (commEngine == static_cast<uint8_t>(OpExecuteConfig::DEFAULT)) {
            commEngine = ccTiling->commEngine;
        }
        if (ccTiling->commEngine != commEngine) {
            HCCL_ERROR("Invalid commEngine %u.", ccTiling->commEngine);
            return HCCL_E_NOT_SUPPORT;
        }
    }

    if (commEngine != static_cast<uint8_t>(OpExecuteConfig::AICPU_TS) &&
        commEngine != static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED)) {
        HCCL_ERROR("Invalid commEngine %u.", commEngine);
        return HCCL_E_NOT_SUPPORT;
    }
    return HCCL_SUCCESS;
}

namespace {
bool IsCcuKfcSupportedOp(uint32_t opType)
{
    return opType == static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER) ||
           opType == static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
}

HcclResult GetOrCreateCcuCtx(HcclComm comm, const std::string& tag, uint64_t ctxSize, void** ctx)
{
    uint64_t actualSize = ctxSize;
    if (HcclEngineCtxGet(comm, tag.c_str(), COMM_ENGINE_AIV, ctx, &actualSize) == HCCL_SUCCESS) {
        HCCL_INFO(
            "[AllocCcuOpResCtx] HcclEngineCtxGet success, tag[%s], ctxAddr[%p], ctxSize[%llu]", tag.c_str(), *ctx,
            static_cast<unsigned long long>(actualSize));
        return HCCL_SUCCESS;
    }
    return HcclEngineCtxCreate(comm, tag.c_str(), COMM_ENGINE_AIV, ctxSize, ctx);
}
} // namespace

HcclResult CheckCcuKfcFlow(const void* mc2Tiling, const void* ccTilingList[], uint32_t tilingNum)
{
    CHK_PTR_NULL(mc2Tiling);
    CHK_PTR_NULL(ccTilingList);
    const auto* initTiling = static_cast<const Mc2InitTilingInner*>(mc2Tiling);
    if (initTiling->version != INIT_TILING_CCU_NEW_VERSION) {
        HCCL_ERROR("Invalid CCU KFC tiling version %u, expected %u.", initTiling->version, INIT_TILING_CCU_NEW_VERSION);
        return HCCL_E_NOT_SUPPORT;
    }
    if (tilingNum == 0U || tilingNum > MAX_CC_TILING_NUM || tilingNum != initTiling->mc2HcommCnt) {
        HCCL_ERROR("Invalid CCU KFC tiling number %u, init tiling number %u.", tilingNum, initTiling->mc2HcommCnt);
        return HCCL_E_NOT_SUPPORT;
    }
    uint32_t expectedOpType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_INVALID);
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        const auto* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        CHK_PTR_NULL(ccTiling);
        if (expectedOpType == static_cast<uint32_t>(HcclCMDType::HCCL_CMD_INVALID)) {
            expectedOpType = ccTiling->opType;
        }
        if (!IsCcuKfcSupportedOp(ccTiling->opType) ||
            ccTiling->commEngine != static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED) ||
            ccTiling->opType != expectedOpType) {
            HCCL_ERROR(
                "Unsupported CCU KFC tiling at index %u, opType %u, commEngine %u.", i, ccTiling->opType,
                ccTiling->commEngine);
            return HCCL_E_NOT_SUPPORT;
        }
    }
    return HCCL_SUCCESS;
}

// 构建 opResCtx 基础字段
HcclResult AllocCcuOpResCtx(HcclComm comm, const std::string& ctxTag, u32 rankSize, u32 userRank, OpResCtx& opResCtx)
{
    // 1. 分配workspace、scratch、comParam（XN）、comSync（CKE）
    constexpr uint32_t comSyncNum = 2;
    constexpr uint64_t scratchSize = Hccl::MC2_WORKSPACE_SIZE;
    uint64_t comParamBufSize = Hccl::CCU_TASK_NUM_MAX * Hccl::CCU_PARAM_NUM_MAX * Hccl::CCU_ONE_PARAM_SIZE;
    uint64_t comSyncBufSize = Hccl::CCU_TASK_NUM_MAX * comSyncNum * Hccl::CCU_ONE_PARAM_SIZE;

    std::string tagWorkspace = ctxTag + "_ccu_workspace";
    std::string tagScratch = ctxTag + "_ccu_scratch";
    std::string tagComParam = ctxTag + "_ccu_comParam";
    std::string tagComSync = ctxTag + "_ccu_comSync";

    void* workspacePtr = nullptr;
    void* scratchPtr = nullptr;
    void* comParamPtr = nullptr;
    void* comSyncPtr = nullptr;

    CHK_RET(GetOrCreateCcuCtx(comm, tagWorkspace, Hccl::MC2_WORKSPACE_SIZE, &workspacePtr));
    CHK_RET(GetOrCreateCcuCtx(comm, tagScratch, scratchSize, &scratchPtr));
    CHK_RET(GetOrCreateCcuCtx(comm, tagComParam, comParamBufSize, &comParamPtr));
    CHK_RET(GetOrCreateCcuCtx(comm, tagComSync, comSyncBufSize, &comSyncPtr));

    uint64_t ckeTokenInfo = hcomm::CcuRep::GetTokenInfo(reinterpret_cast<uint64_t>(comSyncPtr), comSyncBufSize);
    HCCL_INFO(
        "[CcuTokenTrace][ComSync] ckeAddr[0x%llx], size[%llu], token[0x%llx]",
        reinterpret_cast<unsigned long long>(comSyncPtr), static_cast<unsigned long long>(comSyncBufSize),
        static_cast<unsigned long long>(ckeTokenInfo));

    opResCtx.workSpace = reinterpret_cast<uint64_t>(workspacePtr);
    opResCtx.workSpaceSize = Hccl::MC2_WORKSPACE_SIZE;
    opResCtx.rankId = userRank;
    opResCtx.rankSize = rankSize;
    opResCtx.xnAddr = reinterpret_cast<uint64_t>(comParamPtr);
    opResCtx.ckeAddr = reinterpret_cast<uint64_t>(comSyncPtr);
    // ReduceScatter KFC follows pr_4523 and partitions this fixed 16 MiB buffer as rankSize * sliceSize.
    opResCtx.res[0] = reinterpret_cast<uint64_t>(scratchPtr);

    HCCL_INFO(
        "[AllocCcuOpResCtx] workspace[%p], workspaceSize[%llu], scratch[%p], scratchSize[%llu], "
        "comParam[%p], comParamBufSize[%llu], comSync[%p], comSyncBufSize[%llu], "
        "rankId[%u], rankSize[%u], xnAddr[0x%llx], ckeAddr[0x%llx]",
        workspacePtr, Hccl::MC2_WORKSPACE_SIZE, scratchPtr, scratchSize, comParamPtr, comParamBufSize, comSyncPtr,
        comSyncBufSize, userRank, rankSize, static_cast<unsigned long long>(opResCtx.xnAddr),
        static_cast<unsigned long long>(opResCtx.ckeAddr));

    return HCCL_SUCCESS;
}

namespace {
HcclResult AllocOpParamMemory(
    HcclComm comm, const std::string& ctxTag, const std::vector<OpParam>& opParamVec,
    const Mc2InitTilingInner* initTiling, const void* ccTilingList[], OpResCtx& resCtx)
{
    std::vector<uint64_t> opParamAddr(opParamVec.size());
    uint64_t opParamSize = sizeof(OpParam);
    for (uint32_t i = 0U; i < opParamVec.size(); ++i) {
        std::string tagParam = ctxTag + "_" + std::to_string(i);
        void* opParamPtr = nullptr;
        const Mc2CcTilingInner* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        if (HcclEngineCtxGet(
                comm, tagParam.c_str(), OpExecuteConfigToCommEngine(ccTiling->commEngine), &opParamPtr, &opParamSize) ==
            HCCL_SUCCESS) {
            HCCL_INFO(
                "HcclEngineCtxGet success, tagParam[%s], opParamAddr[%p], opParamSize[%u]", tagParam.c_str(),
                opParamPtr, opParamSize);
            opParamAddr[i] = reinterpret_cast<uint64_t>(opParamPtr);
        } else {
            CHK_RET(HcclEngineCtxCreate(
                comm, tagParam.c_str(), OpExecuteConfigToCommEngine(ccTiling->commEngine), opParamSize, &opParamPtr));
            opParamAddr[i] = reinterpret_cast<uint64_t>(opParamPtr);
        }
        HCCL_INFO(
            "HcclAllocOpResCtx the %dth opParam: opParamAddr[%u], opParamSize[%u]", i, opParamAddr[i], opParamSize);

        aclError aclRet = aclrtMemcpy(
            reinterpret_cast<void*>(opParamAddr[i]), opParamSize, &opParamVec[i], opParamSize, aclrtMemcpyKind(1));
        CHK_RET(aclRet == ACL_ERROR_NONE ? HCCL_SUCCESS : HCCL_E_RUNTIME);
        resCtx.algInfo[i].opParam = opParamAddr[i];
        resCtx.algInfo[i].offset = initTiling->offset[i];
    }
    return HCCL_SUCCESS;
}

HcclResult AllocWorkspaceMemory(
    HcclComm comm, const std::string& ctxTag, const Mc2CcTilingInner* ccTiling, OpResCtx& resCtx)
{
    uint64_t memSize = 20 * 1024 * 1024;
    resCtx.workSpaceSize = memSize;
    std::string tagWorkSpace = ctxTag + "_workSpace";
    void* workSpacePtr = nullptr;
    if (HcclEngineCtxGet(
            comm, tagWorkSpace.c_str(), OpExecuteConfigToCommEngine(ccTiling->commEngine), &workSpacePtr, &memSize) ==
        HCCL_SUCCESS) {
        HCCL_INFO(
            "HcclEngineCtxGet success, tagWorkSpace[%s], workSpaceAddr[%p], workSpaceSize[%u]", tagWorkSpace.c_str(),
            workSpacePtr, memSize);
        resCtx.workSpace = reinterpret_cast<uint64_t>(workSpacePtr);
    } else {
        CHK_RET(HcclEngineCtxCreate(
            comm, tagWorkSpace.c_str(), OpExecuteConfigToCommEngine(ccTiling->commEngine), memSize, &workSpacePtr));
        resCtx.workSpace = reinterpret_cast<uint64_t>(workSpacePtr);
    }
    HCCL_INFO("HcclAllocOpResCtx the workSpace: workSpaceAddr[%u], workSpaceSize[%u]", resCtx.workSpace, memSize);
    return HCCL_SUCCESS;
}

HcclResult FillRankInfo(HcclComm comm, OpResCtx& resCtx)
{
    uint32_t rankSize = 0U;
    uint32_t rankId = 0U;
    CHK_RET(HcclGetRankSize(comm, &rankSize));
    CHK_RET(HcclGetRankId(comm, &rankId));
    resCtx.rankSize = rankSize;
    resCtx.rankId = rankId;
    return HCCL_SUCCESS;
}

HcclResult AllocAndCopyOpResCtx(
    HcclComm comm, const std::string& ctxTag, const Mc2CcTilingInner* ccTiling, const OpResCtx& resCtx,
    void** opResCtxPtr)
{
    std::string tagOpResCtx = ctxTag + "_opResCtx";
    uint64_t opResCtxSize = sizeof(OpResCtx);
    if (HcclEngineCtxGet(
            comm, tagOpResCtx.c_str(), OpExecuteConfigToCommEngine(ccTiling->commEngine), opResCtxPtr, &opResCtxSize) ==
        HCCL_SUCCESS) {
        HCCL_INFO(
            "HcclEngineCtxGet success, tagOpResCtx[%s], opResCtxAddr[%p], opResCtxSize[%u]", tagOpResCtx.c_str(),
            opResCtxPtr, opResCtxSize);
    } else {
        CHK_RET(HcclEngineCtxCreate(
            comm, tagOpResCtx.c_str(), OpExecuteConfigToCommEngine(ccTiling->commEngine), opResCtxSize, opResCtxPtr));
    }

    HCCL_INFO("HcclAllocOpResCtx the opResCtx: opResCtxAddr[%u], opResCtxSize[%u]", opResCtxPtr, opResCtxSize);

    aclError aclRet = aclrtMemcpy(*opResCtxPtr, opResCtxSize, &resCtx, opResCtxSize, aclrtMemcpyKind(1));
    CHK_RET(aclRet == ACL_ERROR_NONE ? HCCL_SUCCESS : HCCL_E_RUNTIME);
    return HCCL_SUCCESS;
}
} // namespace

HcclResult HcclAllocOpResCtx(
    HcclComm comm, const std::string& ctxTag, const std::vector<OpParam>& opParamVec, void* mc2Tiling,
    const void* ccTilingList[], void** opResCtxPtr)
{
    CHK_PTR_NULL(opResCtxPtr);
    OpResCtx resCtx{};
    const auto* initTiling = static_cast<const Mc2InitTilingInner*>(mc2Tiling);
    const auto* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[0]);

    CHK_RET(AllocOpParamMemory(comm, ctxTag, opParamVec, initTiling, ccTilingList, resCtx));
    CHK_RET(AllocWorkspaceMemory(comm, ctxTag, ccTiling, resCtx));
    CHK_RET(FillRankInfo(comm, resCtx));
    return AllocAndCopyOpResCtx(comm, ctxTag, ccTiling, resCtx, opResCtxPtr);
}

// AllToAll适配AllToAllV
HcclResult ConvertAlltoAllParam(
    const u64 recvCount, const u32 rankSize, std::vector<u64>& sdispls, std::vector<u64>& rdispls)
{
    u64 dataCountOffset = 0;
    for (u64 i = 0; i < rankSize; i++) {
        sdispls[i] = dataCountOffset;
        rdispls[i] = dataCountOffset;
        dataCountOffset += recvCount;
    }
    return HCCL_SUCCESS;
}

HcclResult PrepareOpsCommParam(const std::string& tag, OpParam& param)
{
    param.opMode = OpMode::OPBASE;
    DevType deviceType = DevType::DEV_TYPE_COUNT;
    CHK_RET(hrtGetDeviceType(deviceType));

    // topoInfo的tag，所有相同的算子可以共享
    int ret = sprintf_s(param.tag, sizeof(param.tag), "%s", tag.c_str());
    if (ret <= 0) {
        HCCL_ERROR("failed to fill param.tag");
        return HCCL_E_INTERNAL;
    }

    param.inputPtr = nullptr;
    param.outputPtr = nullptr;
    param.inputSize = 0;
    param.outputSize = 0;
    param.enableDetour = false;
    param.deviceType = deviceType;

    return HCCL_SUCCESS;
}

HcclResult PrintOpsCommParam(OpParam& param)
{
    HCCL_INFO("commName: %s", param.commName);
    HCCL_INFO("tag:%s", param.tag);
    HCCL_INFO("stream: %p", param.stream);
    HCCL_INFO("inputPtr %p", param.inputPtr);
    HCCL_INFO("outputPtr %p", param.outputPtr);
    HCCL_INFO("inputSize %lu", param.inputSize);
    HCCL_INFO("outputSize %lu", param.outputSize);
    HCCL_INFO("opMode %u", static_cast<uint32_t>(param.opMode));
    HCCL_INFO("deviceType %u", static_cast<uint32_t>(param.deviceType));
    return HCCL_SUCCESS;
}

HcclResult PrepareParamForAllGather(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param)
{
    HCCL_INFO("PrepareParamForAllGather, ccTiling[%p]", ccTiling);
    u32 userRankSize;
    CHK_RET(HcclGetRankSize(comm, &userRankSize)); // 加上，后面计算outputSize可能使用

    HcclResult ret = PrepareOpsCommParam(tag, param);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("failed to fill OpsCommParam");
    }

    param.opType = HcclCMDType::HCCL_CMD_ALLGATHER;
    param.reduceType = HcclReduceOp::HCCL_REDUCE_SUM;
    param.DataDes.dataType = static_cast<HcclDataType>(ccTiling->srcDataType);
    param.DataDes.count = 0;

    HCCL_INFO("Print PrepareParamForAllGather.");
    CHK_RET(PrintOpsCommParam(param));
    HCCL_INFO("opType %u", static_cast<uint32_t>(param.opType));
    HCCL_INFO("DataDes.dataType %u", static_cast<uint32_t>(param.DataDes.dataType));
    HCCL_INFO("DataDes.count %lu", param.DataDes.count);
    HCCL_INFO("Execute PrepareParamForAllGather success.");
    return HCCL_SUCCESS;
}

HcclResult PrepareParamForAllReduce(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param)
{
    HCCL_INFO("PrepareParamForAllReduce, ccTiling[%p]", ccTiling);
    u32 userRankSize;
    CHK_RET(HcclGetRankSize(comm, &userRankSize)); // 加上，后面计算outputSize可能使用

    HcclResult ret = PrepareOpsCommParam(tag, param);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("failed to fill OpsCommParam");
    }

    param.opType = HcclCMDType::HCCL_CMD_ALLREDUCE;
    param.reduceType = static_cast<HcclReduceOp>(ccTiling->reduceType);
    param.DataDes.dataType = static_cast<HcclDataType>(ccTiling->srcDataType);
    param.DataDes.outputType = param.DataDes.dataType;
    param.DataDes.count = 0;

    HCCL_INFO("Print PrepareParamForAllReduce.");
    CHK_RET(PrintOpsCommParam(param));
    HCCL_INFO("opType %u", static_cast<uint32_t>(param.opType));
    HCCL_INFO("reduceType %u", static_cast<uint32_t>(param.reduceType));
    HCCL_INFO("DataDes.dataType %u", static_cast<uint32_t>(param.DataDes.dataType));
    HCCL_INFO("DataDes.outputType %u", static_cast<uint32_t>(param.DataDes.outputType));
    HCCL_INFO("DataDes.count %lu", param.DataDes.count);
    HCCL_INFO("Execute PrepareParamForAllReduce success.");
    return HCCL_SUCCESS;
}

HcclResult PrepareParamForReduceScatter(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param)
{
    HCCL_INFO("PrepareParamForReduceScatter, ccTiling[%p]", ccTiling);
    u32 userRankSize;
    CHK_RET(HcclGetRankSize(comm, &userRankSize)); // 加上，后面计算outputSize可能使用

    HcclResult ret = PrepareOpsCommParam(tag, param);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("failed to fill OpsCommParam");
    }

    param.opType = HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
    param.reduceType = static_cast<HcclReduceOp>(ccTiling->reduceType);
    param.DataDes.dataType = static_cast<HcclDataType>(ccTiling->srcDataType);
    param.DataDes.count = 0;

    HCCL_INFO("Print PrepareParamForReduceScatter.");
    CHK_RET(PrintOpsCommParam(param));
    HCCL_INFO("opType %u", static_cast<uint32_t>(param.opType));
    HCCL_INFO("reduceType %u", static_cast<uint32_t>(param.reduceType));
    HCCL_INFO("DataDes.dataType %u", static_cast<uint32_t>(param.DataDes.dataType));
    HCCL_INFO("DataDes.count %lu", param.DataDes.count);
    HCCL_INFO("Execute PrepareParamForReduceScatter success.");
    return HCCL_SUCCESS;
}

HcclResult PrepareParamForAlltoAll(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param)
{
    HCCL_INFO("PrepareParamForAlltoAll, ccTiling[%p]", ccTiling);
    u32 userRankSize;
    CHK_RET(HcclGetRankSize(comm, &userRankSize));

    HcclResult ret = PrepareOpsCommParam(tag, param);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("failed to fill OpsCommParam");
    }

    u64 varMemSize = ALL_TO_ALL_V_VECTOR_NUM * userRankSize * sizeof(u64);
    param.varMemSize = varMemSize;
    param.opType = HcclCMDType::HCCL_CMD_ALLTOALL;
    param.all2AllVDataDes.sendType = static_cast<HcclDataType>(ccTiling->srcDataType);
    param.all2AllVDataDes.recvType = static_cast<HcclDataType>(ccTiling->dstDataType);
    param.all2AllVDataDes.sendCounts = nullptr;
    param.all2AllVDataDes.recvCounts = nullptr;
    param.all2AllVDataDes.sdispls = nullptr;
    param.all2AllVDataDes.rdispls = nullptr;

    HCCL_INFO("Print PrepareParamForAlltoAll.");
    CHK_RET(PrintOpsCommParam(param));
    HCCL_INFO("varMemSize %lu", param.varMemSize);
    HCCL_INFO("opType %u", static_cast<uint32_t>(param.opType));
    HCCL_INFO("all2AllVDataDes.sendType %u", static_cast<uint32_t>(param.all2AllVDataDes.sendType));
    HCCL_INFO("all2AllVDataDes.recvType %u", static_cast<uint32_t>(param.all2AllVDataDes.recvType));
    HCCL_INFO("Execute PrepareParamForAlltoAll success.");
    return HCCL_SUCCESS;
}

HcclResult PrepareParamForAlltoAllV(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param)
{
    HCCL_INFO("PrepareParamForAlltoAllV, ccTiling[%p]", ccTiling);
    u32 userRankSize;
    CHK_RET(HcclGetRankSize(comm, &userRankSize));

    HcclResult ret = PrepareOpsCommParam(tag, param);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("failed to fill OpsCommParam");
    }

    u64 varMemSize = ALL_TO_ALL_V_VECTOR_NUM * userRankSize * sizeof(u64);
    param.varMemSize = varMemSize;
    param.opType = HcclCMDType::HCCL_CMD_ALLTOALLV;
    param.all2AllVDataDes.sendType = static_cast<HcclDataType>(ccTiling->srcDataType);
    param.all2AllVDataDes.recvType = static_cast<HcclDataType>(ccTiling->dstDataType);
    param.all2AllVDataDes.sendCounts = nullptr;
    param.all2AllVDataDes.recvCounts = nullptr;
    param.all2AllVDataDes.sdispls = nullptr;
    param.all2AllVDataDes.rdispls = nullptr;

    HCCL_INFO("Print PrepareParamForAlltoAllV.");
    CHK_RET(PrintOpsCommParam(param));
    HCCL_INFO("varMemSize %lu", param.varMemSize);
    HCCL_INFO("opType %u", static_cast<uint32_t>(param.opType));
    HCCL_INFO("all2AllVDataDes.sendType %u", static_cast<uint32_t>(param.all2AllVDataDes.sendType));
    HCCL_INFO("all2AllVDataDes.recvType %u", static_cast<uint32_t>(param.all2AllVDataDes.recvType));
    return HCCL_SUCCESS;
}

HcclResult PrepareOpParams(HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& opParam)
{
    auto it = opParamPrepareFuncMap.find(static_cast<HcclCMDType>(ccTiling->opType));
    if (it != opParamPrepareFuncMap.end()) {
        return it->second(comm, tag, ccTiling, opParam);
    }
    HCCL_ERROR("PrepareOpParams error, opType[%d] not found", ccTiling->opType);
    return HCCL_E_INTERNAL;
}

HcclResult InitOpParamByTiling(
    HcclComm comm, void* stream, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& opParam)
{
    opParam.opType = static_cast<HcclCMDType>(ccTiling->opType);
    opParam.stream = reinterpret_cast<aclrtStream>(stream);
    opParam.engine = OpExecuteConfigToCommEngine(ccTiling->commEngine);
    CHK_RET(HcclGetCommName(comm, opParam.commName));
    CHK_RET(PrepareOpParams(comm, tag, ccTiling, opParam));
    if (opParam.opType == HcclCMDType::HCCL_CMD_ALLTOALL) {
        opParam.all2AllVDataDes.sendType = static_cast<HcclDataType>(ccTiling->srcDataType);
        // sendCounts指向的host侧数组由调用方(GetOpParam)持有并填充，此处保持nullptr，
        // 避免将数值当作指针使用导致后续解引用非法地址。
    }
    return HCCL_SUCCESS;
}

HcclResult PrepareTopoInfoForOp(HcclComm comm, OpParam& opParam, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo)
{
    HCCL_INFO("[SelectAlgAndPrepareEngine]start SelectAlgAndPrepareEngine!");
    opParam.hcclComm = comm;
    CHK_RET(HcclGetOpExpansionMode(comm, opParam));
    HCCL_INFO("[SelectAlgAndPrepareEngine]  successfully!");
    CHK_RET(HcclCalcTopoInfo(comm, opParam, topoInfo));
    HCCL_INFO("[SelectAlgAndPrepareEngine]HcclCalcTopoInfo successfully!");
    return HCCL_SUCCESS;
}

HcclResult PrepareEngineForAlg(OpParam& opParam, const std::string& algName)
{
    CHK_RET(SetCommEngine(opParam));
    if (GetExternalInputHcclAivOnlyMode() && opParam.engine != COMM_ENGINE_AIV) {
        HCCL_ERROR(
            "[HcclExecOp] opType[%d] currently do not select aiv mode, aiv only not support.",
            static_cast<int>(opParam.opType));
        return HCCL_E_NOT_SUPPORT;
    }
    if ((opParam.engine == COMM_ENGINE_AICPU_TS) || (opParam.engine == COMM_ENGINE_CPU)) {
        HCCL_DEBUG("[Selector] is aicpu mode");
        CHK_RET(LoadAICPUKernel());
    }
    CHK_RET(SetOpParamAlgTag(opParam, algName));
    return HCCL_SUCCESS;
}

std::unique_ptr<InsCollAlgBase> GetAlgExecutorForOp(const OpParam& opParam, const std::string& algName)
{
    if (UseCannBridge(opParam)) {
        return GetAlgExecViaCann(opParam.opType, algName);
    }
    return CollAlgExecRegistryV2::Instance().GetAlgExec(opParam.opType, algName);
}

HcclResult CheckForcedAlgResource(
    HcclComm comm, const OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo, const std::string& algName)
{
    std::unique_ptr<InsCollAlgBase> executor = GetAlgExecutorForOp(opParam, algName);
    CHK_PRT_RET(
        executor == nullptr,
        HCCL_WARNING(
            "[MC2_FORCE_ALG] fallback, algName[%s] is not registered for opType[%u].", algName.c_str(),
            static_cast<u32>(opParam.opType)),
        HCCL_E_NOT_SUPPORT);

    AlgHierarchyInfoForAllLevel algHierarchyInfo;
    HcclResult ret = executor->CalcAlgHierarchyInfo(comm, topoInfo, algHierarchyInfo);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_WARNING(
            "[MC2_FORCE_ALG] fallback, CalcAlgHierarchyInfo failed, algName[%s], ret[%d].", algName.c_str(),
            static_cast<int>(ret)),
        HCCL_E_NOT_SUPPORT);

    AlgResourceRequest resRequest;
    ret = executor->CalcRes(comm, opParam, topoInfo, algHierarchyInfo, resRequest);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_WARNING(
            "[MC2_FORCE_ALG] fallback, CalcRes failed, algName[%s], ret[%d].", algName.c_str(), static_cast<int>(ret)),
        HCCL_E_NOT_SUPPORT);
    return HCCL_SUCCESS;
}

bool GetForcedAlgName(const Mc2CcTilingInner* ccTiling, std::string& algName)
{
    if (ccTiling == nullptr || ccTiling->algConfig[0] == '\0') {
        return false;
    }

    std::string algConfig(ccTiling->algConfig);
    if (algConfig.find('=') != std::string::npos) {
        HCCL_INFO("[MC2_FORCE_ALG] legacy algConfig[%s], use default selector.", algConfig.c_str());
        return false;
    }

    algName = algConfig;
    return true;
}

HcclResult TryForcedAlgAndPrepareEngine(
    HcclComm comm, const Mc2CcTilingInner* ccTiling, OpParam& opParam, std::string& algName,
    std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo, bool& forcedAlgAccepted)
{
    forcedAlgAccepted = false;
    std::string forcedAlgName;
    if (!GetForcedAlgName(ccTiling, forcedAlgName)) {
        return HCCL_SUCCESS;
    }

    OpParam opParamBackup = opParam;
    CHK_RET(PrepareTopoInfoForOp(comm, opParam, topoInfo));
    HcclResult ret = PrepareEngineForAlg(opParam, forcedAlgName);
    if (ret != HCCL_SUCCESS) {
        opParam = opParamBackup;
        HCCL_ERROR(
            "[MC2_FORCE_ALG] prepare engine failed, algName[%s], ret[%d].", forcedAlgName.c_str(),
            static_cast<int>(ret));
        return ret;
    }

    ret = CheckForcedAlgResource(comm, opParam, topoInfo.get(), forcedAlgName);
    if (ret != HCCL_SUCCESS) {
        opParam = opParamBackup;
        topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();
        algName.clear();
        forcedAlgAccepted = false;
        HCCL_WARNING(
            "[MC2_FORCE_ALG] fallback to default selector, opType[%u], algConfig[%s].",
            static_cast<u32>(opParam.opType), forcedAlgName.c_str());
        return HCCL_SUCCESS;
    }

    algName = forcedAlgName;
    forcedAlgAccepted = true;
    HCCL_INFO("[MC2_FORCE_ALG] accepted, opType[%u], algName[%s].", static_cast<u32>(opParam.opType), algName.c_str());
    return HCCL_SUCCESS;
}

HcclResult SelectAlgAndPrepareEngine(
    HcclComm comm, OpParam& opParam, std::string& algName, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo)
{
    CHK_RET(PrepareTopoInfoForOp(comm, opParam, topoInfo));

    std::shared_ptr<ExecuteSelector> collAlgSelector = std::make_shared<ExecuteSelector>(ExecuteSelector());
    CHK_RET(collAlgSelector->Run(opParam, topoInfo.get(), algName));
    if (algName.empty()) {
        HCCL_ERROR("[Selector] select algname fail!");
        return HCCL_E_PTR;
    }

    CHK_RET(PrepareEngineForAlg(opParam, algName));
    return HCCL_SUCCESS;
}

HcclResult FillOpParamAlgName(OpParam& opParam, const std::string& algName)
{
    int result = sprintf_s(opParam.algName, sizeof(opParam.algName), "%s", algName.c_str());
    CHK_PRT_RET(result <= 0, HCCL_ERROR("failed to fill opParam.algName"), HCCL_E_INTERNAL);
    HCCL_INFO(
        "[GetOpParam] prepared opParam, opType[%u], algName[%s], algTag[%s].", static_cast<u32>(opParam.opType),
        opParam.algName, opParam.algTag);
    return HCCL_SUCCESS;
}

HcclResult HandleSingleRankAndCommMode(HcclComm comm, OpParam& opParam, bool& skipGetRes)
{
    uint32_t userRankSize = 0;
    CHK_RET(HcclGetRankSize(comm, &userRankSize));
    if (userRankSize == 1) {
        HCCL_WARNING("[%s] ranksize == 1, enter SingleRankProc", __func__);
        CHK_RET(SingleRankProc(opParam));
        skipGetRes = true;
        return HCCL_SUCCESS;
    }

    bool isOpBase = true;
    const char* opModeStr = isOpBase ? "_opbase" : "_offload";
    auto ret = sprintf_s(opParam.commModeTag, sizeof(opParam.commModeTag), "%s_%s", opParam.commName, opModeStr);
    if (ret <= 0) {
        HCCL_ERROR("[%s] failed to fill opParam.commModeTag", __func__);
        return HCCL_E_INTERNAL;
    }
    skipGetRes = false;
    return HCCL_SUCCESS;
}

static HcclResult AcquireAlgResources(
    HcclComm comm, OpParam& opParam, std::unique_ptr<InsCollAlgBase>& executor, TopoInfoWithNetLayerDetails* topoInfo,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost, OpResCtx& opResCtx, void** resCtxOut)
{
    HCCL_INFO("[AcquireAlgResources]start AcquireAlgResources!");
    bool isResourceReused = false;
    ThreadHandle cpuTsThread{0};
    ThreadHandle exportedAicpuTsThread{0};
    if ((opParam.engine == COMM_ENGINE_AICPU_TS) || (opParam.engine == COMM_ENGINE_CPU)) {
        CHK_RET(HcclThreadAcquireWithStream(comm, COMM_ENGINE_CPU_TS, opParam.stream, 1, &cpuTsThread));
        CHK_RET(HcclThreadExportToCommEngine(comm, 1, &cpuTsThread, COMM_ENGINE_AICPU_TS, &exportedAicpuTsThread));
    }
    if (opParam.engine == COMM_ENGINE_CCU) {
        // 填充 kfcServerArgs 的所有6个字段（最后一个为 token 占位符，后续更新）
        resCtxHost->kfcServerArgs = {
            opResCtx.xnAddr,          opResCtx.ckeAddr,
            static_cast<uint64_t>(1), // dieNum
            static_cast<uint64_t>(1), // missionNum
            static_cast<uint64_t>(0), // missionIndex
            static_cast<uint64_t>(0), // token 占位符，后续在 GetAlgResCcu 中更新
        };
        resCtxHost->kfcServerArgSize = 6;
        HCCL_INFO(
            "[AcquireAlgResources] kfcServerArgs generated: "
            "argSize[%u], xnAddr[0x%llx], ckeAddr[0x%llx], dieNum[%llu], "
            "missionNum[%llu], missionIndex[%llu], token[%llu]",
            resCtxHost->kfcServerArgSize, static_cast<unsigned long long>(resCtxHost->kfcServerArgs[0]),
            static_cast<unsigned long long>(resCtxHost->kfcServerArgs[1]),
            static_cast<unsigned long long>(resCtxHost->kfcServerArgs[2]),
            static_cast<unsigned long long>(resCtxHost->kfcServerArgs[3]),
            static_cast<unsigned long long>(resCtxHost->kfcServerArgs[4]),
            static_cast<unsigned long long>(resCtxHost->kfcServerArgs[5]));

        CHK_RET(HcclGetAlgRes(comm, opParam, executor, topoInfo, resCtxHost, resCtxOut, isResourceReused));
        opParam.resCtx = *resCtxOut;
    } else {
        CHK_RET(HcclGetAlgRes(comm, opParam, executor, topoInfo, resCtxHost, resCtxOut, isResourceReused));
        opParam.resCtx = *resCtxOut;
    }
    HCCL_INFO("[AcquireAlgResources]end AcquireAlgResources!");
    return HCCL_SUCCESS;
}

HcclResult GetOpParamResCtx(
    HcclComm comm, const std::string& algName, OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo,
    void** resCtxOut)
{
    bool useCannResCtx = UseCannBridge(opParam);
    std::unique_ptr<InsCollAlgBase> executor = nullptr;
    if (useCannResCtx) {
        executor = GetAlgExecViaCann(opParam.opType, algName);
    } else {
        executor = CollAlgExecRegistryV2::Instance().GetAlgExec(opParam.opType, algName);
    }
    CHK_PRT_RET(
        executor.get() == nullptr, HCCL_ERROR("Fail to find executor for algName[%s]", algName.c_str()), HCCL_E_PARA);
    std::unique_ptr<AlgResourceCtxSerializable> resCtxHost = std::make_unique<AlgResourceCtxSerializable>();
    resCtxHost->isHcommBatchTransferOnThreadSupported = HcommIsSupportHcommBatchTransferOnThread();
    HCCL_DEBUG(
        "[MC2_BATCH_TRANSFER][HostSupport] opType[%u], algName[%s], supported[%d].",
        static_cast<uint32_t>(opParam.opType), algName.c_str(),
        static_cast<int>(resCtxHost->isHcommBatchTransferOnThreadSupported));
    bool isResourceReused = false;

    ThreadHandle cpuTsThread{0};
    ThreadHandle exportedAicpuTsThread{0};
    if ((opParam.engine == COMM_ENGINE_AICPU_TS) || (opParam.engine == COMM_ENGINE_CPU)) {
        CHK_RET(HcclThreadAcquireWithStream(comm, COMM_ENGINE_CPU_TS, opParam.stream, 1, &cpuTsThread));
        CHK_RET(HcclThreadExportToCommEngine(comm, 1, &cpuTsThread, COMM_ENGINE_AICPU_TS, &exportedAicpuTsThread));
    }
    CHK_RET(HcclGetAlgRes(comm, opParam, executor, topoInfo, resCtxHost, resCtxOut, isResourceReused));
    opParam.cacheValid = isResourceReused;
    opParam.resCtx = *resCtxOut;
    return HCCL_SUCCESS;
}

HcclResult GetCcuOpParamResCtx(
    HcclComm comm, const std::string& algName, OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo,
    OpResCtx& opResCtx, void** resCtxOut)
{
    HCCL_INFO("[GetCcuOpParamResCtx]start GetCcuOpParamResCtx!");
    HCCL_INFO(
        "[GetCcuOpParamResCtx]received: workspace[%p], size[%llu]", (void*)opResCtx.workSpace, opResCtx.workSpaceSize);
    std::unique_ptr<InsCollAlgBase> executor = CollAlgExecRegistryV2::Instance().GetAlgExec(opParam.opType, algName);
    HCCL_INFO("[GetCcuOpParamResCtx]Generate executor successfully!");
    CHK_PRT_RET(
        executor.get() == nullptr, HCCL_ERROR("Fail to find executor for algName[%s]", algName.c_str()), HCCL_E_PARA);
    std::unique_ptr<AlgResourceCtxSerializable> resCtxHost = std::make_unique<AlgResourceCtxSerializable>();
    return AcquireAlgResources(comm, opParam, executor, topoInfo, resCtxHost, opResCtx, resCtxOut);
}

HcclResult GetOpParam(
    HcclComm comm, void* stream, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& opParam)
{
    CHK_RET(InitOpParamByTiling(comm, stream, tag, ccTiling, opParam));

    // ALLTOALL场景下sendCounts需指向host侧真实数组，且必须在整个GetOpParam调用链
    // (含SelectAlgAndPrepareEngine、GetOpParamResCtx中的GetAlgExecViaCann)期间保持存活。
    // 该数组持有在本函数栈帧，覆盖opParam的全部使用范围。
    constexpr uint64_t ALLTOALL_DEFAULT_SEND_COUNTS = 200ULL * 1024 * 1024;
    std::vector<uint64_t> sendCounts;
    void* origSendCounts = opParam.all2AllVDataDes.sendCounts;
    if (opParam.opType == HcclCMDType::HCCL_CMD_ALLTOALL) {
        uint32_t userRankSize = 0;
        CHK_RET(HcclGetRankSize(comm, &userRankSize));
        sendCounts.assign(userRankSize, ALLTOALL_DEFAULT_SEND_COUNTS);
        opParam.all2AllVDataDes.sendCounts = reinterpret_cast<void*>(sendCounts.data());
    }

    std::string algName;
    std::unique_ptr<TopoInfoWithNetLayerDetails> topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();
    bool forcedAlgAccepted = false;
    OpParam opParamBeforeAlg = opParam;
    CHK_RET(TryForcedAlgAndPrepareEngine(comm, ccTiling, opParam, algName, topoInfo, forcedAlgAccepted));
    if (!forcedAlgAccepted) {
        CHK_RET(SelectAlgAndPrepareEngine(comm, opParam, algName, topoInfo));
    }
    CHK_RET(FillOpParamAlgName(opParam, algName));

    bool skipGetRes = false;
    CHK_RET(HandleSingleRankAndCommMode(comm, opParam, skipGetRes));
    if (skipGetRes) {
        opParam.all2AllVDataDes.sendCounts = origSendCounts;
        return HCCL_SUCCESS;
    }

    void* resCtxSequence = nullptr;
    HcclResult resRet = GetOpParamResCtx(comm, algName, opParam, topoInfo.get(), &resCtxSequence);
    if (resRet != HCCL_SUCCESS && forcedAlgAccepted && resCtxSequence == nullptr) {
        HCCL_WARNING(
            "[MC2_FORCE_ALG] fallback to default selector after resource failure, algName[%s], ret[%d].",
            algName.c_str(), static_cast<int>(resRet));
        opParam = opParamBeforeAlg;
        algName.clear();
        topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();
        CHK_RET(SelectAlgAndPrepareEngine(comm, opParam, algName, topoInfo));
        CHK_RET(FillOpParamAlgName(opParam, algName));
        CHK_RET(HandleSingleRankAndCommMode(comm, opParam, skipGetRes));
        if (skipGetRes) {
            opParam.all2AllVDataDes.sendCounts = origSendCounts;
            return HCCL_SUCCESS;
        }
        CHK_RET(GetOpParamResCtx(comm, algName, opParam, topoInfo.get(), &resCtxSequence));
    } else {
        CHK_RET(resRet);
    }
    // GetOpParamResCtx执行结束，sendCounts的临时host数组已不再需要，
    // 将指向恢复为原值(大概率为nullptr)，避免遗留指向本函数栈内vector的悬空指针。
    opParam.all2AllVDataDes.sendCounts = origSendCounts;
    return HCCL_SUCCESS;
}

HcclResult CcuSelectAlgCheck(const Mc2CcTilingInner* ccTiling, uint32_t tilingIndex)
{
    HCCL_DEBUG("[CcuSelectAlgCheck] Default function.");
    return HCCL_SUCCESS;
}

namespace {
void RunCcuSelectAlgCheck(const Mc2CcTilingInner* ccTiling, uint32_t tilingIndex)
{
    if (CcuSelectAlgCheck(ccTiling, tilingIndex) != HCCL_SUCCESS) {
        HCCL_WARNING(
            "[CcuSelectAlg] CcuSelectAlgCheck failed for ccTiling[%u], opType[%u]. "
            "CCU execution condition not satisfied.",
            tilingIndex, ccTiling->opType);
    }
    HCCL_INFO("[CcuSelectAlg]CcuSelectAlgCheck[%u] successfully!", tilingIndex);
}

HcclResult SelectCcuAlgorithm(
    HcclComm comm, void* stream, const std::string& topoTag, const Mc2CcTilingInner* ccTiling, uint32_t tilingIndex,
    OpParam& opParam, std::string& algName, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo,
    AlgorithmType& algorithmType)
{
    CHK_RET(InitOpParamByTiling(comm, stream, topoTag, ccTiling, opParam));
    HCCL_INFO("[CcuSelectAlg]InitOpParamByTiling[%u] successfully!", tilingIndex);

    CHK_RET(SelectAlgAndPrepareEngine(comm, opParam, algName, topoInfo));
    HCCL_INFO(
        "[CcuSelectAlg]SelectAlgAndPrepareEngine[%u] successfully, algName = [%s]!", tilingIndex, algName.c_str());

    auto it = algorithmMap.find(algName);
    if (it == algorithmMap.end()) {
        HCCL_ERROR(
            "[CcuSelectAlg] ccTiling[%u] selected nonCCU algorithm[%s] for opType[%u].", tilingIndex, algName.c_str(),
            ccTiling->opType);
        return HCCL_E_NOT_SUPPORT;
    }
    algorithmType = it->second;

    int result = sprintf_s(opParam.algName, sizeof(opParam.algName), "%s", algName.c_str());
    CHK_PRT_RET(result <= 0, HCCL_ERROR("failed to fill opParam.algName"), HCCL_E_INTERNAL);
    HCCL_INFO(
        "[CcuSelectAlg] prepared opParam, opType[%u], algName[%s], algTag[%s].", static_cast<u32>(opParam.opType),
        opParam.algName, opParam.algTag);
    return HCCL_SUCCESS;
}

void FillCcuAlgorithmInfo(
    uint32_t tilingIndex, const Mc2CcTilingInner* ccTiling, const std::string& algName, AlgorithmType algorithmType,
    OpParam& opParam, OpResCtx& opResCtx)
{
    opResCtx.opType[tilingIndex] = ccTiling->opType;
    opResCtx.algorithmType[tilingIndex] = algorithmType;
    opResCtx.isKfc[tilingIndex] = true;
    opParam.isKfc = true;

    HCCL_INFO(
        "[CcuSelectAlg] ccTiling[%u]: opType[%u], algName[%s], algorithmType[%u]", tilingIndex,
        opResCtx.opType[tilingIndex], algName.c_str(), opResCtx.algorithmType[tilingIndex]);
}

HcclResult PrepareCcuAlgorithmResource(
    HcclComm comm, uint32_t tilingIndex, const std::string& algName, OpParam& opParam,
    TopoInfoWithNetLayerDetails* topoInfo, OpResCtx& opResCtx, bool& skipGetRes)
{
    CHK_RET(HandleSingleRankAndCommMode(comm, opParam, skipGetRes));
    if (skipGetRes) {
        return HCCL_SUCCESS;
    }
    HCCL_INFO("[CcuSelectAlg]HandleSingleRankAndCommMode[%u] successfully!", tilingIndex);

    void* resCtxSequence = nullptr;
    CHK_RET(GetCcuOpParamResCtx(comm, algName, opParam, topoInfo, opResCtx, &resCtxSequence));
    HCCL_INFO("[CcuSelectAlg]GetCcuOpParamResCtx[%u] successfully!", tilingIndex);
    return HCCL_SUCCESS;
}

HcclResult CopyCcuOpParamToDevice(
    HcclComm comm, uint32_t tilingIndex, const Mc2InitTilingInner* initTiling, const OpParam& opParam,
    OpResCtx& opResCtx)
{
    std::string tagParam = std::to_string(tilingIndex) + "_" + std::string(opParam.algTag);
    void* opParamPtr = nullptr;
    uint64_t opParamSize = sizeof(OpParam);
    if (HcclEngineCtxGet(comm, tagParam.c_str(), COMM_ENGINE_AIV, &opParamPtr, &opParamSize) == HCCL_SUCCESS) {
        HCCL_INFO(
            "HcclEngineCtxGet success, tagParam[%s], opParamAddr[%p], opParamSize[%llu]", tagParam.c_str(), opParamPtr,
            opParamSize);
    } else {
        CHK_RET(HcclEngineCtxCreate(comm, tagParam.c_str(), COMM_ENGINE_AIV, opParamSize, &opParamPtr));
    }
    aclError aclRet = aclrtMemcpy(opParamPtr, opParamSize, &opParam, opParamSize, ACL_MEMCPY_HOST_TO_DEVICE);
    CHK_RET(aclRet == ACL_ERROR_NONE ? HCCL_SUCCESS : HCCL_E_RUNTIME);
    opResCtx.algInfo[tilingIndex].opParam = reinterpret_cast<uint64_t>(opParamPtr);
    opResCtx.algInfo[tilingIndex].offset = initTiling->offset[tilingIndex];
    HCCL_INFO(
        "[CcuSelectAlg] ccTiling[%u]: opParamAddr[%llu], offset[%u]", tilingIndex,
        opResCtx.algInfo[tilingIndex].opParam, opResCtx.algInfo[tilingIndex].offset);
    return HCCL_SUCCESS;
}

HcclResult ProcessCcuTiling(
    HcclComm comm, void* stream, const std::string& topoTag, const Mc2CcTilingInner* ccTiling, uint32_t tilingIndex,
    const Mc2InitTilingInner* initTiling, OpResCtx& opResCtx)
{
    RunCcuSelectAlgCheck(ccTiling, tilingIndex);

    OpParam opParam{};
    std::string algName;
    auto topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();
    AlgorithmType algorithmType;
    CHK_RET(
        SelectCcuAlgorithm(comm, stream, topoTag, ccTiling, tilingIndex, opParam, algName, topoInfo, algorithmType));
    FillCcuAlgorithmInfo(tilingIndex, ccTiling, algName, algorithmType, opParam, opResCtx);

    bool skipGetRes = false;
    CHK_RET(PrepareCcuAlgorithmResource(comm, tilingIndex, algName, opParam, topoInfo.get(), opResCtx, skipGetRes));
    if (skipGetRes) {
        return HCCL_SUCCESS;
    }
    return CopyCcuOpParamToDevice(comm, tilingIndex, initTiling, opParam, opResCtx);
}
} // namespace

// CCU路径逐算子：算法选择 + 资源准备（参照GetOpParam形式）
HcclResult CcuSelectAlg(
    HcclComm comm, void* stream, const std::string topoTag[], const void* ccTilingList[], uint32_t tilingNum,
    void* mc2Tiling, OpResCtx& opResCtx)
{
    HCCL_INFO("[CcuSelectAlg]start CcuSelectAlg!");
    HCCL_INFO("[CcuSelectAlg]received: workspace[%p], size[%llu]", (void*)opResCtx.workSpace, opResCtx.workSpaceSize);
    Mc2InitTilingInner* initTiling = static_cast<Mc2InitTilingInner*>(mc2Tiling);
    for (uint32_t i = 0U; i < tilingNum; ++i) {
        const Mc2CcTilingInner* ccTiling = static_cast<const Mc2CcTilingInner*>(ccTilingList[i]);
        CHK_RET(ProcessCcuTiling(comm, stream, topoTag[i], ccTiling, i, initTiling, opResCtx));
    }

    return HCCL_SUCCESS;
}
