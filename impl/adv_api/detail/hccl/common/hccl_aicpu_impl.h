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
__aicore__ inline bool HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::CheckAlltoAllVPrepareParamValid(
    const CommonPrepareParam& param, uint32_t& rankNum)
{
    rankNum = GetRankDim();
    ASCENDC_HCCL_API_ASSERT(
        param.paramExt.sendCounts != nullptr && param.paramExt.sdispls != nullptr &&
            param.paramExt.recvCounts != nullptr && param.paramExt.rdispls != nullptr,
        { return false; },
        "Call AlltoAllV failed, "
        "param sendCounts/recvCounts/sdispls/rdispls is nullptr, invalid.");
    const uint32_t rankDim = rankNum;
    ASCENDC_HCCL_API_ASSERT(
        IsValidMsgExtRankNum(rankDim), { return false; }, "Call AlltoAllV failed, rank dim is %u, invalid.", rankDim);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if (param.sendBuf == nullptr) {
        bool hasSendData = false;
        for (uint32_t i = 0U; i < rankDim; ++i) {
            hasSendData = hasSendData || (param.paramExt.sendCounts[i] != 0UL);
        }
        ASCENDC_HCCL_API_ASSERT(
            !hasSendData, { return false; },
            "Call AlltoAllV failed, param sendBuf is nullptr while sendCounts is not all zero, invalid.");
    }
    if (param.recvBuf == nullptr) {
        bool hasRecvData = false;
        for (uint32_t i = 0U; i < rankDim; ++i) {
            hasRecvData = hasRecvData || (param.paramExt.recvCounts[i] != 0UL);
        }
        ASCENDC_HCCL_API_ASSERT(
            !hasRecvData, { return false; },
            "Call AlltoAllV failed, param recvBuf is nullptr while recvCounts is not all zero, invalid.");
    }
#endif
    return true;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendNormalMsgLegacy(
    uint16_t queId, const CommonPrepareParam& param, uint64_t tiling, bool compactExt, uint64_t extCtrl,
    uint64_t extXor, bool normalSlotChecked)
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
    (void)compactExt;
    (void)extCtrl;
    (void)extXor;
    (void)normalSlotChecked;
    AssembleHcclMsgV2(param, curVersion_, curHandleId_, tiling, hcclSendMsg, &hcclMsgArea_->controlMsg);
    FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg));
}

