/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "comm_kfc_aicpu_server.h"
#include <algorithm>
#include <numeric>
#include "adapter_rts_common.h"
#include "alg_param.h"
#include "log.h"
#include "common/aicpu_kfc_utils.h"
#ifndef MC2_SERVER_ONLY
#include "hccl_mc2_ex.h"
#include "framework/aicpu_communicator.h"
#include "framework/aicpu_hccl_process.h"
#include "sqe_build_a5.h"
#endif
#include "hccl_common.h"
#include "stream_lite.h"
#include "thread.h"

using namespace HcclApi;
namespace {
static constexpr u64 TIMEOUT_ERROR_THRESHOLD = 120UL;
static constexpr u32 AICPU_ORDER_DFX_INVALID_VALUE = 0xFFFFFFFFU;
static const std::vector<HcclCMDType> SUPPORT_OP_LIST{
    HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALLV, HCCL_CMD_ALLTOALL};

#define MC2_TIMEOUT_LOG(isHardTimeout, format, ...) \
    do {                                            \
        if (isHardTimeout) {                        \
            HCCL_ERROR(format, ##__VA_ARGS__);      \
        } else {                                    \
            HCCL_RUN_INFO(format, ##__VA_ARGS__);   \
        }                                           \
    } while (0)

#ifndef MC2_SERVER_ONLY
hccl::HcclCommAicpu* FindCommAicpuByName(const std::string& commName)
{
    std::vector<std::pair<std::string, hccl::HcclCommAicpu*>> aicpuCommInfo;
    if (AicpuHcclProcess::AicpuGetCommAll(aicpuCommInfo) != HCCL_SUCCESS) {
        return nullptr;
    }
    for (auto& commInfo : aicpuCommInfo) {
        if (commInfo.second != nullptr && commInfo.first == commName) {
            return commInfo.second;
        }
    }
    return nullptr;
}
#endif

HcclResult IsDevice950(bool& isDevice950)
{
    DevType deviceType = DevType::DEV_TYPE_COUNT;
    CHK_RET(hrtGetDeviceType(deviceType));
    isDevice950 = (deviceType == DevType::DEV_TYPE_950);
    return HCCL_SUCCESS;
}

HcclResult GetNextThreadAndStream(const ServerExecCtx& execCtx, hccl::Thread*& thread, Hccl::StreamLite*& streamLite)
{
    CHK_PRT_RET(
        execCtx.mainThread == 0U,
        HCCL_ERROR(
            "Invalid main thread for commName[%s], opParamKey %#llx.", execCtx.commName.c_str(), execCtx.opParamKey),
        HCCL_E_PARA);
    thread = reinterpret_cast<hccl::Thread*>(execCtx.mainThread);
    CHK_PTR_NULL(thread);
    streamLite = static_cast<Hccl::StreamLite*>(thread->GetStreamLitePtr());
    CHK_PTR_NULL(streamLite);
    CHK_PTR_NULL(streamLite->GetRtsq());
    return HCCL_SUCCESS;
}

void LogOpenOpParamBrief(const char* stage, const std::vector<uint8_t>& opParam)
{
    if (opParam.size() < sizeof(mc2_ops_hccl::OpParam)) {
        HCCL_INFO(
            "[MC2_OPEN_DIAG][%s] opParamSize %zu is smaller than OpParam size %zu.", stage, opParam.size(),
            sizeof(mc2_ops_hccl::OpParam));
        return;
    }

    const auto* param = reinterpret_cast<const mc2_ops_hccl::OpParam*>(opParam.data());
    HCCL_INFO(
        "[MC2_OPEN_DIAG][%s] opParamSize %zu, opType %u, algName[%s], inputPtr %p, outputPtr %p, "
        "resCtx %p, ctxSize %llu, stream %p.",
        stage, opParam.size(), static_cast<u32>(param->opType), param->algName, param->inputPtr, param->outputPtr,
        param->resCtx, static_cast<unsigned long long>(param->ctxSize), param->stream);
    if (param->opType == HCCL_CMD_ALLTOALLV || param->opType == HCCL_CMD_ALLTOALL) {
        HCCL_INFO(
            "[MC2_OPEN_DIAG][FormatRun][AllToAllV] sendDataType %u, recvDataType %u, sendCounts %p, recvCounts %p, "
            "sdispls %p, rdispls %p.",
            static_cast<u32>(param->all2AllVDataDes.sendType), static_cast<u32>(param->all2AllVDataDes.recvType),
            param->all2AllVDataDes.sendCounts, param->all2AllVDataDes.recvCounts, param->all2AllVDataDes.sdispls,
            param->all2AllVDataDes.rdispls);
    } else {
        HCCL_INFO(
            "[MC2_OPEN_DIAG][Else] count %llu, dataType %u, outputType %u, strideCount %llu.",
            static_cast<unsigned long long>(param->DataDes.count), static_cast<u32>(param->DataDes.dataType),
            static_cast<u32>(param->DataDes.outputType), static_cast<unsigned long long>(param->DataDes.strideCount));
    }
}

u32 ReadAicpuOrderDfxValue(u64 addr)
{
    if (addr == 0U) {
        return AICPU_ORDER_DFX_INVALID_VALUE;
    }
    return *reinterpret_cast<volatile const u32*>(addr);
}

u32 GetAicpuOrderDfxCommitCnt(const HcclMsgArea* msgArea, u32 pos)
{
    if (msgArea == nullptr || pos >= HCCL_MSG_CNT) {
        return AICPU_ORDER_DFX_INVALID_VALUE;
    }
    return static_cast<u32>(msgArea->commMsg.singleMsg.commitTurnCnt[pos].cnt);
}

u32 GetAicpuOrderDfxFinishedCnt(const HcclMsgArea* msgArea, u32 pos)
{
    if (msgArea == nullptr || pos >= HCCL_MSG_CNT) {
        return AICPU_ORDER_DFX_INVALID_VALUE;
    }
    return static_cast<u32>(msgArea->commMsg.singleMsg.finishedTurnCnt[pos].cnt);
}

void LogAicpuOrderDfxCounter(
    const char* stage, u32 groupIdx, const HcclMsgArea* msgArea, u32 msgPos, u32 repeatIdx, u32 turnIdx, u64 opParamKey,
    u64 waitAddr, u64 recordAddr, u64 turnNumsAddr, const ServerExecCtx* execCtx)
{
    if (LIKELY(!HcclCheckLogLevel(HCCL_LOG_INFO))) {
        return;
    }
    const u64 valueAddr = (turnNumsAddr == 0U) ? 0U : turnNumsAddr + static_cast<u64>(turnIdx) * sizeof(u32);
    const u32 resourceType =
        (execCtx == nullptr) ? AICPU_ORDER_DFX_INVALID_VALUE : static_cast<u32>(execCtx->resourceType);
    const u64 mainThread = (execCtx == nullptr) ? 0U : execCtx->mainThread;
    HCCL_INFO(
        "[AICPU_ORDER_DFX][Counter] stage[%s], group[%u], msgPos[%u], repeatIdx[%u], turnIdx[%u], "
        "opParamKey[%#llx], resourceType[%u], mainThread[%#llx], waitAddr[%#llx], waitVal[%u], "
        "commitTurnCnt[%u], recordAddr[%#llx], recordVal[%u], finishedTurnCnt[%u], valueAddr[%#llx], "
        "valueVal[%u], turnNumsAddr[%#llx].",
        stage, groupIdx, msgPos, repeatIdx, turnIdx, static_cast<unsigned long long>(opParamKey), resourceType,
        static_cast<unsigned long long>(mainThread), static_cast<unsigned long long>(waitAddr),
        ReadAicpuOrderDfxValue(waitAddr), GetAicpuOrderDfxCommitCnt(msgArea, msgPos),
        static_cast<unsigned long long>(recordAddr), ReadAicpuOrderDfxValue(recordAddr),
        GetAicpuOrderDfxFinishedCnt(msgArea, msgPos), static_cast<unsigned long long>(valueAddr),
        ReadAicpuOrderDfxValue(valueAddr), static_cast<unsigned long long>(turnNumsAddr));
}

void LogAicpuOrderDfxCounterWindow(const char* stage, u32 groupIdx, const HcclMsgArea* msgArea)
{
    if (LIKELY(!HcclCheckLogLevel(HCCL_LOG_INFO))) {
        return;
    }
    HCCL_INFO(
        "[AICPU_ORDER_DFX][Counter] stage[%s], group[%u], msgArea[%p], "
        "commitTurnCnt[0..7]=[%u,%u,%u,%u,%u,%u,%u,%u], "
        "finishedTurnCnt[0..7]=[%u,%u,%u,%u,%u,%u,%u,%u].",
        stage, groupIdx, msgArea, GetAicpuOrderDfxCommitCnt(msgArea, 0U), GetAicpuOrderDfxCommitCnt(msgArea, 1U),
        GetAicpuOrderDfxCommitCnt(msgArea, 2U), GetAicpuOrderDfxCommitCnt(msgArea, 3U),
        GetAicpuOrderDfxCommitCnt(msgArea, 4U), GetAicpuOrderDfxCommitCnt(msgArea, 5U),
        GetAicpuOrderDfxCommitCnt(msgArea, 6U), GetAicpuOrderDfxCommitCnt(msgArea, 7U),
        GetAicpuOrderDfxFinishedCnt(msgArea, 0U), GetAicpuOrderDfxFinishedCnt(msgArea, 1U),
        GetAicpuOrderDfxFinishedCnt(msgArea, 2U), GetAicpuOrderDfxFinishedCnt(msgArea, 3U),
        GetAicpuOrderDfxFinishedCnt(msgArea, 4U), GetAicpuOrderDfxFinishedCnt(msgArea, 5U),
        GetAicpuOrderDfxFinishedCnt(msgArea, 6U), GetAicpuOrderDfxFinishedCnt(msgArea, 7U));
}
} // namespace

HcclResult CommKfcAicpuServer::AddOpContext(const HcclApi::OpResCtx* ctx)
{
    CHK_PTR_NULL(ctx);
    CHK_PRT_RET(
        msgArea_ != nullptr && reinterpret_cast<u64>(msgArea_) != ctx->workspace,
        HCCL_ERROR("Group %u: message area addr should be %#llx, not %#llx.", groupIdx_, msgArea_, ctx->workspace),
        HCCL_E_PARA);
    if (msgArea_ == nullptr) {
        msgArea_ = reinterpret_cast<HcclMsgArea*>(ctx->workspace);
        turnNumsAddr_ = reinterpret_cast<u64>(msgArea_ + 1);
        rankNum_ = static_cast<u32>(ctx->rankSize);
        std::iota(reinterpret_cast<u32*>(turnNumsAddr_), reinterpret_cast<u32*>(turnNumsAddr_) + UINT8_MAX + 1U, 0U);
        LogAicpuOrderDfxCounterWindow("AddOpContextInit", groupIdx_, msgArea_);
        KeepAlive();
    }

    for (u32 i = 0U; i < 8U; ++i) {
        const HcclApi::AlgInfo& algInfo = ctx->algInfo[i];
        const u64 opParamKey = algInfo.opParam;
        if (opParamKey == 0U) {
            continue;
        }
        if (MatchExecCtx(opParamKey) != nullptr) {
            HCCL_INFO("Group %u: opParamKey %#llx is already added.", groupIdx_, opParamKey);
            continue;
        }

        std::vector<uint8_t> baseOpParam{};
        std::string commName{};
        CHK_RET(LoadOpenOpParamData(opParamKey, commName, baseOpParam));
        CHK_PRT_RET(
            commName.empty(), HCCL_ERROR("Group %u: empty comm name for opParamKey %#llx.", groupIdx_, opParamKey),
            HCCL_E_PARA);
        CHK_PRT_RET(
            baseOpParam.size() < sizeof(mc2_ops_hccl::OpParam),
            HCCL_ERROR(
                "Group %u: base op param size %zu is smaller than OpParam size %zu for opParamKey %#llx.", groupIdx_,
                baseOpParam.size(), sizeof(mc2_ops_hccl::OpParam), opParamKey),
            HCCL_E_PARA);
        const auto* baseParam = reinterpret_cast<const mc2_ops_hccl::OpParam*>(baseOpParam.data());
        CHK_PTR_NULL(baseParam);

        mc2_open::OpenResCtxHolder resCtxHolder{};
        CHK_RET(mc2_open::AcquireOpenResCtx(opParamKey, *baseParam, resCtxHolder));
        CHK_PRT_RET(
            resCtxHolder == nullptr,
            HCCL_ERROR("Group %u: failed to acquire open resource ctx for opParamKey %#llx.", groupIdx_, opParamKey),
            HCCL_E_INTERNAL);

        ServerExecCtx execCtx{};
        execCtx.offset = algInfo.offset;
        execCtx.opParamKey = opParamKey;
        execCtx.commName = commName;
        execCtx.baseOpParam = std::move(baseOpParam);
        execCtx.resCtxHolder = std::move(resCtxHolder);

        bool isDevice950 = false;
        CHK_RET(IsDevice950(isDevice950));
        if (isDevice950) {
            CHK_PRT_RET(
                execCtx.resCtxHolder->threads.empty() || execCtx.resCtxHolder->threads[0] == 0U,
                HCCL_ERROR(
                    "Group %u: invalid open op resource ctx threads, thread num %zu, opParamKey %#llx.", groupIdx_,
                    execCtx.resCtxHolder->threads.size(), opParamKey),
                HCCL_E_PARA);
            execCtx.resourceType = ServerExecResourceType::NEXT_AICPU;
            execCtx.mainThread = execCtx.resCtxHolder->threads[0];
        } else {
#ifdef MC2_SERVER_ONLY
            HCCL_ERROR(
                "Group %u: mc2_server does not support legacy resource for commName[%s], opParamKey %#llx.", groupIdx_,
                commName.c_str(), opParamKey);
            return HCCL_E_PARA;
#else
            hccl::HcclCommAicpu* commAicpu = FindCommAicpuByName(commName);
            CHK_PRT_RET(
                commAicpu == nullptr,
                HCCL_ERROR(
                    "Group %u: failed to find commAicpu for commName[%s], opParamKey %#llx.", groupIdx_,
                    commName.c_str(), opParamKey),
                HCCL_E_PARA);
            execCtx.commAicpu = commAicpu;
            execCtx.dispatcher = commAicpu->GetDispatcher();
            execCtx.mainStream = &commAicpu->GetMainStream();
#endif
        }
        execCtxList_.push_back(execCtx);
        HCCL_INFO(
            "Group %u: add exec ctx for opParamKey %#llx, commName[%s], resource type %u, "
            "mainThread %#llx, message area address %#llx.",
            groupIdx_, opParamKey, commName.c_str(), static_cast<u32>(execCtx.resourceType), execCtx.mainThread,
            msgArea_);
        HCCL_INFO(
            "[AICPU_ORDER_DFX][HcommApi] AddExecCtx group[%u], opParamKey[%#llx], commName[%s], "
            "resourceType[%u], mainThread[%#llx], msgArea[%p], turnNumsAddr[%#llx].",
            groupIdx_, static_cast<unsigned long long>(opParamKey), commName.c_str(),
            static_cast<u32>(execCtx.resourceType), static_cast<unsigned long long>(execCtx.mainThread), msgArea_,
            static_cast<unsigned long long>(turnNumsAddr_));
    }

    CHK_PRT_RET(execCtxList_.empty(), HCCL_ERROR("Group %u: no exec ctx is added.", groupIdx_), HCCL_E_PARA);
    syncExecCtx_ = &execCtxList_.front();
    return HCCL_SUCCESS;
}

bool IsSupportedOp(HcclCMDType opType)
{
    return std::find(SUPPORT_OP_LIST.begin(), SUPPORT_OP_LIST.end(), opType) != SUPPORT_OP_LIST.end();
}

const ServerExecCtx* CommKfcAicpuServer::MatchExecCtx(u64 opParamKey) const
{
    for (const auto& execCtx : execCtxList_) {
        if (execCtx.opParamKey == opParamKey) {
            return &execCtx;
        }
    }
    return nullptr;
}

HcclResult CommKfcAicpuServer::LaunchOpenCcoreWait(
    const ServerExecCtx& execCtx, u64 waitAddr, u32 turnNum, u64 turnNumsAddr, bool isLast)
{
    const u64 valueAddr = turnNumsAddr + static_cast<u64>(turnNum) * sizeof(u32);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][CcoreWaitBegin] group %u, commName[%s], opParamKey %#llx, resourceType %u, "
        "turnNum %u, waitAddr %#llx, valueAddr %#llx, turnNumsAddr %#llx, isLast %u, mainThread %#llx, "
        "mainStream %p, dispatcher %p.",
        groupIdx_, execCtx.commName.c_str(), static_cast<unsigned long long>(execCtx.opParamKey),
        static_cast<u32>(execCtx.resourceType), turnNum, static_cast<unsigned long long>(waitAddr),
        static_cast<unsigned long long>(valueAddr), static_cast<unsigned long long>(turnNumsAddr),
        static_cast<u32>(isLast), static_cast<unsigned long long>(execCtx.mainThread), execCtx.mainStream,
        execCtx.dispatcher);
    if (execCtx.resourceType == ServerExecResourceType::NEXT_AICPU) {
        hccl::Thread* thread = nullptr;
        Hccl::StreamLite* streamLite = nullptr;
        CHK_RET(GetNextThreadAndStream(execCtx, thread, streamLite));
        HCCL_INFO(
            "[AICPU_ORDER_DFX][Ccore] WaitNext group[%u], commName[%s], opParamKey[%#llx], "
            "thread[%p], streamLite[%p], rtsq[%p], waitAddr[%#llx], waitVal[%u], valueAddr[%#llx], "
            "valueVal[%u], turnNum[%u], isLast[%u].",
            groupIdx_, execCtx.commName.c_str(), static_cast<unsigned long long>(execCtx.opParamKey), thread,
            streamLite, streamLite->GetRtsq(), static_cast<unsigned long long>(waitAddr),
            ReadAicpuOrderDfxValue(waitAddr), static_cast<unsigned long long>(valueAddr),
            ReadAicpuOrderDfxValue(valueAddr), turnNum, static_cast<u32>(isLast));
        HCCL_INFO(
            "[MC2_OPEN_DIAG][CcoreWaitNext] group %u, thread %p, streamLite %p, rtsq %p, "
            "waitAddr %#llx, valueAddr %#llx.",
            groupIdx_, thread, streamLite, streamLite->GetRtsq(), static_cast<unsigned long long>(waitAddr),
            static_cast<unsigned long long>(valueAddr));
        EXECEPTION_CATCH(streamLite->GetRtsq()->CCoreNotifyWait(waitAddr, valueAddr, isLast), return HCCL_E_INTERNAL);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][CcoreWaitSqeDone] group %u, turnNum %u, thread %p, streamLite %p.", groupIdx_, turnNum,
            thread, streamLite);
        EXECEPTION_CATCH(thread->LaunchTask(), return HCCL_E_INTERNAL);
        HCCL_INFO(
            "[AICPU_ORDER_DFX][HcommApi] LaunchTask after CCoreWait group[%u], thread[%p], streamLite[%p], "
            "rtsq[%p], waitAddr[%#llx], waitVal[%u], valueAddr[%#llx], valueVal[%u], turnNum[%u].",
            groupIdx_, thread, streamLite, streamLite->GetRtsq(), static_cast<unsigned long long>(waitAddr),
            ReadAicpuOrderDfxValue(waitAddr), static_cast<unsigned long long>(valueAddr),
            ReadAicpuOrderDfxValue(valueAddr), turnNum);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][CcoreWaitEnd] group %u, turnNum %u, thread %p, streamLite %p.", groupIdx_, turnNum, thread,
            streamLite);
        return HCCL_SUCCESS;
    }

