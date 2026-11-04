/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_alg_base.h"
#include "ccu_kernel_all_to_all_v_mesh1d.h"

#include <algorithm>

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
constexpr uint16_t A2AV_OUTPUT_BIT = static_cast<uint16_t>(1U << A2AV_OUTPUT_XN_ID);
constexpr uint16_t A2AV_TOKEN_BIT = static_cast<uint16_t>(1U << A2AV_TOKEN_XN_ID);
constexpr uint16_t A2AV_PRE_SYNC_BITS = A2AV_OUTPUT_BIT | A2AV_TOKEN_BIT;
constexpr uint16_t A2AV_POST_SYNC_BIT = static_cast<uint16_t>(1U << 5);
constexpr uint32_t A2AV_BITS_PER_EVENT = 16;
constexpr uint64_t A2AV_MAX_TRANSPORT_SIZE = 256ULL * 1024ULL * 1024ULL;
constexpr uint64_t A2AV_GROUP_MEMSLICE_SIZE = 32ULL * 1024ULL;
constexpr uint64_t A2AV_GROUP_LOOP_COUNT = 8ULL;

static CcuResult CcuAlltoAllVMesh1DKernelV1(
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
    CCU_WHILE(completedRankCount != static_cast<uint64_t>(rankSize))
    {
        channelIdx = 0;
        // Start remote transfers before the local self-copy so the latter can overlap
        // with remote DMA. This matches the legacy HCOMM ordering.
        for (uint32_t transferPass = 0; transferPass < 2; transferPass++) {
            const uint32_t transferBegin = transferPass == 0 ? 0 : rankId;
            const uint32_t transferEnd = transferPass == 0 ? rankSize : rankId + 1;
            for (uint32_t rankIdx = transferBegin; rankIdx < transferEnd; rankIdx++) {
                if (transferPass == 0 && rankIdx == rankId) {
                    continue;
                }
                const uint32_t extBase = rankIdx * A2AV_EXT_FIELD_NUM;
                const uint32_t eventIdx = rankIdx / A2AV_BITS_PER_EVENT;
                const uint16_t rankMask = static_cast<uint16_t>(1U << (rankIdx % A2AV_BITS_PER_EVENT));
                ccu::Variable& loopNum = extArgs[extBase + A2AV_LOOP_NUM_IDX];
                ccu::Variable& tailSize = extArgs[extBase + A2AV_TAIL_SIZE_IDX];

                CCU_IF(loopNum == static_cast<uint64_t>(UINT64_MAX))
                {
                    CCU_CHK_RET(ccu::EventRecord(events[eventIdx], rankMask));
                }
                CCU_IF(loopNum != static_cast<uint64_t>(UINT64_MAX))
                {
                    CCU_IF(loopNum == static_cast<uint64_t>(UINT64_MAX - 1U))
                    {
                        if (rankIdx == rankId) {
                            CCU_IF(tailSize != 0)
                            {
                                CCU_CHK_RET(GroupCopy(ctx, localDst, src[rankIdx], tailGoSize, GetCcuVersion()));
                            }
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
                    CCU_IF(loopNum != static_cast<uint64_t>(UINT64_MAX - 1U))
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

// ============================================================================
// Ascend960(CCU_V2) 新逻辑
// ============================================================================
constexpr int INPUT_XN_ID = 0;
constexpr int OUTPUT_XN_ID = 1;
constexpr int TOKEN_XN_ID = 2;
constexpr int CKE_IDX_0 = 0; // pre
constexpr int CKE_IDX_1 = 1; // post
constexpr int CONST_ONE = 1;

static CcuResult LoadAll2allSendRecvInfo(AlltoAllVMesh1DContext& ctx, A2AsingleSendRecvInfo& sendRecvInfo)
{
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] Load Args from Mem");
    sendRecvInfo.loopNum = UINT64_MAX - 1; // MC2 场景 loop num 默认为 1

    // 要求client端排列内存为[size,send,recv][size,send,recv]...
    ccu::Load(ctx.a2avXnAddr, sendRecvInfo.tailSize);
    ctx.a2avXnAddr += ctx.xnLength;

    // 跳过LoopNum
    ctx.a2avXnAddr += ctx.xnLength;

    ccu::Load(ctx.a2avXnAddr, sendRecvInfo.sendOffset);
    ctx.a2avXnAddr += ctx.xnLength;

    ccu::Load(ctx.a2avXnAddr, sendRecvInfo.recvOffset);
    ctx.a2avXnAddr += ctx.xnLength;
    return CCU_SUCCESS;
}

static CcuResult InitResource(AlltoAllVMesh1DContext& ctx)
{
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] InitResource!");
    uint32_t channelIdx = 0;
    if (ctx.channelCount == 0) {
        HCCL_ERROR("[CcuKernelAlltoAllVMesh1D] channels is empty!");
        return CcuResult::CCU_E_INTERNAL;
    }

    HCCL_INFO("ctx.rankSize = %u, ctx.rankId = %u", ctx.rankSize, ctx.rankId);
    ctx.input.resize(ctx.rankSize);
    ctx.output.resize(ctx.rankSize);
    ctx.token.resize(ctx.rankSize);
    for (uint64_t peerId = 0; peerId < ctx.rankSize; peerId++) {
        if (peerId != ctx.rankId) { // 非本地，使用远端Variable
            ctx.input[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], INPUT_XN_ID);
            ctx.output[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], OUTPUT_XN_ID);
            ctx.token[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], TOKEN_XN_ID);
            channelIdx++;
        }
    }
    HCCL_INFO("output size: %zu, token size: %zu", ctx.output.size(), ctx.token.size());

    ctx.src.resize(ctx.rankSize);
    ctx.dst.resize(ctx.rankSize);

    // 前同步。交换信息，将本Rank load的in\out等地址信息写到所有对端的对应Variable中，并同步

    //  all2allv 数据搬运
    ctx.xnLength = 8; // xn长度为8byte

    ctx.resourceAllocated = false;

    return CCU_SUCCESS;
}

static CcuResult PreSync(AlltoAllVMesh1DContext& ctx)
{
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] PreSync begin!");
    ccu::Variable tempDst;

    uint32_t channelIdx = 0;
    for (uint32_t id = 0; id < ctx.rankSize; id++) {
        if (id == ctx.rankId) {
            continue;
        }
        tempDst = ctx.output[ctx.rankId];
        tempDst += ctx.sendRecvInfo[id].recvOffset;
        // index = 0，传递output信息
        ccu::WriteVariableWithNotify(ctx.channels[channelIdx], tempDst, OUTPUT_XN_ID, CKE_IDX_0, 1 << OUTPUT_XN_ID);
        // index = 1，传递token信息
        ccu::WriteVariableWithNotify(
            ctx.channels[channelIdx], ctx.token[ctx.rankId], TOKEN_XN_ID, CKE_IDX_0, 1 << TOKEN_XN_ID);
        channelIdx++;
    }

    uint16_t allBit = static_cast<uint16_t>((1U << OUTPUT_XN_ID) | (1U << TOKEN_XN_ID));
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyWait(ctx.channels[i], CKE_IDX_0, allBit);
    }
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] PreSync end");

    return CCU_SUCCESS;
}