template <const auto& config>
template <HcclCMDType cmdType>
__aicore__ inline bool HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::CheckCommonPrepareParamValid(
    const CommonPrepareParam& param, uint64_t& tiling, uint32_t& rankNum)
{
    tiling = 0UL;
    rankNum = 0U;
    if constexpr (cmdType < HcclCMDType::HCCL_CMD_ALL) {
        if (curVersion_ == HcclTilingVersion::CONTEXT_DECOUPLE_VERSION) {
            tiling = ccOpParamTable_[static_cast<uint32_t>(cmdType)];
        } else {
            tiling = ccOpTilingDataTable_[static_cast<uint32_t>(cmdType)];
        }
    }
    if (curVersion_ == HcclTilingVersion::NEW_TILING_VERSION ||
        curVersion_ == HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION ||
        curVersion_ == HcclTilingVersion::CONTEXT_DECOUPLE_VERSION) {
        ASCENDC_HCCL_API_ASSERT(
            tiling != 0UL, { return false; }, "Failed to prepare for type %u, ensure SetCcTiling has been called.",
            static_cast<uint32_t>(cmdType));
    } else {
        ASCENDC_HCCL_API_ASSERT(
            curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION && tiling == 0UL, { return false; },
            "Failed to prepare for type %u, ensure Init has been called", static_cast<uint32_t>(cmdType));
    }
    ASCENDC_HCCL_API_ASSERT(
#if __NPU_ARCH__ == 3510
        param.dataType >= HCCL_DATA_TYPE_INT8 && param.dataType < HCCL_DATA_TYPE_RESERVED,
#else
        cmdType == HcclCMDType::HCCL_CMD_BATCH_WRITE ||
            (param.dataType >= HCCL_DATA_TYPE_INT8 && param.dataType < HCCL_DATA_TYPE_RESERVED),
#endif
        { return false; }, "Call Prepare[%d] failed, param HcclDataType is %d, invalid.", static_cast<int32_t>(cmdType),
        static_cast<int32_t>(param.dataType));
    if constexpr (cmdType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        return CheckAlltoAllVPrepareParamValid(param, rankNum);
    } else {
        ASCENDC_HCCL_API_ASSERT(
            param.sendBuf != nullptr && param.recvBuf != nullptr, { return false; },
            "Call Prepare[%d] failed, the param sendBuf/recvBuf is nullptr, "
            "which is an invalid parameter.",
            static_cast<int32_t>(cmdType));
        ASCENDC_HCCL_API_ASSERT(
            param.count != 0, { return false; }, "Call Prepare[%d] failed, param sendCount/recvCount is 0, invalid.",
            static_cast<int32_t>(cmdType));
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

    ASCENDC_HCCL_API_ASSERT(msgAddr != 0UL, { return; }, "Init Hccl failed, workspace addr is nullptr.");
#if __NPU_ARCH__ != 3510
    // ensure hcclMsgArea 512B aligned
    if (msgAddr & 0x1ff) {
        msgAddr = (msgAddr & (~((uint64_t)0x1ff))) + 0x200;
    }
#endif
    hcclMsgArea_ = reinterpret_cast<__gm__ HcclMsgArea*>(msgAddr);
#if __NPU_ARCH__ == 3510
    // A5 keeps this table 64B-aligned so Init can use eight wide stores.
    static_assert(
        sizeof(handleIdMsgPosition_) == sizeof(uint64_t) * 8U,
        "A5 handle position table must occupy eight 64-bit words");
    uint64_t* msgPos64 = reinterpret_cast<uint64_t*>(handleIdMsgPosition_);
    for (uint32_t i = 0U; i < sizeof(handleIdMsgPosition_) / sizeof(uint64_t); ++i) {
        msgPos64[i] = UINT64_MAX;
    }
#else
    for (uint32_t i = 0U; i < HCCL_MAX_HANDLE_ID; ++i) {
        handleIdMsgPosition_[i] = -1;
    }
#endif
    InitWorkingFlag();
    curVersion_ = version;
}

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendNormalMsgA5(
    const CommonPrepareParam& param, HcclHandle handle, uint64_t tiling, bool compactExt, uint64_t extCtrl,
    uint64_t extXor, bool normalSlotChecked)
{
    if (!workingFlag_) {
        return;
    }
    __gm__ HcclMsg* hcclSendMsg = hcclMsgArea_->commMsg.singleMsg.sendMsgs + curMsgPosition_[0U];
    if (!normalSlotChecked) {
        do {
            HCCL_CHECK_RESTART(hcclMsgArea_, return);
            FlushDataCache(hcclSendMsg);
        } while (debugMode_ != HCCL_ONLY_COMPUTE && hcclSendMsg->addMsg.v0Msg.valid == HCCL_MSG_VALID_MASK);
    }
    if (!compactExt) {
        AssembleHcclMsgV2(
            param, HcclTilingVersion::CONTEXT_DECOUPLE_VERSION, handle, tiling, hcclSendMsg, &hcclMsgArea_->controlMsg);
    } else {
        AssembleHcclMsgV2Compact(
            param, HcclTilingVersion::CONTEXT_DECOUPLE_VERSION, handle, tiling, hcclSendMsg, extCtrl, extXor);
    }
    FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg));
}
#endif

