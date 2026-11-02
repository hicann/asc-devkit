/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_kfc_all_gather_nhr1d_multi_jetty_mem2mem.h"

namespace mc2_ops_hccl {
namespace {
constexpr uint16_t NHR_OUTPUT_XN_ID = 1U;
constexpr uint16_t NHR_TOKEN_XN_ID = 2U;
constexpr uint16_t NHR_POST_SYNC_BIT = 1U << 3U;
constexpr uint16_t NHR_STEP_PRE_SYNC_BIT = 1U << 4U;
constexpr uint16_t NHR_STEP_POST_SYNC_BIT = 1U << 5U;
constexpr uint16_t NHR_CKE_IDX = 0U;
constexpr uint16_t NHR_MJ_EVENT_BIT_BUDGET = 16U;

struct KfcAllGatherNhrContext : CcuKernelCtxBase {
    const ChannelHandle* channels = nullptr;
    uint32_t channelCount = 0;
    uint32_t rankSize = 0;
    uint32_t rankId = 0;
    uint32_t jettyNum = 1;
    const std::vector<KfcNhrStepInfo>* stepInfoVector = nullptr;
    const std::map<uint32_t, uint32_t>* rank2ChannelIdx = nullptr;

    ccu::Variable input;
    std::vector<ccu::Variable> output;
    std::vector<ccu::Variable> token;
    ccu::Variable sliceSize;
    ccu::Variable sliceSizePerJetty;
    ccu::Variable lastSliceSizePerJetty;
    ccu::Variable repeatNumInv;
    ccu::Variable inputSliceStride;
    ccu::Variable outputSliceStride;
    ccu::Variable inputRepeatStride;
    ccu::Variable outputRepeatStride;
    ccu::Variable isInputOutputEqual;
    GroupOpSizeVars goSize;
    std::vector<ccu::Variable> outputSliceOffset;
    ccu::Variable myRankInputSliceOffset;
    ccu::Variable repeatFlag;
    ccu::Variable repeatNum;
    ccu::Variable constVar1;
    ccu::Event event;
};

CcuResult InitResource(KfcAllGatherNhrContext& ctx)
{
    if (ctx.rankSize == 0U || ctx.rankId >= ctx.rankSize || ctx.channelCount == 0U || ctx.rank2ChannelIdx == nullptr ||
        ctx.rank2ChannelIdx->size() != ctx.channelCount || ctx.jettyNum == 0U || ctx.jettyNum > 16U) {
        HCCL_ERROR(
            "[CcuKfcAllGatherNHR] invalid resource, rankSize[%u], rankId[%u], channels[%u], jetty[%u]", ctx.rankSize,
            ctx.rankId, ctx.channelCount, ctx.jettyNum);
        return CCU_E_INTERNAL;
    }
    ctx.output.resize(ctx.channelCount + 1U);
    ctx.token.resize(ctx.channelCount + 1U);
    for (uint32_t channelIdx = 0; channelIdx < ctx.channelCount; ++channelIdx) {
        ctx.output[channelIdx] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], NHR_OUTPUT_XN_ID);
        ctx.token[channelIdx] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], NHR_TOKEN_XN_ID);
    }
    ctx.outputSliceOffset.resize(ctx.rankSize);
    ctx.resourceAllocated = false;
    return CCU_SUCCESS;
}

CcuResult PreSync(KfcAllGatherNhrContext& ctx)
{
    const uint32_t localIdx = ctx.channelCount;
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.output[localIdx], NHR_OUTPUT_XN_ID, NHR_CKE_IDX, 1U << NHR_OUTPUT_XN_ID));
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.token[localIdx], NHR_TOKEN_XN_ID, NHR_CKE_IDX, 1U << NHR_TOKEN_XN_ID));
    }
    const uint16_t waitMask = (1U << NHR_OUTPUT_XN_ID) | (1U << NHR_TOKEN_XN_ID);
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], NHR_CKE_IDX, waitMask));
    }
    return CCU_SUCCESS;
}

CcuResult PostSync(KfcAllGatherNhrContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::NotifyRecord(ctx.channels[i], NHR_CKE_IDX, NHR_POST_SYNC_BIT));
    }
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], NHR_CKE_IDX, NHR_POST_SYNC_BIT));
    }
    return CCU_SUCCESS;
}

