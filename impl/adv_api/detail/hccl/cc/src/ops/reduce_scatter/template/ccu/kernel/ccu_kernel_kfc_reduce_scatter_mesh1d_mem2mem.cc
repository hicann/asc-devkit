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
#include "ccu_kernel_kfc_reduce_scatter_mesh1d_mem2mem.h"
#include "mc2_type.h"

namespace mc2_ops_hccl {
using namespace hcomm;

constexpr int INPUT_XN_ID = 0;
constexpr int SCRATCH_XN_ID = 1;
constexpr int TOKEN_XN_ID = 2;
constexpr int POST_SYNC_ID = 3;
constexpr int CKE_IDX_0 = 0;
constexpr uint16_t REDUCE_SCATTER_GROUP_REDUCE_MAX_PIECE_CNT = 8;
constexpr uint16_t REDUCE_SCATTER_LOOP_COUNT = 16;
constexpr uint16_t BIT_NUM_PER_CKE = 16; // CKE 的位数,一个 CKE 可处理 16 种信号
constexpr uint32_t RS_UNROLL_NUM = 16;   // 最多支持 8 * 16 = 128 个 rank
// GroupLocalReduce 内 ccu::LoopGroup 的重复次数(moConfig.loopCount),用于 GetLoopParam/GetParallelParam。
// 必须与宿主 CcuPrepareForReduceScatterM2M 里 CalcGoSize(chunkSize, loopCount, ...) 的 loopCount、
// 以及 hccl_inner_def.h 中 CCU_LOOP_COUNT_M2M_RE 保持一致(当前均为 16),否则 goSize 与 LoopGroup 步进错位。
// 本 TU 未 include hccl_inner_def.h,故在此本地定义;若后续该头进入 include 路径,改为引用它即可。
constexpr uint64_t CCU_LOOP_COUNT_M2M_RE = 16;

struct KfcReduceScatterMesh1DMem2MemContext : CcuKernelCtxBase {
    const ChannelHandle* channels{nullptr};
    uint32_t channelCount{0};
    uint32_t rankSize{0};
    uint32_t rankId{0};

    HcclDataType dataType;
    HcclDataType outputDataType;
    HcclReduceOp reduceOp;

    std::vector<ccu::Variable> input;
    ccu::Variable output;
    std::vector<ccu::Variable> scratch;
    std::vector<ccu::Variable> token;
    ccu::Variable currentRankSliceInputOffset;
    ccu::Variable currentRankSliceOutputOffset;
    ccu::Variable chunkSize;
    ccu::Variable chunkLoopNum;
    ccu::Variable currentSliceSize;
    ccu::Variable tailSize;
    GroupOpSizeVars fullGoSize;
    GroupOpSizeVars tailGoSize;
    GroupOpSizeVars goSize; // 当前 chunk 选中的 goSize(full 或 tail)

    // 三阶段设计字段(从 hccl ReduceScatterMesh1DMem2MemContext 移植)
    ccu::Variable normalSliceSize;
    ccu::Variable lastSliceSize;
    ccu::Variable inputRepeatStride;
    ccu::Variable outputRepeatStride;
    ccu::Variable scratchRepeatStride;
    ccu::Variable repeatNum;
    ccu::Variable flag;
    ccu::Variable constVar1;
    ccu::Variable sliceSize;
    ccu::Variable readRepeatNum;
    ccu::Variable waitRepeatNum;
    std::vector<ccu::Event> events;

    ccu::LocalAddr myInput;
    std::vector<ccu::RemoteAddr> remoteInput;
    std::vector<ccu::LocalAddr> scratchMem;
    ccu::Event event;
};

static CcuResult InitResource(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    uint32_t channelIdx = 0;

    if (ctx.channelCount == 0) {
        HCCL_ERROR("[KfcReduceScatterMesh1DMem2Mem] channels is empty!");
        return CcuResult::CCU_E_INTERNAL;
    }

    ctx.input.resize(ctx.rankSize);
    ctx.scratch.resize(ctx.rankSize);
    ctx.token.resize(ctx.rankSize);
    ctx.remoteInput.resize(ctx.rankSize);
    ctx.scratchMem.resize(ctx.rankSize);

    for (uint64_t peerId = 0; peerId < ctx.rankSize; peerId++) {
        if (peerId != ctx.rankId) {
            ctx.input[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], INPUT_XN_ID);
            ctx.scratch[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], SCRATCH_XN_ID);
            ctx.token[peerId] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[channelIdx], TOKEN_XN_ID);
            channelIdx++;
        }
    }

    // 三阶段 events:RS_UNROLL_NUM * numEventsPerIter(单 die 下大部分空着,多 die 才用满)
    uint32_t numEventsPerIter = (ctx.rankSize + BIT_NUM_PER_CKE - 1) / BIT_NUM_PER_CKE;
    ctx.events.resize(RS_UNROLL_NUM * numEventsPerIter);
    ctx.constVar1 = 1;
    return CCU_SUCCESS;
}