template <const auto& config>
template <HcclCMDType cmdType>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendMsgToServer(
    uint16_t queId, const CommonPrepareParam& param, int8_t srcGroupID, HcclHandle srcHandleID, uint64_t tiling)
{
#if __NPU_ARCH__ == 3510
    (void)queId;
    (void)srcGroupID;
    (void)srcHandleID;
    if (!workingFlag_) {
        return;
    }
    __gm__ HcclMsg* hcclSendMsg = hcclMsgArea_->commMsg.singleMsg.sendMsgs + curMsgPosition_[0U];
    do {
        HCCL_CHECK_RESTART(hcclMsgArea_, return);
        FlushDataCache(hcclSendMsg);
    } while (debugMode_ != HCCL_ONLY_COMPUTE && hcclSendMsg->addMsg.v0Msg.valid == HCCL_MSG_VALID_MASK);
    KERNEL_LOG(KERNEL_INFO, "Hccl send msg[%u] is available now.", curMsgPosition_[0U]);
    AssembleHcclMsgV2(param, curVersion_, curHandleId_, tiling, hcclSendMsg, &hcclMsgArea_->controlMsg);
    FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg));
#else
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
#if __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 9202
        AssembleHcclMsgV2(param, curVersion_, curHandleId_, tiling, hcclSendMsg, &hcclMsgArea_->controlMsg);
#else
        AssembleHcclMsg(param, curVersion_, curHandleId_, tiling, hcclSendMsg, &hcclMsgArea_->controlMsg);
#endif
    } else {
        AssembleHcclMsg(param, srcGroupID, srcHandleID, hcclSendMsg);
    }
    FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg));
#endif
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendExtMsgA5(
    const AlltoAllVParamExt& param, uint32_t rankNum, uint64_t& extCtrl, uint64_t& extXor, bool& compactExt,
    bool& normalSlotChecked)
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    if (!workingFlag_) {
        return;
    }
    __gm__ HcclMsgExt* hcclSendMsg = &(hcclMsgArea_->commMsg.singleMsg.paramExtMsgList[curMsgPosition_[0U]]);
    __gm__ HcclMsg* hcclNormalMsg = hcclMsgArea_->commMsg.singleMsg.sendMsgs + curMsgPosition_[0U];
    do {
        HCCL_CHECK_RESTART(hcclMsgArea_, return);
        AscendC::Barrier();
        __asm__("NOP");
        dcci(
            reinterpret_cast<__gm__ void*>(&hcclSendMsg->valid), static_cast<uint64_t>(cache_line_t::SINGLE_CACHE_LINE),
            static_cast<uint64_t>(dcci_dst_t::CACHELINE_OUT));
        dcci(
            reinterpret_cast<__gm__ void*>(hcclNormalMsg), static_cast<uint64_t>(cache_line_t::SINGLE_CACHE_LINE),
            static_cast<uint64_t>(dcci_dst_t::CACHELINE_OUT));
    } while ((debugMode_ != HCCL_ONLY_COMPUTE) && (hcclSendMsg->valid == HCCL_MSG_VALID_MASK));
    if (debugMode_ != HCCL_ONLY_COMPUTE && hcclNormalMsg->addMsg.v0Msg.valid == HCCL_MSG_VALID_MASK) {
        do {
            HCCL_CHECK_RESTART(hcclMsgArea_, return);
            FlushDataCache(hcclNormalMsg);
        } while (hcclNormalMsg->addMsg.v0Msg.valid == HCCL_MSG_VALID_MASK);
    }
    normalSlotChecked = true;
    KERNEL_LOG(KERNEL_INFO, "Hccl send extMsg[%u] is available now.", curMsgPosition_[0U]);
    compactExt = (rankNum <= HCCL_MAX_RANK_NUM_V2);
    if (compactExt) {
        uint32_t xorCheck = HCCL_MSG_EXT_COMPACT_CTRL;
        constexpr uint64_t compactUpper32Mask = 0xFFFFFFFF00000000ULL;
        __gm__ uint64_t* const dstSend = reinterpret_cast<__gm__ uint64_t*>(hcclSendMsg->sendCounts);
        __gm__ uint64_t* const dstRecv = reinterpret_cast<__gm__ uint64_t*>(hcclSendMsg->recvCounts);
        for (uint32_t i = 0U; i < rankNum; ++i) {
            const uint64_t sendCount = param.sendCounts[i];
            const uint64_t sendOffset = param.sdispls[i];
            const uint64_t recvCount = param.recvCounts[i];
            const uint64_t recvOffset = param.rdispls[i];
            if (((sendCount | sendOffset | recvCount | recvOffset) & compactUpper32Mask) != 0U) {
                compactExt = false;
                break;
            }
            const uint64_t sendValue = (sendCount << 32U) | sendOffset;
            const uint64_t recvValue = (recvCount << 32U) | recvOffset;
            dstSend[i] = sendValue;
            dstRecv[i] = recvValue;
            xorCheck ^= static_cast<uint32_t>(sendValue >> 32U);
            xorCheck ^= static_cast<uint32_t>(sendValue);
            xorCheck ^= static_cast<uint32_t>(recvValue >> 32U);
            xorCheck ^= static_cast<uint32_t>(recvValue);
        }
        if (compactExt) {
            extCtrl = HCCL_MSG_EXT_COMPACT_CTRL;
            extXor = xorCheck;
        }
    }
    if (!compactExt) {
        AssembleHcclMsgExt(param, rankNum, hcclSendMsg);
    }
    if (compactExt) {
        // The compact representation stores packed send/recv words in the first two
        // array regions. Publish only the cachelines that contain those words.
        AscendC::Barrier();
        __asm__("NOP");
        for (uint32_t i = 0U; i < rankNum; i += MAX_DCCI_CNT / sizeof(uint64_t)) {
            dcci(
                reinterpret_cast<__gm__ void*>(hcclSendMsg->sendCounts + i),
                static_cast<uint64_t>(cache_line_t::SINGLE_CACHE_LINE),
                static_cast<uint64_t>(dcci_dst_t::CACHELINE_OUT));
            dcci(
                reinterpret_cast<__gm__ void*>(hcclSendMsg->recvCounts + i),
                static_cast<uint64_t>(cache_line_t::SINGLE_CACHE_LINE),
                static_cast<uint64_t>(dcci_dst_t::CACHELINE_OUT));
        }
        DataSyncBarrier<MemDsbT::ALL>();
        return;
    }
    for (uint32_t i = 0U; i < rankNum; i += MAX_DCCI_CNT / sizeof(uint64_t)) {
        FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->sendCounts + i));
        FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->sendOffset + i));
        FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->recvCounts + i));
        FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->recvOffset + i));
    }
    FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->reserved));
