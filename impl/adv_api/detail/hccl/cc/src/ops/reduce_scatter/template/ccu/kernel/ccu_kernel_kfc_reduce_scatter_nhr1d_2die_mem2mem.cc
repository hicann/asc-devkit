/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_kfc_reduce_scatter_nhr1d_2die_mem2mem.h"

namespace mc2_ops_hccl {
namespace {
// XN 槽位与 CKE 信号位定义，与 hccl CcuReduceScatterNHR1DMem2MemKernel 保持一致。
constexpr uint16_t INPUT_XN_ID = 0;
constexpr uint16_t TOKEN_XN_ID = 2;
constexpr uint16_t POST_SYNC_ID = 3;
constexpr uint16_t STEP_PRE_SYNC_ID = 4;
constexpr uint16_t STEP_POST_SYNC_ID = 5;

constexpr uint16_t CKE_IDX_0 = 0;

// 步内流水的在途操作数（slice 数）超出单 event 16 位预算时回退逐 slice 串行等待（MultiLink 同款）。
constexpr uint32_t RS_NHR_2DIE_EVENT_BIT_BUDGET = 16U;

struct KfcReduceScatterNhr2DieContext : CcuKernelCtxBase {
    const ChannelHandle* channels = nullptr;
    uint32_t channelCount = 0;
    uint32_t rankSize = 0;
    uint32_t rankId = 0; // 子通信域虚拟 rankid
    uint32_t axisId = 0; // die 维编号：0=die0, 1=die1
    uint32_t axisSize = 0;
    HcclDataType dataType = HcclDataType::HCCL_DATA_TYPE_RESERVED;
    HcclDataType outputDataType = HcclDataType::HCCL_DATA_TYPE_RESERVED;
    HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
    const std::vector<KfcNhrStepInfo>* stepInfoVector = nullptr;
    const std::map<uint32_t, uint32_t>* rank2ChannelIdx = nullptr;

    std::vector<ccu::variable> input;
    ccu::variable output;
    std::vector<ccu::variable> token;
    ccu::variable die0Size;
    ccu::variable die1Size;
    ccu::variable die0LastSliceSize;
    ccu::variable die1LastSliceSize;
    ccu::variable inputSliceStride;
    ccu::variable currentRankSliceOutputOffset;
    ccu::variable inputRepeatStride;
    ccu::variable outputRepeatStride;
    ccu::variable repeatNumVar;
    ccu::variable repeatNumVarTemp;
    ccu::variable isInputOutputEqual;
    GroupOpSizeVars goSizeNormal;
    GroupOpSizeVars goSizeLast;
    GroupOpSizeVars die1GoSizeNormal;
    GroupOpSizeVars die1GoSizeLast;
    ccu::variable isRepeatIter;
    ccu::variable sliceSize;

    ccu::event event;
    ccu::local_addr localSrc_;
    ccu::local_addr localDst_;
    ccu::remote_addr remoteDst_;
};

CcuResult InitResource(KfcReduceScatterNhr2DieContext& ctx)
{
    if (ctx.channelCount == 0U || ctx.rank2ChannelIdx == nullptr || ctx.rank2ChannelIdx->size() != ctx.channelCount ||
        ctx.axisId >= ctx.axisSize || ctx.axisSize == 0U) {
        HCCL_ERROR(
            "[CcuKfcReduceScatterNHR1D2Die] invalid resource, channels[%u], axisId[%u], axisSize[%u]", ctx.channelCount,
            ctx.axisId, ctx.axisSize);
        return CCU_E_INTERNAL;
    }
    const uint32_t localIdx = ctx.channelCount;
    ctx.input.resize(localIdx + 1U);
    ctx.token.resize(localIdx + 1U);
    for (uint32_t channelIdx = 0; channelIdx < ctx.channelCount; ++channelIdx) {
        ctx.input[channelIdx] = ccu::get_res_by_channel<ccu::variable>(ctx.channels[channelIdx], INPUT_XN_ID);
        ctx.token[channelIdx] = ccu::get_res_by_channel<ccu::variable>(ctx.channels[channelIdx], TOKEN_XN_ID);
    }
    ctx.resourceAllocated = false;
    return CCU_SUCCESS;
}

CcuResult PreSync(KfcReduceScatterNhr2DieContext& ctx)
{
    const uint32_t localIdx = ctx.channelCount;
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::write_variable_with_notify(
            ctx.channels[i], ctx.input[localIdx], INPUT_XN_ID, CKE_IDX_0, 1 << INPUT_XN_ID));
        CCU_CHK_RET(ccu::write_variable_with_notify(
            ctx.channels[i], ctx.token[localIdx], TOKEN_XN_ID, CKE_IDX_0, 1 << TOKEN_XN_ID));
    }
    uint32_t allBit = 1 << INPUT_XN_ID | 1 << TOKEN_XN_ID;
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::notify_wait(ctx.channels[i], CKE_IDX_0, allBit));
    }
    return CCU_SUCCESS;
}