static CcuResult LoadArgs(
    KfcReduceScatterMesh1DMem2MemContext& ctx, ccu::Variable inputAddr, ccu::Variable outputAddr,
    ccu::Variable tokenInfo, ccu::Variable scratch, ccu::Variable currentRankSliceInputOffset, ccu::Variable chunkSize,
    ccu::Variable chunkLoopNum, ccu::Variable tailSize, ccu::Variable fullGoAddrOffset, ccu::Variable fullGoLoopParam,
    ccu::Variable fullGoParallelParam, ccu::Variable fullGoResidual, ccu::Variable tailGoAddrOffset,
    ccu::Variable tailGoLoopParam, ccu::Variable tailGoParallelParam, ccu::Variable tailGoResidual)
{
    ctx.input[ctx.rankId] = inputAddr;
    ctx.output = outputAddr;
    ctx.token[ctx.rankId] = tokenInfo;
    ctx.scratch[ctx.rankId] = scratch;
    ctx.currentRankSliceInputOffset = currentRankSliceInputOffset;
    ctx.currentRankSliceOutputOffset = 0;
    ctx.chunkSize = chunkSize;
    ctx.chunkLoopNum = chunkLoopNum;
    ctx.tailSize = tailSize;
    ctx.fullGoSize.addrOffset = fullGoAddrOffset;
    ctx.fullGoSize.loopParam = fullGoLoopParam;
    ctx.fullGoSize.parallelParam = fullGoParallelParam;
    ctx.fullGoSize.residual = fullGoResidual;
    ctx.tailGoSize.addrOffset = tailGoAddrOffset;
    ctx.tailGoSize.loopParam = tailGoLoopParam;
    ctx.tailGoSize.parallelParam = tailGoParallelParam;
    ctx.tailGoSize.residual = tailGoResidual;
    return CCU_SUCCESS;
}

static CcuResult PreSync(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.input[ctx.rankId], INPUT_XN_ID, CKE_IDX_0, 1 << INPUT_XN_ID));
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.scratch[ctx.rankId], SCRATCH_XN_ID, CKE_IDX_0, 1 << SCRATCH_XN_ID));
        CCU_CHK_RET(ccu::WriteVariableWithNotify(
            ctx.channels[i], ctx.token[ctx.rankId], TOKEN_XN_ID, CKE_IDX_0, 1 << TOKEN_XN_ID));
    }
    uint32_t allBit = 1 << INPUT_XN_ID | 1 << SCRATCH_XN_ID | 1 << TOKEN_XN_ID;
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyWait(ctx.channels[i], CKE_IDX_0, allBit);
    }
    return CCU_SUCCESS;
}

static CcuResult PostSync(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyRecord(ctx.channels[i], CKE_IDX_0, 1 << POST_SYNC_ID);
    }
    for (uint32_t i = 0; i < ctx.channelCount; i++) {
        ccu::NotifyWait(ctx.channels[i], CKE_IDX_0, 1 << POST_SYNC_ID);
    }
    return CCU_SUCCESS;
}

static void DoReduceScatterRead(KfcReduceScatterMesh1DMem2MemContext& ctx, uint32_t unrollIdx)
{
    uint32_t channelId = 0;
    uint32_t numEventsPerIter = (ctx.rankSize + BIT_NUM_PER_CKE - 1) / BIT_NUM_PER_CKE;

    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        uint32_t eventIdx = unrollIdx * numEventsPerIter + rankIdx / BIT_NUM_PER_CKE;
        uint16_t rankMask = 1U << (rankIdx % BIT_NUM_PER_CKE);

        if (rankIdx == ctx.rankId) {
            ccu::LocalCopy(ctx.scratchMem[rankIdx], ctx.myInput, ctx.sliceSize, ctx.events[eventIdx], rankMask);
        } else {
            ccu::Read(
                ctx.channels[channelId], ctx.scratchMem[rankIdx], ctx.remoteInput[rankIdx], ctx.sliceSize,
                ctx.events[eventIdx], rankMask);
            channelId++;
        }
    }
}