#else
    (void)compactExt;
    (void)extCtrl;
    (void)extXor;
    SendExtMsgLegacy(param, rankNum);
#endif
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendExtMsgLegacy(
    const AlltoAllVParamExt& param, uint32_t rankNum)
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
    AssembleHcclMsgExt(param, rankNum, hcclSendMsg);
    for (uint32_t i = 0U; i < rankNum; i += MAX_DCCI_CNT / sizeof(uint64_t)) {
        FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->sendCounts + i));
        FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->sendOffset + i));
        FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->recvCounts + i));
        FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->recvOffset + i));
    }
    FlushDataCache(reinterpret_cast<__gm__ void*>(hcclSendMsg->reserved));
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
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    if (unlikely(!workingFlag_)) {
        return;
    }
#else
    if (queueNum_ != 0U || !workingFlag_) {
        return;
    }
#endif
    __gm__ TurnCnt* commitGM = hcclMsgArea_->commMsg.singleMsg.commitTurnCnt + msgPos;
    __gm__ uint64_t* const cntAddr = reinterpret_cast<__gm__ uint64_t*>(&commitGM->cnt);
    do {
        HCCL_CHECK_RESTART(hcclMsgArea_, return);
    } while ((debugMode_ != HCCL_ONLY_COMPUTE) &&
             (ld_dev(cntAddr, 0) >= static_cast<uint64_t>(handleIdCommitTurnCnt_[handleId])));
    KERNEL_LOG(
        KERNEL_INFO, "Block idx[%d] write commit turn cnt[%u].", DEFAULT_CFG.blockId, handleIdCommitTurnCnt_[handleId]);
    st_dev(static_cast<uint64_t>(handleIdCommitTurnCnt_[handleId]), cntAddr, 0);
    DataSyncBarrier<MemDsbT::ALL>();
    if (workingFlag_) {
#if !(defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510)
        __gm__ TurnCnt* apiInfo = &(hcclMsgArea_->apiStats.commitStats[handleId2CmdType_[handleId]]);
        FlushDataCache(apiInfo);
        apiInfo->cnt += turnCnt;
        FlushDataCache(apiInfo);
#endif
    }
}

