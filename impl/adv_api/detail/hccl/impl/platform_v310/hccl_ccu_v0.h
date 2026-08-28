/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hccl_ccu_d100.h
 * \brief
 */
#ifndef IMPL_V310_HCCL_CCU_D100_H
#define IMPL_V310_HCCL_CCU_D100_H

#include "../../common/hccl_utils.h"
#include "../../ccu/hccl_ccu_v0_prepare.h"

namespace AscendC {
template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::AllReduce(
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
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::AllGather(
    GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t sendCount, HcclDataType dataType, uint64_t strideCount, uint8_t repeat)
{
    return CommonPrepareImpl<commit>(
        {HcclCMDType::HCCL_CMD_ALLGATHER, sendBuf, recvBuf, sendCount, dataType, dataType, HCCL_REDUCE_RESERVED,
         strideCount, repeat});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::ReduceScatter(
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
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::AlltoAll(
    GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t dataCount, HcclDataType dataType, uint64_t strideCount, uint8_t repeat)
{
    return CommonPrepareImpl<commit>(
        {HcclCMDType::HCCL_CMD_ALLTOALL, sendBuf, recvBuf, dataCount, dataType, dataType, HCCL_REDUCE_RESERVED,
         strideCount, repeat});
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::AlltoAllV(
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
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::AlltoAllvWrite(
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
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::InitWorkingFlag()
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
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::InitInner(
    GM_ADDR context, HcclTilingVersion version)
{
    isInited_ = false;
    ASCENDC_HCCL_API_ASSERT(context != nullptr, { return; }, "Init Hccl failed, context addr is nullptr.");
    // 只有 alltoall 且 ccu的会走到这里，并设为true
    if (newCcuFlag_) {
        hcclNewContext_ = (__gm__ HcclApi::OpResCtx*)context;
    } else {
        hcclContext_ = (__gm__ HcclCombineOpParam*)context;
    }

    constexpr uint64_t workspaceAlignMask = 0x1ffU;
    constexpr uint64_t layoutSize =
        MAX_DCCI_CNT + sizeof(CommonPrepareParamCcu) * static_cast<uint64_t>(HCCL_MAX_HANDLE_ID) +
        sizeof(AlltoAllVParamCcu) * static_cast<uint64_t>(HCCL_MAX_HANDLE_ID) + CCU_MSG_EXT_MAX_OFFSET;
    static_assert(
        sizeof(CommonPrepareParamCcu) % ALIGN_64_BYTE == 0U && sizeof(AlltoAllVParamCcu) % ALIGN_64_BYTE == 0U,
        "CCU workspace layout is unaligned");

    const uint64_t workspaceAddr = newCcuFlag_ ? hcclNewContext_->workspace : hcclContext_->workSpace;
    const uint64_t workspaceSize = newCcuFlag_ ? hcclNewContext_->workspaceSize : hcclContext_->workSpaceSize;
    const uint64_t alignPadding = (workspaceAlignMask + 1U - (workspaceAddr & workspaceAlignMask)) & workspaceAlignMask;
    const uint64_t requiredSize = alignPadding + layoutSize;
    if (unlikely(workspaceAddr > UINT64_MAX - alignPadding || workspaceSize < requiredSize)) {
        return;
    }
    const uint64_t msgAddr = workspaceAddr + alignPadding;
    KERNEL_LOG(
        KERNEL_INFO, "ApiClient InitInner msgAddr:0x%llx, workSpaceSize:0x%llx",
        static_cast<unsigned long long>(msgAddr), static_cast<unsigned long long>(workspaceSize));

    uint64_t xnAddr = newCcuFlag_ ? hcclNewContext_->xnAddr : reinterpret_cast<uint64_t>(hcclContext_->xnOffset);
    uint64_t ckeAddr = newCcuFlag_ ? hcclNewContext_->ckeAddr : reinterpret_cast<uint64_t>(hcclContext_->ckeOffset);
#ifndef ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT((reinterpret_cast<uintptr_t>(xnAddr) % ALIGN_64_BYTE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "xnAddr is not 64-byte aligned!");
    });
#endif

    finishCntGM_ = reinterpret_cast<__gm__ uint64_t*>(msgAddr);
    uint64_t finishCntGMOffset = msgAddr + MAX_DCCI_CNT;
    handleParamGM_ = reinterpret_cast<__gm__ CommonPrepareParamCcu*>(finishCntGMOffset);
    uint64_t handleParamGMOffset = finishCntGMOffset + (uint64_t)sizeof(CommonPrepareParamCcu) * HCCL_MAX_HANDLE_ID;
    allToAllVParam_ = reinterpret_cast<__gm__ AlltoAllVParamCcu*>(handleParamGMOffset);
    uint64_t allToAllVParamGMOffset = handleParamGMOffset + (uint64_t)sizeof(AlltoAllVParamCcu) * HCCL_MAX_HANDLE_ID;
    ccuParam_.ccuMsgExt = reinterpret_cast<__gm__ CCUMsgExt*>(allToAllVParamGMOffset);

    InitWorkingFlag();
    if (workingFlag_) {
        WriteHBMData(finishCntGM_, uint64_t(0));
    }
    curVersion_ = version;
    curHandleId_ = 0;
    finishNum_ = 0;
    finishNumTemp_ = 0;

    globalCurWaitId_ = 0;
    globalCurResId_ = 0;

    for (uint8_t i = 0; i < CCU_MAX_MSG_NUM; ++i) {
        msgQueueIsAvailable_[i] = true;
    }

    isInited_ = true;
    KERNEL_LOG(
        KERNEL_INFO, "ApiClient InitInner rankId:%d, rankNum:%d, xnAddr:0x%llx, ckeAddr:0x%llx, ccuMsgExt:0x%llx/n",
        newCcuFlag_ ? hcclNewContext_->rankId : hcclContext_->rankId,
        newCcuFlag_ ? hcclNewContext_->rankSize : hcclContext_->rankNum, xnAddr, ckeAddr,
        reinterpret_cast<uint64_t>(ccuParam_.ccuMsgExt));
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::Init(
    GM_ADDR context, __gm__ void* initTiling)
{
    HcclTilingVersion version =
        (initTiling != nullptr ? HcclTilingVersion::NEW_TILING_VERSION : HcclTilingVersion::DEPRECATED_TILING_VERSION);
    InitInner(context, version);
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::InitV2(
    GM_ADDR context, const void* initTiling)
{
    HcclTilingVersion version =
        (initTiling != nullptr ? HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION :
                                 HcclTilingVersion::DEPRECATED_TILING_VERSION);
    if (initTiling != nullptr) {
        const Mc2InitTilingInner* tilingInner = static_cast<const Mc2InitTilingInner*>(initTiling);
        if (tilingInner->version == INIT_TILING_CCU_NEW_VERSION) {
            newCcuFlag_ = true;
        }
    }
    InitInner(context, version);
    tilingBaseAddr_ = reinterpret_cast<uint64_t>(initTiling);
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::SetCcTiling(
    __gm__ void* ccOpTilingData)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ == HcclTilingVersion::NEW_TILING_VERSION, { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure Hccl::InitV1 func has been called successfully!");
    ASCENDC_HCCL_API_ASSERT(
        ccOpTilingData != nullptr, { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure ccOpTilingData is not nullptr");
    const uint32_t opType = (static_cast<__gm__ Mc2CcTilingInner*>(ccOpTilingData))->opType;
    if (unlikely(opType >= static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL))) {
        return HCCL_FAILED;
    }
    KERNEL_LOG(KERNEL_INFO, "CmdType = %d, ccOpTilingData = %lu ", opType, reinterpret_cast<uint64_t>(ccOpTilingData));
    ccOpTilingDataTable_[opType] = reinterpret_cast<uint64_t>(ccOpTilingData);
    return HCCL_SUCCESS;
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::SetCcTilingV2(uint64_t offset)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ == HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION, { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure Hccl::InitV2 func has been called successfully!");
    const uint32_t opType = (reinterpret_cast<Mc2CcTilingInner*>(tilingBaseAddr_ + offset))->opType;
    if (unlikely(opType >= static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL))) {
        return HCCL_FAILED;
    }
    ccOpTilingDataTable_[opType] = offset;
    return HCCL_SUCCESS;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuPrepareForOp(
    const HcclHandle handleId)
{
    ccuUsedXnNum_ = 8; // 算法默认使用的xn num
    FlushDataCache(&handleParamGM_[handleId]);
    if (handleParamGM_[handleId].commType.prepareType == HcclCMDType::HCCL_CMD_ALLGATHER) {
        if (GetKfcMissionNum(handleId) == KFC_MAX_MISSION_NUM) {
            ccuUsedXnNum_ = KFC_CONCURRENT_AG_PARAM_NUM;
            CcuPrepareForConcurrentAllGatherM2M(&handleParamGM_[handleId]);
        } else {
            ccuUsedXnNum_ = 10;
            CcuPrepareForAllGatherM2M(&handleParamGM_[handleId]);
        }
    } else if (handleParamGM_[handleId].commType.prepareType == HcclCMDType::HCCL_CMD_ALLREDUCE) {
        ccuUsedXnNum_ = 18;
        CcuPrepareForAllReduceM2M(&handleParamGM_[handleId]);
    } else if (handleParamGM_[handleId].commType.prepareType == HcclCMDType::HCCL_CMD_ALLTOALL) {
        ccuUsedXnNum_ = 11;
        CcuPrepareForAllToAll(&handleParamGM_[handleId]);
    } else if (handleParamGM_[handleId].commType.prepareType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        ccuUsedXnNum_ = 10;
        FlushDataCache(&allToAllVParam_[handleId]);
        CcuPrepareForAllToAllV(&handleParamGM_[handleId], &allToAllVParam_[handleId]);
    } else if (handleParamGM_[handleId].commType.prepareType == HcclCMDType::HCCL_CMD_HALF_ALLTOALLV) {
        ccuUsedXnNum_ = 9;
        FlushDataCache(reinterpret_cast<__gm__ uint8_t*>(&handleParamGM_[handleId]) + MAX_DCCI_CNT);
        CcuPrepareForAllToAllVWrite(&handleParamGM_[handleId]);
    } else if (handleParamGM_[handleId].commType.prepareType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER) {
        ccuUsedXnNum_ = 13;
        CcuPrepareForReduceScatterM2M(&handleParamGM_[handleId]);
    }
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CommitMsgInner(const HcclHandle handleId)
{
    handleNeedCommitCnt_[handleId] += handleRepeatCnt_[handleId];
    for (uint32_t i = 0U; i < handleRepeatCnt_[handleId]; ++i) {
        if (msgQueueIsAvailable_[globalCurResId_] == false) {
            break;
        }
        KERNEL_LOG(KERNEL_INFO, "ApiClient do ccu prepare repeatIdx = %d, globalCurResId_ = %d.", i, globalCurResId_);

        ccuParam_.repeatIndex = i;
        ccuParam_.alltoallvCnt = globalCurResId_;

        if (workingFlag_) {
            CcuPrepareForOp(handleId);
            CcuSendMsg(globalCurResId_, handleId);
        }

        msgQueueIsAvailable_[globalCurResId_] = false;
        handleCommitCnt_[handleId]++;
        globalCurCommitCnt_++;
        globalCurResId_++;
        globalCurResId_ %= CCU_MAX_MSG_NUM;
    }
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::InitCcuParam(const HcclHandle handleId)
{
    uint64_t ccTiling = 0;
    if (curVersion_ == HcclTilingVersion::NEW_TILING_VERSION) {
        ccTiling = ccOpTilingDataTable_[static_cast<uint32_t>(handleParamGM_[handleId].commType.prepareType)];
    } else if (curVersion_ == HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION) {
        ccTiling = ccOpTilingDataTable_[static_cast<uint32_t>(handleParamGM_[handleId].commType.prepareType)] +
                   tilingBaseAddr_;
    }
    if (ccTiling != 0) {
        __gm__ Mc2CcTilingInner* tilingPtr = reinterpret_cast<__gm__ Mc2CcTilingInner*>(ccTiling);
        handleParamGM_[handleId].dataType = static_cast<HcclDataType>(tilingPtr->srcDataType);
        handleParamGM_[handleId].dstDataType = static_cast<HcclDataType>(tilingPtr->dstDataType);
        handleParamGM_[handleId].op = static_cast<HcclReduceOp>(tilingPtr->reduceType);
    }

    ccuParam_.rankNum = newCcuFlag_ ? hcclNewContext_->rankSize : hcclContext_->rankNum;
    ccuParam_.rankId = newCcuFlag_ ? hcclNewContext_->rankId : hcclContext_->rankId;
    ccuParam_.scratchAddr = newCcuFlag_ ? hcclNewContext_->res[0] : hcclContext_->windowsOut[0];
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::InitHandleInfo(uint8_t handleId)
{
    handleReqId_[handleId] = 0;
    handleRepeatCnt_[handleId] = 0;
    handleNeedCommitCnt_[handleId] = 0;
    handleCommitCnt_[handleId] = 0;
    handleFinishCnt_[handleId] = 0;
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CommonPrepareImpl(
    const CommonPrepareParam& commonPrepareParam)
{
    if (unlikely(commonPrepareParam.repeat == 0U)) {
        return INVALID_HANDLE_ID;
    }

    const uint32_t prepareType = static_cast<uint32_t>(commonPrepareParam.commType.prepareType);
    if (unlikely(prepareType >= static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL))) {
        return INVALID_HANDLE_ID;
    }
    if (unlikely(
            (commonPrepareParam.commType.prepareType != HcclCMDType::HCCL_CMD_BATCH_WRITE) &&
            (GetHcclDataTypeSize(commonPrepareParam.dataType) == 0U))) {
        return INVALID_HANDLE_ID;
    }
    uint32_t rankNum = 0U;
    if (commonPrepareParam.commType.prepareType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        rankNum = newCcuFlag_ ? hcclNewContext_->rankSize : hcclContext_->rankNum;
        if (unlikely(!IsValidCcuRankNum(rankNum))) {
            return INVALID_HANDLE_ID;
        }
    }

    HcclHandle handleId = curHandleId_;

    ASCENDC_HCCL_API_ASSERT(
        handleId < HCCL_MAX_HANDLE_ID, { return INVALID_HANDLE_ID; },
        "Call Prepare[%d] failed, Prepare interface call num is[%d], expected less than[%d].",
        static_cast<int32_t>(commonPrepareParam.commType.prepareType), handleId + 1, HCCL_MAX_HANDLE_ID);

    InitHandleInfo(handleId);
    uint64_t reqId = handleId == 0 ? 0 : handleReqId_[handleId - 1] + handleRepeatCnt_[handleId - 1];
    handleReqId_[handleId] = (uint8_t)(reqId % CCU_MAX_MSG_NUM);

    handleRepeatCnt_[handleId] = commonPrepareParam.repeat;

    if (workingFlag_) {
        handleParamGM_[handleId].commType.prepareType = commonPrepareParam.commType.prepareType;
        handleParamGM_[handleId].sendBuf = commonPrepareParam.sendBuf;
        handleParamGM_[handleId].recvBuf = commonPrepareParam.recvBuf;
        handleParamGM_[handleId].count = commonPrepareParam.count;
        handleParamGM_[handleId].dataType = commonPrepareParam.dataType;
        handleParamGM_[handleId].dstDataType = commonPrepareParam.dstDataType;
        handleParamGM_[handleId].op = commonPrepareParam.op;
        handleParamGM_[handleId].strideCount = commonPrepareParam.strideCount;
        InitCcuParam(handleId);
        FlushDataCache(&handleParamGM_[handleId]);

        if (commonPrepareParam.commType.prepareType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
            for (uint32_t i = 0; i < rankNum; i++) {
                allToAllVParam_[handleId].sendCounts[i] = commonPrepareParam.paramExt.sendCounts[i];
                allToAllVParam_[handleId].sdispls[i] = commonPrepareParam.paramExt.sdispls[i];
                allToAllVParam_[handleId].recvCounts[i] = commonPrepareParam.paramExt.recvCounts[i];
                allToAllVParam_[handleId].rdispls[i] = commonPrepareParam.paramExt.rdispls[i];
            }

            for (uint32_t j = 0; j < rankNum; j += MAX_DCCI_CNT / sizeof(uint64_t)) {
                FlushDataCache(allToAllVParam_[handleId].sendCounts + j);
                FlushDataCache(allToAllVParam_[handleId].sdispls + j);
                FlushDataCache(allToAllVParam_[handleId].recvCounts + j);
                FlushDataCache(allToAllVParam_[handleId].rdispls + j);
            }
        } else if (commonPrepareParam.commType.prepareType == HcclCMDType::HCCL_CMD_HALF_ALLTOALLV) {
            handleParamGM_[handleId].wParamExt.sendOffsets = commonPrepareParam.wParamExt.sendOffsets;
            handleParamGM_[handleId].wParamExt.sendSizes = commonPrepareParam.wParamExt.sendSizes;
            handleParamGM_[handleId].wParamExt.remoteWinOffset = commonPrepareParam.wParamExt.remoteWinOffset;
            FlushDataCache(reinterpret_cast<__gm__ uint8_t*>(&handleParamGM_[handleId]) + MAX_DCCI_CNT);
        }
    }

    if constexpr (commit) {
        CommitMsgInner(handleId);
    }
    curHandleId_++;
    return handleId;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CcuSendMsg(
    uint8_t resourceId, HcclHandle handleId)
{
    ASCENDC_HCCL_API_ASSERT(
        resourceId >= 0 && resourceId < CCU_MAX_MSG_NUM, { return; }, "ApiClient CcuSendMsg resourceId %d is invalid.",
        resourceId);
    GM_ADDR xnAddr = newCcuFlag_ ? reinterpret_cast<GM_ADDR>(hcclNewContext_->xnAddr) : hcclContext_->xnOffset;
    ccuMsg_.xnAddr = xnAddr + static_cast<uint64_t>(resourceId) * CCU_MSG_XN_NUM * CCU_XN_DATA_SIZE;
    for (int i = 0; i < ccuUsedXnNum_; i++) {
        *(reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + CCU_XN_DATA_SIZE * i)) = xnData_[i];
    }

    // 循环刷新xn, ccuUsedXnNum_ + 7) / 8 向上取整
    for (int i = 0; i < ((ccuUsedXnNum_ + 7) / 8); i++) {
        FlushDataCache(ccuMsg_.xnAddr + MAX_DCCI_CNT * i);
    }

    KERNEL_LOG(
        KERNEL_INFO,
        "ApiClient CcuSendMsg ccuMsg_.xnAddr, Id0: 0x%llx, Id1: 0x%llx, Id2: 0x%llx, Id3: 0x%llx, Id4: 0x%llx, Id5: "
        "0x%llx, Id6: 0x%llx, Id7: 0x%llx, Id8: 0x%llx, Id9: 0x%llx, Id10: 0x%llx, Id11: 0x%llx, Id12: 0x%llx, Id13: "
        "0x%llx, Id14: 0x%llx, Id15: 0x%llx, Id16: 0x%llx, Id17: 0x%llx",
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr), *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 8),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 16),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 24),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 32),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 40),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 48),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 56),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 64),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 72),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 80),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 88),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 96),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 104),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 112),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 120),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 128),
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr + 136));

    const uint8_t missionNum = GetKfcMissionNum(handleId);
    for (uint8_t mission = 0; mission < missionNum; ++mission) {
        ccuMsg_.commitCKEAddr = GetCommitCkeAddr(resourceId, mission);
        WriteHBMData(reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.commitCKEAddr), CCU_MSG_CKE_SET_VALUE);
    }
}