CcuResult PostSync(KfcReduceScatterNhr2DieContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::notify_record(ctx.channels[i], CKE_IDX_0, 1 << POST_SYNC_ID));
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::notify_wait(ctx.channels[i], CKE_IDX_0, 1 << POST_SYNC_ID));
    }
    return CCU_SUCCESS;
}

CcuResult DoRepeatWriteReduceSlices(
    KfcReduceScatterNhr2DieContext& ctx, const u32& toRank, ccu::local_addr& src, ccu::remote_addr& dst,
    const bool islastSlice)
{
    ccu::variable repeatNumAdd;
    repeatNumAdd = 1;
    ctx.isRepeatIter = 0;

    auto toRankIt = ctx.rank2ChannelIdx->find(toRank);
    if (toRankIt == ctx.rank2ChannelIdx->end()) {
        HCCL_ERROR("[CcuKfcReduceScatterNHR1D2Die] rank2ChannelIdx not find toRank key [%u]", toRank);
        return CCU_E_PARA;
    }
    const u32& toRankIdx = toRankIt->second;
    ChannelHandle sendChannel = ctx.channels[toRankIdx];

    ctx.repeatNumVarTemp = ctx.repeatNumVar;
    CCU_WHILE(ctx.repeatNumVarTemp != UINT64_MAX)
    {
        CCU_IF(ctx.repeatNumVarTemp != UINT64_MAX) { ctx.repeatNumVarTemp += repeatNumAdd; }

        CCU_IF(ctx.isRepeatIter == 1)
        {
            src.addr += ctx.inputRepeatStride;
            dst.addr += ctx.inputRepeatStride;
        }
        CCU_IF(ctx.isRepeatIter == 0)
        {
            if (ctx.axisId == 1) {
                src.addr += ctx.die0Size;
                dst.addr += ctx.die0Size;
            }
        }
        ctx.sliceSize = (ctx.axisId == 0) ? (islastSlice ? ctx.die0LastSliceSize : ctx.die0Size) :
                                            (islastSlice ? ctx.die1LastSliceSize : ctx.die1Size);

        CCU_IF(ctx.sliceSize != 0)
        {
            CCU_CHK_RET(
                ccu::write_reduce(sendChannel, dst, src, ctx.sliceSize, ctx.dataType, ctx.reduceOp, ctx.event, 1));
        }
        CCU_IF(ctx.sliceSize == 0) { CCU_CHK_RET(ccu::event_record(ctx.event, 1)); }
        CCU_CHK_RET(ccu::event_wait(ctx.event, 1));
        ctx.isRepeatIter = 1;
    }
    ctx.isRepeatIter = 0;
    return CCU_SUCCESS;
}

// 只发射单个 slice 的 write_reduce/占位 event_record，不等待；事件位 = 1 << sliceSeq。
// 流水路径由调用方在步末统一 event_wait；串行路径不经过此函数。
CcuResult IssueReduceSlice2Die(
    KfcReduceScatterNhr2DieContext& ctx, const ChannelHandle& sendChannel, ccu::local_addr& src, ccu::remote_addr& dst,
    ccu::variable& sliceSize, uint32_t sliceSeq)
{
    CCU_IF(sliceSize != 0)
    {
        CCU_CHK_RET(
            ccu::write_reduce(sendChannel, dst, src, sliceSize, ctx.dataType, ctx.reduceOp, ctx.event, 1U << sliceSeq));
    }
    CCU_IF(sliceSize == 0) { CCU_CHK_RET(ccu::event_record(ctx.event, 1U << sliceSeq)); }
    return CCU_SUCCESS;
}