template <const auto& config>
__aicore__ inline uint64_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::WaitFinishCntFromGm(
    uint8_t msgPos, uint64_t expectedCnt, HcclHandle handleId)
{
    __gm__ TurnCnt* finishGM = hcclMsgArea_->commMsg.singleMsg.finishedTurnCnt + msgPos;
#if !defined(UT_TEST)
    while (true) {
        HCCL_CHECK_RESTART(hcclMsgArea_, return finishGM->cnt);
        FlushDataCacheAicpu(finishGM);
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        if ((debugMode_ == HCCL_ONLY_COMPUTE) || (finishGM->cnt >= expectedCnt)) {
            handleIdFinishedCnt_[handleId] = finishGM->cnt;
            break;
        }
#else
        if ((debugMode_ == HCCL_ONLY_COMPUTE) || (finishGM->cnt >= expectedCnt)) {
            break;
        }
#endif
    }
#endif
    return finishGM->cnt;
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AlltoAllVPrepareImpl(
    const CommonPrepareParam& param)
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    HCCL_CHECK_RESTART(hcclMsgArea_, return INVALID_HANDLE_ID);
    ASCENDC_HCCL_API_ASSERT(
        param.repeat != 0U, { return INVALID_HANDLE_ID; }, "Call Prepare[%d] failed, param repeat is 0, invalid.",
        static_cast<int32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV));
    ASCENDC_HCCL_API_ASSERT(
        GetHcclDataTypeSize(param.dataType) != 0U, { return INVALID_HANDLE_ID; },
        "Call Prepare[%d] failed, param HcclDataType is %d, invalid.",
        static_cast<int32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV), static_cast<int32_t>(param.dataType));

    uint64_t tiling = 0UL;
    uint32_t rankNum = 0U;
    const bool paramValid = CheckCommonPrepareParamValid<HcclCMDType::HCCL_CMD_ALLTOALLV>(param, tiling, rankNum);
    ASCENDC_HCCL_API_ASSERT(
        paramValid, { return INVALID_HANDLE_ID; }, "Call Prepare[%d] failed, param invalid.",
        static_cast<int32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV));

    bool compactExt = false;
    uint64_t extCtrl = 0UL;
    uint64_t extXor = 0UL;
    bool normalSlotChecked = false;

    HcclHandle handleId = ++curHandleId_;
    ASCENDC_HCCL_API_ASSERT(
        handleId < HCCL_MAX_HANDLE_ID, { return INVALID_HANDLE_ID; },
        "Call Prepare[%d] failed, Prepare interface call num is[%d], expected no more than[%d].",
        static_cast<int32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV), handleId + 1, HCCL_MAX_HANDLE_ID);

    SendExtMsgA5(param.paramExt, rankNum, extCtrl, extXor, compactExt, normalSlotChecked);
    SendNormalMsgA5(param, handleId, tiling, compactExt, extCtrl, extXor, normalSlotChecked);
    handleIdMsgPosition_[handleId] = curMsgPosition_[0U];
    handleIdRepeat_[handleId] = param.repeat;
    handleId2CmdType_[handleId] = static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLTOALLV);
    if constexpr (commit) {
        SetCommitTurnCntToGm(curMsgPosition_[0U], param.repeat * GetStepCntsPerRepeatByHandle(handleId), handleId);
    }
    ++curMsgPosition_[0U];
    ASCENDC_HCCL_API_ASSERT(
        curMsgPosition_[0U] < HCCL_MSG_CNT, { return INVALID_HANDLE_ID; },
        "Message amount exceeds the maximum value when prepare.");
    return handleId;