#ifdef MC2_SERVER_ONLY
    HCCL_ERROR("Group %u: mc2_server does not support legacy CCore wait.", groupIdx_);
    return HCCL_E_INTERNAL;
#else
    CHK_PTR_NULL(execCtx.mainStream);
    uint8_t* sqeBuffer = nullptr;
    uint8_t* sqeTypeAddr = nullptr;
    uint8_t* sqeDfxInfoAddr = nullptr;
    uint16_t taskId16 = 0;

    CHK_RET(execCtx.mainStream->GetNextSqeBufferAddr(sqeBuffer, sqeTypeAddr, sqeDfxInfoAddr, taskId16));

    auto* sqeCtx = execCtx.mainStream->GetSqeContextPtr();
    CHK_PTR_NULL(sqeCtx);

    const u32 a5TaskId = (static_cast<u32>(sqeCtx->buffer.filpNum) << 16) | taskId16;
    HCCL_INFO(
        "[MC2_OPEN_DIAG][CcoreWaitLegacy] group %u, turnNum %u, taskId16 %u, a5TaskId %u, "
        "sqeBuffer %p, sqeTypeAddr %p, sqeDfxInfoAddr %p.",
        groupIdx_, turnNum, taskId16, a5TaskId, sqeBuffer, sqeTypeAddr, sqeDfxInfoAddr);

    Hccl::BuildA5SqeCCoreNotifyWait(0, a5TaskId, waitAddr, valueAddr, isLast, sqeBuffer);
    *sqeTypeAddr = static_cast<uint8_t>(SqeType::A5_CCORE_NOTIFY_WAIT_SQE);
    sqeCtx->buffer.addInfo[taskId16 % hccl::HCCL_SQE_MAX_CNT] =
        (static_cast<u32>(turnNum) << 16) | static_cast<u32>(isLast);
    HCCL_INFO(
        "[AICPU_ORDER_DFX][SQE] CCoreNotifyWait legacy group[%u], taskId16[%u], a5TaskId[%u], "
        "sqeType[%u], waitAddr[%#llx], waitVal[%u], valueAddr[%#llx], valueVal[%u], turnNum[%u], isLast[%u].",
        groupIdx_, taskId16, a5TaskId, static_cast<u32>(SqeType::A5_CCORE_NOTIFY_WAIT_SQE),
        static_cast<unsigned long long>(waitAddr), ReadAicpuOrderDfxValue(waitAddr),
        static_cast<unsigned long long>(valueAddr), ReadAicpuOrderDfxValue(valueAddr), turnNum,
        static_cast<u32>(isLast));

    HcclResult ret = LaunchTask(execCtx.dispatcher, *execCtx.mainStream);
    HCCL_INFO("[MC2_OPEN_DIAG][CcoreWaitEnd] group %u, turnNum %u, ret %u.", groupIdx_, turnNum, ret);
    return ret;
