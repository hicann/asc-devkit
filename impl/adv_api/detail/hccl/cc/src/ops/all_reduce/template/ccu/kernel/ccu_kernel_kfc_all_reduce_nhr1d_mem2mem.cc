/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_kfc_all_reduce_nhr1d_mem2mem.h"

namespace mc2_ops_hccl {
namespace {
// XN 槽位与 CKE 信号位定义，与 hccl CcuAllReduceNHR1DKernel 保持一致。
constexpr uint16_t OUTPUT_XN_ID = 1;
constexpr uint16_t TOKEN_XN_ID = 2;
constexpr uint16_t POST_SYNC_ID = 3;
constexpr uint16_t STEP0_PRE_SYNC_ID = 4;
constexpr uint16_t STEP0_POST_SYNC_ID = 5;
constexpr uint16_t STEP1_POST_SYNC_ID = 6;
constexpr uint16_t CKE_IDX_0 = 0;
constexpr uint16_t FST_AXIS_ID = 0;
constexpr uint16_t SEC_AXIS_ID = 1;
constexpr uint16_t RANK_NUM_PER_CKE = 16; // 本rank给远端置位时应当写的CKE，16个对端一个CKE

constexpr uint32_t NHR_NUM_2 = 2;

struct KfcAllReduceNhr1DContext : CcuKernelCtxBase {
    const ChannelHandle* channels = nullptr;
    uint32_t channelCount = 0; // 对端通道数（= 本地槽位下标）
    uint32_t rankSize = 0;
    uint32_t rankId = 0; // 子通信域虚拟 rankid
    uint32_t axisId = 0; // die 维编号：0=die0, 1=die1
    uint32_t axisSize = 0;
    HcclDataType dataType = HcclDataType::HCCL_DATA_TYPE_RESERVED;
    HcclDataType outputDataType = HcclDataType::HCCL_DATA_TYPE_RESERVED;
    HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
    const std::vector<KfcNhrStepInfo>* stepInfoVector = nullptr;
    const std::map<uint32_t, uint32_t>* rank2ChannelIdx = nullptr;

    // 由 dispatch 显式传入的参数
    ccu::Variable input;
    std::vector<ccu::Variable> output;
    std::vector<ccu::Variable> token;
    ccu::Variable isInputOutputEqual;
    ccu::Variable die0SliceSize;
    ccu::Variable die1SliceSize;
    ccu::Variable die0Size;
    ccu::Variable die1Size;
    ccu::Variable die0LastSliceSize;
    ccu::Variable die1LastSliceSize;

    ccu::Event localEvent;
    std::vector<ccu::Variable> sliceOffset;

    ccu::LocalAddr srcMem;
    ccu::RemoteAddr rmtDstMem;
    ccu::LocalAddr locDstMem;
};

CcuResult InitResource(KfcAllReduceNhr1DContext& ctx)
{
    if (ctx.channelCount == 0U || ctx.rankSize == 0U || ctx.rankId >= ctx.rankSize || ctx.rank2ChannelIdx == nullptr ||
        ctx.stepInfoVector == nullptr || ctx.axisSize == 0U || ctx.axisId >= ctx.axisSize) {
        HCCL_ERROR(
            "[CcuKernelKfcAllReduceNHR1DMem2Mem] invalid resource, rankSize[%u], rankId[%u], channels[%u], "
            "axisId[%u], axisSize[%u]",
            ctx.rankSize, ctx.rankId, ctx.channelCount, ctx.axisId, ctx.axisSize);
        return CCU_E_INTERNAL;
    }
    // NHR 的规约是远端原地 reduce（WriteReduce 只携带单一类型），无法表达
    // 「输入类型 != 输出类型」的规约语义；此类场景由 mesh 路径承担，避免按输入类型
    // 解释并写回目的端造成输出宽度/内容错误。
    if (ctx.outputDataType != ctx.dataType) {
        HCCL_ERROR(
            "[CcuKernelKfcAllReduceNHR1DMem2Mem] outputDataType[%d] differs from dataType[%d], "
            "homogeneous reduce type is required by NHR remote reduce.",
            ctx.outputDataType, ctx.dataType);
        return CCU_E_NOT_SUPPORT;
    }
    HCCL_INFO("[CcuKernelKfcAllReduceNHR1DMem2Mem] channels.size: [%u]", ctx.channelCount);

    const uint32_t localIdx = ctx.channelCount;
    ctx.output.resize(localIdx + 1U);
    ctx.token.resize(localIdx + 1U);
    for (uint32_t channelIdx = 0; channelIdx < ctx.channelCount; channelIdx++) {
        ctx.output[channelIdx] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], OUTPUT_XN_ID);
        ctx.token[channelIdx] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], TOKEN_XN_ID);
    }
    ctx.resourceAllocated = false;
    return CCU_SUCCESS;
}

