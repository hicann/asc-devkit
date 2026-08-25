/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "comm_kfc_open_kernel_adapter.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <string>
#include <vector>

#include "log.h"
#include "alg_param.h"

extern HcclResult LaunchOpenOpParamDataImpl(
    std::vector<uint8_t>& opParam, const mc2_ops_hccl::AlgResourceCtxSerializable& resCtx);

namespace {
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
} // namespace

HcclResult LoadOpenOpParamData(
    uint64_t opParamKey, uint64_t opParamSize, std::string& commName, std::vector<uint8_t>& opParam)
{
    if (opParamKey == 0U) {
        HCCL_ERROR("Invalid opParamKey %#llx.", opParamKey);
        return HCCL_E_PARA;
    }
    if (opParamSize < sizeof(mc2_ops_hccl::OpParam) || opParamSize > std::numeric_limits<size_t>::max()) {
        HCCL_ERROR("Invalid OpParam size %llu.", static_cast<unsigned long long>(opParamSize));
        return HCCL_E_PARA;
    }

    const auto* param = reinterpret_cast<const mc2_ops_hccl::OpParam*>(opParamKey);
    const size_t serializedSize = static_cast<size_t>(opParamSize);
    if (param->varMemSize != serializedSize - sizeof(mc2_ops_hccl::OpParam)) {
        HCCL_ERROR(
            "OpParam variable memory size %llu does not match serialized size %zu.",
            static_cast<unsigned long long>(param->varMemSize), serializedSize);
        return HCCL_E_PARA;
    }
    const auto* begin = reinterpret_cast<const uint8_t*>(param);
    opParam.assign(begin, begin + serializedSize);
    commName.assign(param->commName);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][LoadOpenOpParamData] file[%s:%d], key %#llx, sizeof(OpParam) %zu, "
        "varMemSizeOffset %zu, varMemSize %llu, opParamSize %zu.",
        __FILE__, __LINE__, static_cast<unsigned long long>(opParamKey), sizeof(mc2_ops_hccl::OpParam),
        offsetof(mc2_ops_hccl::OpParam, varMemSize), static_cast<unsigned long long>(param->varMemSize),
        serializedSize);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][Load] key %#llx, opParamSize %zu, commName[%s], opType %u, algName[%s], "
        "inputSize %llu, outputSize %llu, varMemSize %llu, resCtx %p, ctxSize %llu.",
        static_cast<unsigned long long>(opParamKey), serializedSize, commName.c_str(), static_cast<u32>(param->opType),
        param->algName, static_cast<unsigned long long>(param->inputSize),
        static_cast<unsigned long long>(param->outputSize), static_cast<unsigned long long>(param->varMemSize),
        param->resCtx, static_cast<unsigned long long>(param->ctxSize));
    if (param->opType == HCCL_CMD_ALLTOALLV || param->opType == HCCL_CMD_ALLTOALL) {
        HCCL_INFO(
            "[MC2_OPEN_DIAG][Load][AllToAllV] sendDataType %u, recvDataType %u, sendCounts %p, recvCounts %p, sdispls "
            "%p, rdispls %p.",
            static_cast<u32>(param->all2AllVDataDes.sendType), static_cast<u32>(param->all2AllVDataDes.recvType),
            param->all2AllVDataDes.sendCounts, param->all2AllVDataDes.recvCounts, param->all2AllVDataDes.sdispls,
            param->all2AllVDataDes.rdispls);
    } else {
        HCCL_INFO(
            "[MC2_OPEN_DIAG][Load][Else] count %llu, dataType %u, outputType %u, strideCount %llu.",
            static_cast<unsigned long long>(param->DataDes.count), static_cast<u32>(param->DataDes.dataType),
            static_cast<u32>(param->DataDes.outputType), static_cast<unsigned long long>(param->DataDes.strideCount));
    }
    return HCCL_SUCCESS;
}