#endif
}

HcclResult CommKfcAicpuServer::LaunchOpenCcorePost(
    const ServerExecCtx& execCtx, u64 recordAddr, u32 turnNum, u64 turnNumsAddr)
{
    const u64 valueAddr = turnNumsAddr + static_cast<u64>(turnNum) * sizeof(u32);
    HCCL_INFO(
        "[MC2_OPEN_DIAG][CcorePostBegin] group %u, commName[%s], opParamKey %#llx, resourceType %u, "
        "turnNum %u, recordAddr %#llx, valueAddr %#llx, turnNumsAddr %#llx, mainThread %#llx, "
        "mainStream %p, dispatcher %p.",
        groupIdx_, execCtx.commName.c_str(), static_cast<unsigned long long>(execCtx.opParamKey),
        static_cast<u32>(execCtx.resourceType), turnNum, static_cast<unsigned long long>(recordAddr),
        static_cast<unsigned long long>(valueAddr), static_cast<unsigned long long>(turnNumsAddr),
        static_cast<unsigned long long>(execCtx.mainThread), execCtx.mainStream, execCtx.dispatcher);
    if (execCtx.resourceType == ServerExecResourceType::NEXT_AICPU) {
        hccl::Thread* thread = nullptr;
        Hccl::StreamLite* streamLite = nullptr;
        CHK_RET(GetNextThreadAndStream(execCtx, thread, streamLite));
        HCCL_INFO(
            "[AICPU_ORDER_DFX][Ccore] PostNext group[%u], commName[%s], opParamKey[%#llx], "
            "thread[%p], streamLite[%p], rtsq[%p], recordAddr[%#llx], recordVal[%u], valueAddr[%#llx], "
            "valueVal[%u], turnNum[%u].",
            groupIdx_, execCtx.commName.c_str(), static_cast<unsigned long long>(execCtx.opParamKey), thread,
            streamLite, streamLite->GetRtsq(), static_cast<unsigned long long>(recordAddr),
            ReadAicpuOrderDfxValue(recordAddr), static_cast<unsigned long long>(valueAddr),
            ReadAicpuOrderDfxValue(valueAddr), turnNum);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][CcorePostNext] group %u, thread %p, streamLite %p, rtsq %p, "
            "recordAddr %#llx, valueAddr %#llx.",
            groupIdx_, thread, streamLite, streamLite->GetRtsq(), static_cast<unsigned long long>(recordAddr),
            static_cast<unsigned long long>(valueAddr));
        EXECEPTION_CATCH(streamLite->GetRtsq()->CCoreNotifyRecord(recordAddr, valueAddr), return HCCL_E_INTERNAL);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][CcorePostSqeDone] group %u, turnNum %u, thread %p, streamLite %p.", groupIdx_, turnNum,
            thread, streamLite);
        EXECEPTION_CATCH(thread->LaunchTask(), return HCCL_E_INTERNAL);
        HCCL_INFO(
            "[AICPU_ORDER_DFX][HcommApi] LaunchTask after CCorePost group[%u], thread[%p], streamLite[%p], "
            "rtsq[%p], recordAddr[%#llx], recordVal[%u], valueAddr[%#llx], valueVal[%u], turnNum[%u].",
            groupIdx_, thread, streamLite, streamLite->GetRtsq(), static_cast<unsigned long long>(recordAddr),
            ReadAicpuOrderDfxValue(recordAddr), static_cast<unsigned long long>(valueAddr),
            ReadAicpuOrderDfxValue(valueAddr), turnNum);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][CcorePostEnd] group %u, turnNum %u, thread %p, streamLite %p.", groupIdx_, turnNum, thread,
            streamLite);
        return HCCL_SUCCESS;
    }

