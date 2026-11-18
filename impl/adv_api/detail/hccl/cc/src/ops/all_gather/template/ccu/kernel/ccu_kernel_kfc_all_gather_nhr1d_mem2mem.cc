/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_kfc_all_gather_nhr1d_mem2mem.h"

namespace mc2_ops_hccl {
namespace {
// 与 hccl ccu_kernel_all_gather_nhr1d_mem2mem.cc 的 XN/CKE 位定义一致。
constexpr uint16_t AG_NHR_OUTPUT_XN_ID = 1;
constexpr uint16_t AG_NHR_TOKEN_XN_ID = 2;
constexpr uint16_t AG_NHR_POST_SYNC_BIT = 1U << 3U;
constexpr uint16_t AG_NHR_STEP_POST_SYNC_BIT = 1U << 5U;
constexpr uint16_t AG_NHR_CKE_IDX = 0;
constexpr uint16_t AG_NHR_BIT_NUM_PER_CKE = 16;

struct KfcAllGatherNhr1DContext : CcuKernelCtxBase {
    const ChannelHandle* channels = nullptr;
    uint32_t channelCount = 0;
    uint32_t rankSize = 0;
    uint32_t rankId = 0;
    const std::vector<KfcNhrStepInfo>* stepInfoVector = nullptr;
    const std::map<uint32_t, uint32_t>* rank2ChannelIdx = nullptr;