// 只发射单个 slice 的 Write/占位 EventRecord，不等待；事件位从 baseBit 起按 jetty 展开。
// 流水路径由调用方统一分配 baseBit 并在步末批量 EventWait；串行路径 baseBit=0 且逐 slice 等待。
CcuResult IssueSlice(
    KfcAllGatherNhrContext& ctx, uint32_t toRank, ccu::LocalAddr& source, ccu::RemoteAddr& destination,
    uint32_t baseBit)
{
    const ChannelHandle channel = ctx.channels[ctx.rank2ChannelIdx->at(toRank)];
    ccu::LocalAddr src = source;
    ccu::RemoteAddr dst = destination;
    CCU_IF(ctx.sliceSizePerJetty != 0)
    {
        for (uint32_t jetty = 0; jetty + 1U < ctx.jettyNum; ++jetty) {
            const uint16_t mask = 1U << (baseBit + jetty);
            CCU_CHK_RET(ccu::Write(channel, dst, src, ctx.sliceSizePerJetty, ctx.event, mask));
            src.addr += ctx.sliceSizePerJetty;
            dst.addr += ctx.sliceSizePerJetty;
        }
    }
    CCU_ELSE
    {
        for (uint32_t jetty = 0; jetty + 1U < ctx.jettyNum; ++jetty) {
            CCU_CHK_RET(ccu::EventRecord(ctx.event, 1U << (baseBit + jetty)));
        }
    }
    const uint16_t lastMask = 1U << (baseBit + ctx.jettyNum - 1U);
    CCU_IF(ctx.lastSliceSizePerJetty != 0)
    {
        CCU_CHK_RET(ccu::Write(channel, dst, src, ctx.lastSliceSizePerJetty, ctx.event, lastMask));
    }
    CCU_ELSE { CCU_CHK_RET(ccu::EventRecord(ctx.event, lastMask)); }
    return CCU_SUCCESS;
}

// 串行回退路径：发射单个 slice 并立即等待其完成（保持原 WriteSlice 语义）。
CcuResult WriteSlice(KfcAllGatherNhrContext& ctx, uint32_t toRank, ccu::LocalAddr& source, ccu::RemoteAddr& destination)
{
    CCU_CHK_RET(IssueSlice(ctx, toRank, source, destination, 0U));
    CCU_CHK_RET(ccu::EventWait(ctx.event, static_cast<uint16_t>((1U << ctx.jettyNum) - 1U)));
    return CCU_SUCCESS;
}