CcuResult DoRepeatReduceScatterNHRSingleStep(
    KfcReduceScatterNhr2DieContext& ctx, const KfcNhrStepInfo& nhrStepInfo,
    const std::vector<ccu::variable>& inputSliceOffset)
{
    auto toRankIt = ctx.rank2ChannelIdx->find(nhrStepInfo.toRank);
    if (toRankIt == ctx.rank2ChannelIdx->end()) {
        HCCL_ERROR("[CcuKfcReduceScatterNHR1D2Die] rank2ChannelIdx not find toRank key [%u]", nhrStepInfo.toRank);
        return CCU_E_PARA;
    }
    const u32& toRankIdx = toRankIt->second;

    auto fromRankIt = ctx.rank2ChannelIdx->find(nhrStepInfo.fromRank);
    if (fromRankIt == ctx.rank2ChannelIdx->end()) {
        HCCL_ERROR("[CcuKfcReduceScatterNHR1D2Die] rank2ChannelIdx not find fromRank key [%u]", nhrStepInfo.fromRank);
        return CCU_E_PARA;
    }
    const u32& fromRankIdx = fromRankIt->second;

    ChannelHandle sendChannel = ctx.channels[toRankIdx];
    ChannelHandle recvChannel = ctx.channels[fromRankIdx];
    const uint32_t localIdx = ctx.channelCount;
    ctx.remoteDst_.token = ctx.token[toRankIdx];
    ctx.localSrc_.token = ctx.token[localIdx];

    bool islastSlice = false;

    // step 非 0 时先做步间就绪同步，保证对端上一轮读取已完成；step 0 的 PreSync 基址交换
    // 刚做过全屏障，跳过（MultiLink kernel 同款先例）
    if (nhrStepInfo.step != 0U) {
        CCU_CHK_RET(ccu::notify_record(recvChannel, CKE_IDX_0, 1 << STEP_PRE_SYNC_ID));
        CCU_CHK_RET(ccu::notify_wait(sendChannel, CKE_IDX_0, 1 << STEP_PRE_SYNC_ID));
    }
    // 发射期分流（宿主期值必须用普通 if，只发射命中分支）：slice 数不超单 event 16 位预算时
    // 走步内流水——本轮所有 slice 先全部发射（各占独立事件位；各 slice 的 src/dst 分片互不相交，
    // 无数据依赖），步末一次 EventWait 批量等完成，消除"发一个等一个"的完成等待空转。
    const uint32_t sliceNum = static_cast<uint32_t>(nhrStepInfo.txSliceIdxs.size());
    if (sliceNum != 0U && sliceNum <= RS_NHR_2DIE_EVENT_BIT_BUDGET) {
        const uint16_t waitMask = static_cast<uint16_t>((1U << sliceNum) - 1U);
        // die1 段起始偏移一次性折叠进 turnOffset（axisId 是 emission 期 kernelArg 常量，普通 if）
        ccu::variable turnOffset;
        ccu::variable repeatNumAdd;
        turnOffset = 0;
        repeatNumAdd = 1;
        if (ctx.axisId == 1) {
            turnOffset += ctx.die0Size;
        }
        ctx.repeatNumVarTemp = ctx.repeatNumVar;
        CCU_WHILE(ctx.repeatNumVarTemp != UINT64_MAX)
        {
            ctx.repeatNumVarTemp += repeatNumAdd;
            for (uint32_t sliceSeq = 0; sliceSeq < sliceNum; ++sliceSeq) {
                const uint32_t sendSliceIdx = nhrStepInfo.txSliceIdxs[sliceSeq];
                ctx.remoteDst_.addr = ctx.input[toRankIdx];
                ctx.remoteDst_.addr += inputSliceOffset[sendSliceIdx];
                ctx.remoteDst_.addr += turnOffset;
                ctx.localSrc_.addr = ctx.input[localIdx];
                ctx.localSrc_.addr += inputSliceOffset[sendSliceIdx];
                ctx.localSrc_.addr += turnOffset;
                islastSlice = (sendSliceIdx + 1 == ctx.rankSize);
                ccu::variable& sliceSizeVar = (ctx.axisId == 0) ? (islastSlice ? ctx.die0LastSliceSize : ctx.die0Size) :
                                                                  (islastSlice ? ctx.die1LastSliceSize : ctx.die1Size);
                CCU_CHK_RET(
                    IssueReduceSlice2Die(ctx, sendChannel, ctx.localSrc_, ctx.remoteDst_, sliceSizeVar, sliceSeq));
            }
            CCU_CHK_RET(ccu::event_wait(ctx.event, waitMask));
            turnOffset += ctx.inputRepeatStride;
        }
    } else {
        // 串行回退（slice 超 16 位预算或空步）：保持原"发一个等一个"结构。
        for (const u32& sendSliceIdx : nhrStepInfo.txSliceIdxs) {
            ctx.remoteDst_.addr = ctx.input[toRankIdx];
            ctx.remoteDst_.addr += inputSliceOffset[sendSliceIdx];
            ctx.localSrc_.addr = ctx.input[localIdx];
            ctx.localSrc_.addr += inputSliceOffset[sendSliceIdx];

            islastSlice = (sendSliceIdx + 1 == ctx.rankSize);
            CCU_CHK_RET(DoRepeatWriteReduceSlices(ctx, nhrStepInfo.toRank, ctx.localSrc_, ctx.remoteDst_, islastSlice));
        }
    }

    // 通知toRank数据写入完毕
    CCU_CHK_RET(ccu::notify_record(sendChannel, CKE_IDX_0, 1 << STEP_POST_SYNC_ID));

    // 等待fromRank通知数据写入完毕
    CCU_CHK_RET(ccu::notify_wait(recvChannel, CKE_IDX_0, 1 << STEP_POST_SYNC_ID));

    return CCU_SUCCESS;
}

