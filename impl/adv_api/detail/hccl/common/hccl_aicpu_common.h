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
 * \file hccl_aicpu_common.h
 * \brief
 */
#ifndef IMPL_COMMON_HCCL_AICPU_COMMON_H
#define IMPL_COMMON_HCCL_AICPU_COMMON_H

#include "hccl_utils.h"

namespace AscendC {
__aicore__ inline void CopyHcclMsg(const uint8_t* src, __gm__ HcclMsg* dst)
{
    constexpr uint32_t HCCL_VALID_POS = 12U;
    __gm__ DataBlock* tmpDst = reinterpret_cast<__gm__ DataBlock*>(dst);
    uint32_t xorCheck = 0U;
    for (uint32_t i = 0; i < HCCL_MSG_DATA_CNT - 1U; ++i) {
        if (i == HCCL_VALID_POS) {
            xorCheck ^= HCCL_MSG_VALID_MASK;
        } else {
            xorCheck ^= tmpDst->data[i] = *(reinterpret_cast<const uint32_t*>(src));
        }
        src += sizeof(tmpDst->data[i]);
    }
    tmpDst->data[HCCL_MSG_DATA_CNT - 1U] = xorCheck;
    tmpDst->data[HCCL_VALID_POS] = HCCL_MSG_VALID_MASK;
}

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
__aicore__ inline void CopyHcclMsgCompact(const HcclMsg* src, __gm__ HcclMsg* dst)
{
    constexpr uint32_t compactValidWord = 12U;
    constexpr uint32_t compactLastWord = 15U;
    static_assert(sizeof(HcclMsg) == sizeof(uint64_t) * 8U, "A5 compact message must be eight 64-bit words");
    static_assert(alignof(HcclMsg) >= alignof(uint64_t), "A5 compact message must support 64-bit access");

    const uint64_t* const srcWords = reinterpret_cast<const uint64_t*>(src);
    __gm__ uint64_t* const dstWords = reinterpret_cast<__gm__ uint64_t*>(dst);
    uint64_t xorWords = 0U;
    for (uint32_t i = 0U; i < compactValidWord / 2U; ++i) {
        xorWords ^= dstWords[i] = srcWords[i];
    }

    const uint64_t validWord = static_cast<uint64_t>(HCCL_MSG_VALID_MASK);
    const uint64_t validPair = (srcWords[compactValidWord / 2U] & ~UINT64_C(0xFFFFFFFF)) | validWord;
    const uint64_t lastPair = srcWords[compactLastWord / 2U];
    xorWords ^= validPair;
    const uint32_t xorCheck =
        static_cast<uint32_t>(xorWords) ^ static_cast<uint32_t>(xorWords >> 32U) ^ static_cast<uint32_t>(lastPair);

    dstWords[compactValidWord / 2U] = validPair;
    dstWords[compactLastWord / 2U] = (static_cast<uint64_t>(xorCheck) << 32U) | static_cast<uint32_t>(lastPair);
}
#endif

__aicore__ inline void AssembleHcclMsgExt(const AlltoAllVParamExt& param, uint32_t rankDim, __gm__ HcclMsgExt* dst)
{
    uint64_t xorCheck = 0U;
    for (uint32_t i = 0U; i < rankDim; ++i) {
        xorCheck ^= dst->sendCounts[i] = param.sendCounts[i];
        xorCheck ^= dst->sendOffset[i] = param.sdispls[i];
        xorCheck ^= dst->recvCounts[i] = param.recvCounts[i];
        xorCheck ^= dst->recvOffset[i] = param.rdispls[i];
    }
    dst->xorCheck = (xorCheck ^ HCCL_MSG_VALID_MASK);
    dst->valid = HCCL_MSG_VALID_MASK;
}

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
// Compact A5 representation. The count and offset are each guaranteed to fit
// in 32 bits before this function is selected; callers must fall back to the
// legacy 64-bit arrays when any value exceeds 32 bits.
__aicore__ inline void AssembleHcclMsgExtCompact(
    const AlltoAllVParamExt& param, uint32_t rankDim, __gm__ HcclMsgExt* dst, uint64_t& extCtrl, uint64_t& extXor)
{
    uint32_t xorCheck = HCCL_MSG_EXT_COMPACT_CTRL;
    __gm__ uint64_t* const dstSend = reinterpret_cast<__gm__ uint64_t*>(dst->sendCounts);
    __gm__ uint64_t* const dstRecv = reinterpret_cast<__gm__ uint64_t*>(dst->recvCounts);
    for (uint32_t i = 0U; i < rankDim; ++i) {
        const uint64_t sendValue = (param.sendCounts[i] << 32U) | param.sdispls[i];
        const uint64_t recvValue = (param.recvCounts[i] << 32U) | param.rdispls[i];
        dstSend[i] = sendValue;
        dstRecv[i] = recvValue;
        xorCheck ^= static_cast<uint32_t>(sendValue >> 32U);
        xorCheck ^= static_cast<uint32_t>(sendValue);
        xorCheck ^= static_cast<uint32_t>(recvValue >> 32U);
        xorCheck ^= static_cast<uint32_t>(recvValue);
    }
    extCtrl = HCCL_MSG_EXT_COMPACT_CTRL;
    extXor = xorCheck;
}
#endif

constexpr uint32_t HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX = 0U;
constexpr uint32_t HCCL_CONTROL_RESERVED_PRIMITIVE_RESET_IDX = 1U;

// Keep primitiveId state in GM reserved bytes to avoid function-local static state across kernel launches.
__aicore__ inline void ResetPrimitiveIdStateInControlMsg(__gm__ ControlHcclMsg* controlMsgGM)
{
    ASCENDC_HCCL_API_ASSERT(controlMsgGM != nullptr, { return; }, "Control msg is nullptr.");
    controlMsgGM->reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX] = 0U;
    controlMsgGM->reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_RESET_IDX] = 0U;
    FlushDataCache(controlMsgGM);
}