CcuResult PreSync(KfcAllReduceNhr1DContext& ctx)
{
    const uint32_t localIdx = ctx.channelCount;
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.output[localIdx], OUTPUT_XN_ID, CKE_IDX_0, 1U << OUTPUT_XN_ID));
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.token[localIdx], TOKEN_XN_ID, CKE_IDX_0, 1U << TOKEN_XN_ID));
    }
    const uint32_t allBit = (1U << OUTPUT_XN_ID) | (1U << TOKEN_XN_ID);
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], CKE_IDX_0, allBit));
    }
    return CCU_SUCCESS;
}

CcuResult PostSync(KfcAllReduceNhr1DContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::NotifyRecord(ctx.channels[i], CKE_IDX_0, 1U << POST_SYNC_ID));
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], CKE_IDX_0, 1U << POST_SYNC_ID));
    }
    return CCU_SUCCESS;
}

CcuResult DoWriteReduceSlice(
    KfcAllReduceNhr1DContext& ctx, const u32& toRank, ccu::LocalAddr& src, ccu::RemoteAddr& dst,
    const u32& sendSliceIdx, u32 signalIndex)
{
    const u32& toRankIdx = ctx.rank2ChannelIdx->at(toRank);
    const ChannelHandle sendChannel = ctx.channels[toRankIdx];

    // 添加 die1 偏移
    if (ctx.axisId == SEC_AXIS_ID) {
        src.addr += ctx.die0Size;
        dst.addr += ctx.die0Size;
    }

    // allreduce切片的最后一块slice，大小可能不一致
    const bool islastSlice = (sendSliceIdx + 1 == ctx.rankSize);
    ccu::Variable& sliceSize = ctx.axisId == FST_AXIS_ID ? (islastSlice ? ctx.die0LastSliceSize : ctx.die0SliceSize) :
                                                           (islastSlice ? ctx.die1LastSliceSize : ctx.die1SliceSize);
    CCU_IF(sliceSize != 0)
    {
        ccu::WriteReduce(
            sendChannel, dst, src, sliceSize, ctx.dataType, ctx.reduceOp, ctx.localEvent, 1U << signalIndex);
    }
    CCU_IF(sliceSize == 0) { ccu::EventRecord(ctx.localEvent, 1U << signalIndex); }
    return CCU_SUCCESS;
}

