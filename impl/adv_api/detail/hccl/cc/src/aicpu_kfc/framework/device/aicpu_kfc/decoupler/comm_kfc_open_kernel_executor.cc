/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "log.h"
#include "alg_param.h"
#include "alg_data_trans_wrapper.h"
#include "coll_alg_v2_exec_registry.h"
#include "cann_dispatch_bridge.h"
#include "hcomm_primitives.h"
#include "task_cache/mc2_aicpu_task_cache.h"
#include "task_cache/mc2_aicpu_task_cache_runner.h"

namespace {
mc2_ops_hccl::OpParam* AsOpenOpParam(std::vector<uint8_t>& opParam)
{
    return reinterpret_cast<mc2_ops_hccl::OpParam*>(opParam.data());
}

u64 GetDataTypeSize(HcclDataType dataType)
{
    switch (dataType) {
        case HCCL_DATA_TYPE_INT8:
        case HCCL_DATA_TYPE_UINT8:
        case HCCL_DATA_TYPE_HIF8:
        case HCCL_DATA_TYPE_FP8E5M2:
        case HCCL_DATA_TYPE_FP8E4M3:
        case HCCL_DATA_TYPE_FP8E8M0:
            return 1U;
        case HCCL_DATA_TYPE_INT16:
        case HCCL_DATA_TYPE_UINT16:
        case HCCL_DATA_TYPE_FP16:
        case HCCL_DATA_TYPE_BFP16:
            return 2U;
        case HCCL_DATA_TYPE_INT32:
        case HCCL_DATA_TYPE_UINT32:
        case HCCL_DATA_TYPE_FP32:
            return 4U;
        case HCCL_DATA_TYPE_INT64:
        case HCCL_DATA_TYPE_UINT64:
        case HCCL_DATA_TYPE_FP64:
            return 8U;
        case HCCL_DATA_TYPE_INT128:
            return 16U;
        default:
            return 0U;
    }
}

HcclResult RestoreVarDataAlltoAllV(mc2_ops_hccl::OpParam& param, const mc2_ops_hccl::AlgResourceCtxSerializable& resCtx)
{
    u64 rankSize = resCtx.topoInfo.userRankSize;
    CHK_PRT_RET(
        param.varMemSize != mc2_ops_hccl::ALL_TO_ALL_V_VECTOR_NUM * rankSize * sizeof(u64),
        HCCL_ERROR(
            "[RestoreVarDataAlltoAllV] param.varMemSize [%llu] is invalid,"
            " ALL_TO_ALL_V_VECTOR_NUM is [%u], rankSize is [%u], sizeof(u64) is [%u],",
            param.varMemSize, mc2_ops_hccl::ALL_TO_ALL_V_VECTOR_NUM, rankSize, sizeof(u64)),
        HCCL_E_PARA);

    HCCL_INFO(
        "[RestoreVarDataAlltoAllV] param.varMemSize [%llu],"
        " ALL_TO_ALL_V_VECTOR_NUM is [%u], rankSize is [%u], sizeof(u64) is [%u],",
        param.varMemSize, mc2_ops_hccl::ALL_TO_ALL_V_VECTOR_NUM, rankSize, sizeof(u64));
    for (uint32_t i = 0; i < rankSize; i++) {
        HCCL_INFO(
            "param.all2AllVDataDes.sendCounts[%d]:[%llu], param.all2AllVDataDes.recvCounts[%d]:[%llu], "
            "param.all2AllVDataDes.sdispls[%d]:[%llu], param.all2AllVDataDes.rdispls[%d]:[%llu]",
            i, reinterpret_cast<u64*>(param.all2AllVDataDes.sendCounts)[i], i,
            reinterpret_cast<u64*>(param.all2AllVDataDes.recvCounts)[i], i,
            reinterpret_cast<u64*>(param.all2AllVDataDes.sdispls)[i], i,
            reinterpret_cast<u64*>(param.all2AllVDataDes.rdispls)[i]);
    }
    constexpr u64 SEND_COUNT_IDX = 0;
    constexpr u64 RECV_COUNT_IDX = 1;
    constexpr u64 SEND_DISPL_IDX = 2;
    constexpr u64 RECV_DISPL_IDX = 3;

    u64* data = reinterpret_cast<u64*>(param.varData);
    for (u64 i = 0; i < mc2_ops_hccl::ALL_TO_ALL_V_VECTOR_NUM * rankSize; i++) {
        u64 val = i / rankSize;
        switch (val) {
            case SEND_COUNT_IDX:
                data[i] = reinterpret_cast<u64*>(param.all2AllVDataDes.sendCounts)[i % rankSize];
                break;
            case RECV_COUNT_IDX:
                data[i] = reinterpret_cast<u64*>(param.all2AllVDataDes.recvCounts)[i % rankSize];
                break;
            case SEND_DISPL_IDX:
                data[i] = reinterpret_cast<u64*>(param.all2AllVDataDes.sdispls)[i % rankSize];
                break;
            case RECV_DISPL_IDX:
                data[i] = reinterpret_cast<u64*>(param.all2AllVDataDes.rdispls)[i % rankSize];
                break;
            default:
                break;
        }
    }

    return HCCL_SUCCESS;
}

HcclResult RestoreVarDataIfNeeded(mc2_ops_hccl::OpParam& param, const mc2_ops_hccl::AlgResourceCtxSerializable& resCtx)
{
    HcclResult ret = HCCL_SUCCESS;
    if (param.opType == HCCL_CMD_ALLTOALLV || param.opType == HCCL_CMD_ALLTOALL) {
        ret = RestoreVarDataAlltoAllV(param, resCtx);
    }
    return ret;
}

} // namespace