// Phase2: 批量 EventWait
static void DoReduceScatterWait(KfcReduceScatterMesh1DMem2MemContext& ctx, uint32_t unrollIdx)
{
    uint32_t numEventsPerIter = (ctx.rankSize + BIT_NUM_PER_CKE - 1) / BIT_NUM_PER_CKE;
    for (uint32_t i = 0; i < numEventsPerIter; i++) {
        uint32_t eventIdx = unrollIdx * numEventsPerIter + i;
        uint32_t sigNum = BIT_NUM_PER_CKE;
        if (ctx.rankSize % BIT_NUM_PER_CKE != 0 && i == (numEventsPerIter - 1)) {
            sigNum = ctx.rankSize % BIT_NUM_PER_CKE;
        }
        uint16_t allBit = static_cast<uint16_t>((1U << sigNum) - 1);
        ccu::EventWait(ctx.events[eventIdx], allBit);
    }
}

// 大 rankSize 兜底:log2 两两规约
static CcuResult PairwiseLocalReduce(
    KfcReduceScatterMesh1DMem2MemContext& ctx, ccu::LocalAddr myOutput, std::vector<ccu::LocalAddr>& inputVec,
    ccu::Variable sliceSize)
{
    ccu::Variable len;

    uint32_t remainPieces = ctx.rankSize;
    while (remainPieces > 1) {
        // 每轮将最后 remain/2 块 reduce 到最前 remain/2 块
        uint32_t reducePieces = remainPieces / 2;
        uint32_t srcIdx = remainPieces - reducePieces;
        uint32_t dstIdx = 0;

        len = sliceSize;
        for (uint32_t i = 0; i < reducePieces - 1; i++) {
            len += sliceSize;
        }

        ccu::LocalReduce(inputVec[dstIdx], inputVec[srcIdx], len, ctx.dataType, ctx.reduceOp, ctx.events[0], 1);
        ccu::EventWait(ctx.events[0], 1);

        remainPieces -= reducePieces;
    }

    ccu::LocalCopy(myOutput, inputVec[0], sliceSize, ctx.events[0], 1);
    ccu::EventWait(ctx.events[0], 1);

    return CCU_SUCCESS;
}

// Phase3 单次规约:小 rankSize 走 GroupLocalReduce(= hccl ReduceLoopGroupV1),大 rankSize 走 PairwiseLocalReduce
static CcuResult DoReduceScatter(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    ccu::LocalAddr myOutput;
    myOutput.addr = ctx.output;
    myOutput.addr += ctx.currentRankSliceOutputOffset;
    myOutput.token = ctx.token[ctx.rankId];

    CCU_IF(ctx.sliceSize != 0)
    {
        if (ctx.rankSize <= REDUCE_SCATTER_GROUP_REDUCE_MAX_PIECE_CNT) {
            std::vector<ccu::LocalAddr> scratch = ctx.scratchMem;
            CCU_CHK_RET(
                GroupLocalReduce(ctx, myOutput, scratch, ctx.goSize, ctx.dataType, ctx.outputDataType, ctx.reduceOp));
        } else {
            CCU_CHK_RET(PairwiseLocalReduce(ctx, myOutput, ctx.scratchMem, ctx.sliceSize));
        }
    }
    return CCU_SUCCESS;
}

// 设本 chunk 的基址(myInput/remoteInput/scratchMem 从当前 input/scratch 派生)
static void InitReduceScatterAddr(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    ccu::Variable scratchOffset;
    scratchOffset = 0;

    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        if (rankIdx == ctx.rankId) {
            ctx.myInput.addr = ctx.input[rankIdx];
            ctx.myInput.addr += ctx.currentRankSliceInputOffset;
            ctx.myInput.token = ctx.token[rankIdx];
        } else {
            ctx.remoteInput[rankIdx].addr = ctx.input[rankIdx];
            ctx.remoteInput[rankIdx].addr += ctx.currentRankSliceInputOffset;
            ctx.remoteInput[rankIdx].token = ctx.token[rankIdx];
        }

        ctx.scratchMem[rankIdx].addr = ctx.scratch[ctx.rankId];
        ctx.scratchMem[rankIdx].addr += scratchOffset;
        scratchOffset += ctx.sliceSize;
        ctx.scratchMem[rankIdx].token = ctx.token[ctx.rankId];
    }
}