static void PostSync(AlltoAllVMesh1DContext& ctx)
{
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] PostSync!");
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyRecord(ctx.channels[i], CKE_IDX_1, 1 << CONST_ONE);
    }

    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyWait(ctx.channels[i], CKE_IDX_1, 1 << CONST_ONE);
    }
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] PostSync End!");
}

static CcuResult LoadArgs(
    AlltoAllVMesh1DContext& ctx, ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo,
    ccu::Variable srcOffset, ccu::Variable dstOffset, ccu::Variable extAddr, ccu::Variable goSize1,
    ccu::Variable goSize2, ccu::Variable goSize3, ccu::Variable goSize4)
{
    (void)goSize1;
    (void)goSize2;
    (void)goSize3;
    (void)goSize4;
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] LoadArgs!");
    ctx.input[0] = inputAddr;
    ctx.output[ctx.rankId] = outputAddr;
    ctx.token[ctx.rankId] = tokenInfo;
    ctx.srcOffset = srcOffset;
    ctx.dstOffset = dstOffset;
    ctx.a2avXnAddr = extAddr;
    ctx.sendRecvInfo.resize(ctx.rankSize);
    for (uint64_t peerId = 0; peerId < ctx.rankSize; peerId++) {
        CCU_CHK_RET(LoadAll2allSendRecvInfo(ctx, ctx.sendRecvInfo[peerId]));
    }
    return CCU_SUCCESS;
}

static void CalcGroupSrcDst(AlltoAllVMesh1DContext& ctx)
{
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] CalcGroupSrcDst!");
    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        ctx.src[rankIdx].token = ctx.token[rankIdx];

        ctx.src[rankIdx].addr = ctx.input[0];
        ctx.src[rankIdx].addr += ctx.sendRecvInfo[rankIdx].sendOffset;
        ctx.src[rankIdx].addr += ctx.srcOffset;

        if (rankIdx == ctx.rankId) {
            // 写目的端为本端时需要特殊处理：使用接收基地址 + 块地址offset + 已发送数据量
            ctx.myDst.token = ctx.token[rankIdx];
            ctx.myDst.addr = ctx.output[rankIdx];
            ctx.myDst.addr += ctx.sendRecvInfo[rankIdx].recvOffset;
            ctx.myDst.addr += ctx.dstOffset;
        } else {
            // 对端交换的接收块起始地址 + 已接收的数据偏移
            ctx.dst[rankIdx].token = ctx.token[rankIdx];
            ctx.dst[rankIdx].addr = ctx.output[rankIdx];
            ctx.dst[rankIdx].addr += ctx.dstOffset;
        }
    }
}