#ifdef MC2_SERVER_ONLY
    HCCL_ERROR("Group %u: mc2_server does not support legacy CCore post.", groupIdx_);
    return HCCL_E_INTERNAL;
#else
    CHK_PTR_NULL(execCtx.mainStream);
    uint8_t* sqeBuffer = nullptr;
    uint8_t* sqeTypeAddr = nullptr;
    uint8_t* sqeDfxInfoAddr = nullptr;
    uint16_t taskId16 = 0;

    CHK_RET(execCtx.mainStream->GetNextSqeBufferAddr(sqeBuffer, sqeTypeAddr, sqeDfxInfoAddr, taskId16));

    auto* sqeCtx = execCtx.mainStream->GetSqeContextPtr();
    CHK_PTR_NULL(sqeCtx);

    const u32 a5TaskId = (static_cast<u32>(sqeCtx->buffer.filpNum) << 16) | taskId16;
    HCCL_INFO(
        "[MC2_OPEN_DIAG][CcorePostLegacy] group %u, turnNum %u, taskId16 %u, a5TaskId %u, "
        "sqeBuffer %p, sqeTypeAddr %p, sqeDfxInfoAddr %p.",
        groupIdx_, turnNum, taskId16, a5TaskId, sqeBuffer, sqeTypeAddr, sqeDfxInfoAddr);

    Hccl::BuildA5SqeCCoreNotifyRecord(0, a5TaskId, recordAddr, valueAddr, sqeBuffer);
    *sqeTypeAddr = static_cast<uint8_t>(SqeType::A5_CCORE_NOTIFY_RECORD_SQE);
    sqeCtx->buffer.addInfo[taskId16 % hccl::HCCL_SQE_MAX_CNT] = turnNum;
    HCCL_INFO(
        "[AICPU_ORDER_DFX][SQE] CCoreNotifyRecord legacy group[%u], taskId16[%u], a5TaskId[%u], "
        "sqeType[%u], recordAddr[%#llx], recordVal[%u], valueAddr[%#llx], valueVal[%u], turnNum[%u].",
        groupIdx_, taskId16, a5TaskId, static_cast<u32>(SqeType::A5_CCORE_NOTIFY_RECORD_SQE),
        static_cast<unsigned long long>(recordAddr), ReadAicpuOrderDfxValue(recordAddr),
        static_cast<unsigned long long>(valueAddr), ReadAicpuOrderDfxValue(valueAddr), turnNum);

    HcclResult ret = LaunchTask(execCtx.dispatcher, *execCtx.mainStream);
    HCCL_INFO("[MC2_OPEN_DIAG][CcorePostEnd] group %u, turnNum %u, ret %u.", groupIdx_, turnNum, ret);
    return ret;