CcuResult DoRepeatReduceScatterNHR(KfcReduceScatterNhr2DieContext& ctx)
{
    ccu::variable tmpSliceOffset;
    tmpSliceOffset = 0;
    // 用来记录每个rank要读取的rank的sliceIdx的偏移
    // 后面会用inputAddr来加上这个偏移获取sliceIdx的地址
    std::vector<ccu::variable> inputSliceOffset;
    for (u64 i = 0; i < ctx.rankSize; i++) {
        inputSliceOffset.push_back(ccu::variable{});
        inputSliceOffset[i] = tmpSliceOffset;
        tmpSliceOffset += ctx.inputSliceStride;
    }

    for (const auto& nhrStepInfo : *ctx.stepInfoVector) {
        CCU_CHK_RET(DoRepeatReduceScatterNHRSingleStep(ctx, nhrStepInfo, inputSliceOffset));
    }
    // 因为所有的修改都是在input上进行的，所以最后需要把input上的数据搬到output上
    const uint32_t localIdx = ctx.channelCount;
    ctx.localSrc_.addr = ctx.input[localIdx];
    ctx.localSrc_.addr += inputSliceOffset[ctx.rankId];
    ctx.localSrc_.token = ctx.token[localIdx];
    ctx.localDst_.addr = ctx.output;
    ctx.localDst_.addr += ctx.currentRankSliceOutputOffset;
    ctx.localDst_.token = ctx.token[localIdx];

    ccu::variable repeatNumAdd2;
    bool islastSlice = (ctx.rankId + 1 == ctx.rankSize);
    repeatNumAdd2 = 1;
    ctx.isRepeatIter = 0;
    CCU_WHILE(ctx.repeatNumVar != UINT64_MAX)
    {
        ctx.repeatNumVar += repeatNumAdd2;
        CCU_IF(ctx.isRepeatIter == 1)
        {
            ctx.localSrc_.addr += ctx.inputRepeatStride;
            ctx.localDst_.addr += ctx.outputRepeatStride;
        }
        CCU_IF(ctx.isRepeatIter == 0)
        {
            if (ctx.axisId == 1) {
                ctx.localSrc_.addr += ctx.die0Size;
                ctx.localDst_.addr += ctx.die0Size;
            }
        }
        ccu::variable& localSliceSize = (ctx.axisId == 0) ? (islastSlice ? ctx.die0LastSliceSize : ctx.die0Size) :
                                                            (islastSlice ? ctx.die1LastSliceSize : ctx.die1Size);
        GroupOpSizeVars& goSize = (ctx.axisId == 0) ? (islastSlice ? ctx.goSizeLast : ctx.goSizeNormal) :
                                                      (islastSlice ? ctx.die1GoSizeLast : ctx.die1GoSizeNormal);
        CCU_IF(localSliceSize != 0)
        {
            CCU_IF(ctx.isInputOutputEqual == 0)
            {
                ccu::local_addr localDst;
                localDst.addr = ctx.localDst_.addr;
                localDst.token = ctx.localDst_.token;
                ccu::local_addr localSrc;
                localSrc.addr = ctx.localSrc_.addr;
                localSrc.token = ctx.localSrc_.token;
                CCU_CHK_RET(GroupCopy(ctx, localDst, localSrc, goSize));
            }
        }
        ctx.isRepeatIter = 1;
    }

    return CCU_SUCCESS;
}
} // namespace

