/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_kfc_all_gather_mesh1d.h"

namespace mc2_ops_hccl {

constexpr int AG_OUTPUT_XN_ID = 1;
constexpr int AG_TOKEN_XN_ID = 2;
constexpr int AG_POST_SYNC_CKE_IDX = 0;
constexpr int AG_OUTPUT_CKE_IDX = 1;
constexpr int AG_TOKEN_CKE_IDX = 2;

struct KfcAllGatherMesh1DContext : CcuKernelCtxBase {
    const ChannelHandle* channels{nullptr};
    uint32_t channelCount{0};
    uint32_t rankSize{0};
    uint32_t rankId{0};

    ccu::Variable input;
    std::vector<ccu::Variable> output;
    std::vector<ccu::Variable> token;
    ccu::Variable offset;
    ccu::Variable sliceSize;
};

static CcuResult InitResource(KfcAllGatherMesh1DContext& ctx)
{
    uint32_t channelIdx = 0;

    if (ctx.channelCount == 0) {
        HCCL_ERROR("[CcuKfcAllGatherMesh1D] channels is empty!");
        return CcuResult::CCU_E_INTERNAL;
    }

    ctx.output.resize(ctx.rankSize);
    ctx.token.resize(ctx.rankSize);

    for (uint64_t peerId = 0; peerId < ctx.rankSize; peerId++) {
        if (peerId != ctx.rankId) {
            ctx.output[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], AG_OUTPUT_XN_ID);
            ctx.token[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], AG_TOKEN_XN_ID);
            channelIdx++;
        }
    }
    ctx.resourceAllocated = false;
    return CCU_SUCCESS;
}

static CcuResult LoadArgs(
    KfcAllGatherMesh1DContext& ctx, ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo,
    ccu::Variable offset, ccu::Variable sliceSize)
{
    ctx.input = inputAddr;
    ctx.output[ctx.rankId] = outputAddr;
    ctx.token[ctx.rankId] = tokenInfo;
    ctx.offset = offset;
    ctx.sliceSize = sliceSize;

    return CCU_SUCCESS;
}

static CcuResult PreSync(KfcAllGatherMesh1DContext& ctx)
{
    const uint16_t selfBit = 1 << ctx.rankId;
    const uint16_t allOtherBit = ((1 << ctx.rankSize) - 1) & (~selfBit);

    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.output[ctx.rankId], AG_OUTPUT_XN_ID, AG_OUTPUT_CKE_IDX, selfBit));
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.token[ctx.rankId], AG_TOKEN_XN_ID, AG_TOKEN_CKE_IDX, selfBit));
    }

    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], AG_OUTPUT_CKE_IDX, allOtherBit));
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], AG_TOKEN_CKE_IDX, allOtherBit));
    }
    return CCU_SUCCESS;
}

static CcuResult PostSync(KfcAllGatherMesh1DContext& ctx)
{
    const uint16_t selfBit = 1 << ctx.rankId;
    const uint16_t allOtherBit = ((1 << ctx.rankSize) - 1) & (~selfBit);

    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyRecord(ctx.channels[i], AG_POST_SYNC_CKE_IDX, selfBit);
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyWait(ctx.channels[i], AG_POST_SYNC_CKE_IDX, allOtherBit);
    }
    return CCU_SUCCESS;
}

static CcuResult DoAllGather(KfcAllGatherMesh1DContext& ctx)
{
    ccu::LocalAddr src;
    ccu::LocalAddr localDst;
    std::vector<ccu::RemoteAddr> dst;
    dst.resize(ctx.rankSize - 1);

    src.addr = ctx.input;
    src.token = ctx.token[ctx.rankId];
    ccu::Event event;

    uint32_t dstId = 0;
    uint32_t curId = 0;
    for (uint64_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        if (rankIdx != ctx.rankId) {
            curId = dstId;
            dst[curId].addr = ctx.output[rankIdx];
            dst[curId].addr += ctx.offset;
            dst[curId].token = ctx.token[rankIdx];
            dstId++;
        } else {
            localDst.addr = ctx.output[rankIdx];
            localDst.addr += ctx.offset;
            localDst.token = ctx.token[rankIdx];
        }
    }

    uint16_t eventMask = 1 << ctx.rankId;
    ccu::LocalCopy(localDst, src, ctx.sliceSize, event, 1 << ctx.rankId);

    uint32_t channelIdx = 0;
    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        if (rankIdx == ctx.rankId) {
            continue;
        }
        eventMask |= 1 << rankIdx;
        ccu::Write(ctx.channels[channelIdx], dst[channelIdx], src, ctx.sliceSize, event, 1 << rankIdx);
        channelIdx++;
    }
    ccu::EventWait(event, eventMask);
    return CCU_SUCCESS;
}

CcuResult CcuKfcAllGatherMesh1DKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable offset,
    ccu::Variable sliceSize, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId)
{
    KfcAllGatherMesh1DContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    InitCcuKernelCtxBase(ctx);

    CCU_CHK_RET(InitResource(ctx));
    CCU_CHK_RET(LoadArgs(ctx, inputAddr, outputAddr, tokenInfo, offset, sliceSize));
    CCU_CHK_RET(PreSync(ctx));
    CCU_CHK_RET(DoAllGather(ctx));
    CCU_CHK_RET(PostSync(ctx));
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