#endif
}

HcclResult CommKfcAicpuServer::FormatOpParamFromMsg(
    const HcclMsg& msg, HcclMsgExt& extMsg, const ServerExecCtx& execCtx, u32 repeatIdx, std::vector<uint8_t>& opParam)
{
    void* stream = nullptr;
    if (execCtx.resourceType == ServerExecResourceType::LEGACY) {
        CHK_PTR_NULL(execCtx.mainStream);
        stream = execCtx.mainStream->ptr();
    } else {
        CHK_PRT_RET(
            execCtx.baseOpParam.size() < sizeof(mc2_ops_hccl::OpParam),
            HCCL_ERROR(
                "Base op param size %zu is smaller than OpParam size %zu.", execCtx.baseOpParam.size(),
                sizeof(mc2_ops_hccl::OpParam)),
            HCCL_E_PARA);
        const auto* baseParam = reinterpret_cast<const mc2_ops_hccl::OpParam*>(execCtx.baseOpParam.data());
        CHK_PTR_NULL(baseParam);
        stream = baseParam->stream;
    }
    return FormatOpenOpParamDataFromMsg(execCtx.baseOpParam, msg, extMsg, rankNum_, repeatIdx, stream, opParam);
}

HcclResult CommKfcAicpuServer::LaunchOpenAicpuKernelServer(
    std::vector<uint8_t>& opParam, const mc2_ops_hccl::AlgResourceCtxSerializable& resCtx)
{
    LogOpenOpParamBrief("KernelServerBegin", opParam);
    HcclResult ret = LaunchOpenOpParamData(opParam, resCtx);
    HCCL_INFO("[MC2_OPEN_DIAG][KernelServerEnd] ret %u, opParamSize %zu.", ret, opParam.size());
    return ret;
}