CcuResult RunStep(KfcAllGatherNhrContext& ctx, const KfcNhrStepInfo& stepInfo)
{
    const uint32_t toChannelIdx = ctx.rank2ChannelIdx->at(stepInfo.toRank);
    const uint32_t fromChannelIdx = ctx.rank2ChannelIdx->at(stepInfo.fromRank);
    const ChannelHandle sendChannel = ctx.channels[toChannelIdx];
    const ChannelHandle recvChannel = ctx.channels[fromChannelIdx];
    const uint32_t localIdx = ctx.channelCount;

    CCU_CHK_RET(ccu::NotifyRecord(recvChannel, NHR_CKE_IDX, NHR_STEP_PRE_SYNC_BIT));
    CCU_CHK_RET(ccu::NotifyWait(sendChannel, NHR_CKE_IDX, NHR_STEP_PRE_SYNC_BIT));

    // 发射期分流（宿主期值必须用普通 if，只发射命中分支）：
    // 在途操作数（slice 数 × jetty 数）不超单 event 16 位预算时走步内流水——本轮所有 slice
    // 先全部发射（各占独立事件位；各 slice 的 src/dst 分片互不相交，无数据依赖），
    // 步末一次 EventWait 批量等完成，消除"发一个等一个"的完成等待空转间隙。
    // 先例：RS NHR IssueReduceSlice、SoleNHR AG IssueSendSlice 的步内流水。
    const uint32_t sliceNum = static_cast<uint32_t>(stepInfo.txSliceIdxs.size());
    const uint32_t inflightOps = sliceNum * ctx.jettyNum;
    if (sliceNum != 0U && inflightOps <= NHR_MJ_EVENT_BIT_BUDGET) {
        // 流水路径 repeat 外层、slice 内层（与串行路径嵌套相反）：同一 repeat 轮内所有 slice
        // 共享一组事件位，轮末批量等待清位后再进下一轮。
        const uint16_t waitMask = static_cast<uint16_t>((1U << inflightOps) - 1U);
        ccu::Variable turnOffsetIn;
        ccu::Variable turnOffsetOut;
        turnOffsetIn = 0;
        turnOffsetOut = 0;
        ctx.repeatNum = ctx.repeatNumInv;
        CCU_WHILE(ctx.repeatNum != UINT64_MAX)
        {
            ctx.repeatNum += ctx.constVar1;
            for (uint32_t sliceSeq = 0; sliceSeq < sliceNum; ++sliceSeq) {
                const uint32_t sliceIdx = stepInfo.txSliceIdxs[sliceSeq];
                ccu::LocalAddr src;
                if (sliceIdx == ctx.rankId) {
                    src.addr = ctx.input;
                    src.addr += ctx.myRankInputSliceOffset;
                } else {
                    src.addr = ctx.output[localIdx];
                    src.addr += ctx.outputSliceOffset[sliceIdx];
                }
                src.addr += turnOffsetIn;
                src.token = ctx.token[localIdx];
                ccu::RemoteAddr dst;
                dst.addr = ctx.output[toChannelIdx];
                dst.addr += ctx.outputSliceOffset[sliceIdx];
                dst.addr += turnOffsetOut;
                dst.token = ctx.token[toChannelIdx];
                CCU_CHK_RET(IssueSlice(ctx, stepInfo.toRank, src, dst, sliceSeq * ctx.jettyNum));
            }
            CCU_CHK_RET(ccu::EventWait(ctx.event, waitMask));
            turnOffsetIn += ctx.inputRepeatStride;
            turnOffsetOut += ctx.outputRepeatStride;
        }
    } else {
        // 串行回退（slice×jetty 超 16 位预算或空步）：保持原"发一个等一个"结构。
        for (uint32_t sliceIdx : stepInfo.txSliceIdxs) {
            ccu::LocalAddr src;
            if (sliceIdx == ctx.rankId) {
                src.addr = ctx.input;
                src.addr += ctx.myRankInputSliceOffset;
            } else {
                src.addr = ctx.output[localIdx];
                src.addr += ctx.outputSliceOffset[sliceIdx];
            }
            src.token = ctx.token[localIdx];
            ccu::RemoteAddr dst;
            dst.addr = ctx.output[toChannelIdx];
            dst.addr += ctx.outputSliceOffset[sliceIdx];
            dst.token = ctx.token[toChannelIdx];

            ctx.repeatFlag = 0;
            ctx.repeatNum = ctx.repeatNumInv;
            CCU_WHILE(ctx.repeatNum != UINT64_MAX)
            {
                ctx.repeatNum += ctx.constVar1;
                CCU_IF(ctx.repeatFlag != 0)
                {
                    src.addr += ctx.inputRepeatStride;
                    dst.addr += ctx.outputRepeatStride;
                }
                CCU_CHK_RET(WriteSlice(ctx, stepInfo.toRank, src, dst));
                ctx.repeatFlag = 1U;
            }
        }
    }
    CCU_CHK_RET(ccu::NotifyRecord(sendChannel, NHR_CKE_IDX, NHR_STEP_POST_SYNC_BIT));
    CCU_CHK_RET(ccu::NotifyWait(recvChannel, NHR_CKE_IDX, NHR_STEP_POST_SYNC_BIT));
    return CCU_SUCCESS;
}

CcuResult RunNhr(KfcAllGatherNhrContext& ctx)
{
    ccu::Variable sliceOffset;
    sliceOffset = 0;
    ctx.myRankInputSliceOffset = 0;
    for (uint32_t rank = 0; rank < ctx.rankId; ++rank) {
        ctx.myRankInputSliceOffset += ctx.inputSliceStride;
    }
    for (uint32_t rank = 0; rank < ctx.rankSize; ++rank) {
        ctx.outputSliceOffset[rank] = sliceOffset;
        sliceOffset += ctx.outputSliceStride;
    }
    for (const auto& stepInfo : *ctx.stepInfoVector) {
        CCU_CHK_RET(RunStep(ctx, stepInfo));
    }

    const uint32_t localIdx = ctx.channelCount;
    ccu::LocalAddr src;
    src.addr = ctx.input;
    src.addr += ctx.myRankInputSliceOffset;
    src.token = ctx.token[localIdx];
    ccu::LocalAddr dst;
    dst.addr = ctx.output[localIdx];
    dst.addr += ctx.outputSliceOffset[ctx.rankId];
    dst.token = ctx.token[localIdx];
    ctx.repeatFlag = 0;
    ctx.repeatNum = ctx.repeatNumInv;
    CCU_WHILE(ctx.repeatNum != UINT64_MAX)
    {
        ctx.repeatNum += ctx.constVar1;
        CCU_IF(ctx.repeatFlag != 0)
        {
            src.addr += ctx.inputRepeatStride;
            dst.addr += ctx.outputRepeatStride;
        }
        // WriteSlice 已等待并清除 event；本地 copy 每次也立即等待，复用 bit0。
        // 不能用 1<<rankId：NHR 子域可以超过 16 rank，而 event mask 只有 16 位。
        constexpr uint16_t rankMask = 1U;
        CCU_IF(ctx.isInputOutputEqual == 0)
        {
            CCU_CHK_RET(GroupCopy(ctx, dst, src, ctx.goSize, GetCcuVersion()));
            CCU_CHK_RET(ccu::EventRecord(ctx.event, rankMask));
        }
        CCU_ELSE { CCU_CHK_RET(ccu::EventRecord(ctx.event, rankMask)); }
        CCU_CHK_RET(ccu::EventWait(ctx.event, rankMask));
        ctx.repeatFlag = 1U;
    }
    return CCU_SUCCESS;
}

