/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_DATAPLANE_HCOMM
#define CCU_DATAPLANE_HCOMM
#endif
#include "ccu_kernel_all_to_all_mesh1d_a6.h"
#include "ccu_kernel_alg_base.h"

#include <algorithm>

namespace mc2_ops_hccl {

namespace {
constexpr int INPUT_XN_ID = 0;
constexpr int OUTPUT_XN_ID = 1;
constexpr int TOKEN_XN_ID = 2;
constexpr int CKE_IDX_0 = 0;
constexpr int CKE_IDX_1 = 1;
constexpr int CKE_IDX_2 = 2;
constexpr int POST_SYNC_BIT = 5;            // PostSync 阶段使用的 event bit
constexpr uint32_t A2A_BITS_PER_EVENT = 16; // 单个 ccu::Event 的 mask 宽度（uint16_t）

struct AlltoAllMesh1DContext : CcuKernelCtxBase {
    const ChannelHandle* channels;
    uint32_t channelCount;
    uint32_t rankSize;
    uint32_t rankId;
    bool loadFromMem;

    std::vector<ccu::Variable> input;
    std::vector<ccu::Variable> output;
    std::vector<ccu::Variable> token;
    ccu::Variable sliceSize;
    ccu::Variable srcStride;
    ccu::Variable srcOffset;
    ccu::Variable dstOffset;
    GroupOpSizeVars goSize;
};
} // namespace

static CcuResult InitResource(AlltoAllMesh1DContext& ctx)
{
    uint32_t channelIdx = 0;

    if (ctx.channelCount == 0) {
        HCCL_ERROR("[CcuKernelAlltoAllMesh1D] channels is empty!");
        return CcuResult::CCU_E_INTERNAL;
    }

    // 按照rank号从小到大遍历channels，遇到本rank就填充本地资源，否则依次取远端资源，要求算法返回的Link同样是按顺序排列的
    ctx.input.resize(ctx.rankSize);
    ctx.output.resize(ctx.rankSize);
    ctx.token.resize(ctx.rankSize);
    for (uint64_t peerId = 0; peerId < ctx.rankSize; peerId++) {
        if (peerId != ctx.rankId) {
            ctx.input[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], INPUT_XN_ID);
            ctx.output[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], OUTPUT_XN_ID);
            ctx.token[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], TOKEN_XN_ID);
            channelIdx++;
        }
    }

    ctx.resourceAllocated = false;

    return CCU_SUCCESS;
}

static CcuResult LoadArgs(
    AlltoAllMesh1DContext& ctx, ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo,
    ccu::Variable sliceSize, ccu::Variable srcStride, ccu::Variable srcOffset, ccu::Variable dstOffset,
    ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3, ccu::Variable goSize4)
{
    ctx.input[ctx.rankId] = inputAddr;
    ctx.output[ctx.rankId] = outputAddr;
    ctx.token[ctx.rankId] = tokenInfo;
    ctx.sliceSize = sliceSize;
    ctx.srcStride = srcStride;
    ctx.srcOffset = srcOffset;
    ctx.dstOffset = dstOffset;
    ctx.goSize.addrOffset = goSize1;
    ctx.goSize.loopParam = goSize2;
    ctx.goSize.parallelParam = goSize3;
    ctx.goSize.residual = goSize4;
    ctx.srcOffset += ctx.input[ctx.rankId];
    return CCU_SUCCESS;
}

static void PreSync(AlltoAllMesh1DContext& ctx)
{
    HCCL_INFO("[CcuKernelAlltoAllMesh1D] AlltoAllMesh1D PreSync begin.");

    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.output[ctx.rankId], OUTPUT_XN_ID, CKE_IDX_0, 1 << OUTPUT_XN_ID);
        ccu::WriteVariableWithNotify(ctx.channels[i], ctx.token[ctx.rankId], TOKEN_XN_ID, CKE_IDX_0, 1 << TOKEN_XN_ID);
    }

    uint32_t allBit = (1 << OUTPUT_XN_ID) | (1 << TOKEN_XN_ID);
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyWait(ctx.channels[i], CKE_IDX_0, allBit);
    }
    HCCL_INFO("[CcuKernelAlltoAllMesh1D] AlltoAllMesh1D PreSync end.");
}

static void PostSync(AlltoAllMesh1DContext& ctx)
{
    HCCL_INFO("[CcuKernelAlltoAllMesh1D] AlltoAllMesh1D PostSync begin.");

    uint16_t postBit = 1U << POST_SYNC_BIT;
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyRecord(ctx.channels[i], CKE_IDX_1, postBit);
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyWait(ctx.channels[i], CKE_IDX_1, postBit);
    }
    HCCL_INFO("[CcuKernelAlltoAllMesh1D] AlltoAllMesh1D PostSync end.");
}