template <const auto& config>
__aicore__ inline uint8_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::GetKfcMissionNum(
    HcclHandle handleId) const
{
    uint32_t prepareType = UINT32_MAX;
    uint32_t algorithmType = UINT32_MAX;
    if (newCcuFlag_ && handleId >= 0 && handleId < HCCL_MAX_HANDLE_ID) {
        prepareType = static_cast<uint32_t>(handleParamGM_[handleId].commType.prepareType);
        for (uint32_t index = 0U; index < HCCL_API_MAX_OP_NUM; ++index) {
            if (prepareType != static_cast<uint32_t>(HcclCMDType::HCCL_CMD_INVALID) &&
                hcclNewContext_->opType[index] == prepareType) {
                algorithmType = hcclNewContext_->algorithmType[index];
                break;
            }
        }
    }
    uint8_t missionNum = 1U;
    if (algorithmType == static_cast<uint32_t>(AlgorithmType::CcuSchedAllGatherConcurMeshNHRMultiLink)) {
        missionNum = KFC_MAX_MISSION_NUM;
    }
    return missionNum;
}

template <const auto& config>
__aicore__ inline GM_ADDR HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::GetCommitCkeAddr(
    uint8_t msgId, uint8_t missionIndex)
{
    GM_ADDR ckeAddr = newCcuFlag_ ? reinterpret_cast<GM_ADDR>(hcclNewContext_->ckeAddr) : hcclContext_->ckeOffset;
    const uint64_t region =
        static_cast<uint64_t>(missionIndex) * KFC_SIGNAL_REGIONS_PER_MISSION + KFC_COMMIT_REGION_INDEX;
    const uint64_t regionOffset = region * CCU_MAX_MSG_NUM * CCU_CKE_SIZE;
    return ckeAddr + regionOffset + static_cast<uint64_t>(msgId) * CCU_CKE_SIZE;
}

