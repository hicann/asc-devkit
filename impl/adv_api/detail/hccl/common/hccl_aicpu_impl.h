/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hccl_aicpu_impl.h
 * \brief
 */
#ifndef IMPL_COMMON_HCCL_AICPU_IMPL_H
#define IMPL_COMMON_HCCL_AICPU_IMPL_H

#include "hccl_aicpu_common.h"
#include "hccl_control.h"

namespace AscendC {
template <const auto& config>
__aicore__ inline bool HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::CheckCommonPrepareParamValid(
    const CommonPrepareParam& param)
{
    const HcclCMDType commType = param.commType.prepareType;
    uint64_t tiling = 0UL;
    if (commType < HcclCMDType::HCCL_CMD_ALL) {
        tiling = ccOpTilingDataTable_[static_cast<uint32_t>(commType)];
    }
    if (curVersion_ == HcclTilingVersion::NEW_TILING_VERSION ||
        curVersion_ == HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION ||
        curVersion_ == HcclTilingVersion::CONTEXT_DECOUPLE_VERSION) {
        ASCENDC_HCCL_API_ASSERT(
            tiling != 0UL, { return false; }, "Failed to prepare for type %u, ensure SetCcTiling has been called.",
            static_cast<uint32_t>(commType));
    } else {
        ASCENDC_HCCL_API_ASSERT(
            curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION && tiling == 0UL, { return false; },
            "Failed to prepare for type %u, ensure Init has been called", static_cast<uint32_t>(commType));
    }
    ASCENDC_HCCL_API_ASSERT(
        param.sendBuf != nullptr && param.recvBuf != nullptr, { return false; },
        "Call Prepare[%d] failed, the param sendBuf/recvBuf is nullptr, "
        "which is an invalid parameter.",
        static_cast<int32_t>(commType));
    ASCENDC_HCCL_API_ASSERT(
        commType == HcclCMDType::HCCL_CMD_BATCH_WRITE ||
            (param.dataType >= HCCL_DATA_TYPE_INT8 && param.dataType < HCCL_DATA_TYPE_RESERVED),
        { return false; }, "Call Prepare[%d] failed, param HcclDataType is %d, invalid.",
        static_cast<int32_t>(commType), static_cast<int32_t>(param.dataType));
    if (commType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        ASCENDC_HCCL_API_ASSERT(
            param.paramExt.sendCounts != nullptr && param.paramExt.sdispls != nullptr &&
                param.paramExt.recvCounts != nullptr && param.paramExt.rdispls != nullptr,
            { return false; },
            "Call AlltoAllV failed, "
            "param sendCounts/recvCounts/sdispls/rdispls is nullptr, invalid.");
    } else {
        ASCENDC_HCCL_API_ASSERT(
            param.count != 0, { return false; }, "Call Prepare[%d] failed, param sendCount/recvCount is 0, invalid.",
            static_cast<int32_t>(commType));
    }
    return true;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::InitWorkingFlag()
{
    using T = decltype(config);
    static_assert(std::is_same<T, const HcclServerConfig&>::value);
    KERNEL_LOG(KERNEL_INFO, "Working core type %u id %u.", static_cast<uint8_t>(config.type), config.blockId);
    if constexpr (config.type == CoreType::ON_AIV) {
        workingFlag_ = (g_coreType == AscendC::AIV && GetBlockIdx() == config.blockId);
    } else if constexpr (config.type == CoreType::ON_AIC) {
        workingFlag_ = (g_coreType == AscendC::AIC && GetBlockIdx() == config.blockId);
    } else {
        workingFlag_ = (GetBlockIdx() == config.blockId);
    }
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::InitInner(
    uint64_t msgAddr, HcclTilingVersion version)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ == HcclTilingVersion::INVALID_TILING_VERSION, { return; }, "Init repeatedly is not allowed.");

    if (unlikely(msgAddr == 0UL)) {
        return;
    }
    // ensure hcclMsgArea 512B aligned
    if (msgAddr & 0x1ff) {
        msgAddr = (msgAddr & (~((uint64_t)0x1ff))) + 0x200;
    }
    hcclMsgArea_ = reinterpret_cast<__gm__ HcclMsgArea*>(msgAddr);
    for (uint32_t i = 0U; i < HCCL_MAX_HANDLE_ID; ++i) {
        handleIdMsgPosition_[i] = -1;
    }
    InitWorkingFlag();
    curVersion_ = version;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendMsgToServer(
    uint16_t queId, const CommonPrepareParam& param, int8_t srcGroupID, HcclHandle srcHandleID)
{
    if (!workingFlag_ && queueNum_ == 0U) {
        return;
    }
    __gm__ HcclMsg* hcclSendMsg;
    if (queueNum_ == 0U) {
        hcclSendMsg = hcclMsgArea_->commMsg.singleMsg.sendMsgs + curMsgPosition_[0U];
    } else {
        hcclSendMsg =
            hcclMsgArea_->commMsg.multiMsg.sendMsgs[queId + GetBlockIdx() * queueNum_] + curMsgPosition_[queId];
    }

    do {
        HCCL_CHECK_RESTART(hcclMsgArea_, return);
        FlushDataCache(hcclSendMsg);
    } while (debugMode_ != HCCL_ONLY_COMPUTE && hcclSendMsg->addMsg.v0Msg.valid == HCCL_MSG_VALID_MASK);
    KERNEL_LOG(KERNEL_INFO, "Hccl send msg[%u] is available now.", curMsgPosition_[queId]);
    if (srcGroupID < 0) {
        uint64_t tiling = 0UL;
        if (param.commType.prepareType < HcclCMDType::HCCL_CMD_ALL) {
            if (curVersion_ == HcclTilingVersion::CONTEXT_DECOUPLE_VERSION) {
                tiling = ccOpParamTable_[static_cast<uint32_t>(param.commType.prepareType)];
            } else {
                tiling = ccOpTilingDataTable_[static_cast<uint32_t>(param.commType.prepareType)];
            }
        }
#if __NPU_ARCH__ == 3510
        AssembleHcclMsgV2(param, curVersion_, curHandleId_, tiling, hcclSendMsg, &hcclMsgArea_->controlMsg);
#else
        AssembleHcclMsg(param, curVersion_, curHandleId_, tiling, hcclSendMsg, &hcclMsgArea_->controlMsg);
#endif
    } else {
        AssembleHcclMsg(param, srcGroupID, srcHandleID, hcclSendMsg);
    }
    FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg));
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendMsgToServer(
    const AlltoAllVParamExt& param)
{
    if (!workingFlag_) {
        return;
    }
    __gm__ HcclMsgExt* hcclSendMsg = &(hcclMsgArea_->commMsg.singleMsg.paramExtMsgList[curMsgPosition_[0U]]);
    do {
        HCCL_CHECK_RESTART(hcclMsgArea_, return);
        FlushDataCache(hcclSendMsg);
    } while ((debugMode_ != HCCL_ONLY_COMPUTE) && (hcclSendMsg->valid == HCCL_MSG_VALID_MASK));
    KERNEL_LOG(KERNEL_INFO, "Hccl send extMsg[%u] is available now.", curMsgPosition_[0U]);
    uint32_t rankNum = GetRankDim();
    if ((rankNum == 0U) || (rankNum > HCCL_MAX_RANK_NUM_V2)) {
        KERNEL_LOG(KERNEL_ERROR, "Invalid rank num %u for extended message.", rankNum);
        return;
    }
    AssembleHcclMsgExt(param, rankNum, hcclSendMsg);
    GlobalTensor<int64_t> globalHcclMsgArea;
    for (uint32_t i = 0U; i < rankNum; i += MAX_DCCI_CNT / sizeof(uint64_t)) {
        FlushDataCache(globalHcclMsgArea, (hcclSendMsg->sendCounts + i));
        FlushDataCache(globalHcclMsgArea, (hcclSendMsg->sendOffset + i));
        FlushDataCache(globalHcclMsgArea, (hcclSendMsg->recvCounts + i));
        FlushDataCache(globalHcclMsgArea, (hcclSendMsg->recvOffset + i));
    }
    FlushDataCache(globalHcclMsgArea, hcclSendMsg->reserved);
}

template <const auto& config>
__aicore__ inline uint16_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetStepSizeByHandle(
    HcclHandle handle)
{
    if (curVersion_ != HcclTilingVersion::NEW_TILING_VERSION &&
        curVersion_ != HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION &&
        curVersion_ != HcclTilingVersion::CONTEXT_DECOUPLE_VERSION) {
        return 0U;
    }
    const uint8_t commType = handleId2CmdType_[handle];
    if (commType != static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLTOALLV)) {
        return 0U;
    }
    __gm__ Mc2CcTilingInner* tilingPtr;
    if (curVersion_ == HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION ||
        curVersion_ == HcclTilingVersion::CONTEXT_DECOUPLE_VERSION) {
        tilingPtr = reinterpret_cast<__gm__ Mc2CcTilingInner*>(ccOpTilingDataTable_[commType] + tilingBaseAddr_);
    } else {
        tilingPtr = reinterpret_cast<__gm__ Mc2CcTilingInner*>(ccOpTilingDataTable_[commType]);
    }
    if (tilingPtr == nullptr) {
        return 0U;
    }
    return tilingPtr->stepSize;
}