static CcuResult DoAll2AllVMultiLoop(AlltoAllVMesh1DContext& ctx)
{
    HCCL_DEBUG("[CcuKernelAlltoAllVMesh1D] alltoallv mesh 1d use GroupCopy start");
    ctx.xnMaxTransportSize = UB_MAX_TRANS_SIZE;
    ctx.completedRankCount = 0;
    ctx.xnConst1 = 1;
    // rankSize<=16 时与老实现等价（单 event 全位收齐）；>16 按 16 位一组拆 event 逐组等待
    const uint32_t eventNum = (ctx.rankSize + A2AV_BITS_PER_EVENT - 1) / A2AV_BITS_PER_EVENT;
    ctx.events.resize(eventNum);
    ctx.eventMaskOfRank.resize(ctx.rankSize);
    ctx.eventIdxOfRank.resize(ctx.rankSize);
    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        const uint32_t eventIdx = rankIdx / A2AV_BITS_PER_EVENT;
        const uint16_t rankMask = static_cast<uint16_t>(1U << (rankIdx % A2AV_BITS_PER_EVENT));
        ctx.eventMaskOfRank[rankIdx] = rankMask;
        ctx.eventIdxOfRank[rankIdx] = eventIdx;
    }
    CCU_WHILE(ctx.completedRankCount != static_cast<uint64_t>(ctx.rankSize))
    {
        uint32_t channelId = 0;
        for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
            if (rankIdx == ctx.rankId) {
                continue;
            }
            const uint32_t eventIdx = ctx.eventIdxOfRank[rankIdx];
            const uint16_t rankMask = ctx.eventMaskOfRank[rankIdx];
            CCU_IF(ctx.sendRecvInfo[rankIdx].loopNum == UINT64_MAX)
            { // 已经完成，直接置位完成信号
                ccu::EventRecord(ctx.events[eventIdx], rankMask);
            }
            CCU_IF(ctx.sendRecvInfo[rankIdx].loopNum != UINT64_MAX)
            { // 还没有完成，则继续循环
                CCU_IF(ctx.sendRecvInfo[rankIdx].loopNum == UINT64_MAX - 1)
                { // 最后一轮循环, 发送尾块数据
                    CCU_IF(ctx.sendRecvInfo[rankIdx].tailSize == 0)
                    { // 尾块数据量为 0，则不需要发送尾块数据
                        ccu::EventRecord(ctx.events[eventIdx], rankMask);
                    }
                    CCU_IF(ctx.sendRecvInfo[rankIdx].tailSize != 0)
                    { // 尾块数据量不为 0，则需要发送尾块数据
                        ccu::Write(
                            ctx.channels[channelId], ctx.dst[rankIdx], ctx.src[rankIdx],
                            ctx.sendRecvInfo[rankIdx].tailSize, ctx.events[eventIdx], rankMask);
                    }
                    ctx.completedRankCount += ctx.xnConst1; // 之后一轮循环完成，更新已完成的rank数
                }
                CCU_IF(ctx.sendRecvInfo[rankIdx].loopNum != UINT64_MAX - 1)
                { // 未完成，则继续循环，发送整块数据
                    ccu::Write(
                        ctx.channels[channelId], ctx.dst[rankIdx], ctx.src[rankIdx], ctx.xnMaxTransportSize,
                        ctx.events[eventIdx], rankMask);
                    // 更新偏移
                    ctx.src[rankIdx].addr += ctx.xnMaxTransportSize;
                    ctx.dst[rankIdx].addr += ctx.xnMaxTransportSize;
                }
                ctx.sendRecvInfo[rankIdx].loopNum += ctx.xnConst1;
            }
            channelId++;
        }

        // 本 rank 自拷贝段
        {
            const uint32_t eventIdx = ctx.eventIdxOfRank[ctx.rankId];
            const uint16_t rankMask = ctx.eventMaskOfRank[ctx.rankId];
            CCU_IF(ctx.sendRecvInfo[ctx.rankId].loopNum == UINT64_MAX)
            { // 已经完成，直接置位完成信号
                ccu::EventRecord(ctx.events[eventIdx], rankMask);
            }

            CCU_IF(ctx.sendRecvInfo[ctx.rankId].loopNum != UINT64_MAX)
            { // 还没有完成，则继续循环
                CCU_IF(ctx.sendRecvInfo[ctx.rankId].loopNum == UINT64_MAX - 1)
                { // 最后一轮循环, 发送尾块数据
                    CCU_IF(ctx.sendRecvInfo[ctx.rankId].tailSize == 0)
                    { // 尾块数据量为 0，则不需要发送尾块数据
                        ccu::EventRecord(ctx.events[eventIdx], rankMask);
                    }
                    CCU_IF(ctx.sendRecvInfo[ctx.rankId].tailSize != 0)
                    { // 尾块数据量不为 0，则需要发送尾块数据
                        ccu::LocalCopy(
                            ctx.myDst, ctx.src[ctx.rankId], ctx.sendRecvInfo[ctx.rankId].tailSize, ctx.events[eventIdx],
                            rankMask);
                    }
                    ctx.completedRankCount += ctx.xnConst1; // 之后一轮循环完成，更新已完成的rank数
                }
                CCU_IF(ctx.sendRecvInfo[ctx.rankId].loopNum != UINT64_MAX - 1)
                { // 未完成，则继续循环，发送整块数据
                    ccu::LocalCopy(
                        ctx.myDst, ctx.src[ctx.rankId], ctx.xnMaxTransportSize, ctx.events[eventIdx], rankMask);
                    // 更新偏移
                    ctx.src[ctx.rankId].addr += ctx.xnMaxTransportSize;
                    ctx.myDst.addr += ctx.xnMaxTransportSize;
                }
                ctx.sendRecvInfo[ctx.rankId].loopNum += ctx.xnConst1;
            }
        }
        // 等待本轮发送完成
        for (uint32_t eventIdx = 0; eventIdx < eventNum; eventIdx++) {
            const uint32_t remainingRanks = ctx.rankSize - eventIdx * A2AV_BITS_PER_EVENT;
            const uint32_t bitCount = std::min(remainingRanks, A2AV_BITS_PER_EVENT);
            const uint16_t eventMask = bitCount == A2AV_BITS_PER_EVENT ? static_cast<uint16_t>(UINT16_MAX) :
                                                                         static_cast<uint16_t>((1U << bitCount) - 1);
            ccu::EventWait(ctx.events[eventIdx], eventMask);
        }
    }

    return CCU_SUCCESS;
}

