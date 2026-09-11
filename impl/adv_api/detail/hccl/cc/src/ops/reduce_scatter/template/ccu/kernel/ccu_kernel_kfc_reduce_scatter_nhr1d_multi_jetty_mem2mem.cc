/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_kfc_reduce_scatter_nhr1d_multi_jetty_mem2mem.h"

namespace mc2_ops_hccl {
namespace {
// XN 槽位与 CKE 信号位定义，与 hccl CcuReduceScatterNhrMem2Mem1DMultiJettyKernel 保持一致。
constexpr uint16_t RS_NHR_INPUT_XN_ID = 0U;
constexpr uint16_t RS_NHR_TOKEN_XN_ID = 1U;
constexpr uint16_t RS_NHR_CKE_IDX = 0U;
constexpr uint16_t RS_NHR_INPUT_BIT = 1U << RS_NHR_INPUT_XN_ID;
constexpr uint16_t RS_NHR_TOKEN_BIT = 1U << RS_NHR_TOKEN_XN_ID;
constexpr uint16_t RS_NHR_STEP_PRE_SYNC_BIT = 1U << 2U;
constexpr uint16_t RS_NHR_STEP_POST_SYNC_BIT = 1U << 3U;
constexpr uint16_t RS_NHR_POST_SYNC_BIT = 1U << 4U;

struct KfcReduceScatterNhrContext : CcuKernelCtxBase {
    const ChannelHandle* channels = nullptr;
    uint32_t channelCount = 0;
    uint32_t rankSize = 0;
    uint32_t rankId = 0;
    uint32_t jettyNum = 1;
    HcclDataType dataType = HcclDataType::HCCL_DATA_TYPE_RESERVED;
    HcclDataType outputDataType = HcclDataType::HCCL_DATA_TYPE_RESERVED;
    HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
    const std::vector<KfcNhrStepInfo>* stepInfoVector = nullptr;
    const std::map<uint32_t, uint32_t>* rank2ChannelIdx = nullptr;

    std::vector<ccu::Variable> input;
    ccu::Variable output;
    std::vector<ccu::Variable> token;
    std::vector<ccu::Variable> inputSliceOffset;
    ccu::Variable sliceSize;
    ccu::Variable inputSliceStride;
    ccu::Variable sliceOneJettySize;
    ccu::Variable sliceLastJettySize;
    ccu::Variable repeatNumInv;
    ccu::Variable inputRepeatStride;
    ccu::Variable outputRepeatStride;
    ccu::Event event;
    ccu::Variable repeatNum;
    ccu::Variable repeatFlag;
    ccu::Variable constVar1;
    ccu::LocalAddr localSrc_;
    ccu::LocalAddr localDst_;
    ccu::RemoteAddr remoteDst_;
};

CcuResult InitResource(KfcReduceScatterNhrContext& ctx)
{
    if (ctx.rankSize == 0U || ctx.rankId >= ctx.rankSize || ctx.channelCount == 0U || ctx.rank2ChannelIdx == nullptr ||
        ctx.rank2ChannelIdx->size() != ctx.channelCount || ctx.jettyNum == 0U) {
        HCCL_ERROR(
            "[CcuKfcReduceScatterNHR] invalid resource, rankSize[%u], rankId[%u], channels[%u], jetty[%u]",
            ctx.rankSize, ctx.rankId, ctx.channelCount, ctx.jettyNum);
        return CCU_E_INTERNAL;
    }
    const uint32_t localIdx = ctx.channelCount;
    ctx.input.resize(localIdx + 1U);
    ctx.token.resize(localIdx + 1U);
    for (uint32_t channelIdx = 0; channelIdx < ctx.channelCount; ++channelIdx) {
        ctx.input[channelIdx] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], RS_NHR_INPUT_XN_ID);
        ctx.token[channelIdx] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], RS_NHR_TOKEN_XN_ID);
    }
    ctx.resourceAllocated = false;
    return CCU_SUCCESS;
}

CcuResult PreSync(KfcReduceScatterNhrContext& ctx)
{
    const uint32_t localIdx = ctx.channelCount;
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.input[localIdx], RS_NHR_INPUT_XN_ID, RS_NHR_CKE_IDX, RS_NHR_INPUT_BIT));
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.token[localIdx], RS_NHR_TOKEN_XN_ID, RS_NHR_CKE_IDX, RS_NHR_TOKEN_BIT));
    }
    const uint16_t waitMask = RS_NHR_INPUT_BIT | RS_NHR_TOKEN_BIT;
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], RS_NHR_CKE_IDX, waitMask));
    }
    return CCU_SUCCESS;
}