CcuResult LoadNhrArgs(
    KfcAllGatherNhrContext& ctx, ccu::Variable input, ccu::Variable output, ccu::Variable token,
    ccu::Variable sliceSize, ccu::Variable sliceSizePerJetty, ccu::Variable lastSliceSizePerJetty,
    ccu::Variable repeatNumInv, ccu::Variable inputSliceStride, ccu::Variable outputSliceStride,
    ccu::Variable inputRepeatStride, ccu::Variable outputRepeatStride, ccu::Variable isInputOutputEqual,
    ccu::Variable goSize0, ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3)
{
    const uint32_t localIdx = ctx.channelCount;
    ctx.input = input;
    ctx.output[localIdx] = output;
    ctx.token[localIdx] = token;
    ctx.sliceSize = sliceSize;
    ctx.sliceSizePerJetty = sliceSizePerJetty;
    ctx.lastSliceSizePerJetty = lastSliceSizePerJetty;
    ctx.repeatNumInv = repeatNumInv;
    ctx.inputSliceStride = inputSliceStride;
    ctx.outputSliceStride = outputSliceStride;
    ctx.inputRepeatStride = inputRepeatStride;
    ctx.outputRepeatStride = outputRepeatStride;
    ctx.isInputOutputEqual = isInputOutputEqual;
    ctx.goSize.addrOffset = goSize0;
    ctx.goSize.loopParam = goSize1;
    ctx.goSize.parallelParam = goSize2;
    ctx.goSize.residual = goSize3;
    ctx.constVar1 = 1U;
    return CCU_SUCCESS;
}

void AddVariableNTimes(ccu::Variable& result, const ccu::Variable& value, uint32_t times)
{
    result = 0U;
    for (uint32_t i = 0; i < times; ++i) {
        result += value;
    }
}
} // namespace

CcuResult CcuKfcAllGatherNHR1DMultiJettyMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable sliceSize,
    ccu::Variable sliceSizePerJetty, ccu::Variable lastSliceSizePerJetty, ccu::Variable repeatNumInv,
    ccu::Variable inputSliceStride, ccu::Variable outputSliceStride, ccu::Variable inputRepeatStride,
    ccu::Variable outputRepeatStride, ccu::Variable isInputOutputEqual, ccu::Variable goSize0, ccu::Variable goSize1,
    ccu::Variable goSize2, ccu::Variable goSize3, const ChannelHandle channels[], uint32_t channelCount,
    uint32_t rankSize, uint32_t rankId, uint32_t jettyNum, const std::vector<KfcNhrStepInfo>& stepInfoVector,
    const std::map<uint32_t, uint32_t>& rank2ChannelIdx)
{
    KfcAllGatherNhrContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.jettyNum = jettyNum;
    ctx.stepInfoVector = &stepInfoVector;
    ctx.rank2ChannelIdx = &rank2ChannelIdx;
    InitCcuKernelCtxBase(ctx);
    CCU_CHK_RET(InitResource(ctx));

    CCU_CHK_RET(LoadNhrArgs(
        ctx, inputAddr, outputAddr, tokenInfo, sliceSize, sliceSizePerJetty, lastSliceSizePerJetty, repeatNumInv,
        inputSliceStride, outputSliceStride, inputRepeatStride, outputRepeatStride, isInputOutputEqual, goSize0,
        goSize1, goSize2, goSize3));

    CCU_CHK_RET(PreSync(ctx));
    CCU_IF(ctx.sliceSize != 0) { CCU_CHK_RET(RunNhr(ctx)); }
    CCU_CHK_RET(PostSync(ctx));
    return CCU_SUCCESS;
}