CcuResult DoReduceScatterNHRSingleStep(KfcAllReduceNhr1DContext& ctx, const KfcNhrStepInfo& nhrStepInfo)
{
    const u32& toRankIdx = ctx.rank2ChannelIdx->at(nhrStepInfo.toRank);
    const u32& fromRankIdx = ctx.rank2ChannelIdx->at(nhrStepInfo.fromRank);
    const ChannelHandle sendChannel = ctx.channels[toRankIdx];
    const ChannelHandle recvChannel = ctx.channels[fromRankIdx];
    const std::vector<u32>& sendSliceIdxList = nhrStepInfo.txSliceIdxs;
    const uint32_t localIdx = ctx.channelCount;
    ctx.srcMem.token = ctx.token[localIdx];
    ctx.rmtDstMem.token = ctx.token[toRankIdx];

    if (nhrStepInfo.step != 0) {
        // 通知fromRank，可以写入
        CCU_CHK_RET(ccu::NotifyRecord(recvChannel, CKE_IDX_0, 1U << STEP0_PRE_SYNC_ID));

        // 等待toRank通知其可以写入
        CCU_CHK_RET(ccu::NotifyWait(sendChannel, CKE_IDX_0, 1U << STEP0_PRE_SYNC_ID));
    }

    for (u32 i = 0; i < sendSliceIdxList.size(); i++) {
        const u32 sendSliceIdx = sendSliceIdxList[i];

        if (i != 0) {
            if (i % RANK_NUM_PER_CKE == 0) {
                CCU_CHK_RET(ccu::EventWait(ctx.localEvent, (1U << RANK_NUM_PER_CKE) - 1U));
            }
        }

        if (nhrStepInfo.step == 0) {
            ctx.srcMem.addr = ctx.input;
            ctx.srcMem.addr += ctx.sliceOffset[sendSliceIdx];
        } else {
            ctx.srcMem.addr = ctx.output[localIdx];
            ctx.srcMem.addr += ctx.sliceOffset[sendSliceIdx];
        }

        ctx.rmtDstMem.addr = ctx.output[toRankIdx];
        ctx.rmtDstMem.addr += ctx.sliceOffset[sendSliceIdx];

        CCU_CHK_RET(
            DoWriteReduceSlice(ctx, nhrStepInfo.toRank, ctx.srcMem, ctx.rmtDstMem, sendSliceIdx, i % RANK_NUM_PER_CKE));
    }
    // 最后一组slice可能不足16个，也可能正好16个；size%16为0时需等待满16个bit
    // 空列表场景下未进入循环，不产生任何event bit，跳过EventWait避免死锁
    if (!sendSliceIdxList.empty()) {
        const u32 lastGroupSize = ((sendSliceIdxList.size() - 1U) % RANK_NUM_PER_CKE) + 1U;
        CCU_CHK_RET(ccu::EventWait(ctx.localEvent, (1U << lastGroupSize) - 1U));
    }

    // 通知toRank数据写入完毕
    CCU_CHK_RET(ccu::NotifyRecord(sendChannel, CKE_IDX_0, 1U << STEP0_POST_SYNC_ID));
    // 等待fromRank通知数据写入完毕
    CCU_CHK_RET(ccu::NotifyWait(recvChannel, CKE_IDX_0, 1U << STEP0_POST_SYNC_ID));
    HCCL_DEBUG(
        "[DoReduceScatterNHRSingleStep] rank %u step %u, toRank=%u, fromRank=%u, nSlice=%zu", ctx.rankId,
        nhrStepInfo.step, nhrStepInfo.toRank, nhrStepInfo.fromRank, sendSliceIdxList.size());
    return CCU_SUCCESS;
}

CcuResult DoReduceScatterNHR(KfcAllReduceNhr1DContext& ctx)
{
    for (u64 i = 0; i < ctx.stepInfoVector->size() / NHR_NUM_2; i++) {
        const KfcNhrStepInfo& nhrStepInfo = (*ctx.stepInfoVector)[i];
        CCU_CHK_RET(DoReduceScatterNHRSingleStep(ctx, nhrStepInfo));
    }
    return CCU_SUCCESS;
}