    ccu::variable input;
    std::vector<ccu::variable> output;
    std::vector<ccu::variable> token;
    ccu::variable die0Size;
    ccu::variable die0LastSize;
    ccu::variable repeatNum;
    ccu::variable inputSliceStride;
    ccu::variable outputSliceStride;
    ccu::variable inputRepeatStride;
    ccu::variable outputRepeatStride;
    ccu::event localEvent;
    ccu::variable repeatTimeflag;
    std::vector<ccu::variable> outputSliceOffset;
    ccu::variable myrankInputSliceOffset;
    ccu::local_addr srcMem;
    ccu::remote_addr dstMem;
    ccu::local_addr localDst;
    ccu::variable constVar1;
    GroupOpSizeVars goSize;
    ccu::variable groupCopyRepeatNum;
};

// 对应 hccl ParseKernelArg + InitResource：localSize/myRankIdx == channelCount（每 peer 一条通道）。
// 通道绑定变量必须以拷贝构造（句柄共享）保存：ccu::variable 的 operator= 是寄存器值搬运指令，
// 会在 PreSync 握手完成前执行，快照到对端写入前的旧槽位值（对端 output 地址/token 陈旧，
// 远端写将落到错误目的或因 token 失配被丢弃）。hccl 原版 InitResource 即用 push_back 活绑定。
CcuResult InitResource(KfcAllGatherNhr1DContext& ctx)
{
    if (ctx.rankSize == 0U || ctx.rankId >= ctx.rankSize || ctx.channelCount == 0U || ctx.rank2ChannelIdx == nullptr ||
        ctx.rank2ChannelIdx->size() != ctx.channelCount || ctx.stepInfoVector == nullptr) {
        HCCL_ERROR(
            "[CcuKernelKfcAllGatherNHR1DMem2Mem] invalid resource, rankSize[%u], rankId[%u], channels[%u]",
            ctx.rankSize, ctx.rankId, ctx.channelCount);
        return CCU_E_INTERNAL;
    }
    for (uint32_t channelIdx = 0; channelIdx < ctx.channelCount; ++channelIdx) {
        ctx.output.push_back(ccu::get_res_by_channel<ccu::variable>(ctx.channels[channelIdx], AG_NHR_OUTPUT_XN_ID));
        ctx.token.push_back(ccu::get_res_by_channel<ccu::variable>(ctx.channels[channelIdx], AG_NHR_TOKEN_XN_ID));
    }
    // 本地槽 [channelCount]：值来自本轮参数帧（kernel 启动前已写入），LoadNhr1DArgs 值赋值无竞争。
    ctx.output.push_back(ccu::variable());
    ctx.token.push_back(ccu::variable());
    ctx.outputSliceOffset.resize(ctx.rankSize);
    ctx.myrankInputSliceOffset = 0;
    ctx.repeatTimeflag = 0;
    ctx.constVar1 = 1;
    return CCU_SUCCESS;
}

// 对应 hccl LoadArgs：本 rank 的 output/token 从队列参数取得，其余形参直赋。
// sole-NHR 常量参数不占 xnData 槽，在此内联（prepare 恒定写入，见
// KfcAllGatherSoleNhrParamIndex）：die0LastSize=die0Size、repeatNum=1（REPEAT_NUM_INV=
// UINT64_MAX-1）、三 stride=0；isInputOutputEqual 经 goSize 全零表达（in-place 时 AIV 按
// 0 字节计算 goSize，GroupCopy 内部 CCU_IF 块均不触发），kernel 无需该参数。
void LoadNhr1DArgs(
    KfcAllGatherNhr1DContext& ctx, ccu::variable input, ccu::variable output, ccu::variable token,
    ccu::variable die0Size, ccu::variable die1Size, ccu::variable outputSliceStride, ccu::variable goSize0,
    ccu::variable goSize1, ccu::variable goSize2, ccu::variable goSize3, uint32_t axisId)
{
    const uint32_t localIdx = ctx.channelCount;
    ctx.input = input;
    ctx.output[localIdx] = output;
    ctx.token[localIdx] = token;
    // D4 双 die（hccl axisId 语义）：axis0 处理 [0, die0Size)、axis1 处理 [die0Size, +die1Size)。
    // die0Size/die0LastSize 字段复用为"本 die 档"（下游 IssueSendSlice/GroupCopy 档位选择零改动）；
    // axis1 将 die0Size 基偏移烘焙进 input/output[localIdx]——直发/转发源、拷贝源、以及 PreSync
    // 交换给对端同 die 内核的输出基均随之带偏移（对端 die1 内核写本端 [die0Size,+die1Size) 半区）。
    if (axisId == 0U) {
        ctx.die0Size = die0Size;
        ctx.die0LastSize = die0Size;
    } else {
        ctx.die0Size = die1Size;
        ctx.die0LastSize = die1Size;
        ctx.input += die0Size;
        ctx.output[localIdx] += die0Size;
    }
    ctx.repeatNum = UINT64_MAX - 1ULL;
    ctx.inputSliceStride = 0;
    ctx.outputSliceStride = outputSliceStride;
    ctx.inputRepeatStride = 0;
    ctx.outputRepeatStride = 0;
    ctx.goSize.addr_offset = goSize0;
    ctx.goSize.loop_param = goSize1;
    ctx.goSize.parallel_param = goSize2;
    ctx.goSize.residual = goSize3;
}

// 对应 hccl PreSync：向所有 peer 交换本 rank 的 output/token xn 变量。
CcuResult PreSync(KfcAllGatherNhr1DContext& ctx)
{
    const uint32_t localIdx = ctx.channelCount;
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::write_variable_with_notify(
            ctx.channels[i], ctx.output[localIdx], AG_NHR_OUTPUT_XN_ID, AG_NHR_CKE_IDX, 1U << AG_NHR_OUTPUT_XN_ID));
        CCU_CHK_RET(ccu::write_variable_with_notify(
            ctx.channels[i], ctx.token[localIdx], AG_NHR_TOKEN_XN_ID, AG_NHR_CKE_IDX, 1U << AG_NHR_TOKEN_XN_ID));
    }
    const uint16_t allBit = (1U << AG_NHR_OUTPUT_XN_ID) | (1U << AG_NHR_TOKEN_XN_ID);
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::notify_wait(ctx.channels[i], AG_NHR_CKE_IDX, allBit));
    }
    return CCU_SUCCESS;
}