__aicore__ inline void ResetPrimitiveIdOnceInControlMsg(__gm__ ControlHcclMsg* controlMsgGM)
{
    ASCENDC_HCCL_API_ASSERT(controlMsgGM != nullptr, { return; }, "Control msg is nullptr.");
    FlushDataCache(controlMsgGM);
    if (controlMsgGM->reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_RESET_IDX] == 0U) {
        controlMsgGM->reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX] = 0U;
        controlMsgGM->reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_RESET_IDX] = 1U;
        FlushDataCache(controlMsgGM);
    }
}

__aicore__ inline uint8_t FetchAndIncPrimitiveIdInControlMsg(__gm__ ControlHcclMsg* controlMsgGM)
{
    ASCENDC_HCCL_API_ASSERT(controlMsgGM != nullptr, { return 0U; }, "Control msg is nullptr.");
    FlushDataCache(controlMsgGM);
    uint8_t seqNum = controlMsgGM->reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX];
    controlMsgGM->reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX] = static_cast<uint8_t>(seqNum + 1U);
    FlushDataCache(controlMsgGM);
    return seqNum;
}

__aicore__ inline void AssembleHcclMsgV2(
    const CommonPrepareParam& param, HcclTilingVersion ver, HcclHandle handle, uint64_t tiling, __gm__ HcclMsg* dst,
    __gm__ ControlHcclMsg* controlMsgGM)
{
    HcclMsg tmp{};
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    // A5 derives the sequence number from the Prepare handle. The shared control message is
    // not consumed by the A5 AICPU server on this path.
    (void)controlMsgGM;
#else
    FlushDataCache(controlMsgGM);
    if (controlMsgGM->resetSeq > 0) {
        controlMsgGM->resetSeq = 0;
        ResetPrimitiveIdOnceInControlMsg(controlMsgGM);
    }
#endif
    tmp.commType.msgType = param.commType.msgType;
    if (param.commType.msgType == ControlMsgType::HCCL_CMD_FINALIZE) {
#if !(defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510)
        ResetPrimitiveIdStateInControlMsg(controlMsgGM);
#endif
    } else {
        tmp.opType = param.op;
        tmp.sendBuffer = reinterpret_cast<uint64_t>(param.sendBuf);
        tmp.recvBuffer = reinterpret_cast<uint64_t>(param.recvBuf);
        tmp.dataCnt = param.count;
        tmp.strideCount = param.strideCount;
        if (ver == HcclTilingVersion::DEPRECATED_TILING_VERSION) {
            tmp.addMsg.v0Msg.hcclDataType = param.dataType;
            tmp.addMsg.v0Msg.repeatCnt = param.repeat;
            tmp.addMsg.v0Msg.selfHandleID = handle;
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
            tmp.addMsg.v0Msg.seqNum = static_cast<uint8_t>(handle);
#else
            tmp.addMsg.v0Msg.seqNum = FetchAndIncPrimitiveIdInControlMsg(controlMsgGM);
#endif
            tmp.addMsg.v0Msg.version = ver;
        } else {
            tmp.addMsg.v1Msg.ccOpTilingData = tiling;
            tmp.addMsg.v1Msg.hcclDataType = param.dataType;
            tmp.addMsg.v1Msg.repeatCnt = param.repeat;
            tmp.addMsg.v1Msg.selfHandleID = handle;
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
            tmp.addMsg.v1Msg.seqNum = static_cast<uint8_t>(handle);
#else
            tmp.addMsg.v1Msg.seqNum = FetchAndIncPrimitiveIdInControlMsg(controlMsgGM);
#endif
            tmp.addMsg.v1Msg.version = ver;
        }
    }
    tmp.addMsg.v0Msg.valid = HCCL_MSG_VALID_MASK;
    CopyHcclMsg(reinterpret_cast<const uint8_t*>(&tmp), dst);
}

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
__aicore__ inline void AssembleHcclMsgV2Compact(
    const CommonPrepareParam& param, HcclTilingVersion ver, HcclHandle handle, uint64_t tiling, __gm__ HcclMsg* dst,
    uint64_t extCtrl, uint64_t extXor)
{
    HcclMsg tmp{};
    tmp.commType.msgType = param.commType.msgType;
    tmp.opType = param.op;
    tmp.sendBuffer = reinterpret_cast<uint64_t>(param.sendBuf);
    tmp.recvBuffer = reinterpret_cast<uint64_t>(param.recvBuf);
    tmp.dataCnt = param.count;
    tmp.strideCount = param.strideCount;
    if (ver == HcclTilingVersion::DEPRECATED_TILING_VERSION) {
        tmp.addMsg.v0Msg.hcclDataType = param.dataType;
        tmp.addMsg.v0Msg.repeatCnt = param.repeat;
        tmp.addMsg.v0Msg.selfHandleID = handle;
        tmp.addMsg.v0Msg.seqNum = static_cast<uint8_t>(handle);
        tmp.addMsg.v0Msg.version = ver;
    } else {
        tmp.addMsg.v1Msg.ccOpTilingData = tiling;
        tmp.addMsg.v1Msg.hcclDataType = param.dataType;
        tmp.addMsg.v1Msg.repeatCnt = param.repeat;
        tmp.addMsg.v1Msg.selfHandleID = handle;
        tmp.addMsg.v1Msg.seqNum = static_cast<uint8_t>(handle);
        tmp.addMsg.v1Msg.version = ver;
    }
    tmp.strideCount = (extCtrl << 32U) | (extXor & 0xFFFFFFFFULL);
    tmp.addMsg.v0Msg.valid = HCCL_MSG_VALID_MASK;
    CopyHcclMsgCompact(&tmp, dst);
}
#endif
__aicore__ inline void AssembleHcclMsg(
    const CommonPrepareParam& param, HcclTilingVersion ver, HcclHandle handle, uint64_t tiling, __gm__ HcclMsg* dst,
    __gm__ ControlHcclMsg* controlMsgGM)
{
    HcclMsg tmp{};
    static uint8_t primitiveId = 0U;
    static bool isResetPrimitiveId = false;
    FlushDataCache(controlMsgGM);
    if (controlMsgGM->resetSeq > 0) {
        controlMsgGM->resetSeq = 0;
        if (!isResetPrimitiveId) {
            primitiveId = 0U;
            isResetPrimitiveId = true;
        }
    }
    tmp.commType.msgType = param.commType.msgType;
    if (param.commType.msgType == ControlMsgType::HCCL_CMD_FINALIZE) {
        primitiveId = 0U;
        isResetPrimitiveId = false;
    } else {
        tmp.opType = param.op;
        tmp.sendBuffer = reinterpret_cast<uint64_t>(param.sendBuf);
        tmp.recvBuffer = reinterpret_cast<uint64_t>(param.recvBuf);
        tmp.dataCnt = param.count;
        tmp.strideCount = param.strideCount;
        if (ver == HcclTilingVersion::DEPRECATED_TILING_VERSION) {
            tmp.addMsg.v0Msg.hcclDataType = param.dataType;
            tmp.addMsg.v0Msg.repeatCnt = param.repeat;
            tmp.addMsg.v0Msg.selfHandleID = handle;
            tmp.addMsg.v0Msg.seqNum = primitiveId++;
            tmp.addMsg.v0Msg.version = ver;
        } else {
            tmp.addMsg.v1Msg.ccOpTilingData = tiling;
            tmp.addMsg.v1Msg.hcclDataType = param.dataType;
            tmp.addMsg.v1Msg.repeatCnt = param.repeat;
            tmp.addMsg.v1Msg.selfHandleID = handle;
            tmp.addMsg.v1Msg.seqNum = primitiveId++;
            tmp.addMsg.v1Msg.version = ver;
        }
    }
    tmp.addMsg.v0Msg.valid = HCCL_MSG_VALID_MASK;
    CopyHcclMsg(reinterpret_cast<const uint8_t*>(&tmp), dst);
}

