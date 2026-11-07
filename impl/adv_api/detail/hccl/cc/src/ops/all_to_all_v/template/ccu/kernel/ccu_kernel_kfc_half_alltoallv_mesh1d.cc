/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ccu_kernel_kfc_half_alltoallv_mesh1d.h"

#include <algorithm>

namespace mc2_ops_hccl {

constexpr int HA2AV_BUFFER_XN_ID = 1;
constexpr int HA2AV_TOKEN_XN_ID = 2;
constexpr int HA2AV_POST_SYNC_CKE_IDX = 0;
constexpr int HA2AV_BUFFER_CKE_IDX = 1;
constexpr int HA2AV_TOKEN_CKE_IDX = 2;
constexpr uint16_t HA2AV_POST_SYNC_BIT = 1U;
constexpr uint16_t HA2AV_BUFFER_BIT = 1U << HA2AV_BUFFER_XN_ID;
constexpr uint16_t HA2AV_TOKEN_BIT = 1U << HA2AV_TOKEN_XN_ID;
// 每个 Event 有 16 个 bit，一个 peer 占一个 bit，与 AllGather Mem2Mem 口径一致
constexpr uint32_t HA2AV_BITS_PER_EVENT = 16;
// 双 mission 并发档位：missionNum 的合法取值（单 mission=1，KFC 双 mission=2）
constexpr uint32_t HA2AV_DUAL_MISSION_NUM = 2;

// 双 mission 共享同一组 channel：PreSync/PostSync 的通知位按 missionIndex 错开，
// 否则同一位被两个 mission 各 post 一次、各 wait 一次，wait 消费位后第二个 wait
// 永远等不到（死锁）。hostMissionIndex 为构图期常量，这里用编译期三元选位。
constexpr uint16_t HA2AV_POST_SYNC_BITS[KFC_MAX_MISSION_NUM] = {HA2AV_POST_SYNC_BIT, HA2AV_POST_SYNC_BIT << 1};
constexpr uint16_t HA2AV_BUFFER_BITS[KFC_MAX_MISSION_NUM] = {HA2AV_BUFFER_BIT, HA2AV_BUFFER_BIT << 1};
constexpr uint16_t HA2AV_TOKEN_BITS[KFC_MAX_MISSION_NUM] = {HA2AV_TOKEN_BIT, HA2AV_TOKEN_BIT << 1};

struct KfcHalfAlltoAllVMesh1DContext : CcuKernelCtxBase {
    const ChannelHandle* channels{nullptr};
    uint32_t channelCount{0};
    uint32_t rankSize{0};
    uint32_t rankId{0};

    ccu::Variable userInAddr;
    ccu::Variable recvOffset;
    // 建图期常量：本端 window 基址
    ccu::Variable localWinAddr;
    // peerBuffer[peerId] 为对端 cclBuffer 首地址；本 rank 槽位由 myBufferAddr 填入
    std::vector<ccu::Variable> peerBuffer;
    std::vector<ccu::Variable> token;
    // 前 rankSize 个为 A 片参数，后 rankSize 个为 B 片参数
    std::unique_ptr<ccu::Array<ccu::Variable>> sendSize;
    std::unique_ptr<ccu::Array<ccu::Variable>> sendOffset;
    GroupOpSizeVars goSize;
    std::vector<ccu::Event> writeDoneEvents;

    ccu::Variable missionNum;
    ccu::Variable missionIndex;
    // 构图期常量（CCU_IF 为运行时分支，构图期两分支都会执行，tag/位选择必须用宿主常量）
    uint32_t hostMissionIndex = 0;
    uint32_t hostMissionNum = 1;
};

static CcuResult InitResource(KfcHalfAlltoAllVMesh1DContext& ctx)
{
    if (ctx.rankSize == 0 || ctx.rankId >= ctx.rankSize || ctx.channelCount + 1 != ctx.rankSize) {
        HCCL_ERROR(
            "[CcuKfcHalfAlltoAllVMesh1D] invalid rank/channel, rankSize[%u], rankId[%u], channelCount[%u]",
            ctx.rankSize, ctx.rankId, ctx.channelCount);
        return CcuResult::CCU_E_INTERNAL;
    }

    ctx.peerBuffer.resize(ctx.rankSize);
    ctx.token.resize(ctx.rankSize);
    // 按 rank 号升序遍历 channel，遇到本 rank 跳过，要求框架返回的 channel 同样按序排列
    uint32_t channelIdx = 0;
    for (uint32_t peerId = 0; peerId < ctx.rankSize; peerId++) {
        if (peerId == ctx.rankId) {
            continue;
        }
        ctx.peerBuffer[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], HA2AV_BUFFER_XN_ID);
        ctx.token[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], HA2AV_TOKEN_XN_ID);
        channelIdx++;
    }

    const uint32_t sliceArgNum = ctx.rankSize * NUM_TWO;
    ctx.sendSize = std::make_unique<ccu::Array<ccu::Variable>>(sliceArgNum);
    ctx.sendOffset = std::make_unique<ccu::Array<ccu::Variable>>(sliceArgNum);

    const uint32_t eventNum = (ctx.rankSize + HA2AV_BITS_PER_EVENT - 1) / HA2AV_BITS_PER_EVENT;
    ctx.writeDoneEvents.resize(eventNum);
    ctx.resourceAllocated = false;
    return CCU_SUCCESS;
}