template <const auto& config>
__aicore__ inline GM_ADDR HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::GetWaitCkeAddr(
    uint8_t msgId, uint8_t missionIndex)
{
    const uint64_t region =
        static_cast<uint64_t>(missionIndex) * KFC_SIGNAL_REGIONS_PER_MISSION + KFC_WAIT_REGION_INDEX;
    uint64_t offset = region * CCU_MAX_MSG_NUM * CCU_CKE_SIZE + static_cast<uint64_t>(msgId) * CCU_CKE_SIZE;
    GM_ADDR ckeAddr = newCcuFlag_ ? reinterpret_cast<GM_ADDR>(hcclNewContext_->ckeAddr) : hcclContext_->ckeOffset;
    return ckeAddr + offset;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::CommitMsg(HcclHandle handleId)
{
    ccuParam_.repeatIndex = handleCommitCnt_[handleId];
    ccuParam_.alltoallvCnt = globalCurResId_;
    if (workingFlag_) {
        CcuPrepareForOp(handleId);
        CcuSendMsg(globalCurResId_, handleId);
    }

    msgQueueIsAvailable_[globalCurResId_] = false;
    handleCommitCnt_[handleId]++;
    globalCurCommitCnt_++;
    globalCurResId_++;
    globalCurResId_ %= CCU_MAX_MSG_NUM;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::Commit(HcclHandle handleId)
{
    ASCENDC_HCCL_API_ASSERT(
        isInited_, { return; },
        "Call Commit failed, please ensure Hccl::Init func has been called successfully already!");

    if (unlikely(handleId >= HCCL_MAX_HANDLE_ID || handleId <= INVALID_HANDLE_ID)) {
        KERNEL_LOG(KERNEL_ERROR, "Call Commit failed, handleId[%u] is invalid.", handleId);
        return;
    }

    handleNeedCommitCnt_[handleId]++;
    if (msgQueueIsAvailable_[globalCurResId_] == false) {
        KERNEL_LOG(
            KERNEL_ERROR, "Call Commit failed, the th msg is not avaliable! handleId = %d", globalCurResId_, handleId);
        return;
    }

    CommitMsg(handleId);
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::Wait(HcclHandle handleId)
{
    ASCENDC_HCCL_API_ASSERT(
        isInited_, { return HCCL_FAILED; },
        "Call Wait failed, please ensure Hccl::Init func has been called successfully already!");
    ASCENDC_HCCL_API_ASSERT(
        handleCommitCnt_[handleId] > 0, { return HCCL_FAILED; }, "Call Wait failed,  commitCnt [%u] is invalid.",
        handleCommitCnt_[handleId]);
    ASCENDC_HCCL_API_ASSERT(
        handleId < curHandleId_, { return HCCL_FAILED; },
        "Call Wait failed,  handleId = %u is invalid please call Preapre Interface before Wait.", handleId);

    if (unlikely(handleId >= HCCL_MAX_HANDLE_ID || handleId <= INVALID_HANDLE_ID)) {
        KERNEL_LOG(KERNEL_ERROR, "Call Wait failed, handleId[%u] is invalid.", handleId);
        return HCCL_FAILED;
    }

    if (unlikely(handleNeedCommitCnt_[handleId] <= 0)) {
        KERNEL_LOG(
            KERNEL_ERROR, "Call Wait failed, handleNeedCommitCnt_[%u] = %u is invalid.", handleId,
            handleNeedCommitCnt_[handleId]);
        return HCCL_FAILED;
    }
    uint8_t reqId = handleReqId_[handleId] + handleFinishCnt_[handleId];
    reqId %= CCU_MAX_MSG_NUM;

#ifndef ASCENDC_CPU_DEBUG
    if (workingFlag_) {
        const uint8_t missionNum = GetKfcMissionNum(handleId);
        for (uint8_t mission = 0; mission < missionNum; ++mission) {
            GM_ADDR waitCKEAddr = GetWaitCkeAddr(reqId, mission);
            while (true) {
                uint64_t waitCke = ReadHBMData(waitCKEAddr);
                if (waitCke != 0) {
                    WriteHBMData(reinterpret_cast<__gm__ uint64_t*>(waitCKEAddr), CCU_MSG_CKE_INIT_VALUE);
                    break;
                }
            }
        }
        finishNumTemp_++;
        WriteHBMData(finishCntGM_, finishNumTemp_);
    } else {
        while (true) {
            uint64_t finshCnt = ReadHBMData(finishCntGM_);
            if (finshCnt > finishNum_) {
                break;
            }
        }
    }
#endif

    msgQueueIsAvailable_[reqId] = true;
    handleFinishCnt_[handleId]++;
    finishNum_++;
    globalCurWaitId_++;
    globalCurWaitId_ %= CCU_MAX_MSG_NUM;

    // 补发消息
    // 场景1:同一个handleId需要补发
    if (handleCommitCnt_[handleId] < handleNeedCommitCnt_[handleId]) {
        CommitMsg(handleId);
    } else { // 场景2:下一个handleId需要补发
        uint32_t nextHandleId = handleId + 1;
        if (nextHandleId < curHandleId_ && (handleCommitCnt_[nextHandleId] < handleNeedCommitCnt_[nextHandleId])) {
            CommitMsg(nextHandleId);
        }
    }

    KERNEL_LOG(
        KERNEL_INFO, "ApiClient Wait finish finishCntGM_:%d, finishCnt_:%d", *finishCntGM_, handleFinishCnt_[handleId]);
    return HCCL_SUCCESS;
}

template <const auto& config>
template <bool sync>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config>::Finalize()
{
    ASCENDC_HCCL_API_ASSERT(
        isInited_, { return; },
        "Call Finalize failed, please ensure Hccl::Init func has been called successfully already!");

    if (workingFlag_) {
        HcclHandle handleId = curHandleId_ - 1;

        while (handleFinishCnt_[handleId] < handleCommitCnt_[handleId]) {
            Wait(handleId);
        }

        KERNEL_LOG(KERNEL_INFO, "ApiClient Finalize handleId:%d, globalCurWaitId_:%d", handleId, globalCurWaitId_);

        GM_ADDR xnAddr = newCcuFlag_ ? reinterpret_cast<GM_ADDR>(hcclNewContext_->xnAddr) : hcclContext_->xnOffset;
        ccuMsg_.xnAddr = xnAddr + CCU_MSG_XN_NUM * CCU_XN_DATA_SIZE * globalCurWaitId_;
        *reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.xnAddr) = 0xffffffffffffffff;
        FlushDataCache(ccuMsg_.xnAddr);
        const uint8_t missionNum = GetKfcMissionNum(handleId);
        for (uint8_t mission = 0; mission < missionNum; ++mission) {
            ccuMsg_.commitCKEAddr = GetCommitCkeAddr(globalCurWaitId_, mission);
            WriteHBMData(reinterpret_cast<__gm__ uint64_t*>(ccuMsg_.commitCKEAddr), CCU_MSG_CKE_SET_VALUE);
        }
        KERNEL_LOG(
            KERNEL_INFO, "ApiClient Finalize success handleId:%d, globalCurWaitId_:%d", handleId, globalCurWaitId_);
    }
}

} // namespace AscendC
#endif