HcclResult CreateOpParamByBaseOpParam(
    const std::vector<uint8_t>& baseOpParam, const HcclApi::HcclMsg& msg, HcclApi::HcclMsgExt& extMsg, uint32_t rankNum,
    void* stream, std::vector<uint8_t>& runOpParam)
{
    const HcclCMDType msgOpType = static_cast<HcclCMDType>(msg.commType.prepareType);
    const auto* baseParam = reinterpret_cast<const mc2_ops_hccl::OpParam*>(baseOpParam.data());
    const size_t opParamSize = sizeof(mc2_ops_hccl::OpParam) + baseParam->varMemSize;
    runOpParam.resize(opParamSize);
    auto* param = reinterpret_cast<mc2_ops_hccl::OpParam*>(runOpParam.data());
    CHK_SAFETY_FUNC_RET(memcpy_s(param, sizeof(mc2_ops_hccl::OpParam), baseParam, sizeof(mc2_ops_hccl::OpParam)));

    if (baseParam->varMemSize > 0) {
        CHK_SAFETY_FUNC_RET(memcpy_s(param->varData, baseParam->varMemSize, baseParam->varData, baseParam->varMemSize));
    }

    param->inputPtr = reinterpret_cast<void*>(msg.sendBuffer);
    param->outputPtr = reinterpret_cast<void*>(msg.recvBuffer);
    if (param->opType == HcclCMDType::HCCL_CMD_ALLTOALL) {
        uint64_t offset = (msg.strideCount > 0) ? msg.strideCount : msg.dataCnt;
        for (uint32_t i = 0U; i < rankNum; ++i) {
            extMsg.sendCounts[i] = extMsg.recvCounts[i] = msg.dataCnt;
            extMsg.sendOffset[i] = extMsg.recvOffset[i] = offset * i;
        }
        param->opType = static_cast<HcclCMDType>(HcclCMDType::HCCL_CMD_ALLTOALLV);
    }
    if (param->opType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        param->all2AllVDataDes.sendType = param->all2AllVDataDes.recvType =
            static_cast<HcclDataType>(msg.addMsg.v1Msg.hcclDataType);
        param->all2AllVDataDes.sendCounts = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(extMsg.sendCounts));
        param->all2AllVDataDes.recvCounts = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(extMsg.recvCounts));
        param->all2AllVDataDes.sdispls = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(extMsg.sendOffset));
        param->all2AllVDataDes.rdispls = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(extMsg.recvOffset));
    } else {
        param->DataDes.dataType = param->DataDes.outputType = static_cast<HcclDataType>(msg.addMsg.v1Msg.hcclDataType);
        param->DataDes.count = msg.dataCnt;
        u64 effectiveStrideCount = msg.strideCount;
        const u32 repeatCnt = (msg.addMsg.v1Msg.repeatCnt == 0U) ? 1U : static_cast<u32>(msg.addMsg.v1Msg.repeatCnt);
        if (effectiveStrideCount == 0U && repeatCnt > 1U &&
            (msgOpType == HCCL_CMD_ALLGATHER || msgOpType == HCCL_CMD_REDUCE_SCATTER)) {
            effectiveStrideCount = msg.dataCnt * static_cast<u64>(repeatCnt);
            HCCL_INFO(
                "[MC2_OPEN_DIAG][FormatStrideFallback] opType %u, dataCnt %llu, repeatCnt %u, "
                "effectiveStrideCount %llu.",
                static_cast<u32>(msgOpType), static_cast<unsigned long long>(msg.dataCnt), repeatCnt,
                static_cast<unsigned long long>(effectiveStrideCount));
        }
        param->DataDes.strideCount = effectiveStrideCount;
    }

    param->opType = static_cast<HcclCMDType>(msg.commType.prepareType);
    param->reduceType = static_cast<HcclReduceOp>(msg.opType);
    param->stream = stream;
    return HCCL_SUCCESS;
}