// 从设备内存读入 A/B 切片参数（sendSize/sendOffset 各 2*rankSize 个 u64）
static CcuResult LoadSliceArgs(
    KfcHalfAlltoAllVMesh1DContext& ctx, ccu::Variable sendSizeAddr, ccu::Variable sendOffsetAddr)
{
    const uint32_t sliceArgNum = ctx.rankSize * NUM_TWO;
    CCU_CHK_RET(ccu::Load(sendSizeAddr, *ctx.sendSize, sliceArgNum));
    CCU_CHK_RET(ccu::Load(sendOffsetAddr, *ctx.sendOffset, sliceArgNum));
    return CCU_SUCCESS;
}

static CcuResult LoadArgsFromMem(
    KfcHalfAlltoAllVMesh1DContext& ctx, ccu::Variable userInAddr, ccu::Variable sendSizeAddr,
    ccu::Variable sendOffsetAddr, ccu::Variable goSize0, ccu::Variable goSize1, ccu::Variable goSize2,
    ccu::Variable goSize3, ccu::Variable recvOffset, ccu::Variable tokenInfo)
{
    ctx.userInAddr = userInAddr;
    // 本端 window 基址来自建图期的 kernelArg，非运行时 xn 参数
    ctx.peerBuffer[ctx.rankId] = ctx.localWinAddr;
    ctx.token[ctx.rankId] = tokenInfo;
    ctx.recvOffset = recvOffset;
    ctx.goSize.addrOffset = goSize0;
    ctx.goSize.loopParam = goSize1;
    ctx.goSize.parallelParam = goSize2;
    ctx.goSize.residual = goSize3;
    return LoadSliceArgs(ctx, sendSizeAddr, sendOffsetAddr);
}

static CcuResult PreSync(KfcHalfAlltoAllVMesh1DContext& ctx)
{
    // 广播本端 cclBuffer 地址与 token；通知位按 missionIndex 错开
    const uint16_t bufferBit = HA2AV_BUFFER_BITS[ctx.hostMissionIndex];
    const uint16_t tokenBit = HA2AV_TOKEN_BITS[ctx.hostMissionIndex];
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.peerBuffer[ctx.rankId], HA2AV_BUFFER_XN_ID, HA2AV_BUFFER_CKE_IDX, bufferBit));
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.token[ctx.rankId], HA2AV_TOKEN_XN_ID, HA2AV_TOKEN_CKE_IDX, tokenBit));
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], HA2AV_BUFFER_CKE_IDX, bufferBit));
        CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], HA2AV_TOKEN_CKE_IDX, tokenBit));
    }
    return CCU_SUCCESS;
}