#else
    HCCL_CHECK_RESTART(hcclMsgArea_, return INVALID_HANDLE_ID);
    ASCENDC_HCCL_API_ASSERT(
        param.repeat != 0U, { return INVALID_HANDLE_ID; }, "Call Prepare[%d] failed, param repeat is 0, invalid.",
        static_cast<int32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV));
    ASCENDC_HCCL_API_ASSERT(
        GetHcclDataTypeSize(param.dataType) != 0U, { return INVALID_HANDLE_ID; },
        "Call Prepare[%d] failed, param HcclDataType is %d, invalid.",
        static_cast<int32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV), static_cast<int32_t>(param.dataType));

    uint64_t tiling = 0UL;
    uint32_t rankNum = 0U;
    const bool paramValid = CheckCommonPrepareParamValid<HcclCMDType::HCCL_CMD_ALLTOALLV>(param, tiling, rankNum);
    ASCENDC_HCCL_API_ASSERT(
        paramValid, { return INVALID_HANDLE_ID; }, "Call Prepare[%d] failed, param invalid.",
        static_cast<int32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV));

    HcclHandle handleId = ++curHandleId_;
    ASCENDC_HCCL_API_ASSERT(
        handleId < HCCL_MAX_HANDLE_ID, { return INVALID_HANDLE_ID; },
        "Call Prepare[%d] failed, Prepare interface call num is[%d], expected no more than[%d].",
        static_cast<int32_t>(HcclCMDType::HCCL_CMD_ALLTOALLV), handleId + 1, HCCL_MAX_HANDLE_ID);

    bool compactExt = false;
    uint64_t extCtrl = 0UL;
    uint64_t extXor = 0UL;
    bool normalSlotChecked = false;
    SendExtMsgLegacy(param.paramExt, rankNum);
    const uint16_t queId = (queueNum_ == 0U ? 0U : static_cast<uint16_t>(param.dataType));
    SendNormalMsgLegacy(queId, param, tiling, compactExt, extCtrl, extXor, normalSlotChecked);
    handleIdMsgPosition_[handleId] = curMsgPosition_[queId];
    handleIdRepeat_[handleId] = param.repeat;
    handleId2CmdType_[handleId] = static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLTOALLV);
    if constexpr (commit) {
        SetCommitTurnCntToGm(curMsgPosition_[queId], param.repeat * GetStepCntsPerRepeatByHandle(handleId), handleId);
    }
    ++curMsgPosition_[queId];
    ASCENDC_HCCL_API_ASSERT(
        curMsgPosition_[queId] < HCCL_MSG_CNT, { return INVALID_HANDLE_ID; },
        "Message amount exceeds the maximum value when prepare.");
    return handleId;
#endif
}