// 对应 hccl PostSync。
CcuResult PostSync(KfcAllGatherNhr1DContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::notify_record(ctx.channels[i], AG_NHR_CKE_IDX, AG_NHR_POST_SYNC_BIT));
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::notify_wait(ctx.channels[i], AG_NHR_CKE_IDX, AG_NHR_POST_SYNC_BIT));
    }
    return CCU_SUCCESS;
}

// 只发射单个 slice 的 Write/占位 EventRecord，不等待；事件位由调用方分配。
// 流水路径步末批量 EventWait；串行路径逐 slice 等待（mesh DoAllGatherWrite/Wait 先例）。
CcuResult IssueSendSlice(
    KfcAllGatherNhr1DContext& ctx, const u32& toRank, ccu::local_addr& src, ccu::remote_addr& dst, u32 signalIndex,
    bool islastSlice)
{
    const ChannelHandle sendChannel = ctx.channels[ctx.rank2ChannelIdx->at(toRank)];
    ccu::variable& sliceSize = islastSlice ? ctx.die0LastSize : ctx.die0Size;
    const uint16_t signalMask = 1U << signalIndex;
    CCU_IF(sliceSize != 0) { CCU_CHK_RET(ccu::write(sendChannel, dst, src, sliceSize, ctx.localEvent, signalMask)); }
    CCU_ELSE { CCU_CHK_RET(ccu::event_record(ctx.localEvent, signalMask)); }
    return CCU_SUCCESS;
}

// 对应 hccl DoRepeatSendRecvSlices（串行回退路径；sliceSize 取"本 die 档"——D4 经 LoadNhr1DArgs
// 复用 die0Size/die0LastSize 字段实现，轴语义见其注释）。
// 串行回退路径：发射单个 slice 并立即等待其完成。
CcuResult DoRepeatSendRecvSlices(
    KfcAllGatherNhr1DContext& ctx, const u32& toRank, ccu::local_addr& src, ccu::remote_addr& dst, u32 signalIndex,
    bool islastSlice)
{
    const ChannelHandle sendChannel = ctx.channels[ctx.rank2ChannelIdx->at(toRank)];
    ccu::variable tmpRepeatNum;
    tmpRepeatNum = ctx.repeatNum;
    ctx.repeatTimeflag = 0;

    CCU_WHILE(tmpRepeatNum != UINT64_MAX)
    {
        tmpRepeatNum += ctx.constVar1;
        CCU_IF(ctx.repeatTimeflag == 1)
        {
            src.addr += ctx.inputRepeatStride;
            dst.addr += ctx.outputRepeatStride;
        }
        ccu::variable& sliceSize = islastSlice ? ctx.die0LastSize : ctx.die0Size;

        const uint16_t signalMask = 1U << signalIndex;
        CCU_IF(sliceSize != 0)
        {
            CCU_CHK_RET(ccu::write(sendChannel, dst, src, sliceSize, ctx.localEvent, signalMask));
            CCU_CHK_RET(ccu::event_wait(ctx.localEvent, signalMask));
        }
        ctx.repeatTimeflag = 1;
    }

    return CCU_SUCCESS;
}

// 对应 hccl DoAllGatherGroupCopy：本地拷贝自身分片。不再需要 isInputOutputEqual 判断——
// in-place 时 AIV 将 goSize 按 0 字节计算（全零），GroupCopy 内部两个 CCU_IF 块
// （addrOffset/parallelParam != 0）均不触发，拷贝退化为空。
CcuResult DoAllGatherGroupCopy(KfcAllGatherNhr1DContext& ctx)
{
    CCU_IF(ctx.groupCopyRepeatNum != UINT64_MAX)
    {
        ctx.repeatTimeflag = 0;
        CCU_WHILE(ctx.groupCopyRepeatNum != UINT64_MAX)
        {
            ctx.groupCopyRepeatNum += ctx.constVar1;
            CCU_IF(ctx.repeatTimeflag != 0)
            {
                ctx.localDst.addr += ctx.outputRepeatStride;
                ctx.srcMem.addr += ctx.inputRepeatStride;
            }
            CCU_CHK_RET(GroupCopy(ctx, ctx.localDst, ctx.srcMem, ctx.goSize));
            ctx.repeatTimeflag = 1;
        }
    }
    return CCU_SUCCESS;
}