// 源端保序方案，当前最大支持 8P 通信域；通道保序方案明确后可放开
static CcuResult PostSync(KfcHalfAlltoAllVMesh1DContext& ctx)
{
    // 远程 Post 同步仅 mission 0 执行；CCU_IF 内不可提前 return
    CCU_IF(ctx.missionNum == HA2AV_DUAL_MISSION_NUM)
    {
        // 双 mission 模式
        CCU_IF(ctx.missionIndex == 0)
        {
            // mission 0 执行远程 Post 同步
            for (uint32_t i = 0; i < ctx.channelCount; i++) {
                CCU_CHK_RET(ccu::NotifyRecord(ctx.channels[i], HA2AV_POST_SYNC_CKE_IDX, HA2AV_POST_SYNC_BIT));
            }
            for (uint32_t i = 0; i < ctx.channelCount; i++) {
                CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], HA2AV_POST_SYNC_CKE_IDX, HA2AV_POST_SYNC_BIT));
            }
        }
        // mission 1: 什么都不做
    }
    CCU_ELSE
    {
        // 单 mission 模式：执行远程 Post 同步
        for (uint32_t i = 0; i < ctx.channelCount; i++) {
            CCU_CHK_RET(ccu::NotifyRecord(ctx.channels[i], HA2AV_POST_SYNC_CKE_IDX, HA2AV_POST_SYNC_BIT));
        }
        for (uint32_t i = 0; i < ctx.channelCount; i++) {
            CCU_CHK_RET(ccu::NotifyWait(ctx.channels[i], HA2AV_POST_SYNC_CKE_IDX, HA2AV_POST_SYNC_BIT));
        }
    }
    return CCU_SUCCESS;
}

// 按单/双 mission 选取本端 peerId 的发送参数：双 mission 下 m0 发 A 片（sendSize 前段）、
// m1 发 B 片（sendSize 后段，目的地址 += sizeA）；单 mission 合并 A+B 一次搬运。
// ctx 不可取 const：CCU_IF 宏内部对 missionNum/missionIndex 调用非 const 的比较运算符。
static void SelectSendSlice(
    KfcHalfAlltoAllVMesh1DContext& ctx, uint32_t peerId, ccu::Variable& curCount, ccu::Variable& curOffset)
{
    const ccu::Variable sizeA = (*ctx.sendSize)[peerId];
    const ccu::Variable sizeB = (*ctx.sendSize)[ctx.rankSize + peerId];
    const ccu::Variable offsetA = (*ctx.sendOffset)[peerId];
    const ccu::Variable offsetB = (*ctx.sendOffset)[ctx.rankSize + peerId];

    CCU_IF(ctx.missionNum == 1)
    {
        curCount = sizeA + sizeB;
        curOffset = offsetA;
    }
    CCU_ELSE
    {
        CCU_IF(ctx.missionIndex == 0)
        {
            curCount = sizeA;
            curOffset = offsetA;
        }
        CCU_ELSE
        {
            curCount = sizeB;
            curOffset = offsetB;
        }
    }
}

// 双 mission 下 m1 的接收地址须跳过 m0 的数据段：偏移量是本端发往该 peer 的 A 片大小，
static void ApplyDualMissionRecvOffset(KfcHalfAlltoAllVMesh1DContext& ctx, ccu::Address& addr, uint32_t peerId)
{
    CCU_IF(ctx.missionNum == HA2AV_DUAL_MISSION_NUM)
    {
        CCU_IF(ctx.missionIndex != 0) { addr += (*ctx.sendSize)[peerId]; }
    }
}

static CcuResult DoHalfAlltoAllVWrite(
    KfcHalfAlltoAllVMesh1DContext& ctx, ccu::LocalAddr& localSrc, ccu::LocalAddr& localDst)
{
    ccu::Variable curCount;
    ccu::Variable curOffset;
    uint32_t channelIdx = 0;

    for (uint32_t peerId = 0; peerId < ctx.rankSize; peerId++) {
        const uint32_t eventIdx = peerId / HA2AV_BITS_PER_EVENT;
        const uint16_t peerBit = 1U << (peerId % HA2AV_BITS_PER_EVENT);

        SelectSendSlice(ctx, peerId, curCount, curOffset);

        ccu::LocalAddr src;
        src.addr = ctx.userInAddr;
        src.addr += curOffset;
        src.token = ctx.token[ctx.rankId];

        if (peerId == ctx.rankId) {
            localSrc.addr = src.addr;
            localSrc.token = src.token;
            localDst.addr = ctx.peerBuffer[ctx.rankId];
            localDst.addr += ctx.recvOffset;

            ApplyDualMissionRecvOffset(ctx, localDst.addr, peerId);

            localDst.token = ctx.token[ctx.rankId];
            CCU_CHK_RET(ccu::EventRecord(ctx.writeDoneEvents[eventIdx], peerBit));
            continue;
        }

        ccu::RemoteAddr dst;
        dst.addr = ctx.peerBuffer[peerId];
        dst.addr += ctx.recvOffset;

        ApplyDualMissionRecvOffset(ctx, dst.addr, peerId);

        dst.token = ctx.token[peerId];

        // 空分片只置位不发数据
        CCU_IF(curCount != 0)
        {
            CCU_CHK_RET(
                ccu::Write(ctx.channels[channelIdx], dst, src, curCount, ctx.writeDoneEvents[eventIdx], peerBit));
        }
        CCU_IF(curCount == 0) { CCU_CHK_RET(ccu::EventRecord(ctx.writeDoneEvents[eventIdx], peerBit)); }
        channelIdx++;
    }
    return CCU_SUCCESS;
}