HcclResult CommKfcAicpuServer::Orchestrate(const HcclMsg& msg, HcclMsgExt& extMsg, u32 msgPos)
{
    KeepAlive();
    CHK_PTR_NULL(msgArea_);
    const HcclCMDType opType = static_cast<HcclCMDType>(msg.commType.prepareType);
    CHK_PRT_RET(!IsSupportedOp(opType), HCCL_ERROR("Unsupported comm type %u.", static_cast<u32>(opType)), HCCL_E_PARA);

    const u64 opParamKey = msg.addMsg.v1Msg.ccOpTilingData;
    const ServerExecCtx* execCtx = MatchExecCtx(opParamKey);
    CHK_PRT_RET(
        execCtx == nullptr, HCCL_ERROR("Group %u: exec ctx %#llx is not added by host.", groupIdx_, opParamKey),
        HCCL_E_PARA);
    CHK_PRT_RET(
        execCtx->resCtxHolder == nullptr,
        HCCL_ERROR("Group %u: open resource ctx is not cached for opParamKey %#llx.", groupIdx_, opParamKey),
        HCCL_E_INTERNAL);

    const HcclHandle handle = msg.addMsg.v1Msg.selfHandleID;
    CHK_PRT_RET(handle < 0, HCCL_ERROR("Group %u: invalid handle id %d.", groupIdx_, handle), HCCL_E_INTERNAL);
    const u32 repeatCnt = static_cast<u32>(msg.addMsg.v1Msg.repeatCnt);
    const u64 waitAddr = reinterpret_cast<u64>(&(msgArea_->commMsg.singleMsg.commitTurnCnt[msgPos].cnt));
    const u64 recordAddr = reinterpret_cast<u64>(&(msgArea_->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt));
    HCCL_INFO(
        "[MC2_OPEN_DIAG][ServerMsg] msgPos %u, handle %d, seqNum %u, repeatCnt %u, opParamKey %#llx, "
        "msgOpType %u, msgReduceType %u, sendBuffer %#llx, recvBuffer %#llx, dataCnt %llu, "
        "dataType %u, strideCount %llu, waitAddr %#llx, recordAddr %#llx, resourceType %u.",
        msgPos, handle, msg.addMsg.v1Msg.seqNum, repeatCnt, static_cast<unsigned long long>(opParamKey),
        static_cast<u32>(opType), static_cast<u32>(msg.opType), static_cast<unsigned long long>(msg.sendBuffer),
        static_cast<unsigned long long>(msg.recvBuffer), static_cast<unsigned long long>(msg.dataCnt),
        static_cast<u32>(msg.addMsg.v1Msg.hcclDataType), static_cast<unsigned long long>(msg.strideCount),
        static_cast<unsigned long long>(waitAddr), static_cast<unsigned long long>(recordAddr),
        static_cast<u32>(execCtx->resourceType));
    LogAicpuOrderDfxCounterWindow("ServerMsg", groupIdx_, msgArea_);
    LogAicpuOrderDfxCounter(
        "ServerMsg", groupIdx_, msgArea_, msgPos, 0U, 0U, opParamKey, waitAddr, recordAddr, turnNumsAddr_, execCtx);

    std::vector<uint8_t> runParam{};
    for (u32 i = 0U; i < repeatCnt; ++i) {
        const u32 turnIdx = i + 1U;
        UpdateProgress("LoopBegin", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "LoopBegin", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopBegin] group %u, msgPos %u, repeatIdx %u, turnIdx %u, repeatCnt %u, "
            "opParamKey %#llx, resourceType %u.",
            groupIdx_, msgPos, i, turnIdx, repeatCnt, static_cast<unsigned long long>(opParamKey),
            static_cast<u32>(execCtx->resourceType));

        UpdateProgress("BeforeFormat", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "BeforeFormat", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HcclResult ret = FormatOpParamFromMsg(msg, extMsg, *execCtx, i, runParam);
        UpdateProgress("AfterFormat", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "AfterFormat", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopAfterFormat] group %u, msgPos %u, repeatIdx %u, ret %u, runParamSize %zu.", groupIdx_,
            msgPos, i, ret, runParam.size());
        CHK_RET(ret);

        UpdateProgress("BeforeWait", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "BeforeWait", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopBeforeWait] group %u, msgPos %u, repeatIdx %u, turnIdx %u.", groupIdx_, msgPos, i,
            turnIdx);
        ret = LaunchOpenCcoreWait(*execCtx, waitAddr, turnIdx, turnNumsAddr_, turnIdx == repeatCnt);
        UpdateProgress("AfterWait", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "AfterWait", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopAfterWait] group %u, msgPos %u, repeatIdx %u, turnIdx %u, ret %u.", groupIdx_, msgPos,
            i, turnIdx, ret);
        CHK_RET(ret);

        UpdateProgress("BeforeKernel", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "BeforeKernel", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopBeforeKernel] group %u, msgPos %u, repeatIdx %u, turnIdx %u.", groupIdx_, msgPos, i,
            turnIdx);
        ret = LaunchOpenAicpuKernelServer(runParam, *execCtx->resCtxHolder);
        UpdateProgress("AfterKernel", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "AfterKernel", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopAfterKernel] group %u, msgPos %u, repeatIdx %u, turnIdx %u, ret %u.", groupIdx_,
            msgPos, i, turnIdx, ret);
        CHK_RET(ret);

        UpdateProgress("BeforePost", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "BeforePost", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopBeforePost] group %u, msgPos %u, repeatIdx %u, turnIdx %u.", groupIdx_, msgPos, i,
            turnIdx);
        ret = LaunchOpenCcorePost(*execCtx, recordAddr, turnIdx, turnNumsAddr_);
        UpdateProgress("AfterPost", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "AfterPost", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopAfterPost] group %u, msgPos %u, repeatIdx %u, turnIdx %u, ret %u.", groupIdx_, msgPos,
            i, turnIdx, ret);
        CHK_RET(ret);

        UpdateProgress("LoopEnd", msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr);
        LogAicpuOrderDfxCounter(
            "LoopEnd", groupIdx_, msgArea_, msgPos, i, turnIdx, opParamKey, waitAddr, recordAddr, turnNumsAddr_,
            execCtx);
        HCCL_INFO(
            "[MC2_OPEN_DIAG][LoopEnd] group %u, msgPos %u, repeatIdx %u, turnIdx %u.", groupIdx_, msgPos, i, turnIdx);
    }
    SetMsgPosByHandle(handle, msgPos);
    SetRepeatByHandle(handle, repeatCnt);
    return HCCL_SUCCESS;
}

HcclResult CommKfcAicpuServer::Finalize(u32 msgPos)
{
    KeepAlive();
    if (msgArea_ != nullptr && msgPos < HCCL_MSG_CNT) {
        const u64 waitAddr = reinterpret_cast<u64>(&(msgArea_->commMsg.singleMsg.commitTurnCnt[msgPos].cnt));
        const u64 recordAddr = reinterpret_cast<u64>(&(msgArea_->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt));
        LogAicpuOrderDfxCounter(
            "Finalize", groupIdx_, msgArea_, msgPos, 0U, 0U, 0U, waitAddr, recordAddr, turnNumsAddr_, syncExecCtx_);
    }
    return HCCL_SUCCESS;
}