template <const auto& config>
template <bool commit, HcclCMDType cmdType>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::CommonPrepareImpl(
    const CommonPrepareParam& param)
{
    HCCL_CHECK_RESTART(hcclMsgArea_, return INVALID_HANDLE_ID);
    ASCENDC_HCCL_API_ASSERT(
        param.repeat != 0U, { return INVALID_HANDLE_ID; }, "Call Prepare[%d] failed, param repeat is 0, invalid.",
        static_cast<int32_t>(cmdType));
#if __NPU_ARCH__ != 3510
    if constexpr (cmdType != HcclCMDType::HCCL_CMD_BATCH_WRITE) {
#endif
        ASCENDC_HCCL_API_ASSERT(
            GetHcclDataTypeSize(param.dataType) != 0U, { return INVALID_HANDLE_ID; },
            "Call Prepare[%d] failed, param HcclDataType is %d, invalid.", static_cast<int32_t>(cmdType),
            static_cast<int32_t>(param.dataType));
#if __NPU_ARCH__ != 3510
    }
#endif
    uint64_t tiling = 0UL;
    uint32_t rankNum = 0U;
    const bool paramValid = CheckCommonPrepareParamValid<cmdType>(param, tiling, rankNum);
    ASCENDC_HCCL_API_ASSERT(
        paramValid, { return INVALID_HANDLE_ID; }, "Call Prepare[%d] failed, param invalid.",
        static_cast<int32_t>(cmdType));

    HcclHandle handleId = ++curHandleId_;
    ASCENDC_HCCL_API_ASSERT(
        handleId < HCCL_MAX_HANDLE_ID, { return INVALID_HANDLE_ID; },
        "Call Prepare[%d] failed, Prepare interface call num is[%d], expected no more than[%d].",
        static_cast<int32_t>(cmdType), handleId + 1, HCCL_MAX_HANDLE_ID);
#if __NPU_ARCH__ == 3510
    constexpr uint16_t queId = 0U;
#else
    const uint16_t queId = (queueNum_ == 0U ? 0U : static_cast<uint16_t>(param.dataType));
#endif
    SendMsgToServer<cmdType>(queId, param, -1, INVALID_HANDLE_ID, tiling);
    handleIdMsgPosition_[handleId] = curMsgPosition_[queId];
    handleIdRepeat_[handleId] = param.repeat;
    handleId2CmdType_[handleId] = static_cast<uint8_t>(cmdType);
    if constexpr (commit) {
        SetCommitTurnCntToGm(curMsgPosition_[queId], param.repeat, handleId);
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
    for (uint32_t i = 0U; i <= curMsgPosition_[0U]; ++i) {
        __gm__ TurnCnt* finishGM = finishArea + i;
        finishGM->cnt = 0;
        FlushDataCache(finishGM);
    }
}

template <const auto& config>
template <bool sync>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SendFinalizeMsg()
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    constexpr uint16_t totalQueNum = 1U;
#else
    const uint16_t totalQueNum = (queueNum_ == 0U ? 1U : queueNum_);
#endif
    CommonPrepareParam param;
    param.commType.msgType = ControlMsgType::HCCL_CMD_FINALIZE;
    for (uint16_t idx = 0U; idx < totalQueNum; ++idx) {
        KERNEL_LOG(
            KERNEL_INFO, "Only block idx[%ld] write sendMsgList[%u] when Finalize.", GetBlockIdx(),
            curMsgPosition_[idx]);
        SendMsgToServer<HcclCMDType::HCCL_CMD_INVALID>(idx, param, -1, INVALID_HANDLE_ID, 0UL);
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

    return CommonPrepareImpl<commit, HcclCMDType::HCCL_CMD_ALLREDUCE>(
        {HcclCMDType::HCCL_CMD_ALLREDUCE, sendBuf, recvBuf, count, dataType, dataType, op, 0, repeat});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AllGather(
    GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t sendCount, HcclDataType dataType, uint64_t strideCount, uint8_t repeat)
{
    return CommonPrepareImpl<commit, HcclCMDType::HCCL_CMD_ALLGATHER>(
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
    return CommonPrepareImpl<commit, HcclCMDType::HCCL_CMD_REDUCE_SCATTER>(
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, sendBuf, recvBuf, recvCount, dataType, dataType, op, strideCount,
         repeat});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AlltoAll(
    GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t dataCount, HcclDataType dataType, uint64_t strideCount, uint8_t repeat)
{
    return CommonPrepareImpl<commit, HcclCMDType::HCCL_CMD_ALLTOALL>(
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
    return AlltoAllVPrepareImpl<commit>(
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
#if !defined(__NPU_ARCH__) || __NPU_ARCH__ != 3510
    if (queueNum_ != 0U) {
        return 0;
    }
#endif
    int8_t curMsgPos = handleIdMsgPosition_[handleId];
    ASCENDC_HCCL_API_ASSERT(
        curMsgPos >= 0, { return HCCL_FAILED; }, "Call Query failed, handleId[%d] was not got by Prepare interface.",
        handleId);
    return WaitFinishCntFromGm(curMsgPos, 0UL, handleId);
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Wait(HcclHandle handleId)
{
    HCCL_CHECK_RESTART(hcclMsgArea_, return HCCL_FAILED);
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION, { return HCCL_FAILED; },
        "Call Wait failed, ensure Hccl::Init func has been called successfully!");
#if !defined(__NPU_ARCH__) || __NPU_ARCH__ != 3510
    if (queueNum_ != 0U) {
        return HCCL_SUCCESS;
    }
#endif
    ASCENDC_HCCL_API_ASSERT(
        handleId > INVALID_HANDLE_ID && handleId < HCCL_MAX_HANDLE_ID, { return HCCL_FAILED; },
        "Failed to wait, handleId is[%d], expected to be in range of [0, %d).", handleId, HCCL_MAX_HANDLE_ID);
    uint16_t& waitCnt = handleIdWaitCallNum_[handleId];
    ASCENDC_HCCL_API_ASSERT(
        waitCnt < handleIdCommitTurnCnt_[handleId], { return HCCL_FAILED; },
        "Failed to wait, call num of Wait for handleId[%d] is[%u], expected to be no larger than Commit num[%u].",
        handleId, waitCnt + 1, handleIdCommitTurnCnt_[handleId]);
#if !defined(__NPU_ARCH__) || __NPU_ARCH__ != 3510
    if (workingFlag_) {
        __gm__ TurnCnt* apiInfo = &(hcclMsgArea_->apiStats.waitStats[handleId2CmdType_[handleId]]);
        FlushDataCache(apiInfo);
        ++(apiInfo->cnt);
        FlushDataCache(apiInfo);
    }
#endif
    int8_t curMsgPos = handleIdMsgPosition_[handleId];
    ASCENDC_HCCL_API_ASSERT(
        curMsgPos >= 0, { return HCCL_FAILED; }, "Call Wait failed, handleId[%d] was not got by Prepare interface.",
        handleId);
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    waitCnt += 1U;
    if (handleIdFinishedCnt_[handleId] >= waitCnt) {
        return HCCL_SUCCESS;
    }
#else
    const uint16_t stepSize = GetStepSizeByHandle(handleId);
    waitCnt += (stepSize == 0U ? 1U : stepSize);
#endif
    (void)WaitFinishCntFromGm(curMsgPos, waitCnt, handleId);
    return HCCL_SUCCESS;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Commit(HcclHandle handleId)
{
    HCCL_CHECK_RESTART(hcclMsgArea_, return);
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION, { return; },
        "Call Commit failed, ensure Hccl::Init func has been called successfully!");
    ASCENDC_HCCL_API_ASSERT(
        handleId > INVALID_HANDLE_ID && handleId < HCCL_MAX_HANDLE_ID, { return; },
        "Call Commit failed, handleId is[%d], expected in range of [0, %d).", handleId, HCCL_MAX_HANDLE_ID);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    const uint16_t commitCnt = handleIdCommitTurnCnt_[handleId];
    const uint16_t taskNum = handleIdRepeat_[handleId] * GetStepCntsPerRepeatByHandle(handleId);
    ASCENDC_HCCL_API_ASSERT(
        commitCnt < taskNum, { return; },
        "Call Commit for handleId[%d] failed, call num is[%u], expected no larger than task num[%u].", handleId,
        commitCnt + 1, taskNum);
#endif
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    SetCommitTurnCntToGm(handleIdMsgPosition_[handleId], 1U, handleId);
#else
    const uint16_t stepSize = GetStepSizeByHandle(handleId);
    SetCommitTurnCntToGm(handleIdMsgPosition_[handleId], (stepSize == 0U ? 1U : stepSize), handleId);
#endif
}

template <const auto& config>
template <bool sync>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Finalize()
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ != HcclTilingVersion::INVALID_TILING_VERSION, { return; },
        "Call Finalize failed, ensure Hccl::Init func has been called successfully!");
    HCCL_CHECK_RESTART(hcclMsgArea_, return);

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    if (!workingFlag_) {
#else
    if (!workingFlag_ && queueNum_ == 0U) {
#endif
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
#if !defined(__NPU_ARCH__) || __NPU_ARCH__ != 3510
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
#endif
    }
}
} // namespace AscendC

#endif