// 对应 hccl DoRepeatAllGatherNHRSingleStep（末步跳过步间同步与 hccl 一致）。
// own slot 在各步均由 input 直发——txSliceIdxs 每步以 own slot 开头
// （见 ccu_temp_kfc_all_gather_nhr_1d_mem2mem.cc GetStepInfo），若走 output 转发会依赖
// GroupCopy 先落位；直发后 GroupCopy 推迟至末步提交，转发读 output 与拷贝写 own slot 无依赖。
// 性能优化：在途 slice 数不超 16 位事件预算时走步内流水——本轮所有 slice 先全部
// 发射（各占独立事件位；各 slice 的 src/dst 分片互不相交，无数据依赖），轮末一次
// EventWait 批量等完成，消除"发一个等一个"的完成等待空转间隙。
// 先例：mesh kernel DoAllGatherWrite/DoAllGatherWait 的 Phase1/Phase2 批量等待。
CcuResult DoRepeatAllGatherNHRSingleStep(KfcAllGatherNhr1DContext& ctx, const KfcNhrStepInfo& nhrStepInfo)
{
    const u32 toRankIdx = ctx.rank2ChannelIdx->at(nhrStepInfo.toRank);
    const u32 fromRankIdx = ctx.rank2ChannelIdx->at(nhrStepInfo.fromRank);
    const ChannelHandle sendChannel = ctx.channels[toRankIdx];
    const ChannelHandle recvChannel = ctx.channels[fromRankIdx];
    const std::vector<u32>& sendSliceIdxList = nhrStepInfo.txSliceIdxs;
    const uint32_t localIdx = ctx.channelCount;
    const bool isLastStep = (nhrStepInfo.step + 1 == ctx.stepInfoVector->size());

    ctx.srcMem.token = ctx.token[localIdx];
    ctx.dstMem.token = ctx.token[toRankIdx];

    const uint32_t sliceNum = static_cast<uint32_t>(sendSliceIdxList.size());
    if (sliceNum != 0U && sliceNum <= AG_NHR_BIT_NUM_PER_CKE) {
        // 流水路径 repeat 外层、slice 内层：同一 repeat 轮内所有 slice 共享一组事件位，
        // 轮末批量等待清位后再进下一轮；各分片地址 = 基址 + 分片偏移 + 帧偏移。
        const uint16_t waitMask = static_cast<uint16_t>((1U << sliceNum) - 1U);
        ccu::variable turnOffsetIn;
        ccu::variable turnOffsetOut;
        turnOffsetIn = 0;
        turnOffsetOut = 0;
        ccu::variable tmpRepeatNum;
        tmpRepeatNum = ctx.repeatNum;
        ctx.repeatTimeflag = 0;
        CCU_WHILE(tmpRepeatNum != UINT64_MAX)
        {
            tmpRepeatNum += ctx.constVar1;
            for (uint32_t sliceSeq = 0; sliceSeq < sliceNum; ++sliceSeq) {
                const u32 sendSliceIdx = sendSliceIdxList[sliceSeq];
                const bool islastSlice = (sendSliceIdx + 1 == ctx.rankSize);
                // own slot 由 input 直发（D1）：不依赖 GroupCopy 落位，拷贝方可推迟至末步
                if (nhrStepInfo.step == 0 || sendSliceIdx == ctx.rankId) {
                    ctx.srcMem.addr = ctx.input;
                    ctx.srcMem.addr += ctx.myrankInputSliceOffset;
                } else {
                    ctx.srcMem.addr = ctx.output[localIdx];
                    ctx.srcMem.addr += ctx.outputSliceOffset[sendSliceIdx];
                }
                ctx.srcMem.addr += turnOffsetIn;
                ctx.dstMem.addr = ctx.output[toRankIdx];
                ctx.dstMem.addr += ctx.outputSliceOffset[sendSliceIdx];
                ctx.dstMem.addr += turnOffsetOut;
                CCU_CHK_RET(IssueSendSlice(ctx, nhrStepInfo.toRank, ctx.srcMem, ctx.dstMem, sliceSeq, islastSlice));
            }
            if (isLastStep) {
                // 末批 Write 在飞时提交本地拷贝——LoopGroup 阻塞期间 XN 写引擎并发推进，
                // 拷贝被末步写耗时掩盖。重设源/目的地址（slice 循环已复用 srcMem 地址寄存器）；
                // repeatNum>1 时经 groupCopyRepeatNum 哨兵空转（仅首轮提交全部帧的拷贝）。
                ctx.srcMem.addr = ctx.input;
                ctx.srcMem.addr += ctx.myrankInputSliceOffset;
                ctx.localDst.addr = ctx.output[localIdx];
                ctx.localDst.addr += ctx.outputSliceOffset[ctx.rankId];
                CCU_CHK_RET(DoAllGatherGroupCopy(ctx));
            }
            CCU_CHK_RET(ccu::event_wait(ctx.localEvent, waitMask));
            turnOffsetIn += ctx.inputRepeatStride;
            turnOffsetOut += ctx.outputRepeatStride;
            ctx.repeatTimeflag = 1;
        }
    } else {
        // 串行回退（slice 数超 16 位事件预算或空步）：保持原"发一个等一个"结构。
        for (u32 i = 0; i < sliceNum; i++) {
            const u32 sendSliceIdx = sendSliceIdxList[i];
            if (nhrStepInfo.step == 0 || sendSliceIdx == ctx.rankId) {
                ctx.srcMem.addr = ctx.input;
                ctx.srcMem.addr += ctx.myrankInputSliceOffset;
            } else {
                ctx.srcMem.addr = ctx.output[localIdx];
                ctx.srcMem.addr += ctx.outputSliceOffset[sendSliceIdx];
            }
            ctx.dstMem.addr = ctx.output[toRankIdx];
            ctx.dstMem.addr += ctx.outputSliceOffset[sendSliceIdx];
            const bool islastSlice = (sendSliceIdx + 1 == ctx.rankSize);
            CCU_CHK_RET(DoRepeatSendRecvSlices(
                ctx, nhrStepInfo.toRank, ctx.srcMem, ctx.dstMem, i % AG_NHR_BIT_NUM_PER_CKE, islastSlice));
        }
        if (isLastStep) {
            // 串行写逐笔等待、已无在途批次，拷贝提前于步同步/PostSync 串行执行（正确性等价）
            ctx.srcMem.addr = ctx.input;
            ctx.srcMem.addr += ctx.myrankInputSliceOffset;
            ctx.localDst.addr = ctx.output[localIdx];
            ctx.localDst.addr += ctx.outputSliceOffset[ctx.rankId];
            CCU_CHK_RET(DoAllGatherGroupCopy(ctx));
        }
    }

    // D3：末步也做步间同步——turn-end 提前到 PostSync 前的前置条件（末步接收分片的到达
    // 保证由此承担，与 hccl 仅末步跳过同步不同）；record 先于 wait 且不阻塞，无死锁。
    CCU_CHK_RET(ccu::notify_record(sendChannel, AG_NHR_CKE_IDX, AG_NHR_STEP_POST_SYNC_BIT));
    CCU_CHK_RET(ccu::notify_wait(recvChannel, AG_NHR_CKE_IDX, AG_NHR_STEP_POST_SYNC_BIT));

    return CCU_SUCCESS;
}