CcuResult CcuKfcReduceScatterNHR1D2DieMem2MemKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable die0Size,
    ccu::variable die1Size, ccu::variable die0LastSliceSize, ccu::variable die1LastSliceSize,
    ccu::variable inputSliceStride, ccu::variable currentRankSliceOutputOffset, ccu::variable inputRepeatStride,
    ccu::variable outputRepeatStride, ccu::variable repeatNumVar, ccu::variable isInputOutputEqual,
    ccu::variable goSizeNormalAddrOffset, ccu::variable goSizeNormalLoopParam, ccu::variable goSizeNormalParallelParam,
    ccu::variable goSizeNormalResidual, ccu::variable goSizeLastAddrOffset, ccu::variable goSizeLastLoopParam,
    ccu::variable goSizeLastParallelParam, ccu::variable goSizeLastResidual, ccu::variable die1GoSizeNormalAddrOffset,
    ccu::variable die1GoSizeNormalLoopParam, ccu::variable die1GoSizeNormalParallelParam,
    ccu::variable die1GoSizeNormalResidual, ccu::variable die1GoSizeLastAddrOffset,
    ccu::variable die1GoSizeLastLoopParam, ccu::variable die1GoSizeLastParallelParam,
    ccu::variable die1GoSizeLastResidual, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize,
    uint32_t rankId, uint32_t axisId, uint32_t axisSize, const HcclDataType& dataType, const HcclDataType& outputType,
    const HcclReduceOp& reduceType, const std::vector<KfcNhrStepInfo>& stepInfoVector,
    const std::map<uint32_t, uint32_t>& rank2ChannelIdx)
{
    KfcReduceScatterNhr2DieContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.axisId = axisId;
    ctx.axisSize = axisSize;
    ctx.dataType = dataType;
    ctx.outputDataType = outputType;
    if (ctx.outputDataType == HcclDataType::HCCL_DATA_TYPE_RESERVED) {
        ctx.outputDataType = dataType;
    }
    ctx.reduceOp = reduceType;
    ctx.stepInfoVector = &stepInfoVector;
    ctx.rank2ChannelIdx = &rank2ChannelIdx;
    InitCcuKernelCtxBase(ctx);
    CCU_CHK_RET(InitResource(ctx));

    const uint32_t localIdx = channelCount;
    ctx.input[localIdx] = inputAddr;
    ctx.output = outputAddr;
    ctx.token[localIdx] = tokenInfo;
    ctx.die0Size = die0Size;
    ctx.die1Size = die1Size;
    ctx.die0LastSliceSize = die0LastSliceSize;
    ctx.die1LastSliceSize = die1LastSliceSize;
    ctx.inputSliceStride = inputSliceStride;
    ctx.currentRankSliceOutputOffset = currentRankSliceOutputOffset;
    ctx.inputRepeatStride = inputRepeatStride;
    ctx.outputRepeatStride = outputRepeatStride;
    ctx.repeatNumVar = repeatNumVar;
    ctx.isInputOutputEqual = isInputOutputEqual;
    ctx.goSizeNormal.addr_offset = goSizeNormalAddrOffset;
    ctx.goSizeNormal.loop_param = goSizeNormalLoopParam;
    ctx.goSizeNormal.parallel_param = goSizeNormalParallelParam;
    ctx.goSizeNormal.residual = goSizeNormalResidual;
    ctx.goSizeLast.addr_offset = goSizeLastAddrOffset;
    ctx.goSizeLast.loop_param = goSizeLastLoopParam;
    ctx.goSizeLast.parallel_param = goSizeLastParallelParam;
    ctx.goSizeLast.residual = goSizeLastResidual;
    ctx.die1GoSizeNormal.addr_offset = die1GoSizeNormalAddrOffset;
    ctx.die1GoSizeNormal.loop_param = die1GoSizeNormalLoopParam;
    ctx.die1GoSizeNormal.parallel_param = die1GoSizeNormalParallelParam;
    ctx.die1GoSizeNormal.residual = die1GoSizeNormalResidual;
    ctx.die1GoSizeLast.addr_offset = die1GoSizeLastAddrOffset;
    ctx.die1GoSizeLast.loop_param = die1GoSizeLastLoopParam;
    ctx.die1GoSizeLast.parallel_param = die1GoSizeLastParallelParam;
    ctx.die1GoSizeLast.residual = die1GoSizeLastResidual;

    HCCL_INFO(
        "[CcuKfcReduceScatterNHR1D2Die] run, rankId[%u], rankSize[%u], axisId[%u], axisSize[%u], dataType[%d]",
        ctx.rankId, ctx.rankSize, ctx.axisId, ctx.axisSize, ctx.dataType);
    CCU_CHK_RET(PreSync(ctx));
    CCU_CHK_RET(DoRepeatReduceScatterNHR(ctx));
    CCU_CHK_RET(PostSync(ctx));
    HCCL_INFO("[CcuKfcReduceScatterNHR1D2Die] end.");

    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