static CcuResult DoHalfAlltoAllVWait(KfcHalfAlltoAllVMesh1DContext& ctx)
{
    const uint32_t eventNum = ctx.writeDoneEvents.size();
    for (uint32_t eventIdx = 0; eventIdx < eventNum; eventIdx++) {
        const uint32_t remainingRanks = ctx.rankSize - eventIdx * HA2AV_BITS_PER_EVENT;
        const uint32_t bitCount = std::min(remainingRanks, HA2AV_BITS_PER_EVENT);
        const uint16_t eventMask = bitCount == HA2AV_BITS_PER_EVENT ? static_cast<uint16_t>(UINT16_MAX) :
                                                                      static_cast<uint16_t>((1U << bitCount) - 1);
        CCU_CHK_RET(ccu::EventWait(ctx.writeDoneEvents[eventIdx], eventMask));
    }
    return CCU_SUCCESS;
}

static CcuResult DoHalfAlltoAllV(KfcHalfAlltoAllVMesh1DContext& ctx)
{
    ccu::LocalAddr localSrc;
    ccu::LocalAddr localDst;
    CCU_CHK_RET(DoHalfAlltoAllVWrite(ctx, localSrc, localDst));

    // 本地拷贝仅 mission 0 执行
    CCU_IF(ctx.missionIndex == 0) { CCU_CHK_RET(GroupCopy(ctx, localDst, localSrc, ctx.goSize, GetCcuVersion())); }

    CCU_CHK_RET(DoHalfAlltoAllVWait(ctx));

    // 收尾栅栏：两 mission 互等对方 writeDone 全清；bit 按 mission 错开
    if (ctx.hostMissionNum == KFC_MAX_MISSION_NUM) {
        if (ctx.hostMissionIndex == 0) {
            CCU_CHK_RET(ccu::EventRecord("MiFin_m0", 1U << 0));
            CCU_CHK_RET(ccu::EventWait("MiFin_m1", 1U << 1));
        } else {
            CCU_CHK_RET(ccu::EventRecord("MiFin_m1", 1U << 1));
            CCU_CHK_RET(ccu::EventWait("MiFin_m0", 1U << 0));
        }
    }
    return CCU_SUCCESS;
}

// 通信主流程：PreSync 广播 window/token → DoHalfAlltoAllV 收发搬运 → PostSync 远程同步
static CcuResult RunHalfAlltoAllV(KfcHalfAlltoAllVMesh1DContext& ctx)
{
    CCU_CHK_RET(PreSync(ctx));
    CCU_CHK_RET(DoHalfAlltoAllV(ctx));
    CCU_CHK_RET(PostSync(ctx));
    return CCU_SUCCESS;
}

CcuResult CcuKfcHalfAlltoAllVMesh1DKernel(
    ccu::Variable userInAddr, ccu::Variable sendSizeAddr, ccu::Variable sendOffsetAddr, ccu::Variable goSize0,
    ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3, ccu::Variable recvOffset,
    ccu::Variable tokenInfo, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId,
    uint64_t localWinAddr, ccu::Variable missionNum, ccu::Variable missionIndex, uint32_t hostMissionIndex,
    uint32_t hostMissionNum)
{
    KfcHalfAlltoAllVMesh1DContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    InitCcuKernelCtxBase(ctx);

    ctx.missionNum = missionNum;
    ctx.missionIndex = missionIndex;
    ctx.hostMissionIndex = hostMissionIndex;
    ctx.hostMissionNum = hostMissionNum;

    CCU_CHK_RET(InitResource(ctx));
    ctx.localWinAddr = localWinAddr;
    CCU_CHK_RET(LoadArgsFromMem(
        ctx, userInAddr, sendSizeAddr, sendOffsetAddr, goSize0, goSize1, goSize2, goSize3, recvOffset, tokenInfo));
    CCU_CHK_RET(RunHalfAlltoAllV(ctx));
    return CCU_SUCCESS;
}
} // namespace mc2_ops_hccl
