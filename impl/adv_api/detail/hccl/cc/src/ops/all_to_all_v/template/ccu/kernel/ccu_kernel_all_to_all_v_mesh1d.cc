/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_all_to_all_v_mesh1d.h"

namespace mc2_ops_hccl {

constexpr uint32_t A2AV_EXT_FIELD_NUM = 4;
constexpr uint32_t A2AV_TAIL_SIZE_IDX = 0;
constexpr uint32_t A2AV_LOOP_NUM_IDX = 1;
constexpr uint32_t A2AV_SEND_OFFSET_IDX = 2;
constexpr uint32_t A2AV_RECV_OFFSET_IDX = 3;
constexpr int A2AV_OUTPUT_XN_ID = 0;
constexpr int A2AV_TOKEN_XN_ID = 1;
constexpr int A2AV_PRE_SYNC_CKE_IDX = 0;
constexpr int A2AV_POST_SYNC_CKE_IDX = 1;
constexpr uint16_t A2AV_OUTPUT_BIT = 1U << A2AV_OUTPUT_XN_ID;
constexpr uint16_t A2AV_TOKEN_BIT = 1U << A2AV_TOKEN_XN_ID;
constexpr uint16_t A2AV_PRE_SYNC_BITS = A2AV_OUTPUT_BIT | A2AV_TOKEN_BIT;
constexpr uint16_t A2AV_POST_SYNC_BIT = 1U << 5;
constexpr uint32_t A2AV_BITS_PER_EVENT = 16;
constexpr uint64_t A2AV_MAX_TRANSPORT_SIZE = 256ULL * 1024ULL * 1024ULL;
constexpr uint64_t A2AV_GROUP_MEMSLICE_SIZE = 32ULL * 1024ULL;
constexpr uint64_t A2AV_GROUP_LOOP_COUNT = 8ULL;

CcuResult CcuAlltoAllVMesh1DKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable srcOffset,
    ccu::Variable dstOffset, ccu::Variable extAddr, ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3,
    ccu::Variable goSize4, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId)
{
    std::vector<ccu::Variable> output(rankSize);
    std::vector<ccu::Variable> token(rankSize);
    std::vector<ccu::LocalAddr> src(rankSize);
    std::vector<ccu::RemoteAddr> dst(rankSize);
    ccu::LocalAddr localDst;
    std::vector<ccu::Event> events((rankSize + A2AV_BITS_PER_EVENT - 1) / A2AV_BITS_PER_EVENT);
    CcuKernelCtxBase ctx;
    InitCcuKernelCtxBase(ctx);
    GroupOpSizeVars tailGoSize{goSize1, goSize2, goSize3, goSize4};
    GroupOpSizeVars fullBlockGoSize;
    fullBlockGoSize.addrOffset = A2AV_MAX_TRANSPORT_SIZE;
    fullBlockGoSize.loopParam = A2AV_MAX_TRANSPORT_SIZE / (A2AV_GROUP_MEMSLICE_SIZE * A2AV_GROUP_LOOP_COUNT);
    fullBlockGoSize.parallelParam = 0;
    fullBlockGoSize.residual = 0;

    const uint32_t extArgNum = rankSize * A2AV_EXT_FIELD_NUM;
    ccu::Array<ccu::Variable> extArgs(extArgNum);
    ccu::Load(extAddr, extArgs, extArgNum);

    output[rankId] = outputAddr;
    token[rankId] = tokenInfo;

    uint32_t channelIdx = 0;
    for (uint32_t peerId = 0; peerId < rankSize; peerId++) {
        if (peerId == rankId) {
            continue;
        }
        output[peerId] = ccu::GetResByChannel<ccu::Variable>(channels[channelIdx], A2AV_OUTPUT_XN_ID);
        token[peerId] = ccu::GetResByChannel<ccu::Variable>(channels[channelIdx], A2AV_TOKEN_XN_ID);
        channelIdx++;
    }

    channelIdx = 0;
    for (uint32_t peerId = 0; peerId < rankSize; peerId++) {
        if (peerId == rankId) {
            continue;
        }
        ccu::Variable peerRecvBase;
        peerRecvBase = outputAddr + extArgs[peerId * A2AV_EXT_FIELD_NUM + A2AV_RECV_OFFSET_IDX];
        ccu::WriteVariableWithNotify(
            channels[channelIdx], peerRecvBase, A2AV_OUTPUT_XN_ID, A2AV_PRE_SYNC_CKE_IDX, A2AV_OUTPUT_BIT);
        ccu::WriteVariableWithNotify(
            channels[channelIdx], tokenInfo, A2AV_TOKEN_XN_ID, A2AV_PRE_SYNC_CKE_IDX, A2AV_TOKEN_BIT);
        channelIdx++;
    }

    for (uint32_t i = 0; i < channelCount; i++) {
        ccu::NotifyWait(channels[i], A2AV_PRE_SYNC_CKE_IDX, A2AV_PRE_SYNC_BITS);
    }

    for (uint32_t rankIdx = 0; rankIdx < rankSize; rankIdx++) {
        const uint32_t extBase = rankIdx * A2AV_EXT_FIELD_NUM;
        src[rankIdx].addr = inputAddr;
        src[rankIdx].addr += extArgs[extBase + A2AV_SEND_OFFSET_IDX];
        src[rankIdx].addr += srcOffset;
        src[rankIdx].token = token[rankIdx];

        if (rankIdx == rankId) {
            localDst.addr = outputAddr;
            localDst.addr += extArgs[extBase + A2AV_RECV_OFFSET_IDX];
            localDst.addr += dstOffset;
            localDst.token = token[rankIdx];
        } else {
            dst[rankIdx].addr = output[rankIdx];
            dst[rankIdx].addr += dstOffset;
            dst[rankIdx].token = token[rankIdx];
        }
    }

    ccu::Variable completedRankCount;
    ccu::Variable one;
    ccu::Variable maxTransportSize;
    completedRankCount = 0;
    one = 1;
    maxTransportSize = A2AV_MAX_TRANSPORT_SIZE;
    CCU_WHILE(completedRankCount != rankSize)
    {
        channelIdx = 0;
        for (uint32_t rankIdx = 0; rankIdx < rankSize; rankIdx++) {
            const uint32_t extBase = rankIdx * A2AV_EXT_FIELD_NUM;
            const uint32_t eventIdx = rankIdx / A2AV_BITS_PER_EVENT;
            const uint16_t rankMask = 1U << (rankIdx % A2AV_BITS_PER_EVENT);
            ccu::Variable& loopNum = extArgs[extBase + A2AV_LOOP_NUM_IDX];
            ccu::Variable& tailSize = extArgs[extBase + A2AV_TAIL_SIZE_IDX];

            CCU_IF(loopNum == UINT64_MAX) { CCU_CHK_RET(ccu::EventRecord(events[eventIdx], rankMask)); }
            CCU_IF(loopNum != UINT64_MAX)
            {
                CCU_IF(loopNum == UINT64_MAX - 1U)
                {
                    if (rankIdx == rankId) {
                        CCU_IF(tailSize != 0) { CCU_CHK_RET(GroupCopy(ctx, localDst, src[rankIdx], tailGoSize)); }
                        CCU_CHK_RET(ccu::EventRecord(events[eventIdx], rankMask));
                    } else {
                        CCU_IF(tailSize != 0)
                        {
                            CCU_CHK_RET(ccu::Write(
                                channels[channelIdx], dst[rankIdx], src[rankIdx], tailSize, events[eventIdx],
                                rankMask));
                        }
                        CCU_IF(tailSize == 0) { CCU_CHK_RET(ccu::EventRecord(events[eventIdx], rankMask)); }
                    }
                    completedRankCount += one;
                }
                CCU_IF(loopNum != UINT64_MAX - 1U)
                {
                    if (rankIdx == rankId) {
                        CCU_CHK_RET(GroupCopy(ctx, localDst, src[rankIdx], fullBlockGoSize));
                        CCU_CHK_RET(ccu::EventRecord(events[eventIdx], rankMask));
                        localDst.addr += maxTransportSize;
                    } else {
                        CCU_CHK_RET(ccu::Write(
                            channels[channelIdx], dst[rankIdx], src[rankIdx], maxTransportSize, events[eventIdx],
                            rankMask));
                        dst[rankIdx].addr += maxTransportSize;
                    }
                    src[rankIdx].addr += maxTransportSize;
                }
                loopNum += one;
            }
            if (rankIdx != rankId) {
                channelIdx++;
            }
        }

        for (uint32_t eventIdx = 0; eventIdx < events.size(); eventIdx++) {
            const uint32_t remainingRanks = rankSize - eventIdx * A2AV_BITS_PER_EVENT;
            const uint32_t bitCount = std::min(remainingRanks, A2AV_BITS_PER_EVENT);
            const uint16_t eventMask = bitCount == A2AV_BITS_PER_EVENT ? static_cast<uint16_t>(UINT16_MAX) :
                                                                         static_cast<uint16_t>((1U << bitCount) - 1);
            CCU_CHK_RET(ccu::EventWait(events[eventIdx], eventMask));
        }
    }

    for (uint32_t i = 0; i < channelCount; i++) {
        ccu::NotifyRecord(channels[i], A2AV_POST_SYNC_CKE_IDX, A2AV_POST_SYNC_BIT);
    }
    for (uint32_t i = 0; i < channelCount; i++) {
        ccu::NotifyWait(channels[i], A2AV_POST_SYNC_CKE_IDX, A2AV_POST_SYNC_BIT);
    }
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