static CcuResult DoAlltoAll(AlltoAllMesh1DContext& ctx)
{
    HCCL_INFO("DoAlltoAll Start.");
    std::vector<ccu::LocalAddr> src(ctx.rankSize);
    std::vector<ccu::RemoteAddr> dst(ctx.rankSize);
    ccu::LocalAddr localDst;

    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        if (rankIdx != ctx.rankId) {
            dst[rankIdx].token = ctx.token[rankIdx];
            dst[rankIdx].addr = ctx.output[rankIdx];
            dst[rankIdx].addr += ctx.dstOffset;
        } else {
            localDst.token = ctx.token[rankIdx];
            localDst.addr = ctx.output[rankIdx];
            localDst.addr += ctx.dstOffset;
        }

        src[rankIdx].addr = ctx.srcOffset;
        src[rankIdx].token = ctx.token[rankIdx];
        for (uint64_t i = 0; i < rankIdx; i++) {
            src[rankIdx].addr += ctx.srcStride;
        }
    }

    // uint16_t event mask 最多 16 位：rankSize>16 时按 16 rank 一组拆多个 event，逐组收齐。
    // rankSize<=16 保持单 event（与既有路径行为一致）。
    const uint32_t eventNum = (ctx.rankSize + A2A_BITS_PER_EVENT - 1) / A2A_BITS_PER_EVENT;
    std::vector<ccu::Event> events(eventNum);
    auto rankMaskOf = [](uint32_t rankIdx) { return static_cast<uint16_t>(1U << (rankIdx % A2A_BITS_PER_EVENT)); };
    auto eventMaskOfGroup = [&ctx](uint32_t eventIdx) {
        const uint32_t remainingRanks = ctx.rankSize - eventIdx * A2A_BITS_PER_EVENT;
        const uint32_t bitCount = std::min(remainingRanks, A2A_BITS_PER_EVENT);
        return bitCount == A2A_BITS_PER_EVENT ? static_cast<uint16_t>(UINT16_MAX) :
                                                static_cast<uint16_t>((1U << bitCount) - 1);
    };
    auto eventWaitAll = [&events, &eventMaskOfGroup]() -> CcuResult {
        for (uint32_t eventIdx = 0; eventIdx < events.size(); eventIdx++) {
            CCU_CHK_RET(ccu::EventWait(events[eventIdx], eventMaskOfGroup(eventIdx)));
        }
        return CCU_SUCCESS;
    };

    uint32_t channelId = 0;
    if (ctx.loadFromMem) {
        HCCL_INFO("[DoAlltoAll] loadFromMem=true, rankSize[%u] rankId[%u] sliceSize", ctx.rankSize, ctx.rankId);
        for (uint64_t r = 0; r < ctx.rankSize; r++) {
            const uint32_t eventIdx = r / A2A_BITS_PER_EVENT;
            if (r == ctx.rankId) {
                ccu::LocalCopy(localDst, src[r], ctx.sliceSize, events[eventIdx], rankMaskOf(r));
            } else {
                ccu::Write(ctx.channels[channelId], dst[r], src[r], ctx.sliceSize, events[eventIdx], rankMaskOf(r));
                channelId++;
            }
        }
        CCU_CHK_RET(eventWaitAll());
    } else {
        HCCL_INFO("[DoAlltoAll] loadFromMem=false, rankSize[%u] rankId[%u] sliceSize", ctx.rankSize, ctx.rankId);
        for (uint64_t r = 0; r < ctx.rankSize; r++) {
            if (r != ctx.rankId) {
                const uint32_t eventIdx = r / A2A_BITS_PER_EVENT;
                ccu::Write(ctx.channels[channelId], dst[r], src[r], ctx.sliceSize, events[eventIdx], rankMaskOf(r));
                channelId++;
            }
        }
        HCCL_INFO("[KFC][Alltoall][Kernel]Run groupCopy for alltoall.");
        GroupCopy(ctx, localDst, src[ctx.rankId], ctx.goSize, GetCcuVersion());
        // 本 rank 位无需等待（GroupCopy 同步完成），只收远端位
        for (uint32_t eventIdx = 0; eventIdx < events.size(); eventIdx++) {
            uint16_t eventMask = eventMaskOfGroup(eventIdx);
            if (eventIdx == ctx.rankId / A2A_BITS_PER_EVENT) {
                eventMask &= static_cast<uint16_t>(~rankMaskOf(ctx.rankId));
            }
            CCU_CHK_RET(ccu::EventWait(events[eventIdx], eventMask));
        }
    }

    return CCU_SUCCESS;
}

// ============================================================================
// 主入口 Kernel 函数
// ============================================================================
CcuResult CcuAlltoAllMesh1DKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable sliceSize,
    ccu::Variable srcStride, ccu::Variable srcOffset, ccu::Variable dstOffset, ccu::Variable goSize1,
    ccu::Variable goSize2, ccu::Variable goSize3, ccu::Variable goSize4, const ChannelHandle channels[],
    uint32_t channelCount, uint32_t rankSize, uint32_t rankId, bool loadFromMem)
{
    AlltoAllMesh1DContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.loadFromMem = loadFromMem;
    InitCcuKernelCtxBase(ctx);

    HCCL_INFO("[CcuKernelAlltoAllMesh1D] AlltoAllMesh1D run");
    CCU_CHK_RET(InitResource(ctx));
    CCU_CHK_RET(LoadArgs(
        ctx, inputAddr, outputAddr, tokenInfo, sliceSize, srcStride, srcOffset, dstOffset, goSize1, goSize2, goSize3,
        goSize4));

    PreSync(ctx);

    CCU_CHK_RET(DoAlltoAll(ctx));

    PostSync(ctx);
    HCCL_INFO("[CcuKernelAlltoAllMesh1D] AlltoAllMesh1D end");

    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