CcuResult CcuKfcParallelAllGatherNHR1DMultiJettyMem2MemKernel(
    ccu::Variable inputBase, ccu::Variable outputBase, ccu::Variable tokenInfo, ccu::Variable outputStride,
    ccu::Variable part0Size, ccu::Variable part1Size, ccu::Variable part1Offset, ccu::Variable meshPhaseDoneAddr,
    ccu::Variable nhrPhaseDoneAddr, ccu::Variable part0SliceSizePerJetty, ccu::Variable part0LastSliceSizePerJetty,
    ccu::Variable part1SliceSizePerJetty, ccu::Variable part1LastSliceSizePerJetty, ccu::Variable part0GoSize0,
    ccu::Variable part0GoSize1, ccu::Variable part0GoSize2, ccu::Variable part0GoSize3, ccu::Variable part1GoSize0,
    ccu::Variable part1GoSize1, ccu::Variable part1GoSize2, ccu::Variable part1GoSize3, const ChannelHandle channels[],
    uint32_t channelCount, uint32_t rankSizeLevel0, uint32_t rankIdxLevel0, uint32_t rankSizeLevel1,
    uint32_t rankIdxLevel1, uint32_t jettyNum, const std::vector<KfcNhrStepInfo>& stepInfoVector,
    const std::map<uint32_t, uint32_t>& rank2ChannelIdx)
{
    KfcAllGatherNhrContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSizeLevel1;
    ctx.rankId = rankIdxLevel1;
    ctx.jettyNum = jettyNum;
    ctx.stepInfoVector = &stepInfoVector;
    ctx.rank2ChannelIdx = &rank2ChannelIdx;
    InitCcuKernelCtxBase(ctx);
    CCU_CHK_RET(InitResource(ctx));

    ccu::Variable zero;
    ccu::Variable one;
    ccu::Variable level1Stride;
    ccu::Variable phase0Input;
    ccu::Variable phase0Output;
    ccu::Variable phase1RepeatNumInv;
    ccu::Variable phase0RankOffset;
    ccu::Variable phase0RepeatNumInv;
    zero = 0U;
    one = 1U;
    AddVariableNTimes(level1Stride, outputStride, rankSizeLevel0);
    phase0Input = inputBase + part1Offset;
    AddVariableNTimes(phase0RankOffset, outputStride, rankIdxLevel0);
    phase0Output = outputBase + phase0RankOffset;
    phase0Output += part1Offset;
    phase1RepeatNumInv = UINT64_MAX - rankSizeLevel0;
    phase0RepeatNumInv = UINT64_MAX - 1U;

    // Stage 0: NHR gathers part1 along the level-1 (CLOS) dimension.
    CCU_CHK_RET(LoadNhrArgs(
        ctx, phase0Input, phase0Output, tokenInfo, part1Size, part1SliceSizePerJetty, part1LastSliceSizePerJetty,
        phase0RepeatNumInv, zero, level1Stride, zero, zero, zero, part1GoSize0, part1GoSize1, part1GoSize2,
        part1GoSize3));
    CCU_CHK_RET(PreSync(ctx));
    CCU_IF(part1Size != 0) { CCU_CHK_RET(RunNhr(ctx)); }
    CCU_CHK_RET(PostSync(ctx));

    CCU_CHK_RET(ccu::Store(nhrPhaseDoneAddr, one));
    ccu::Variable meshDone;
    meshDone = 0U;
    CCU_WHILE(meshDone != 1U) { CCU_CHK_RET(ccu::Load(meshPhaseDoneAddr, meshDone)); }

    // Stage 1: NHR propagates part0, which stage-0 Mesh has already gathered.
    CCU_CHK_RET(LoadNhrArgs(
        ctx, outputBase, outputBase, tokenInfo, part0Size, part0SliceSizePerJetty, part0LastSliceSizePerJetty,
        phase1RepeatNumInv, level1Stride, level1Stride, outputStride, outputStride, one, part0GoSize0, part0GoSize1,
        part0GoSize2, part0GoSize3));
    CCU_CHK_RET(PreSync(ctx));
    CCU_IF(part0Size != 0) { CCU_CHK_RET(RunNhr(ctx)); }
    CCU_CHK_RET(PostSync(ctx));
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
