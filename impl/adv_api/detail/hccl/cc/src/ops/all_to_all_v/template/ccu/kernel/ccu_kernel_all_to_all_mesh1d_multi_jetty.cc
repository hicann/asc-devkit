/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_all_to_all_mesh1d_multi_jetty.h"

namespace mc2_ops_hccl {
namespace {
constexpr uint16_t A2A_OUTPUT_XN_ID = 1U;
constexpr uint16_t A2A_TOKEN_XN_ID = 2U;
constexpr uint16_t A2A_POST_SYNC_BIT = static_cast<uint16_t>(1U << 5U);
constexpr uint16_t A2A_CKE_IDX = 0U;

struct AllToAllMesh1DMultiJettyContext : CcuKernelCtxBase {
    const ChannelHandle* channels = nullptr;
    uint32_t channelCount = 0;
    uint32_t rankSize = 0;
    uint32_t rankId = 0;
    uint32_t jettyNum = 1U;

    ccu::variable input;
    ccu::variable output;
    ccu::variable token;
    std::vector<ccu::variable> peerOutput;
    std::vector<ccu::variable> peerToken;
    ccu::variable sliceSize;
    ccu::variable srcStride;
    ccu::variable srcOffset;
    ccu::variable dstOffset;
    ccu::variable sliceSizePerJetty;
    ccu::variable lastSliceSizePerJetty;
    GroupOpSizeVars goSize;
    std::vector<ccu::event> eventList;
};

CcuResult InitResource(AllToAllMesh1DMultiJettyContext& ctx)
{
    if (ctx.channelCount == 0U || ctx.rankSize == 0U || ctx.rankId >= ctx.rankSize || ctx.jettyNum == 0U) {
        HCCL_ERROR(
            "[CcuAllToAllMultiJetty] invalid resource, channelCount[%u], rankSize[%u], rankId[%u], jetty[%u]",
            ctx.channelCount, ctx.rankSize, ctx.rankId, ctx.jettyNum);
        return CCU_E_INTERNAL;
    }
    ctx.peerOutput.resize(ctx.rankSize);
    ctx.peerToken.resize(ctx.rankSize);
    ctx.eventList.resize(ctx.rankSize);
    uint32_t channelIdx = 0;
    for (uint32_t peerId = 0; peerId < ctx.rankSize; ++peerId) {
        if (peerId == ctx.rankId) {
            continue;
        }
        ctx.peerOutput[peerId] = ccu::get_res_by_channel<ccu::variable>(ctx.channels[channelIdx], A2A_OUTPUT_XN_ID);
        ctx.peerToken[peerId] = ccu::get_res_by_channel<ccu::variable>(ctx.channels[channelIdx], A2A_TOKEN_XN_ID);
        ++channelIdx;
    }
    ctx.resourceAllocated = false;
    return CCU_SUCCESS;
}

CcuResult PreSync(AllToAllMesh1DMultiJettyContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::write_variable_with_notify(
            ctx.channels[i], ctx.output, A2A_OUTPUT_XN_ID, A2A_CKE_IDX, 1U << A2A_OUTPUT_XN_ID));
        CCU_CHK_RET(ccu::write_variable_with_notify(
            ctx.channels[i], ctx.token, A2A_TOKEN_XN_ID, A2A_CKE_IDX, 1U << A2A_TOKEN_XN_ID));
    }
    const uint16_t waitMask = (1U << A2A_OUTPUT_XN_ID) | (1U << A2A_TOKEN_XN_ID);
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::notify_wait(ctx.channels[i], A2A_CKE_IDX, waitMask));
    }
    return CCU_SUCCESS;
}

CcuResult PostSync(AllToAllMesh1DMultiJettyContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::notify_record(ctx.channels[i], A2A_CKE_IDX, A2A_POST_SYNC_BIT));
    }
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::notify_wait(ctx.channels[i], A2A_CKE_IDX, A2A_POST_SYNC_BIT));
    }
    return CCU_SUCCESS;
}