HcclResult LaunchOpenOpParamDataImpl(
    std::vector<uint8_t>& opParam, const mc2_ops_hccl::AlgResourceCtxSerializable& resCtx)
{
    auto* param = AsOpenOpParam(opParam);
    CHK_PTR_NULL(param);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][LaunchOpenOpParamDataImpl] file[%s:%d], opParamSize %zu, "
        "sizeof(OpParam) %zu, varMemSizeOffset %zu, varMemSize %llu, opType %u, algName[%s].",
        __FILE__, __LINE__, opParam.size(), sizeof(mc2_ops_hccl::OpParam), offsetof(mc2_ops_hccl::OpParam, varMemSize),
        static_cast<unsigned long long>(param->varMemSize), static_cast<u32>(param->opType), param->algName);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][Launch] opType %u, algName[%s], inputPtr %p, outputPtr %p, resCtx %p, ctxSize %llu, stream %p."
        "userRank %u, userRankSize %u, threadNum %zu, cclMemAddr %p, cclMemSize %llu.",
        static_cast<u32>(param->opType), param->algName, param->inputPtr, param->outputPtr, param->resCtx,
        static_cast<unsigned long long>(param->ctxSize), param->stream, resCtx.topoInfo.userRank,
        resCtx.topoInfo.userRankSize, resCtx.threads.size(), resCtx.cclMem.addr,
        static_cast<unsigned long long>(resCtx.cclMem.size));
    if (param->opType == HCCL_CMD_ALLTOALLV || param->opType == HCCL_CMD_ALLTOALL) {
        HCCL_INFO(
            "[MC2_OPEN_DIAG][Launch][AllToAllV] sendDataType %u, recvDataType %u, sendCounts %p, recvCounts %p, "
            "sdispls %p, rdispls %p.",
            static_cast<u32>(param->all2AllVDataDes.sendType), static_cast<u32>(param->all2AllVDataDes.recvType),
            param->all2AllVDataDes.sendCounts, param->all2AllVDataDes.recvCounts, param->all2AllVDataDes.sdispls,
            param->all2AllVDataDes.rdispls);
    } else {
        HCCL_INFO(
            "[MC2_OPEN_DIAG][Launch][Else] count %llu, dataType %u, outputType %u, strideCount %llu.",
            static_cast<unsigned long long>(param->DataDes.count), static_cast<u32>(param->DataDes.dataType),
            static_cast<u32>(param->DataDes.outputType), static_cast<unsigned long long>(param->DataDes.strideCount));
    }
    const u64 dataTypeSize = GetDataTypeSize(param->DataDes.dataType);
    const u64 inputBytes = param->DataDes.count * dataTypeSize;
    const bool isAllGather = (param->opType == HCCL_CMD_ALLGATHER);
    const u64 rankStrideCount = (param->DataDes.strideCount == 0U) ? param->DataDes.count : param->DataDes.strideCount;
    const u64 outputSpanCount =
        isAllGather ? (rankStrideCount * (resCtx.topoInfo.userRankSize == 0U ? 0U : resCtx.topoInfo.userRankSize - 1U) +
                       param->DataDes.count) :
                      param->DataDes.count;
    const u64 outputBytes = outputSpanCount * dataTypeSize;
    const u64 rankStrideBytes = isAllGather ? rankStrideCount * dataTypeSize : 0U;
    const u64 inputBegin = reinterpret_cast<u64>(param->inputPtr);
    const u64 outputBegin = reinterpret_cast<u64>(param->outputPtr);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][LaunchRange] userRank %u, userRankSize %u, dataTypeSize %llu, inputBytes %llu, "
        "rankStrideCount %llu, rankStrideBytes %llu, outputSpanCount %llu, outputBytes %llu, "
        "inputRange [%#llx, %#llx), outputSpan [%#llx, %#llx), outputSpanRankScaled %u, engine %u.",
        resCtx.topoInfo.userRank, resCtx.topoInfo.userRankSize, static_cast<unsigned long long>(dataTypeSize),
        static_cast<unsigned long long>(inputBytes), static_cast<unsigned long long>(rankStrideCount),
        static_cast<unsigned long long>(rankStrideBytes), static_cast<unsigned long long>(outputSpanCount),
        static_cast<unsigned long long>(outputBytes), static_cast<unsigned long long>(inputBegin),
        static_cast<unsigned long long>(inputBegin + inputBytes), static_cast<unsigned long long>(outputBegin),
        static_cast<unsigned long long>(outputBegin + outputBytes), isAllGather, static_cast<u32>(param->engine));

    CHK_RET(RestoreVarDataIfNeeded(*param, resCtx));

    const bool useCannBridge =
        (param->opType == HCCL_CMD_ALLTOALL || param->opType == HCCL_CMD_ALLTOALLV ||
         param->opType == HCCL_CMD_ALLREDUCE);
    if (useCannBridge) {
        HcclResult cannRet = mc2_ops_hccl::RunWithMc2TaskCache(
            *param, resCtx, [&]() -> HcclResult { return mc2_ops_hccl::LaunchViaCann(*param, resCtx); });
        CHK_PRT_RET(
            cannRet != HCCL_SUCCESS,
            HCCL_ERROR(
                "[CannBridge] dispatch failed (ret=%d), opType=%u algName[%s]", static_cast<int>(cannRet),
                static_cast<u32>(param->opType), param->algName),
            cannRet);
        return HCCL_SUCCESS;
    }

    auto executor =
        mc2_ops_hccl::CollAlgExecRegistryV2::Instance().GetAlgExec(param->opType, std::string(param->algName));
    CHK_SMART_PTR_NULL(executor);

    CHK_RET(mc2_ops_hccl::InitHcommBatchTransferOnThreadSupported(resCtx.isHcommBatchTransferOnThreadSupported));
    HcclResult ret = mc2_ops_hccl::RunWithMc2TaskCache(
        *param, resCtx, [&]() -> HcclResult { return executor->Orchestrate(*param, resCtx); });
    CHK_PRT_RET(ret != HCCL_SUCCESS, HCCL_ERROR("orchestrate failed for alg:%s", param->algName), ret);

    return HCCL_SUCCESS;
}
