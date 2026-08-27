/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ccu_kernel_kfc_all_gather_mesh1d_mem2mem.h"

namespace mc2_ops_hccl {

constexpr int AG_M2M_OUTPUT_XN_ID = 1;
constexpr int AG_M2M_TOKEN_XN_ID = 2;
constexpr int AG_M2M_POST_SYNC_CKE_IDX = 0;
constexpr int AG_M2M_OUTPUT_CKE_IDX = 1;
constexpr int AG_M2M_TOKEN_CKE_IDX = 2;
constexpr uint16_t AG_M2M_POST_SYNC_BIT = 1U;
constexpr uint16_t AG_M2M_OUTPUT_BIT = 1U << AG_M2M_OUTPUT_XN_ID;
constexpr uint16_t AG_M2M_TOKEN_BIT = 1U << AG_M2M_TOKEN_XN_ID;
constexpr uint32_t AG_M2M_BITS_PER_EVENT = 16;

struct KfcAllGatherMesh1DMem2MemContext : CcuKernelCtxBase {
    const ChannelHandle* channels{nullptr};
    uint32_t channelCount{0};
    uint32_t rankSize{0};
    uint32_t rankId{0};

    ccu::Variable input;
    std::vector<ccu::Variable> output;
    std::vector<ccu::Variable> token;
    ccu::Variable outputOffset;
    ccu::Variable sliceSize;
    ccu::Variable isInputOutputEqual;
    GroupOpSizeVars goSize;
    std::vector<ccu::Event> events;
};

static CcuResult InitResource(KfcAllGatherMesh1DMem2MemContext& ctx)
{
    if (ctx.rankSize == 0 || ctx.rankId >= ctx.rankSize || ctx.channelCount + 1 != ctx.rankSize) {
        HCCL_ERROR(
            "[CcuKfcAllGatherMesh1DMem2Mem] invalid rank/channel, rankSize[%u], rankId[%u], channelCount[%u]",
            ctx.rankSize, ctx.rankId, ctx.channelCount);
        return CcuResult::CCU_E_INTERNAL;
    }

    ctx.output.resize(ctx.rankSize);
    ctx.token.resize(ctx.rankSize);
    uint32_t channelIdx = 0;
    for (uint32_t peerId = 0; peerId < ctx.rankSize; peerId++) {
        if (peerId == ctx.rankId) {
            continue;
        }
        ctx.output[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], AG_M2M_OUTPUT_XN_ID);
        ctx.token[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], AG_M2M_TOKEN_XN_ID);
        channelIdx++;
    }

    const uint32_t eventNum = (ctx.rankSize + AG_M2M_BITS_PER_EVENT - 1) / AG_M2M_BITS_PER_EVENT;
    ctx.events.resize(eventNum);
    ctx.resourceAllocated = false;
    return CCU_SUCCESS;
}

static CcuResult LoadArgs(
    KfcAllGatherMesh1DMem2MemContext& ctx, ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo,
    ccu::Variable outputOffset, ccu::Variable sliceSize, ccu::Variable goSize0, ccu::Variable goSize1,
    ccu::Variable goSize2, ccu::Variable goSize3, ccu::Variable isInputOutputEqual)
{
    ctx.input = inputAddr;
    ctx.output[ctx.rankId] = outputAddr;
    ctx.token[ctx.rankId] = tokenInfo;
    ctx.outputOffset = outputOffset;
    ctx.sliceSize = sliceSize;
    ctx.goSize.addrOffset = goSize0;
    ctx.goSize.loopParam = goSize1;
    ctx.goSize.parallelParam = goSize2;
    ctx.goSize.residual = goSize3;
    ctx.isInputOutputEqual = isInputOutputEqual;
    return CCU_SUCCESS;
}

static CcuResult PreSync(KfcAllGatherMesh1DMem2MemContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.output[ctx.rankId], AG_M2M_OUTPUT_XN_ID, AG_M2M_OUTPUT_CKE_IDX, AG_M2M_OUTPUT_BIT));
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.token[ctx.rankId], AG_M2M_TOKEN_XN_ID, AG_M2M_TOKEN_CKE_IDX, AG_M2M_TOKEN_BIT));
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], AG_M2M_OUTPUT_CKE_IDX, AG_M2M_OUTPUT_BIT));
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], AG_M2M_TOKEN_CKE_IDX, AG_M2M_TOKEN_BIT));
    }
    return CCU_SUCCESS;
}