static CcuResult CcuAlltoAllVMesh1DKernelV2(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable srcOffset,
    ccu::Variable dstOffset, ccu::Variable extAddr, ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3,
    ccu::Variable goSize4, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId)
{
    AlltoAllVMesh1DContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    InitCcuKernelCtxBase(ctx);

    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] AlltoAllVMesh1D run");
    CCU_CHK_RET(InitResource(ctx));
    CCU_CHK_RET(LoadArgs(
        ctx, inputAddr, outputAddr, tokenInfo, srcOffset, dstOffset, extAddr, goSize1, goSize2, goSize3, goSize4));

    CCU_CHK_RET(PreSync(ctx));
    CalcGroupSrcDst(ctx);

    CCU_CHK_RET(DoAll2AllVMultiLoop(ctx));

    PostSync(ctx);
    HCCL_INFO("[CcuKernelAlltoAllVMesh1D] AlltoAllVMesh1D end");

    return CCU_SUCCESS;
}

CcuResult CcuAlltoAllVMesh1DKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable srcOffset,
    ccu::Variable dstOffset, ccu::Variable extAddr, ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3,
    ccu::Variable goSize4, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId)
{
    // Ascend960(CCU_V2) 使用新的多轮次 LocalCopy/Write 方案；Ascend950(CCU_V1) 保持 GroupCopy 老逻辑。
    const CcuVersion ccuVersion = GetCcuVersion();
    const bool useNewImpl = (ccuVersion == CcuVersion::CCU_V2);
    HCCL_INFO(
        "[CcuKernelAlltoAllVMesh1D] ccuVersion[%u], useNewImpl[%d]", static_cast<uint32_t>(ccuVersion),
        static_cast<int32_t>(useNewImpl));
    if (useNewImpl) {
        return CcuAlltoAllVMesh1DKernelV2(
            inputAddr, outputAddr, tokenInfo, srcOffset, dstOffset, extAddr, goSize1, goSize2, goSize3, goSize4,
            channels, channelCount, rankSize, rankId);
    }
    return CcuAlltoAllVMesh1DKernelV1(
        inputAddr, outputAddr, tokenInfo, srcOffset, dstOffset, extAddr, goSize1, goSize2, goSize3, goSize4, channels,
        channelCount, rankSize, rankId);
}

} // namespace mc2_ops_hccl