CcuResult PostSync(KfcReduceScatterNhrContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::NotifyRecord(ctx.channels[i], RS_NHR_CKE_IDX, RS_NHR_POST_SYNC_BIT));
    }
    for (uint32_t i = 0; i < ctx.channelCount; ++i) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], RS_NHR_CKE_IDX, RS_NHR_POST_SYNC_BIT));
    }
    return CCU_SUCCESS;
}

CcuResult WriteReduceSlice(KfcReduceScatterNhrContext& ctx, uint32_t toRank)
{
    const ChannelHandle channel = ctx.channels[ctx.rank2ChannelIdx->at(toRank)];
    CCU_IF(ctx.sliceOneJettySize != 0)
    {
        for (uint32_t jettyId = 0; jettyId + 1U < ctx.jettyNum; ++jettyId) {
            CCU_CHK_RET(ccu::WriteReduce(
                channel, ctx.remoteDst_, ctx.localSrc_, ctx.sliceOneJettySize, ctx.dataType, ctx.reduceOp, ctx.event,
                1U << jettyId));
            ctx.remoteDst_.addr += ctx.sliceOneJettySize;
            ctx.localSrc_.addr += ctx.sliceOneJettySize;
        }
    }
    CCU_ELSE
    {
        for (uint32_t jettyId = 0; jettyId + 1U < ctx.jettyNum; ++jettyId) {
            CCU_CHK_RET(ccu::EventRecord(ctx.event, 1U << jettyId));
        }
    }
    const uint32_t lastJettyId = ctx.jettyNum - 1U;
    const uint16_t lastMask = 1U << lastJettyId;
    CCU_IF(ctx.sliceLastJettySize == 0) { CCU_CHK_RET(ccu::EventRecord(ctx.event, lastMask)); }
    CCU_ELSE
    {
        CCU_CHK_RET(ccu::WriteReduce(
            channel, ctx.remoteDst_, ctx.localSrc_, ctx.sliceLastJettySize, ctx.dataType, ctx.reduceOp, ctx.event,
            lastMask));
    }
    CCU_CHK_RET(ccu::EventWait(ctx.event, static_cast<uint16_t>((1U << ctx.jettyNum) - 1U)));
    return CCU_SUCCESS;
}

CcuResult RunStep(KfcReduceScatterNhrContext& ctx, const KfcNhrStepInfo& stepInfo)
{
    const uint32_t toChannelIdx = ctx.rank2ChannelIdx->at(stepInfo.toRank);
    const uint32_t fromChannelIdx = ctx.rank2ChannelIdx->at(stepInfo.fromRank);
    const ChannelHandle sendChannel = ctx.channels[toChannelIdx];
    const ChannelHandle recvChannel = ctx.channels[fromChannelIdx];
    const uint32_t localIdx = ctx.channelCount;

    ctx.remoteDst_.token = ctx.token[toChannelIdx];
    ctx.localSrc_.token = ctx.token[localIdx];

    // step 非 0 时先做步间就绪同步，保证对端上一轮读取已完成。
    if (stepInfo.step != 0U) {
        CCU_CHK_RET(ccu::NotifyRecord(recvChannel, RS_NHR_CKE_IDX, RS_NHR_STEP_PRE_SYNC_BIT));
        CCU_CHK_RET(ccu::NotifyWait(sendChannel, RS_NHR_CKE_IDX, RS_NHR_STEP_PRE_SYNC_BIT));
    }
    for (uint32_t sendSliceIdx : stepInfo.txSliceIdxs) {
        // 对端对应分片地址 = 对端 input 基址 + 分片偏移；本端源地址同理。
        ctx.remoteDst_.addr = ctx.input[toChannelIdx];
        ctx.remoteDst_.addr += ctx.inputSliceOffset[sendSliceIdx];
        ctx.localSrc_.addr = ctx.input[localIdx];
        ctx.localSrc_.addr += ctx.inputSliceOffset[sendSliceIdx];
        ctx.repeatFlag = 0;
        ctx.repeatNum = ctx.repeatNumInv;
        CCU_WHILE(ctx.repeatNum != UINT64_MAX)
        {
            ctx.repeatNum += ctx.constVar1;
            CCU_IF(ctx.repeatFlag != 0)
            {
                ctx.localSrc_.addr += ctx.inputRepeatStride;
                ctx.remoteDst_.addr += ctx.inputRepeatStride;
            }
            CCU_CHK_RET(WriteReduceSlice(ctx, stepInfo.toRank));
            ctx.repeatFlag = 1U;
        }
    }
    CCU_CHK_RET(ccu::NotifyRecord(sendChannel, RS_NHR_CKE_IDX, RS_NHR_STEP_POST_SYNC_BIT));
    CCU_CHK_RET(ccu::NotifyWait(recvChannel, RS_NHR_CKE_IDX, RS_NHR_STEP_POST_SYNC_BIT));
    return CCU_SUCCESS;
}