// 对应 hccl DoRepeatAllGatherNHR（axisId==1 的 die0 偏移经 LoadNhr1DArgs 烘焙进 input/output 基址，
// 本函数零改动——D4）。
// GroupCopy 移至末步内与通信并发提交（见 DoRepeatAllGatherNHRSingleStep），
// 本 rank 贡献在各步均由 input 直发，step 循环前不再需要先落位。
CcuResult DoRepeatAllGatherNHR(KfcAllGatherNhr1DContext& ctx)
{
    ccu::variable tmpSliceOffset;
    tmpSliceOffset = 0;

    for (u64 i = 0; i < ctx.rankId; i++) {
        ctx.myrankInputSliceOffset += ctx.inputSliceStride;
    }

    for (u64 i = 0; i < ctx.rankSize; i++) {
        ctx.outputSliceOffset[i] = tmpSliceOffset;
        tmpSliceOffset += ctx.outputSliceStride;
    }

    const uint32_t localIdx = ctx.channelCount;
    ctx.srcMem.addr = ctx.input;
    ctx.srcMem.addr += ctx.myrankInputSliceOffset;
    ctx.srcMem.token = ctx.token[localIdx];
    ctx.localDst.addr = ctx.output[localIdx];
    ctx.localDst.addr += ctx.outputSliceOffset[ctx.rankId];
    ctx.localDst.token = ctx.token[localIdx];
    ctx.groupCopyRepeatNum = ctx.repeatNum;

    if (ctx.stepInfoVector->empty()) {
        // 空步骤向量（防御）：退化为仅本地拷贝
        CCU_CHK_RET(DoAllGatherGroupCopy(ctx));
        return CCU_SUCCESS;
    }

    for (const KfcNhrStepInfo& nhrStepInfo : *ctx.stepInfoVector) {
        CCU_CHK_RET(DoRepeatAllGatherNHRSingleStep(ctx, nhrStepInfo));
    }

    return CCU_SUCCESS;
}
} // namespace