CcuResult DoSendRecvSlice(
    KfcAllReduceNhr1DContext& ctx, const u32& toRank, ccu::LocalAddr& src, ccu::RemoteAddr& dst,
    const u32& sendSliceIdx, u32 signalIndex)
{
    const u32& toRankIdx = ctx.rank2ChannelIdx->at(toRank);
    const ChannelHandle sendChannel = ctx.channels[toRankIdx];

    // 添加 die1 偏移
    if (ctx.axisId == SEC_AXIS_ID) {
        src.addr += ctx.die0Size;
        dst.addr += ctx.die0Size;
    }

    const bool islastSlice = (sendSliceIdx + 1 == ctx.rankSize);
    ccu::Variable& sliceSize = ctx.axisId == FST_AXIS_ID ? (islastSlice ? ctx.die0LastSliceSize : ctx.die0SliceSize) :
                                                           (islastSlice ? ctx.die1LastSliceSize : ctx.die1SliceSize);
    CCU_IF(sliceSize != 0) { ccu::Write(sendChannel, dst, src, sliceSize, ctx.localEvent, 1U << signalIndex); }
    CCU_IF(sliceSize == 0) { ccu::EventRecord(ctx.localEvent, 1U << signalIndex); }
    return CCU_SUCCESS;
}

CcuResult DoAllGatherNHRSingleStep(KfcAllReduceNhr1DContext& ctx, const KfcNhrStepInfo& nhrStepInfo)
{
    const u32& toRankIdx = ctx.rank2ChannelIdx->at(nhrStepInfo.toRank);
    const u32& fromRankIdx = ctx.rank2ChannelIdx->at(nhrStepInfo.fromRank);
    const ChannelHandle sendChannel = ctx.channels[toRankIdx];
    const ChannelHandle recvChannel = ctx.channels[fromRankIdx];
    const std::vector<u32>& sendSliceIdxList = nhrStepInfo.txSliceIdxs;
    const uint32_t localIdx = ctx.channelCount;
    ctx.srcMem.token = ctx.token[localIdx];
    ctx.rmtDstMem.token = ctx.token[toRankIdx];

    for (u32 i = 0; i < sendSliceIdxList.size(); i++) {
        const u32 sendSliceIdx = sendSliceIdxList[i];

        if (i != 0) {
            if (i % RANK_NUM_PER_CKE == 0) {
                CCU_CHK_RET(ccu::EventWait(ctx.localEvent, (1U << RANK_NUM_PER_CKE) - 1U));
            }
        }

        ctx.srcMem.addr = ctx.output[localIdx];
        ctx.srcMem.addr += ctx.sliceOffset[sendSliceIdx];

        ctx.rmtDstMem.addr = ctx.output[toRankIdx];
        ctx.rmtDstMem.addr += ctx.sliceOffset[sendSliceIdx];
        CCU_CHK_RET(
            DoSendRecvSlice(ctx, nhrStepInfo.toRank, ctx.srcMem, ctx.rmtDstMem, sendSliceIdx, i % RANK_NUM_PER_CKE));
    }
    // 最后一组slice可能不足16个，也可能正好16个；size%16为0时需等待满16个bit
    // 空列表场景下未进入循环，不产生任何event bit，跳过EventWait避免死锁
    if (!sendSliceIdxList.empty()) {
        const u32 lastGroupSize = ((sendSliceIdxList.size() - 1U) % RANK_NUM_PER_CKE) + 1U;
        CCU_CHK_RET(ccu::EventWait(ctx.localEvent, (1U << lastGroupSize) - 1U));
    }

    if (nhrStepInfo.step + 1 != ctx.stepInfoVector->size()) { // 最后一步不需要同步
        // 通知toRank，写入完毕
        CCU_CHK_RET(ccu::NotifyRecord(sendChannel, CKE_IDX_0, 1U << STEP1_POST_SYNC_ID));
        // 等待fromRank通知写入完毕
        CCU_CHK_RET(ccu::NotifyWait(recvChannel, CKE_IDX_0, 1U << STEP1_POST_SYNC_ID));
    }

    HCCL_DEBUG(
        "[DoAllGatherNHRSingleStep] rank %u step %u, toRank=%u, fromRank=%u, nSlice=%zu", ctx.rankId, nhrStepInfo.step,
        nhrStepInfo.toRank, nhrStepInfo.fromRank, sendSliceIdxList.size());
    return CCU_SUCCESS;
}