CcuResult RunNhr(KfcReduceScatterNhrContext& ctx)
{
    const uint32_t localIdx = ctx.channelCount;
    ccu::Variable tmpSliceOffset;
    tmpSliceOffset = 0;
    ctx.inputSliceOffset.resize(ctx.rankSize);
    for (uint32_t rank = 0; rank < ctx.rankSize; ++rank) {
        ctx.inputSliceOffset[rank] = tmpSliceOffset;
        tmpSliceOffset += ctx.inputSliceStride;
    }

    for (const auto& stepInfo : *ctx.stepInfoVector) {
        CCU_CHK_RET(RunStep(ctx, stepInfo));
    }

    // 收尾：本 rank 的输入分片 LocalCopy 到输出。
    ctx.localDst_.addr = ctx.output;
    ctx.localDst_.token = ctx.token[localIdx];
    ctx.localSrc_.addr = ctx.input[localIdx];
    ctx.localSrc_.addr += ctx.inputSliceOffset[ctx.rankId];
    ctx.localSrc_.token = ctx.token[localIdx];
    ctx.repeatFlag = 0;
    ctx.repeatNum = ctx.repeatNumInv;
    CCU_WHILE(ctx.repeatNum != UINT64_MAX)
    {
        ctx.repeatNum += ctx.constVar1;
        CCU_IF(ctx.repeatFlag != 0)
        {
            ctx.localSrc_.addr += ctx.inputRepeatStride;
            ctx.localDst_.addr += ctx.outputRepeatStride;
        }
        CCU_IF(ctx.sliceSize == 0) { CCU_CHK_RET(ccu::EventRecord(ctx.event, 1U)); }
        CCU_ELSE { CCU_CHK_RET(ccu::LocalCopy(ctx.localDst_, ctx.localSrc_, ctx.sliceSize, ctx.event, 1U)); }
        CCU_CHK_RET(ccu::EventWait(ctx.event, 1U));
        ctx.repeatFlag = 1U;
    }
    return CCU_SUCCESS;
}
} // namespace

CcuResult CcuKfcReduceScatterNHR1DMultiJettyMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable sliceSize,
    ccu::Variable inputSliceStride, ccu::Variable sliceOneJettySize, ccu::Variable sliceLastJettySize,
    ccu::Variable repeatNumInv, ccu::Variable inputRepeatStride, ccu::Variable outputRepeatStride,
    const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId, uint32_t jettyNum,
    const HcclDataType& dataType, const HcclDataType& outputType, const HcclReduceOp& reduceType,
    const std::vector<KfcNhrStepInfo>& stepInfoVector, const std::map<uint32_t, uint32_t>& rank2ChannelIdx)
{
    KfcReduceScatterNhrContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.jettyNum = jettyNum;
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
    ctx.sliceSize = sliceSize;
    ctx.inputSliceStride = inputSliceStride;
    ctx.sliceOneJettySize = sliceOneJettySize;
    ctx.sliceLastJettySize = sliceLastJettySize;
    ctx.repeatNumInv = repeatNumInv;
    ctx.inputRepeatStride = inputRepeatStride;
    ctx.outputRepeatStride = outputRepeatStride;
    ctx.constVar1 = 1U;

    CCU_CHK_RET(PreSync(ctx));
    CCU_IF(ctx.sliceSize != 0) { CCU_CHK_RET(RunNhr(ctx)); }
    CCU_CHK_RET(PostSync(ctx));
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
