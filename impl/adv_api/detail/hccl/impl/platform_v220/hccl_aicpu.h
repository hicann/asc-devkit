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
 * \file hccl_aicpu.h
 * \brief
 */
#ifndef IMPL_V220_HCCL_AICPU_H
#define IMPL_V220_HCCL_AICPU_H

#include "../../common/hccl_aicpu_impl.h"

namespace AscendC {
template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::BatchWrite(
    GM_ADDR batchWriteInfo, uint32_t itemNum, uint16_t queueID)
{
    return CommonPrepareImpl<true>(
        {HcclCMDType::HCCL_CMD_BATCH_WRITE, batchWriteInfo, batchWriteInfo, itemNum, static_cast<HcclDataType>(queueID),
         static_cast<HcclDataType>(queueID), static_cast<HcclReduceOp>(queueID + GetBlockIdx() * queueNum_)});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AlltoAllvWrite(
    GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize)
{
    CommonPrepareParam commonPrepareParam = {
        HcclCMDType::HCCL_CMD_HALF_ALLTOALLV,
        usrIn,
        usrIn,
        localDataSize,
        HCCL_DATA_TYPE_INT8,
        HCCL_DATA_TYPE_INT8,
        HCCL_REDUCE_RESERVED,
        0,
        1,
        {},
        {reinterpret_cast<uint64_t>(sendOffsets), reinterpret_cast<uint64_t>(sendSizes), remoteWinOffset}};

    return CommonPrepareImpl<commit>(commonPrepareParam);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::InterHcclGroupSync(
    int8_t srcGroupID, HcclHandle srcHandleID)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION, { return; },
        "Call InterHcclGroupSync failed, ensure Hccl::Init func has been called successfully!");
    CommonPrepareParam param;
    param.commType.msgType = ControlMsgType::HCCL_CMD_INTER_GROUP_SYNC;
    SendMsgToServer(0U, param, srcGroupID, srcHandleID);
    ++(curMsgPosition_[0U]);
    ASCENDC_HCCL_API_ASSERT(
        curMsgPosition_[0U] < HCCL_MSG_CNT, { return; }, "Message amount exceeds the maximum value when sync group.");
    if (workingFlag_) {
        UpdateControlMsgCount(hcclMsgArea_, ControlMsgType::HCCL_CMD_INTER_GROUP_SYNC);
    }
}

template <const auto& config>
__aicore__ inline GM_ADDR HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetWindowsInAddr(uint32_t rankId)
{
    ASCENDC_HCCL_API_ASSERT(
        rankId < GetRankDim(), { return nullptr; }, "GetWindowsInAddr failed, rankId[%u], expected less than[%u]",
        rankId, GetRankDim());
    if (devType_ != HCCL_ASCEND910B) {
        __gm__ HcclContextDef::HcclOpResParam* hcclContext = (__gm__ HcclContextDef::HcclOpResParam*)hcclContext_;
        if (rankId == hcclContext->rankId) {
            return reinterpret_cast<GM_ADDR>(hcclContext->localWindowsIn);
        } else {
            const auto addr = GetRemoteRankAddrs(hcclContext, rankId);
            return reinterpret_cast<GM_ADDR>(addr != nullptr ? addr->windowsIn : 0UL);
        }
    } else {
        if (hcclContext_->multiFlag == 0U) {
            return (GM_ADDR)hcclContext_->windowsIn[rankId];
        } else {
            if (rankId == hcclContext_->rankId) {
                return (GM_ADDR)(hcclContext_->data[rankId].localInput.addr);
            } else {
                return (GM_ADDR)(hcclContext_->data[rankId].remoteInput.addr);
            }
        }
    }
}

template <const auto& config>
__aicore__ inline GM_ADDR HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetWindowsOutAddr(uint32_t rankId)
{
    ASCENDC_HCCL_API_ASSERT(
        rankId < GetRankDim(), { return nullptr; }, "GetWindowsOutAddr failed, rankId[%u], expected less than[%u]",
        rankId, GetRankDim());
    if (devType_ != HCCL_ASCEND910B) {
        __gm__ HcclContextDef::HcclOpResParam* hcclContext = (__gm__ HcclContextDef::HcclOpResParam*)hcclContext_;
        if (rankId == hcclContext->rankId) {
            return reinterpret_cast<GM_ADDR>(hcclContext->localWindowsOut);
        } else {
            const auto addr = GetRemoteRankAddrs(hcclContext, rankId);
            return reinterpret_cast<GM_ADDR>(addr != nullptr ? addr->windowsOut : 0UL);
        }
    } else {
        if (hcclContext_->multiFlag == 0U) {
            return (GM_ADDR)hcclContext_->windowsOut[rankId];
        } else {
            if (rankId == hcclContext_->rankId) {
                return (GM_ADDR)(hcclContext_->data[rankId].localOutput.addr);
            } else {
                return (GM_ADDR)(hcclContext_->data[rankId].remoteOutput.addr);
            }
        }
    }
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Init(
    GM_ADDR context, __gm__ void* initTiling)
{
    HcclTilingVersion version;
    if (initTiling != nullptr) {
        version = HcclTilingVersion::NEW_TILING_VERSION;
        auto initTilingPtr = static_cast<__gm__ Mc2InitTilingInner*>(initTiling);
        debugMode_ = initTilingPtr->debugMode;
        queueNum_ = initTilingPtr->queueNum;
        devType_ = initTilingPtr->devType;
    } else {
        version = HcclTilingVersion::DEPRECATED_TILING_VERSION;
        devType_ = HCCL_ASCEND910B;
    }

    hcclContext_ = (__gm__ HcclCombineOpParam*)context;
    ASCENDC_HCCL_API_ASSERT(hcclContext_ != nullptr, { return; }, "Init Hccl failed, context addr is nullptr.");
    InitInner(hcclContext_->workSpace, version);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::InitV2(
    GM_ADDR context, const void* initTiling)
{
    ASCENDC_HCCL_API_ASSERT(
        initTiling != nullptr, { return; }, "Call InitV2 failed, ensure initTiling is not nullptr!");
    const Mc2InitTilingInner* initTilingPtr = static_cast<const Mc2InitTilingInner*>(initTiling);
    debugMode_ = initTilingPtr->debugMode;
    queueNum_ = initTilingPtr->queueNum;
    devType_ = initTilingPtr->devType;
    tilingBaseAddr_ = reinterpret_cast<uint64_t>(initTiling);

    hcclContext_ = (__gm__ HcclCombineOpParam*)context;
    ASCENDC_HCCL_API_ASSERT(hcclContext_ != nullptr, { return; }, "Init Hccl failed, context addr is nullptr.");
    InitInner(hcclContext_->workSpace, HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION);
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SetCcTiling(
    __gm__ void* ccOpTilingData)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ == HcclTilingVersion::NEW_TILING_VERSION, { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure Hccl::InitV1 func has been called successfully!");
    ASCENDC_HCCL_API_ASSERT(
        ccOpTilingData != nullptr, { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure ccOpTilingData is not nullptr");
    const uint32_t opType = (static_cast<__gm__ Mc2CcTilingInner*>(ccOpTilingData))->opType;
    ASCENDC_HCCL_API_ASSERT(
        opType < static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL), { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure cmdType is valid");
    KERNEL_LOG(KERNEL_INFO, "CmdType = %d, ccOpTilingData = %lu ", opType, reinterpret_cast<uint64_t>(ccOpTilingData));
    ccOpTilingDataTable_[opType] = reinterpret_cast<uint64_t>(ccOpTilingData);
    return HCCL_SUCCESS;
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SetCcTilingV2(uint64_t offset)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ == HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION, { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure Hccl::InitV2 func has been called successfully!");
    const uint32_t opType = (reinterpret_cast<Mc2CcTilingInner*>(tilingBaseAddr_ + offset))->opType;
    ASCENDC_HCCL_API_ASSERT(
        opType < static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL), { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure cmdType is valid");
    ccOpTilingDataTable_[opType] = offset;
    return HCCL_SUCCESS;
}

template <const auto& config>
__aicore__ inline uint32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetRankDim()
{
    ASCENDC_HCCL_API_ASSERT(
        hcclContext_ != nullptr, { return UINT32_MAX; },
        "Call GetRankDim failed, ensure InitV2 or Init has been called!");
    return hcclContext_->rankNum;
}

template <const auto& config>
template <ScopeType type>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::QueueBarrier(uint16_t queueID)
{
    const uint32_t availableQueueNum = (queueNum_ == 0U) ? 1U : static_cast<uint32_t>(queueNum_);
    const uint64_t queueIndex =
        static_cast<uint64_t>(queueID) + static_cast<uint64_t>(GetBlockIdx()) * static_cast<uint64_t>(queueNum_);
    if ((static_cast<uint32_t>(queueID) >= availableQueueNum) || (queueIndex >= MAX_QUE_NUM)) {
        KERNEL_LOG(
            KERNEL_ERROR, "Invalid queue ID %u, available queue num %u, queue index %llu.",
            static_cast<uint32_t>(queueID), availableQueueNum, static_cast<unsigned long long>(queueIndex));
        return;
    }
    CommonPrepareParam param;
    param.commType.msgType = ControlMsgType::HCCL_CMD_BARRIER;
    SendMsgToServer(queueID, param);
    ++(curMsgPosition_[queueID]);
    ASCENDC_HCCL_API_ASSERT(
        curMsgPosition_[queueID] < HCCL_MSG_CNT, { return; }, "Message amount exceeds the maximum value when barrier.");
    if (workingFlag_) {
        UpdateControlMsgCount(hcclMsgArea_, ControlMsgType::HCCL_CMD_BARRIER);
    }
}

template <const auto& config>
template <bool sync>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Iterate(
    HcclHandle handleId, uint16_t* seqSlices, uint16_t seqSliceLen)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ == HcclTilingVersion::NEW_TILING_VERSION ||
            curVersion_ == HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION,
        { return HCCL_FAILED; }, "Initialization has not been done properly.");
    ASCENDC_HCCL_API_ASSERT(
        seqSlices != nullptr && seqSliceLen != 0U, { return HCCL_FAILED; }, "Invalid param for Iterate.");
    const uint16_t stepSize = GetStepSizeByHandle(handleId);
    const uint16_t stepsPerRepeat = GetStepCntsPerRepeatByHandle(handleId);
    ASCENDC_HCCL_API_ASSERT(
        stepSize > 0U && stepsPerRepeat > 1U, { return HCCL_FAILED; }, "Handle id %d is not for fine-grained.",
        handleId);
    uint16_t& curSlice = handleId2CurrSliceId_[handleId];
    KERNEL_LOG(
        KERNEL_INFO, "The step size for handle %d is %u, current slice and total slices are %u/%u.", handleId, stepSize,
        curSlice, stepsPerRepeat);

    // Only for All2AllV + pairwise
    if (curSlice >= stepsPerRepeat * handleIdRepeat_[handleId]) {
        KERNEL_LOG(KERNEL_INFO, "The step id %u for handle id %d reach the maximum.", handleId, curSlice);
        return 0;
    }
    const uint16_t slicesPerRepeat = stepsPerRepeat;
    const uint32_t rankId = GetRankId();
    const uint32_t rankDim = GetRankDim();
    ASCENDC_HCCL_API_ASSERT(rankDim != 0U, { return HCCL_FAILED; }, "Invalid rank-dim.");
    for (uint16_t i = 0U; i < seqSliceLen; ++i) {
        if constexpr (sync) {
            if ((curSlice + 1) % stepSize == 0) {
                (void)Wait(handleId);
            }
            seqSlices[i] = (rankId + rankDim - curSlice % slicesPerRepeat) % rankDim;
        } else {
            seqSlices[i] = (rankId + curSlice % slicesPerRepeat) % rankDim;
        }
        ++curSlice;
    }
    return seqSliceLen;
}
} // namespace AscendC

#endif