__aicore__ inline void AssembleHcclMsg(
    const CommonPrepareParam& param, int8_t srcGroupID, HcclHandle srcHandleID, __gm__ HcclMsg* dst)
{
    HcclMsg tmp{};
    tmp.commType.msgType = param.commType.msgType;
    tmp.addMsg.v0Msg.commDepGroupID = srcGroupID;
    tmp.addMsg.v0Msg.commDepHandleID = srcHandleID;
    tmp.addMsg.v0Msg.valid = HCCL_MSG_VALID_MASK;
    CopyHcclMsg(reinterpret_cast<const uint8_t*>(&tmp), dst);
}

__aicore__ inline HcclContextDef::HcclRankRelationResV2* GetRemoteRankAddrs(
    __gm__ HcclContextDef::HcclOpResParam* ctx, uint32_t rankId)
{
    const HcclContextDef::RemoteResPtr* remoteRes =
        reinterpret_cast<const HcclContextDef::RemoteResPtr*>(reinterpret_cast<uintptr_t>(ctx) + ctx->rWinStart);
    return remoteRes[rankId].nextDevicePtr;
}

__aicore__ inline void UpdateControlMsgCount(__gm__ HcclMsgArea* hcclMsgArea, ControlMsgType msg)
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    (void)hcclMsgArea;
    (void)msg;
#else
    const uint32_t msgValue = static_cast<uint32_t>(msg);
    const uint32_t firstMsgValue = static_cast<uint32_t>(ControlMsgType::HCCL_CMD_FINALIZE);
    const uint32_t maxMsgValue = static_cast<uint32_t>(ControlMsgType::HCCL_CMD_MAX);
    if ((msgValue < firstMsgValue) || (msgValue >= maxMsgValue)) {
        KERNEL_LOG(KERNEL_ERROR, "Invalid msg type %u.", msgValue);
        return;
    }
    __gm__ TurnCnt* apiInfo = &(hcclMsgArea->apiStats.msgStats[msgValue - firstMsgValue]);
    FlushDataCache(apiInfo);
    ++(apiInfo->cnt);
    FlushDataCache(apiInfo);
#endif
}
} // namespace AscendC

#endif