// Phase3 前复位:myInput/scratchMem 回本 chunk 基址
static void ResetReduceScatterAddr(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    ccu::Variable scratchOffset;
    scratchOffset = 0;
    ctx.myInput.addr = ctx.input[ctx.rankId];
    ctx.myInput.addr += ctx.currentRankSliceInputOffset;
    for (uint32_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
        ctx.scratchMem[rankIdx].addr = ctx.scratch[ctx.rankId];
        ctx.scratchMem[rankIdx].addr += scratchOffset;
        scratchOffset += ctx.sliceSize;
    }
}

// Phase3: 串行规约(CCU_WHILE(readRepeatNum)),逐子块推进 strides 后调 DoReduceScatter
static CcuResult DoReduceScatterReduce(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    CCU_IF(ctx.readRepeatNum != UINT64_MAX)
    {
        ctx.flag = 0;
        CCU_WHILE(ctx.readRepeatNum != UINT64_MAX)
        {
            ctx.readRepeatNum += ctx.constVar1;
            CCU_IF(ctx.flag != 0)
            {
                for (uint64_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
                    ctx.scratchMem[rankIdx].addr += ctx.scratchRepeatStride;
                }
                ctx.myInput.addr += ctx.inputRepeatStride;
                ctx.output += ctx.outputRepeatStride;
            }
            CCU_CHK_RET(DoReduceScatter(ctx));
            ctx.flag = 1;
        }
    }
    return CCU_SUCCESS;
}

// 三阶段总调度(从 hccl DoRepeatReduceScatter 移植)。单 die 下 repeatNum=1,Phase1/2 各触发 1 路,Phase3 1 次。
static CcuResult DoReduceScatterThreePhase(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    InitReduceScatterAddr(ctx);

    // 软件展开三阶段:Phase1 批量非阻塞 ReadNb / Phase2 批量 Wait / Phase3 串行 Reduce
    ctx.waitRepeatNum = ctx.repeatNum;
    ctx.readRepeatNum = ctx.repeatNum;

    // Phase1: 第 1 路(不需地址步进)
    CCU_IF(ctx.repeatNum != UINT64_MAX)
    {
        ctx.repeatNum += ctx.constVar1;
        DoReduceScatterRead(ctx, 0);
    }

    // Phase1: 第 2~RS_UNROLL_NUM 路(需按 strides 步进)
    for (uint32_t i = 1; i < RS_UNROLL_NUM; i++) {
        CCU_IF(ctx.repeatNum != UINT64_MAX)
        {
            ctx.repeatNum += ctx.constVar1;
            for (uint64_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
                if (rankIdx == ctx.rankId) {
                    ctx.myInput.addr += ctx.inputRepeatStride;
                } else {
                    ctx.remoteInput[rankIdx].addr += ctx.inputRepeatStride;
                }
                ctx.scratchMem[rankIdx].addr += ctx.scratchRepeatStride;
            }
            DoReduceScatterRead(ctx, i);
        }
    }

    // Phase2: 批量 WaitEvent
    for (uint32_t i = 0; i < RS_UNROLL_NUM; i++) {
        CCU_IF(ctx.waitRepeatNum != UINT64_MAX)
        {
            ctx.waitRepeatNum += ctx.constVar1;
            DoReduceScatterWait(ctx, i);
        }
    }

    // 复位地址,为 Phase3 从本 chunk 起始重新步进
    ResetReduceScatterAddr(ctx);

    // Phase3: 串行 Reduce
    CCU_CHK_RET(DoReduceScatterReduce(ctx));
    return CCU_SUCCESS;
}