HcclResult CommKfcAicpuServer::IsAllTaskFinished(u32 msgPos, bool& isFinish)
{
    CHK_PTR_NULL(msgArea_);

    isFinish = false;
    if (syncExecCtx_ != nullptr && syncExecCtx_->resourceType == ServerExecResourceType::NEXT_AICPU) {
        const u64 waitAddr = reinterpret_cast<u64>(&(msgArea_->commMsg.singleMsg.commitTurnCnt[msgPos].cnt));
        const u64 recordAddr = reinterpret_cast<u64>(&(msgArea_->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt));
        LogAicpuOrderDfxCounter(
            "IsAllTaskFinishedBeforeSet", groupIdx_, msgArea_, msgPos, 0U, 0U, 0U, waitAddr, recordAddr, turnNumsAddr_,
            syncExecCtx_);
        msgArea_->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt = FINALIZE_FINISH_CNT;
#ifdef __aarch64__
        __asm__ __volatile__("dsb st" : : : "memory");
#endif
        isFinish = true;
        LogAicpuOrderDfxCounter(
            "IsAllTaskFinishedAfterSet", groupIdx_, msgArea_, msgPos, 0U, 0U, 0U, waitAddr, recordAddr, turnNumsAddr_,
            syncExecCtx_);
        HCCL_INFO("Group %u: next aicpu tasks are treated as finished at message pos %u.", groupIdx_, msgPos);
        return HCCL_SUCCESS;
    }

#ifdef MC2_SERVER_ONLY
    HCCL_ERROR("Group %u: mc2_server does not support legacy task finish check.", groupIdx_);
    return HCCL_E_INTERNAL;
#else
    // opHandles_能保证不为空，同一个通信域检查任何一个ophandle即可
    CHK_PRT_RET(ctxToOpHandle_.empty(), HCCL_ERROR("Group %u: legacy op handle is empty.", groupIdx_), HCCL_E_INTERNAL);
    void* firstHandle = ctxToOpHandle_.begin()->second;
    if (HcclCheckFinishByStream(firstHandle) != HCCL_SUCCESS) {
        return HCCL_SUCCESS;
    }

    HcclTaskStatus status;
    if (HcclGetTaskStatus(firstHandle, &status) != HCCL_SUCCESS || status != HcclTaskStatus::HCCL_NORMAL_STATUS) {
        HCCL_ERROR("Group %u: abnormal task status %u.", groupIdx_, static_cast<u32>(status));
        return HCCL_E_INTERNAL;
    }

    msgArea_->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt = FINALIZE_FINISH_CNT;
#ifdef __aarch64__
    __asm__ __volatile__("dsb st" : : : "memory");
#endif
    isFinish = true;
    {
        const u64 waitAddr = reinterpret_cast<u64>(&(msgArea_->commMsg.singleMsg.commitTurnCnt[msgPos].cnt));
        const u64 recordAddr = reinterpret_cast<u64>(&(msgArea_->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt));
        LogAicpuOrderDfxCounter(
            "IsAllTaskFinishedLegacyAfterSet", groupIdx_, msgArea_, msgPos, 0U, 0U, 0U, waitAddr, recordAddr,
            turnNumsAddr_, syncExecCtx_);
    }
    HCCL_INFO("Group %u: all task is finished at message pos %u.", groupIdx_, msgPos);
    for (auto it : ctxToOpHandle_) {
        CHK_RET(HcclReleaseComm(it.second));
        HCCL_INFO("Group %u: Op handle %#llx is released, HCCL context %#llxx.", groupIdx_, it.second, it.first);
    }
    return HCCL_SUCCESS;
#endif
}

HcclResult CommKfcAicpuServer::InterGroupSync(const CommKfcAicpuServer& otherServer, HcclHandle handle)
{
    KeepAlive();
    u32 msgPos, repeat;
    HcclResult ret = otherServer.GetServerInfoForSync(handle, msgPos, repeat);
    if (ret != HCCL_SUCCESS) {
        HCCL_INFO("Group %u: group sync info is not obtained, return code %u.", groupIdx_, ret);
        return ret;
    }
    CHK_PRT_RET(
        msgPos >= HCCL_MSG_CNT, HCCL_ERROR("Group %u: invalid message index %u.", groupIdx_, msgPos), HCCL_E_PARA);

    HcclMsgArea* msgArea = otherServer.GetMsgAreaAddr();
    CHK_PTR_NULL(msgArea);
    const u64 waitAddr = reinterpret_cast<u64>(&(msgArea->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt));
    HCCL_INFO(
        "Group %u: group sync for handle %d: message index %u, finish count %u.", groupIdx_, handle, msgPos, repeat);
    CHK_PTR_NULL(syncExecCtx_);
    if (syncExecCtx_->resourceType == ServerExecResourceType::NEXT_AICPU) {
        return LaunchOpenCcoreWait(*syncExecCtx_, waitAddr, repeat, turnNumsAddr_, false);
    }
#ifdef MC2_SERVER_ONLY
    HCCL_ERROR("Group %u: mc2_server does not support legacy inter-group sync.", groupIdx_);
    return HCCL_E_INTERNAL;
#else
    CHK_PRT_RET(ctxToOpHandle_.empty(), HCCL_ERROR("Group %u: legacy op handle is empty.", groupIdx_), HCCL_E_INTERNAL);
    return HcclLaunchCcoreWait(ctxToOpHandle_.begin()->second, waitAddr, repeat, turnNumsAddr_, false);
#endif
}

HcclResult CommKfcAicpuServer::CheckTimeOut(u32 msgPos)
{
    if (!IsTimeout()) {
        return HCCL_SUCCESS;
    }
    const bool error = (timeout_ >= TIMEOUT_ERROR_THRESHOLD);
    DumpTimeoutDfx(msgPos, error);
    HcclResult ret;
    if (error) {
        HCCL_ERROR("Group %u: timeout %u seconds at message pos %u.", groupIdx_, timeout_, msgPos);
        ret = HCCL_E_TIMEOUT;
    } else {
        HCCL_RUN_INFO("Group %u: timeout %u seconds at message pos %u.", groupIdx_, timeout_, msgPos);
        ret = HCCL_E_AGAIN;
    }
    timeout_ *= 2U;
    return ret;
}

void CommKfcAicpuServer::UpdateProgress(
    const char* stage, u32 msgPos, u32 repeatIdx, u32 turnIdx, u64 opParamKey, u64 waitAddr, u64 recordAddr)
{
    lastProgress_.stage = stage;
    lastProgress_.msgPos = msgPos;
    lastProgress_.repeatIdx = repeatIdx;
    lastProgress_.turnIdx = turnIdx;
    lastProgress_.opParamKey = opParamKey;
    lastProgress_.waitAddr = waitAddr;
    lastProgress_.recordAddr = recordAddr;
}