CcuResult DoAllGatherNHR(KfcAllReduceNhr1DContext& ctx)
{
    for (u64 i = ctx.stepInfoVector->size() / NHR_NUM_2; i < ctx.stepInfoVector->size(); i++) {
        const KfcNhrStepInfo& nhrStepInfo = (*ctx.stepInfoVector)[i];
        CCU_CHK_RET(DoAllGatherNHRSingleStep(ctx, nhrStepInfo));
    }
    return CCU_SUCCESS;
}

std::vector<u32> GetNonTxSliceIdxs(KfcAllReduceNhr1DContext& ctx, const std::vector<u32>& txSliceIdxs)
{
    std::vector<bool> isTx(ctx.rankSize, false);
    for (u32 idx : txSliceIdxs) {
        if (idx < ctx.rankSize) {
            isTx[idx] = true;
        }
    }

    std::vector<u32> nonTxSliceIdxs;
    for (u32 idx = 0; idx < ctx.rankSize; ++idx) {
        if (!isTx[idx]) {
            nonTxSliceIdxs.push_back(idx);
        }
    }

    return nonTxSliceIdxs;
}

CcuResult DoLocalCopySlice(
    KfcAllReduceNhr1DContext& ctx, ccu::LocalAddr& src, ccu::LocalAddr& dst, const u32& copySliceIdx, u32 signalIndex)
{
    // 添加 die1 偏移
    if (ctx.axisId == SEC_AXIS_ID) {
        src.addr += ctx.die0Size;
        dst.addr += ctx.die0Size;
    }

    const bool islastSlice = (copySliceIdx + 1 == ctx.rankSize);
    ccu::Variable& sliceSize = ctx.axisId == FST_AXIS_ID ? (islastSlice ? ctx.die0LastSliceSize : ctx.die0SliceSize) :
                                                           (islastSlice ? ctx.die1LastSliceSize : ctx.die1SliceSize);
    CCU_IF(sliceSize != 0) { ccu::LocalCopy(dst, src, sliceSize, ctx.localEvent, 1U << signalIndex); }
    CCU_IF(sliceSize == 0) { ccu::EventRecord(ctx.localEvent, 1U << signalIndex); }
    return CCU_SUCCESS;
}

CcuResult LocalCopySlices(KfcAllReduceNhr1DContext& ctx)
{
    for (u64 i = 0; i < ctx.rankSize; i++) {
        ctx.sliceOffset.push_back(ccu::Variable());
        if (i == 0) {
            ctx.sliceOffset[i] = 0;
        } else {
            ctx.sliceOffset[i] =
                ctx.sliceOffset[i - 1] + (ctx.axisId == FST_AXIS_ID ? ctx.die0SliceSize : ctx.die1SliceSize);
        }
    }

    // 当input == output时，不需要拷贝
    CCU_IF(ctx.isInputOutputEqual == 0)
    {
        // 将step0中不需要写的slice，拷贝到本rank的output中
        const KfcNhrStepInfo& nhrStepInfo = (*ctx.stepInfoVector)[0];
        const std::vector<u32>& nonTxSliceIdxList = GetNonTxSliceIdxs(ctx, nhrStepInfo.txSliceIdxs);
        for (u32 i = 0; i < nonTxSliceIdxList.size(); i++) {
            const u32 nonTxSliceIdx = nonTxSliceIdxList[i];

            if (i != 0) {
                if (i % RANK_NUM_PER_CKE == 0) {
                    CCU_CHK_RET(ccu::EventWait(ctx.localEvent, (1U << RANK_NUM_PER_CKE) - 1U));
                }
            }

            ctx.srcMem.addr = ctx.input;
            ctx.srcMem.addr += ctx.sliceOffset[nonTxSliceIdx];
            ctx.srcMem.token = ctx.token[ctx.channelCount];

            ctx.locDstMem.addr = ctx.output[ctx.channelCount];
            ctx.locDstMem.addr += ctx.sliceOffset[nonTxSliceIdx];
            ctx.locDstMem.token = ctx.token[ctx.channelCount];
            CCU_CHK_RET(DoLocalCopySlice(ctx, ctx.srcMem, ctx.locDstMem, nonTxSliceIdx, i % RANK_NUM_PER_CKE));
        }
        // 最后一组slice可能不足16个，也可能正好16个；size%16为0时需等待满16个bit
        // 空列表场景下未进入循环，不产生任何event bit，跳过EventWait避免死锁
        if (!nonTxSliceIdxList.empty()) {
            const u32 lastGroupSize = ((nonTxSliceIdxList.size() - 1U) % RANK_NUM_PER_CKE) + 1U;
            CCU_CHK_RET(ccu::EventWait(ctx.localEvent, (1U << lastGroupSize) - 1U));
        }
    }
    return CCU_SUCCESS;
}
} // namespace