inline void LocalPrintElseOpParam(uint32_t rankNum, uint32_t repeatIdx, std::vector<uint8_t>& runOpParam)
{
    auto* param = reinterpret_cast<mc2_ops_hccl::OpParam*>(runOpParam.data());
    const u64 dataTypeSize = GetDataTypeSize(param->DataDes.dataType);
    const u64 inputBytes = param->DataDes.count * dataTypeSize;
    const bool isAllGather = (param->opType == HCCL_CMD_ALLGATHER);
    const u64 rankStrideCount = (param->DataDes.strideCount == 0U) ? param->DataDes.count : param->DataDes.strideCount;
    const u64 outputSpanCount = isAllGather ?
                                    (rankStrideCount * (rankNum == 0U ? 0U : rankNum - 1U) + param->DataDes.count) :
                                    param->DataDes.count;
    const u64 outputBytes = outputSpanCount * dataTypeSize;
    const u64 rankStrideBytes = isAllGather ? rankStrideCount * dataTypeSize : 0U;
    const u64 inputBegin = reinterpret_cast<u64>(param->inputPtr);
    const u64 outputBegin = reinterpret_cast<u64>(param->outputPtr);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][FormatRun] runOpType %u, runAlgName[%s], runInputPtr %p, "
        "runOutputPtr %p, runCount %llu, runDataType %u, outputType %u, runStrideCount %llu, stream %p.",
        static_cast<u32>(param->opType), param->algName, param->inputPtr, param->outputPtr,
        static_cast<unsigned long long>(param->DataDes.count), static_cast<u32>(param->DataDes.dataType),
        static_cast<u32>(param->DataDes.outputType), static_cast<unsigned long long>(param->DataDes.strideCount),
        param->stream);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][FormatRange] repeatIdx %u, rankNum %u, dataTypeSize %llu, inputBytes %llu, "
        "rankStrideCount %llu, rankStrideBytes %llu, outputSpanCount %llu, outputBytes %llu, "
        "inputRange [%#llx, %#llx), outputSpan [%#llx, %#llx), outputSpanRankScaled %u.",
        repeatIdx, rankNum, static_cast<unsigned long long>(dataTypeSize), static_cast<unsigned long long>(inputBytes),
        static_cast<unsigned long long>(rankStrideCount), static_cast<unsigned long long>(rankStrideBytes),
        static_cast<unsigned long long>(outputSpanCount), static_cast<unsigned long long>(outputBytes),
        static_cast<unsigned long long>(inputBegin), static_cast<unsigned long long>(inputBegin + inputBytes),
        static_cast<unsigned long long>(outputBegin), static_cast<unsigned long long>(outputBegin + outputBytes),
        isAllGather);
}