template <const auto& config>
__aicore__ inline uint16_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetStepCntsPerRepeatByHandle(
    HcclHandle handle)
{
    return (GetStepSizeByHandle(handle) == 0U ? 1U : GetRankDim());
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SetCommitTurnCntToGm(
    uint8_t msgPos, uint64_t turnCnt, HcclHandle handleId)
{
    handleIdCommitTurnCnt_[handleId] += turnCnt;
    if (queueNum_ != 0U || !workingFlag_) {
        return;
    }

    __gm__ TurnCnt* commitGM = hcclMsgArea_->commMsg.singleMsg.commitTurnCnt + msgPos;
    do {
        HCCL_CHECK_RESTART(hcclMsgArea_, return);
        FlushDataCache(commitGM);
    } while ((debugMode_ != HCCL_ONLY_COMPUTE) && (commitGM->cnt >= handleIdCommitTurnCnt_[handleId]));
    KERNEL_LOG(
        KERNEL_INFO, "Block idx[%d] write commit turn cnt[%lu].", DEFAULT_CFG.blockId,
        handleIdCommitTurnCnt_[handleId]);
    commitGM->cnt = handleIdCommitTurnCnt_[handleId];
    commitGM->valid = COMMIT_VALID_MASK;
    FlushDataCache(commitGM);
    if (workingFlag_) {
        __gm__ TurnCnt* apiInfo = &(hcclMsgArea_->apiStats.commitStats[handleId2CmdType_[handleId]]);
        FlushDataCache(apiInfo);
        apiInfo->cnt += turnCnt;
        FlushDataCache(apiInfo);
    }
}

template <const auto& config>
__aicore__ inline uint64_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::WaitFinishCntFromGm(
    uint8_t msgPos, uint64_t expectedCnt)
{
    __gm__ TurnCnt* finishGM = hcclMsgArea_->commMsg.singleMsg.finishedTurnCnt + msgPos;
    GlobalTensor<int64_t> globalHcclMsgArea;
#if !defined(UT_TEST)
    while (true) {
        HCCL_CHECK_RESTART(hcclMsgArea_, return finishGM->cnt);
        FlushDataCacheAicpu(globalHcclMsgArea, finishGM);
        if ((debugMode_ == HCCL_ONLY_COMPUTE) || (finishGM->cnt >= expectedCnt)) {
            break;
        }
    }
#endif
    return finishGM->cnt;
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::CommonPrepareImpl(
    const CommonPrepareParam& param)
{
    HCCL_CHECK_RESTART(hcclMsgArea_, return INVALID_HANDLE_ID);
    if (unlikely(param.repeat == 0U)) {
        return INVALID_HANDLE_ID;
    }
    if ((param.commType.prepareType != HcclCMDType::HCCL_CMD_BATCH_WRITE) &&
        (GetHcclDataTypeSize(param.dataType) == 0U)) {
        KERNEL_LOG(
            KERNEL_ERROR, "Call Prepare[%d] failed, param HcclDataType is %d, invalid.",
            static_cast<int32_t>(param.commType.prepareType), static_cast<int32_t>(param.dataType));
        return INVALID_HANDLE_ID;
    }
    ASCENDC_HCCL_API_ASSERT(
        CheckCommonPrepareParamValid(param), { return INVALID_HANDLE_ID; }, "Call Prepare[%d] failed, param invalid.",
        static_cast<int32_t>(param.commType.prepareType));

    HcclHandle handleId = ++curHandleId_;
    ASCENDC_HCCL_API_ASSERT(
        handleId < HCCL_MAX_HANDLE_ID, { return INVALID_HANDLE_ID; },
        "Call Prepare[%d] failed, Prepare interface call num is[%d], expected no more than[%d].",
        static_cast<int32_t>(param.commType.prepareType), handleId + 1, HCCL_MAX_HANDLE_ID);
    if (param.commType.prepareType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        SendMsgToServer(param.paramExt);
    }
    const uint16_t queId = (queueNum_ == 0U ? 0U : static_cast<uint16_t>(param.dataType));
    SendMsgToServer(queId, param);
    handleIdMsgPosition_[handleId] = curMsgPosition_[queId];
    handleIdRepeat_[handleId] = param.repeat;
    handleId2CmdType_[handleId] = static_cast<uint8_t>(param.commType.prepareType);
    if constexpr (commit) {
        SetCommitTurnCntToGm(curMsgPosition_[queId], param.repeat * GetStepCntsPerRepeatByHandle(handleId), handleId);
    }
    ++(curMsgPosition_[queId]);
    ASCENDC_HCCL_API_ASSERT(
        curMsgPosition_[queId] < HCCL_MSG_CNT, { return INVALID_HANDLE_ID; },
        "Message amount exceeds the maximum value when prepare.");
    return handleId;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::ResetFinishedTurnCnt()
{
    __gm__ TurnCnt* finishArea = hcclMsgArea_->commMsg.singleMsg.finishedTurnCnt;
    GlobalTensor<int64_t> globalHcclMsgArea;
    for (uint32_t i = 0U; i <= curMsgPosition_[0U]; ++i) {
        __gm__ TurnCnt* finishGM = finishArea + i;
        finishGM->cnt = 0;
        FlushDataCache(globalHcclMsgArea, finishGM);
    }
}

template <const auto& config>
template <bool sync>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendFinalizeMsg()
{
    const uint16_t totalQueNum = (queueNum_ == 0U ? 1U : queueNum_);
    CommonPrepareParam param;
    param.commType.msgType = ControlMsgType::HCCL_CMD_FINALIZE;
    for (uint16_t idx = 0U; idx < totalQueNum; ++idx) {
        KERNEL_LOG(
            KERNEL_INFO, "Only block idx[%ld] write sendMsgList[%u] when Finalize.", GetBlockIdx(),
            curMsgPosition_[idx]);
        SendMsgToServer(idx, param);
        if constexpr (!sync) {
            ++(curMsgPosition_[idx]);
            ASCENDC_HCCL_API_ASSERT(
                curMsgPosition_[idx] < HCCL_MSG_CNT, { return; },
                "Message amount exceeds the maximum value when finalize.");
        }
    }
}

template <const auto& config>
__aicore__ inline uint32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetRankId()
{
    ASCENDC_HCCL_API_ASSERT(
        hcclContext_ != nullptr, { return UINT32_MAX; }, "Call GetRankDim failed, ensure InitV2 has been called!");
    return hcclContext_->rankId;
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AllReduce(
    GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, uint8_t repeat)
{
    ASCENDC_HCCL_API_ASSERT(
        op >= HCCL_REDUCE_SUM && op < HCCL_REDUCE_RESERVED, { return INVALID_HANDLE_ID; },
        "Call AllReduce failed, param HcclReduceOp is %d, invalid.", static_cast<int32_t>(op));

    return CommonPrepareImpl<commit>(
        {HcclCMDType::HCCL_CMD_ALLREDUCE, sendBuf, recvBuf, count, dataType, dataType, op, 0, repeat});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AllGather(
    GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t sendCount, HcclDataType dataType, uint64_t strideCount, uint8_t repeat)
{
    return CommonPrepareImpl<commit>(
        {HcclCMDType::HCCL_CMD_ALLGATHER, sendBuf, recvBuf, sendCount, dataType, dataType, HCCL_REDUCE_RESERVED,
         strideCount, repeat});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::ReduceScatter(
    GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t recvCount, HcclDataType dataType, HcclReduceOp op, uint64_t strideCount,
    uint8_t repeat)
{
    ASCENDC_HCCL_API_ASSERT(
        op >= HCCL_REDUCE_SUM && op < HCCL_REDUCE_RESERVED, { return INVALID_HANDLE_ID; },
        "Call ReduceScatter failed, param HcclReduceOp is %d, invalid.", static_cast<int32_t>(op));
    return CommonPrepareImpl<commit>(
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, sendBuf, recvBuf, recvCount, dataType, dataType, op, strideCount,
         repeat});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AlltoAll(
    GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t dataCount, HcclDataType dataType, uint64_t strideCount, uint8_t repeat)
{
    return CommonPrepareImpl<commit>(
        {HcclCMDType::HCCL_CMD_ALLTOALL, sendBuf, recvBuf, dataCount, dataType, dataType, HCCL_REDUCE_RESERVED,
         strideCount, repeat});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AlltoAllV(
    GM_ADDR sendBuf, void* sendCounts, void* sdispls, HcclDataType sendType, GM_ADDR recvBuf, void* recvCounts,
    void* rdispls, HcclDataType recvType, uint8_t repeat)
{
    ASCENDC_HCCL_API_ASSERT(
        sendType == recvType, { return INVALID_HANDLE_ID; },
        "Call AlltoAllV failed, param sendType[%d] is not equal to recvType[%d], invalid.",
        static_cast<int32_t>(sendType), static_cast<int32_t>(recvType));
    return CommonPrepareImpl<commit>(
        {HcclCMDType::HCCL_CMD_ALLTOALLV,
         sendBuf,
         recvBuf,
         0U,
         sendType,
         recvType,
         HCCL_REDUCE_RESERVED,
         0U,
         repeat,
         {static_cast<uint64_t*>(sendCounts), static_cast<uint64_t*>(sdispls), static_cast<uint64_t*>(recvCounts),
          static_cast<uint64_t*>(rdispls)}});
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Query(HcclHandle handleId)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION, { return HCCL_FAILED; },
        "Call Query failed, ensure Hccl::Init func has been called successfully!");
    ASCENDC_HCCL_API_ASSERT(
        (handleId > INVALID_HANDLE_ID) && (handleId < HCCL_MAX_HANDLE_ID), { return HCCL_FAILED; },
        "Call Query failed, handleId is[%d], expected in range of [0, %d).", handleId, HCCL_MAX_HANDLE_ID);
    if (queueNum_ != 0U) {
        return 0;
    }
    int8_t curMsgPos = handleIdMsgPosition_[handleId];
    ASCENDC_HCCL_API_ASSERT(
        curMsgPos >= 0, { return HCCL_FAILED; }, "Call Query failed, handleId[%d] was not got by Prepare interface.",
        handleId);
    return WaitFinishCntFromGm(curMsgPos, 0UL);
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Wait(HcclHandle handleId)
{
    HCCL_CHECK_RESTART(hcclMsgArea_, return HCCL_FAILED);
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION, { return HCCL_FAILED; },
        "Call Wait failed, ensure Hccl::Init func has been called successfully!");
    if (queueNum_ != 0U) {
        return HCCL_SUCCESS;
    }
    if (unlikely(handleId <= INVALID_HANDLE_ID || handleId >= HCCL_MAX_HANDLE_ID)) {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to wait, handleId is[%d], expected to be in range of [0, %d).", handleId,
            HCCL_MAX_HANDLE_ID);
        return HCCL_FAILED;
    }
    uint16_t& waitCnt = handleIdWaitCallNum_[handleId];
    if (unlikely(waitCnt >= handleIdCommitTurnCnt_[handleId])) {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to wait, call num of Wait for handleId[%d] is[%u], expected to be no larger "
            "than Commit num[%u].",
            handleId, waitCnt + 1, handleIdCommitTurnCnt_[handleId]);
        return HCCL_FAILED;
    }
    if (workingFlag_) {
        __gm__ TurnCnt* apiInfo = &(hcclMsgArea_->apiStats.waitStats[handleId2CmdType_[handleId]]);
        FlushDataCache(apiInfo);
        ++(apiInfo->cnt);
        FlushDataCache(apiInfo);
    }
    int8_t curMsgPos = handleIdMsgPosition_[handleId];
    ASCENDC_HCCL_API_ASSERT(
        curMsgPos >= 0, { return HCCL_FAILED; }, "Call Wait failed, handleId[%d] was not got by Prepare interface.",
        handleId);
    const uint16_t stepSize = GetStepSizeByHandle(handleId);
    waitCnt += (stepSize == 0U ? 1U : stepSize);
    (void)WaitFinishCntFromGm(curMsgPos, waitCnt);
    return HCCL_SUCCESS;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Commit(HcclHandle handleId)
{
    HCCL_CHECK_RESTART(hcclMsgArea_, return);
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION, { return; },
        "Call Commit failed, ensure Hccl::Init func has been called successfully!");
    if (unlikely(handleId <= INVALID_HANDLE_ID || handleId >= HCCL_MAX_HANDLE_ID)) {
        KERNEL_LOG(
            KERNEL_ERROR, "Call Commit failed, handleId is[%d], expected in range of [0, %d).", handleId,
            HCCL_MAX_HANDLE_ID);
        return;
    }
    const uint16_t commitCnt = handleIdCommitTurnCnt_[handleId];
    if (unlikely(commitCnt >= handleIdRepeat_[handleId] * GetStepCntsPerRepeatByHandle(handleId))) {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Call Commit for handleId[%d] failed, call num is[%u], "
            "expected no larger than task num[%u].",
            handleId, commitCnt + 1, handleIdRepeat_[handleId] * GetStepCntsPerRepeatByHandle(handleId));
        return;
    }
    const uint16_t stepSize = GetStepSizeByHandle(handleId);
    SetCommitTurnCntToGm(handleIdMsgPosition_[handleId], (stepSize == 0U ? 1U : stepSize), handleId);
}

template <const auto& config>
template <bool sync>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Finalize()
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION, { return; },
        "Call Finalize failed, ensure Hccl::Init func has been called successfully!");
    HCCL_CHECK_RESTART(hcclMsgArea_, return);

    if (!workingFlag_ && queueNum_ == 0U) {
        ++(curMsgPosition_[0U]);
        ASCENDC_HCCL_API_ASSERT(
            curMsgPosition_[0U] < HCCL_MSG_CNT, { return; }, "Message amount exceeds the maximum value when finalize.");
        return;
    }

    // 1. wait until last hccl task finished(the commitTurnCnt will be reset by aicpu-server before task finished),
    //    then commitTurnCnt can be used by next op.
    if constexpr (sync) {
        if (curHandleId_ > INVALID_HANDLE_ID) {
            KERNEL_LOG(KERNEL_INFO, "Wait hccl task finished for last HandleId[%d] when Finalize.", curHandleId_);
            while ((debugMode_ != HCCL_ONLY_COMPUTE) && (Query(curHandleId_) < handleIdRepeat_[curHandleId_])) {
                HCCL_CHECK_RESTART(hcclMsgArea_, return);
            }
        }
    }

    // 2. send Finalize msg
    SendFinalizeMsg<sync>();

    if constexpr (sync) {
        // 3. wait for server sqe task finished, and client can ResetFinishedTurnCnt
        // 4. reset finishedTurnCnt, then the finishedTurnCnt can be used by next op.
        __gm__ TurnCnt* finishGM = hcclMsgArea_->commMsg.singleMsg.finishedTurnCnt + curMsgPosition_[0U];
        KERNEL_LOG(KERNEL_INFO, "Only block idx[%ld] wait until Finalize msg has been read.", GetBlockIdx());
        do {
            HCCL_CHECK_RESTART(hcclMsgArea_, return);
            FlushDataCache(finishGM);
        } while ((debugMode_ != HCCL_ONLY_COMPUTE) && (finishGM->cnt != FINALIZE_FINISH_CNT));
        KERNEL_LOG(KERNEL_INFO, "Only block idx[%ld] will ResetFinishedTurnCnt.", GetBlockIdx());
        ResetFinishedTurnCnt();
        ++(curMsgPosition_[0U]);
        ASCENDC_HCCL_API_ASSERT(
            curMsgPosition_[0U] < HCCL_MSG_CNT, { return; }, "Message amount exceeds the maximum value when finalize.");
    }
    if (workingFlag_) {
        UpdateControlMsgCount(hcclMsgArea_, ControlMsgType::HCCL_CMD_FINALIZE);
        __gm__ TurnCnt* snapshots = hcclMsgArea_->apiStats.snapshots;
        FlushDataCache(snapshots);
        for (auto handleId = 0; handleId <= curHandleId_; ++handleId) {
            auto& apiSnapshot = snapshots[snapshots->cnt % HCCL_API_SNAPSHOTS_CNT + 1UL];
            apiSnapshot.cnt = handleId2CmdType_[handleId];
            FlushDataCache(&apiSnapshot);
            ++(snapshots->cnt);
        }
        FlushDataCache(snapshots);
    }
}
} // namespace AscendC

#endif