CcuResult CcuKernelKfcAllReduceNHR1DMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable isInputOutputEqual,
    ccu::Variable die0Size, ccu::Variable die1Size, ccu::Variable die0SliceSize, ccu::Variable die1SliceSize,
    ccu::Variable die0LastSliceSize, ccu::Variable die1LastSliceSize, const ChannelHandle channels[],
    uint32_t channelCount, uint32_t rankSize, uint32_t rankId, uint32_t axisId, uint32_t axisSize,
    const HcclDataType& dataType, const HcclDataType& outputType, const HcclReduceOp& reduceType,
    const std::vector<KfcNhrStepInfo>& stepInfoVector, const std::map<uint32_t, uint32_t>& rank2ChannelIdx)
{
    KfcAllReduceNhr1DContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.axisId = axisId;
    ctx.axisSize = axisSize;
    ctx.dataType = dataType;
    ctx.outputDataType = outputType;
    if (ctx.outputDataType == HcclDataType::HCCL_DATA_TYPE_RESERVED) {
        ctx.outputDataType = ctx.dataType;
        HCCL_DEBUG(
            "[CcuKernelKfcAllReduceNHR1DMem2Mem] outputDataType is [INVALID], set outputDataType to[%d]", ctx.dataType);
    }
    ctx.reduceOp = reduceType;
    ctx.stepInfoVector = &stepInfoVector;
    ctx.rank2ChannelIdx = &rank2ChannelIdx;
    InitCcuKernelCtxBase(ctx);

    HCCL_INFO(
        "[CcuKernelKfcAllReduceNHR1DMem2Mem] run, rankId[%u], rankSize[%u], axisId[%u], axisSize[%u], dataType[%d], "
        "outputDataType[%d], reduceOp[%d]",
        ctx.rankId, ctx.rankSize, ctx.axisId, ctx.axisSize, ctx.dataType, ctx.outputDataType, ctx.reduceOp);
    CCU_CHK_RET(InitResource(ctx));

    const uint32_t localIdx = channelCount;
    ctx.input = inputAddr;
    ctx.output[localIdx] = outputAddr;
    ctx.token[localIdx] = tokenInfo;
    ctx.isInputOutputEqual = isInputOutputEqual;
    ctx.die0Size = die0Size;
    ctx.die1Size = die1Size;
    ctx.die0SliceSize = die0SliceSize;
    ctx.die1SliceSize = die1SliceSize;
    ctx.die0LastSliceSize = die0LastSliceSize;
    ctx.die1LastSliceSize = die1LastSliceSize;

    CCU_CHK_RET(LocalCopySlices(ctx));
    CCU_CHK_RET(PreSync(ctx));
    CCU_CHK_RET(DoReduceScatterNHR(ctx));
    CCU_CHK_RET(DoAllGatherNHR(ctx));
    CCU_CHK_RET(PostSync(ctx));

    HCCL_INFO("[CcuKernelKfcAllReduceNHR1DMem2Mem] end.");
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