HcclResult FormatOpenOpParamDataFromMsg(
    const std::vector<uint8_t>& baseOpParam, const HcclApi::HcclMsg& msg, HcclApi::HcclMsgExt& extMsg, uint32_t rankNum,
    uint32_t repeatIdx, void* stream, std::vector<uint8_t>& runOpParam)
{
    CHK_PRT_RET(!HcclApi::IsValidHcclMsgRankSize(rankNum), HCCL_ERROR("Invalid rank size %u.", rankNum), HCCL_E_PARA);
    CHK_PRT_RET(
        baseOpParam.size() < sizeof(mc2_ops_hccl::OpParam),
        HCCL_ERROR(
            "Base op param size %zu is smaller than OpParam size %zu.", baseOpParam.size(),
            sizeof(mc2_ops_hccl::OpParam)),
        HCCL_E_PARA);
    const auto* baseParam = reinterpret_cast<const mc2_ops_hccl::OpParam*>(baseOpParam.data());
    CHK_PRT_RET(
        baseParam->varMemSize > baseOpParam.size() - sizeof(mc2_ops_hccl::OpParam),
        HCCL_ERROR(
            "Base OpParam variable memory size %llu exceeds buffer size %zu.",
            static_cast<unsigned long long>(baseParam->varMemSize), baseOpParam.size()),
        HCCL_E_PARA);
    auto* param = reinterpret_cast<mc2_ops_hccl::OpParam*>(runOpParam.data());
    if (repeatIdx == 0U) {
        CHK_RET(CreateOpParamByBaseOpParam(baseOpParam, msg, extMsg, rankNum, stream, runOpParam));
        param = reinterpret_cast<mc2_ops_hccl::OpParam*>(runOpParam.data());
    } else {
        if (runOpParam.empty()) {
            HCCL_ERROR("Run op param is empty at repeat %u.", repeatIdx);
            return HCCL_E_PARA;
        }
        if (param->opType == HCCL_CMD_ALLTOALLV || param->opType == HCCL_CMD_ALLTOALL) {
            for (u32 i = 0U; i < rankNum; ++i) {
                extMsg.sendOffset[i] += extMsg.sendCounts[i];
                extMsg.recvOffset[i] += extMsg.recvCounts[i];
            }
        } else {
            const u64 dataLen = msg.dataCnt * GetDataTypeSize(static_cast<HcclDataType>(msg.addMsg.v1Msg.hcclDataType));
            param->inputPtr = reinterpret_cast<void*>(reinterpret_cast<int8_t*>(param->inputPtr) + dataLen);
            param->outputPtr = reinterpret_cast<void*>(reinterpret_cast<int8_t*>(param->outputPtr) + dataLen);
        }
    }
    const HcclCMDType opType = static_cast<HcclCMDType>(msg.commType.prepareType);

    HCCL_INFO(
        "[MC2_OPEN_DIAG][FormatMsg] repeatIdx %u, rankNum %u, msgOpType %u, msgReduceType %u, "
        "msgSendBuffer %#llx, msgRecvBuffer %#llx, msgDataCnt %llu, msgDataType %u, msgRepeatCnt %u, "
        "msgStrideCount %llu, ccOpTilingData %#llx.",
        repeatIdx, rankNum, static_cast<u32>(opType), static_cast<u32>(msg.opType),
        static_cast<unsigned long long>(msg.sendBuffer), static_cast<unsigned long long>(msg.recvBuffer),
        static_cast<unsigned long long>(msg.dataCnt), static_cast<u32>(msg.addMsg.v1Msg.hcclDataType),
        static_cast<u32>(msg.addMsg.v1Msg.repeatCnt), static_cast<unsigned long long>(msg.strideCount),
        static_cast<unsigned long long>(msg.addMsg.v1Msg.ccOpTilingData));
    HCCL_INFO(
        "[MC2_OPEN_DIAG][FormatBase] baseOpType %u, baseAlgName[%s], baseInputSize %llu, "
        "baseOutputSize %llu, baseCount %llu, baseDataType %u, baseOutputType %u, baseStrideCount %llu, "
        "baseInputPtr %p, baseOutputPtr %p.",
        static_cast<u32>(baseParam->opType), baseParam->algName, static_cast<unsigned long long>(baseParam->inputSize),
        static_cast<unsigned long long>(baseParam->outputSize),
        static_cast<unsigned long long>(baseParam->DataDes.count), static_cast<u32>(baseParam->DataDes.dataType),
        static_cast<u32>(baseParam->DataDes.outputType),
        static_cast<unsigned long long>(baseParam->DataDes.strideCount), baseParam->inputPtr, baseParam->outputPtr);

    if ((param->opType == HCCL_CMD_ALLTOALLV) || param->opType == HCCL_CMD_ALLTOALL) {
        HCCL_INFO(
            "[MC2_OPEN_DIAG][FormatBase] sendDataType %u, recvDataType %u, sendCounts %p, recvCounts %p, sdispls %p, "
            "rdispls %p, "
            "extMsg.sendCounts %p, extMsg.recvCounts %p, extMsg.sendOffset %p, extMsg.recvOffset %p.",
            static_cast<u32>(param->all2AllVDataDes.sendType), static_cast<u32>(param->all2AllVDataDes.recvType),
            param->all2AllVDataDes.sendCounts, param->all2AllVDataDes.recvCounts, param->all2AllVDataDes.sdispls,
            param->all2AllVDataDes.rdispls, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(extMsg.sendCounts)),
            reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(extMsg.recvCounts)),
            reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(extMsg.sendOffset)),
            reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(extMsg.recvOffset)));
        for (uint32_t i = 0U; i < rankNum; ++i) {
            HCCL_INFO(
                "[MC2_OPEN_DIAG][FormatBase][ALLTOALLV] extMsg.sendCounts[%d] %llu, "
                "extMsg.recvCounts[%d] %llu, extMsg.sendOffset[%d] %llu, extMsg.recvOffset[%d] %llu, "
                "sendCounts[%d] %llu, recvCounts[%d] %llu, sdispls[%d] %llu, rdispls[%d] %llu.",
                i, extMsg.sendCounts[i], i, extMsg.recvCounts[i], i, extMsg.sendOffset[i], i, extMsg.recvOffset[i], i,
                reinterpret_cast<u64*>(param->all2AllVDataDes.sendCounts)[i], i,
                reinterpret_cast<u64*>(param->all2AllVDataDes.recvCounts)[i], i,
                reinterpret_cast<u64*>(param->all2AllVDataDes.sdispls)[i], i,
                reinterpret_cast<u64*>(param->all2AllVDataDes.rdispls)[i]);
        }
    } else {
        LocalPrintElseOpParam(rankNum, repeatIdx, runOpParam);
    }

    HCCL_INFO(
        "Formatted open op param: repeat index %u, op type %u, input addr %#llx, output addr %#llx.", repeatIdx,
        static_cast<u32>(opType), param->inputPtr, param->outputPtr);

    return HCCL_SUCCESS;
}

HcclResult LaunchOpenOpParamData(std::vector<uint8_t>& opParam, const mc2_ops_hccl::AlgResourceCtxSerializable& resCtx)
{
    if (opParam.empty()) {
        HCCL_ERROR("Op param is empty.");
        return HCCL_E_PARA;
    }
    return LaunchOpenOpParamDataImpl(opParam, resCtx);
}