CcuResult DoAllToAll(AllToAllMesh1DMultiJettyContext& ctx)
{
    ctx.srcOffset += ctx.input;

    uint32_t channelId = 0;
    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; ++rankIdx) {
        if (rankIdx == ctx.rankId) {
            continue;
        }
        ccu::local_addr src;
        src.addr = ctx.srcOffset;
        for (uint32_t i = 0; i < rankIdx; ++i) {
            src.addr += ctx.srcStride;
        }
        src.token = ctx.peerToken[rankIdx];
        ccu::remote_addr dst;
        dst.addr = ctx.peerOutput[rankIdx];
        dst.addr += ctx.dstOffset;
        dst.token = ctx.peerToken[rankIdx];

        CCU_IF(ctx.sliceSizePerJetty != 0)
        {
            for (uint32_t jetty = 0; jetty + 1U < ctx.jettyNum; ++jetty) {
                const uint16_t jettyMask = static_cast<uint16_t>(1U << jetty);
                CCU_CHK_RET(ccu::write(
                    ctx.channels[channelId], dst, src, ctx.sliceSizePerJetty, ctx.eventList[rankIdx], jettyMask));
                src.addr += ctx.sliceSizePerJetty;
                dst.addr += ctx.sliceSizePerJetty;
            }
        }
        CCU_ELSE
        {
            for (uint32_t jetty = 0; jetty + 1U < ctx.jettyNum; ++jetty) {
                CCU_CHK_RET(ccu::event_record(ctx.eventList[rankIdx], static_cast<uint16_t>(1U << jetty)));
            }
        }
        const uint16_t lastMask = static_cast<uint16_t>(1U << (ctx.jettyNum - 1U));
        CCU_IF(ctx.lastSliceSizePerJetty != 0)
        {
            CCU_CHK_RET(ccu::write(
                ctx.channels[channelId], dst, src, ctx.lastSliceSizePerJetty, ctx.eventList[rankIdx], lastMask));
        }
        CCU_ELSE { CCU_CHK_RET(ccu::event_record(ctx.eventList[rankIdx], lastMask)); }
        ++channelId;
    }

    ccu::local_addr localSrc;
    localSrc.addr = ctx.srcOffset;
    for (uint32_t i = 0; i < ctx.rankId; ++i) {
        localSrc.addr += ctx.srcStride;
    }
    localSrc.token = ctx.token;
    ccu::local_addr localDst;
    localDst.addr = ctx.output;
    localDst.addr += ctx.dstOffset;
    localDst.token = ctx.token;
    CCU_IF(ctx.sliceSize != 0)
    {
        CCU_CHK_RET(GroupCopy(ctx, localDst, localSrc, ctx.goSize, GetCcuVersion()));
        CCU_CHK_RET(ccu::event_record(ctx.eventList[ctx.rankId], 1U));
        CCU_CHK_RET(ccu::event_wait(ctx.eventList[ctx.rankId], 1U));
    }
    CCU_CHK_RET(ccu::event_record(ctx.eventList[ctx.rankId], 1U));

    for (uint32_t r = 0; r < ctx.rankSize; ++r) {
        const uint16_t waitMask = (r == ctx.rankId) ? 1U : static_cast<uint16_t>((1U << ctx.jettyNum) - 1U);
        CCU_CHK_RET(ccu::event_wait(ctx.eventList[r], waitMask));
    }
    return CCU_SUCCESS;
}
} // namespace

CcuResult CcuAllToAllMesh1DMultiJettyKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable sliceSize,
    ccu::variable srcStride, ccu::variable srcOffset, ccu::variable dstOffset, ccu::variable goSize0,
    ccu::variable goSize1, ccu::variable goSize2, ccu::variable goSize3, ccu::variable sliceSizePerJetty,
    ccu::variable lastSliceSizePerJetty, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize,
    uint32_t rankId, uint32_t jettyNum)
{
    AllToAllMesh1DMultiJettyContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.jettyNum = jettyNum;
    InitCcuKernelCtxBase(ctx);
    CCU_CHK_RET(InitResource(ctx));

    ctx.input = inputAddr;
    ctx.output = outputAddr;
    ctx.token = tokenInfo;
    ctx.sliceSize = sliceSize;
    ctx.srcStride = srcStride;
    ctx.srcOffset = srcOffset;
    ctx.dstOffset = dstOffset;
    ctx.sliceSizePerJetty = sliceSizePerJetty;
    ctx.lastSliceSizePerJetty = lastSliceSizePerJetty;
    ctx.goSize.addr_offset = goSize0;
    ctx.goSize.loop_param = goSize1;
    ctx.goSize.parallel_param = goSize2;
    ctx.goSize.residual = goSize3;

    CCU_CHK_RET(PreSync(ctx));
    CCU_CHK_RET(DoAllToAll(ctx));
    CCU_CHK_RET(PostSync(ctx));
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