void CommKfcAicpuServer::DumpTimeoutDfx(u32 msgPos, bool isHardTimeout) const
{
    const u32 commitCnt = (msgArea_ != nullptr && msgPos < HCCL_MSG_CNT) ?
                              msgArea_->commMsg.singleMsg.commitTurnCnt[msgPos].cnt :
                              INVALID_UINT;
    const u32 finishedCnt = (msgArea_ != nullptr && msgPos < HCCL_MSG_CNT) ?
                                msgArea_->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt :
                                INVALID_UINT;
    MC2_TIMEOUT_LOG(
        isHardTimeout,
        "[MC2_OPEN_TIMEOUT][Summary] group %u, msgPos %u, timeout %llu, threshold %llu, rankNum %u, "
        "msgArea %p, turnNumsAddr %#llx, commitCnt %u, finishedCnt %u.",
        groupIdx_, msgPos, static_cast<unsigned long long>(timeout_),
        static_cast<unsigned long long>(TIMEOUT_ERROR_THRESHOLD), rankNum_, msgArea_,
        static_cast<unsigned long long>(turnNumsAddr_), commitCnt, finishedCnt);
    MC2_TIMEOUT_LOG(
        isHardTimeout,
        "[MC2_OPEN_TIMEOUT][Progress] group %u, stage[%s], progressMsgPos %u, repeatIdx %u, turnIdx %u, "
        "opParamKey %#llx, waitAddr %#llx, recordAddr %#llx.",
        groupIdx_, lastProgress_.stage, lastProgress_.msgPos, lastProgress_.repeatIdx, lastProgress_.turnIdx,
        static_cast<unsigned long long>(lastProgress_.opParamKey),
        static_cast<unsigned long long>(lastProgress_.waitAddr),
        static_cast<unsigned long long>(lastProgress_.recordAddr));

    for (const auto& execCtx : execCtxList_) {
        MC2_TIMEOUT_LOG(
            isHardTimeout,
            "[MC2_OPEN_TIMEOUT][ExecCtx] group %u, commName[%s], opParamKey %#llx, resourceType %u, "
            "mainThread %#llx, mainStream %p, dispatcher %p.",
            groupIdx_, execCtx.commName.c_str(), static_cast<unsigned long long>(execCtx.opParamKey),
            static_cast<u32>(execCtx.resourceType), static_cast<unsigned long long>(execCtx.mainThread),
            execCtx.mainStream, execCtx.dispatcher);
        if (execCtx.resourceType != ServerExecResourceType::NEXT_AICPU || execCtx.mainThread == 0U) {
            continue;
        }

        auto* thread = reinterpret_cast<hccl::Thread*>(execCtx.mainThread);
        if (thread == nullptr) {
            MC2_TIMEOUT_LOG(
                isHardTimeout, "[MC2_OPEN_TIMEOUT][Thread] group %u, commName[%s], thread is nullptr.", groupIdx_,
                execCtx.commName.c_str());
            continue;
        }

        auto* streamLite = static_cast<Hccl::StreamLite*>(thread->GetStreamLitePtr());
        if (streamLite == nullptr || streamLite->GetRtsq() == nullptr) {
            MC2_TIMEOUT_LOG(
                isHardTimeout, "[MC2_OPEN_TIMEOUT][Rtsq] group %u, commName[%s], streamLite %p or rtsq is nullptr.",
                groupIdx_, execCtx.commName.c_str(), streamLite);
            continue;
        }
        auto* rtsq = streamLite->GetRtsq();
        u32 queryHead = INVALID_UINT;
        u32 queryTail = INVALID_UINT;
        EXECEPTION_CATCH(queryHead = rtsq->QuerySqHead(), queryHead = INVALID_UINT);
        EXECEPTION_CATCH(queryTail = rtsq->QuerySqTail(), queryTail = INVALID_UINT);
        MC2_TIMEOUT_LOG(
            isHardTimeout,
            "[MC2_OPEN_TIMEOUT][Rtsq] group %u, commName[%s], streamLite %p, rtsq %p, cachedHead %u, "
            "cachedTail %u, depth %u, taskId %u, queryHead %u, queryTail %u.",
            groupIdx_, execCtx.commName.c_str(), streamLite, rtsq, rtsq->GetHead(), rtsq->GetTail(), rtsq->GetSqDepth(),
            rtsq->GetTaskId(), queryHead, queryTail);
    }
}

HcclResult CommKfcAicpuServer::GetServerInfoForSync(HcclHandle handle, u32& msgPos, u32& repeat) const
{
    CHK_PRT_RET(handle < 0, HCCL_ERROR("Group %u: invalid handle id %d.", groupIdx_, handle), HCCL_E_PARA);
    auto it = handleIdToMsgPos_.find(handle);
    if (it == handleIdToMsgPos_.end()) {
        HCCL_INFO("Group %u: handle %d in this group is not ready.", groupIdx_, handle);
        return HCCL_E_AGAIN;
    }
    msgPos = it->second;

    it = handleIdToRepeat_.find(handle);
    CHK_PRT_RET(
        it == handleIdToRepeat_.end(), HCCL_ERROR("Group %u: handle %d in this group is not ready.", groupIdx_, handle),
        HCCL_E_INTERNAL);
    repeat = it->second;
    return HCCL_SUCCESS;
}

HcclResult CommKfcAicpuServer::ErrorDfxProcess(HcclResult errorCode)
{
    if (errorCode == HCCL_SUCCESS) {
        return errorCode;
    }

    if (syncExecCtx_ != nullptr && syncExecCtx_->resourceType == ServerExecResourceType::NEXT_AICPU) {
        AicpuKfcUtils::PrintAllHcclMsgArea(msgArea_, rankNum_, errorCode != HCCL_E_AGAIN);
        HCCL_ERROR(
            "Group %u: next aicpu server error process, commName[%s], errorCode[%u].", groupIdx_,
            syncExecCtx_->commName.c_str(), static_cast<u32>(errorCode));
        return (errorCode == HCCL_E_AGAIN) ? HCCL_SUCCESS : errorCode;
    }

    if (syncExecCtx_ == nullptr && ctxToOpHandle_.empty()) {
        AicpuKfcUtils::PrintAllHcclMsgArea(msgArea_, rankNum_, errorCode != HCCL_E_AGAIN);
        HCCL_ERROR(
            "Group %u: server error before exec ctx is ready, errorCode[%u].", groupIdx_, static_cast<u32>(errorCode));
        return (errorCode == HCCL_E_AGAIN) ? HCCL_SUCCESS : errorCode;
    }

#ifdef MC2_SERVER_ONLY
    AicpuKfcUtils::PrintAllHcclMsgArea(msgArea_, rankNum_, errorCode != HCCL_E_AGAIN);
    HCCL_ERROR(
        "Group %u: mc2_server does not support legacy error process, errorCode[%u].", groupIdx_,
        static_cast<u32>(errorCode));
    return (errorCode == HCCL_E_AGAIN) ? HCCL_SUCCESS : errorCode;
#else
    CHK_PRT_RET(ctxToOpHandle_.empty(), HCCL_ERROR("Group %u: legacy op handle is empty.", groupIdx_), HCCL_E_INTERNAL);
    void* firstHandle = ctxToOpHandle_.begin()->second;
    if (errorCode == HCCL_E_AGAIN) {
        AicpuKfcUtils::PrintAllHcclMsgArea(msgArea_, rankNum_);
        HcclPrintTaskExceptionAllComm(firstHandle);
        errorCode = HCCL_SUCCESS;
    } else {
        AicpuKfcUtils::PrintAllHcclMsgArea(msgArea_, rankNum_, true);
        HcclPrintTaskExceptionAllComm(firstHandle);
    }
    return errorCode;
#endif
}