CcuResult CcuKernelKfcAllGatherNHR1DMem2MemKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable die0Size,
    ccu::variable die1Size, ccu::variable outputSliceStride, ccu::variable goSize0, ccu::variable goSize1,
    ccu::variable goSize2, ccu::variable goSize3, const ChannelHandle channels[], uint32_t channelCount,
    uint32_t rankSize, uint32_t rankId, uint32_t axisId, const std::vector<KfcNhrStepInfo>& stepInfoVector,
    const std::map<uint32_t, uint32_t>& rank2ChannelIdx, ccu::variable turnEndAddr)
{
    KfcAllGatherNhr1DContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.stepInfoVector = &stepInfoVector;
    ctx.rank2ChannelIdx = &rank2ChannelIdx;
    InitCcuKernelCtxBase(ctx);
    CCU_CHK_RET(InitResource(ctx));

    LoadNhr1DArgs(
        ctx, inputAddr, outputAddr, tokenInfo, die0Size, die1Size, outputSliceStride, goSize0, goSize1, goSize2,
        goSize3, axisId);

    CCU_CHK_RET(PreSync(ctx));

    CCU_CHK_RET(DoRepeatAllGatherNHR(ctx));

    // D3：末步同步完成后本 mission 半区输出已完整（各步接收分片到达 + GroupCopy 阻塞落位 +
    // 本 rank 写 EventWait；双 die 两 mission 数据不相交，AIV 等齐两个 turn-end 即全量完整），
    // 在 PostSync 前写 turn-end，AIV 提前返回；PostSync 仍在 server 指令流中先于下一轮
    // PreSync 执行，跨轮 XN 保护不变。
    CCU_CHK_RET(ccu::store(turnEndAddr, ctx.constVar1));

    CCU_CHK_RET(PostSync(ctx));

    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