// chunk 循环(= hccl executor loopTimes,串行);每 chunk 内跑三阶段。
// 推进 ctx.input/output 替代 executor 的逐 launch 推进,使 Init/Reset 可逐字搬运。
static CcuResult DoRepeatReduceScatter(KfcReduceScatterMesh1DMem2MemContext& ctx)
{
    ccu::Variable repeatNumAdd;
    repeatNumAdd = 1;

    CCU_WHILE(ctx.chunkLoopNum != UINT64_MAX)
    {
        // 本 chunk 的参数:满 chunk 用 chunkSize/fullGoSize,尾 chunk 用 tailSize/tailGoSize
        ctx.currentSliceSize = ctx.chunkSize;
        ctx.normalSliceSize = ctx.chunkSize;
        ctx.lastSliceSize = ctx.tailSize;
        ctx.sliceSize = ctx.chunkSize;
        ctx.goSize = ctx.fullGoSize;
        CCU_IF(ctx.chunkLoopNum == UINT64_MAX - 1)
        {
            ctx.currentSliceSize = ctx.tailSize;
            ctx.sliceSize = ctx.tailSize;
            ctx.goSize = ctx.tailGoSize;
        }
        ctx.chunkLoopNum += repeatNumAdd;

        // 单 die 三阶段参数:repeatNum=1(哨兵 UINT64_MAX-1),strides=0,RS_UNROLL_NUM 休眠。
        // 多 die 时 repeatNum=rankSizeLevel1_、strides 由宿主下发,此处可扩展。
        ctx.repeatNum = UINT64_MAX - 1;
        ctx.readRepeatNum = ctx.repeatNum;
        ctx.waitRepeatNum = ctx.repeatNum;
        ctx.inputRepeatStride = 0;
        ctx.outputRepeatStride = 0;
        ctx.scratchRepeatStride = 0;
        ctx.constVar1 = 1;

        CCU_CHK_RET(DoReduceScatterThreePhase(ctx));

        // 推进到下一 chunk(inter-chunk,替代 executor)
        for (uint64_t rankIdx = 0; rankIdx < ctx.rankSize; rankIdx++) {
            ctx.input[rankIdx] += ctx.currentSliceSize;
        }
        ctx.output += ctx.currentSliceSize;
    }
    return CCU_SUCCESS;
}

CcuResult CcuReduceScatterMesh1DMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable scratch,
    ccu::Variable currentRankSliceInputOffset, ccu::Variable chunkSize, ccu::Variable chunkLoopNum,
    ccu::Variable tailSize, ccu::Variable fullGoAddrOffset, ccu::Variable fullGoLoopParam,
    ccu::Variable fullGoParallelParam, ccu::Variable fullGoResidual, ccu::Variable tailGoAddrOffset,
    ccu::Variable tailGoLoopParam, ccu::Variable tailGoParallelParam, ccu::Variable tailGoResidual,
    const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId,
    const HcclDataType& dataType, const HcclDataType& outputType, const HcclReduceOp& reduceType)
{
    KfcReduceScatterMesh1DMem2MemContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.dataType = dataType;
    ctx.outputDataType = outputType;
    if (ctx.outputDataType == HcclDataType::HCCL_DATA_TYPE_RESERVED) {
        ctx.outputDataType = dataType;
    }
    ctx.reduceOp = reduceType;
    HCCL_INFO(
        "[CcuKernelReduceScatterMesh1DMem2Mem] Init, KernelArgs are rankId[%u], rankSize_[%u], dataType[%d], "
        "outputDataType[%d], reduceOp[%d]",
        ctx.rankId, ctx.rankSize, ctx.dataType, ctx.outputDataType, ctx.reduceOp);

    ctx.resourceAllocated = false;
    ctx.moConfig.msInterleave = 0;
    ctx.moConfig.loopCount = 0;
    ctx.moConfig.memSlice = 0;
    ctx.moRes.eventCount = 0;
    ctx.moRes.bufCount = 0;
    ctx.moConfig.msInterleave = CCU_MS_INTERLEAVE;
    ctx.moConfig.loopCount = CCU_LOOP_COUNT_M2M_RE;
    ctx.moConfig.memSlice = CCU_MS_SIZE;

    HCCL_INFO("[CcuKernelReduceScatterMesh1DMem2Mem] ReduceScatterMesh1DMem2Mem run");
    CCU_CHK_RET(InitResource(ctx));
    CCU_CHK_RET(LoadArgs(
        ctx, inputAddr, outputAddr, tokenInfo, scratch, currentRankSliceInputOffset, chunkSize, chunkLoopNum, tailSize,
        fullGoAddrOffset, fullGoLoopParam, fullGoParallelParam, fullGoResidual, tailGoAddrOffset, tailGoLoopParam,
        tailGoParallelParam, tailGoResidual));
    CCU_CHK_RET(PreSync(ctx));
    CCU_CHK_RET(DoRepeatReduceScatter(ctx));
    CCU_CHK_RET(PostSync(ctx));
    HCCL_INFO("[CcuKernelReduceScatterMesh1DMem2Mem] ReduceScatterMesh1DMem2Mem end");

    return CCU_SUCCESS;
}
} // namespace mc2_ops_hccl