static CcuResult PostSync(KfcAllGatherMesh1DMem2MemContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::NotifyRecord(ctx.channels[i], AG_M2M_POST_SYNC_CKE_IDX, AG_M2M_POST_SYNC_BIT));
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], AG_M2M_POST_SYNC_CKE_IDX, AG_M2M_POST_SYNC_BIT));
    }
    return CCU_SUCCESS;
}

static CcuResult DoAllGatherWrite(
    KfcAllGatherMesh1DMem2MemContext& ctx, const ccu::LocalAddr& src, const std::vector<ccu::RemoteAddr>& dst)
{
    uint32_t channelIdx = 0;
    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        const uint32_t eventIdx = rankIdx / AG_M2M_BITS_PER_EVENT;
        const uint16_t rankMask = 1U << (rankIdx % AG_M2M_BITS_PER_EVENT);
        if (rankIdx == ctx.rankId) {
            CCU_CHK_RET(ccu::EventRecord(ctx.events[eventIdx], rankMask));
            continue;
        }
        CCU_CHK_RET(
            ccu::Write(ctx.channels[channelIdx], dst[rankIdx], src, ctx.sliceSize, ctx.events[eventIdx], rankMask));
        channelIdx++;
    }
    return CCU_SUCCESS;
}

static CcuResult DoAllGatherWait(KfcAllGatherMesh1DMem2MemContext& ctx)
{
    const uint32_t eventNum = ctx.events.size();
    for (uint32_t eventIdx = 0; eventIdx < eventNum; eventIdx++) {
        const uint32_t remainingRanks = ctx.rankSize - eventIdx * AG_M2M_BITS_PER_EVENT;
        const uint32_t bitCount = std::min(remainingRanks, AG_M2M_BITS_PER_EVENT);
        const uint16_t eventMask = bitCount == AG_M2M_BITS_PER_EVENT ? static_cast<uint16_t>(UINT16_MAX) :
                                                                       static_cast<uint16_t>((1U << bitCount) - 1);
        CCU_CHK_RET(ccu::EventWait(ctx.events[eventIdx], eventMask));
    }
    return CCU_SUCCESS;
}

static CcuResult DoAllGather(KfcAllGatherMesh1DMem2MemContext& ctx)
{
    ccu::LocalAddr src;
    src.addr = ctx.input;
    src.token = ctx.token[ctx.rankId];

    ccu::LocalAddr localDst;
    localDst.addr = ctx.output[ctx.rankId];
    localDst.addr += ctx.outputOffset;
    localDst.token = ctx.token[ctx.rankId];

    std::vector<ccu::RemoteAddr> dst(ctx.rankSize);
    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        if (rankIdx == ctx.rankId) {
            continue;
        }
        dst[rankIdx].addr = ctx.output[rankIdx];
        dst[rankIdx].addr += ctx.outputOffset;
        dst[rankIdx].token = ctx.token[rankIdx];
    }

    CCU_CHK_RET(DoAllGatherWrite(ctx, src, dst));
    CCU_IF(ctx.isInputOutputEqual == 0) { CCU_CHK_RET(GroupCopy(ctx, localDst, src, ctx.goSize)); }
    CCU_CHK_RET(DoAllGatherWait(ctx));
    return CCU_SUCCESS;
}

CcuResult CcuKfcAllGatherMesh1DMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable outputOffset,
    ccu::Variable sliceSize, ccu::Variable goSize0, ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3,
    ccu::Variable isInputOutputEqual, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize,
    uint32_t rankId)
{
    KfcAllGatherMesh1DMem2MemContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    InitCcuKernelCtxBase(ctx);

    CCU_CHK_RET(InitResource(ctx));
    CCU_CHK_RET(LoadArgs(
        ctx, inputAddr, outputAddr, tokenInfo, outputOffset, sliceSize, goSize0, goSize1, goSize2, goSize3,
        isInputOutputEqual));
    CCU_CHK_RET(PreSync(ctx));
    CCU_IF(ctx.sliceSize != 0) { CCU_CHK_RET(DoAllGather(ctx)); }
    CCU_CHK_RET(PostSync(ctx));
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
