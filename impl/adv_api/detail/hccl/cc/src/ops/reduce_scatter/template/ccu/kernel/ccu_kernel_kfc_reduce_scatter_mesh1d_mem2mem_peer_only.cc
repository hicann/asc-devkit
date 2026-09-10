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
#include "ccu_kernel_kfc_reduce_scatter_mesh1d_mem2mem_peer_only.h"
#include "mc2_type.h"

namespace mc2_ops_hccl {
using namespace hcomm;

constexpr int PEER_ONLY_INPUT_XN_ID = 0;
constexpr int PEER_ONLY_TOKEN_XN_ID = 2;
constexpr int PEER_ONLY_POST_SYNC_ID = 3;
constexpr int PEER_ONLY_CKE_IDX = 0;

struct KfcReduceScatterMesh1DMem2MemPeerOnlyContext : CcuKernelCtxBase {
    const ChannelHandle* channels{nullptr};
    uint32_t channelCount{0};
    uint32_t rankSize{0};
    uint32_t rankId{0};
    HcclDataType dataType;
    HcclReduceOp reduceOp;
    std::vector<ccu::Variable> input;
    std::vector<ccu::Variable> token;
    ccu::Variable output;
    ccu::Variable currentRankSliceInputOffset;
    ccu::Variable tailSize;
    ccu::Variable chunkLoopNum;
    ccu::Event event;
};

static CcuResult InitPeerOnlyResource(KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx)
{
    if (ctx.rankSize != 2U || ctx.rankId >= ctx.rankSize || ctx.channelCount != 1U) {
        HCCL_ERROR(
            "[CcuSchedReduceScatterSoleMeshPeerOnly] requires TP2 and one peer channel, rankSize[%u], "
            "rankId[%u], channelCount[%u].",
            ctx.rankSize, ctx.rankId, ctx.channelCount);
        return CcuResult::CCU_E_INTERNAL;
    }

    ctx.input.resize(ctx.rankSize);
    ctx.token.resize(ctx.rankSize);
    const uint32_t peerRank = ctx.rankId ^ 1U;
    ctx.input[peerRank] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[0], PEER_ONLY_INPUT_XN_ID);
    ctx.token[peerRank] = ccu::GetResByChannel<ccu::Variable>(ctx.channels[0], PEER_ONLY_TOKEN_XN_ID);
    return CCU_SUCCESS;
}

static CcuResult LoadPeerOnlyArgs(
    KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx, ccu::Variable inputAddr, ccu::Variable outputAddr,
    ccu::Variable tokenInfo, ccu::Variable currentRankSliceInputOffset, ccu::Variable tailSize,
    ccu::Variable chunkLoopNum)
{
    ctx.input[ctx.rankId] = inputAddr;
    ctx.token[ctx.rankId] = tokenInfo;
    ctx.output = outputAddr;
    ctx.currentRankSliceInputOffset = currentRankSliceInputOffset;
    ctx.tailSize = tailSize;
    ctx.chunkLoopNum = chunkLoopNum;
    return CCU_SUCCESS;
}

static CcuResult PeerOnlyPreSync(KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx)
{
    CCU_CHK_RET(ccu::WriteVariableWithNotify(
        ctx.channels[0], ctx.input[ctx.rankId], PEER_ONLY_INPUT_XN_ID, PEER_ONLY_CKE_IDX, 1U << PEER_ONLY_INPUT_XN_ID));
    CCU_CHK_RET(ccu::WriteVariableWithNotify(
        ctx.channels[0], ctx.token[ctx.rankId], PEER_ONLY_TOKEN_XN_ID, PEER_ONLY_CKE_IDX, 1U << PEER_ONLY_TOKEN_XN_ID));
    constexpr uint32_t allBit = (1U << PEER_ONLY_INPUT_XN_ID) | (1U << PEER_ONLY_TOKEN_XN_ID);
    ccu::NotifyWait(ctx.channels[0], PEER_ONLY_CKE_IDX, allBit);
    return CCU_SUCCESS;
}

static CcuResult PeerOnlyPostSync(KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx)
{
    ccu::NotifyRecord(ctx.channels[0], PEER_ONLY_CKE_IDX, 1U << PEER_ONLY_POST_SYNC_ID);
    ccu::NotifyWait(ctx.channels[0], PEER_ONLY_CKE_IDX, 1U << PEER_ONLY_POST_SYNC_ID);
    return CCU_SUCCESS;
}

static CcuResult DoPeerOnlyReduceScatter(KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx)
{
    const uint32_t peerRank = ctx.rankId ^ 1U;
    ccu::RemoteAddr remoteInput;
    remoteInput.addr = ctx.input[peerRank];
    remoteInput.addr += ctx.currentRankSliceInputOffset;
    remoteInput.token = ctx.token[peerRank];

    ccu::LocalAddr output;
    output.addr = ctx.output;
    output.token = ctx.token[ctx.rankId];

    ccu::Variable chunkSize{};
    chunkSize = UB_MAX_TRANS_SIZE;
    ccu::Variable chunkLoopNumAdd{};
    chunkLoopNumAdd = 1;
    CCU_WHILE(ctx.chunkLoopNum != UINT64_MAX)
    {
        CCU_IF(ctx.chunkLoopNum == UINT64_MAX - 1) { chunkSize = ctx.tailSize; }
        ctx.chunkLoopNum += chunkLoopNumAdd;
        CCU_IF(chunkSize != 0)
        {
            ccu::ReadReduce(
                ctx.channels[0], output, remoteInput, chunkSize, ctx.dataType, ctx.reduceOp, ctx.event, 1U << peerRank);
            ccu::EventWait(ctx.event, 1U << peerRank);
        }
        remoteInput.addr += chunkSize;
        output.addr += chunkSize;
    }
    return CCU_SUCCESS;
}

CcuResult CcuReduceScatterMeshMem2Mem1DPeerOnlyKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo,
    ccu::Variable currentRankSliceInputOffset, ccu::Variable tailSize, ccu::Variable chunkLoopNum,
    const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId,
    const HcclDataType& dataType, const HcclReduceOp& reduceType)
{
    KfcReduceScatterMesh1DMem2MemPeerOnlyContext ctx;
    ctx.channels = channels;
    ctx.channelCount = channelCount;
    ctx.rankSize = rankSize;
    ctx.rankId = rankId;
    ctx.dataType = dataType;
    ctx.reduceOp = reduceType;
    ctx.resourceAllocated = false;
    ctx.moConfig.msInterleave = 0;
    ctx.moConfig.loopCount = 0;
    ctx.moConfig.memSlice = 0;
    ctx.moRes.eventCount = 0;
    ctx.moRes.bufCount = 0;

    HCCL_INFO(
        "[CcuSchedReduceScatterSoleMeshPeerOnly] run, rankId[%u], rankSize[%u], dataType[%d], reduceOp[%d].",
        ctx.rankId, ctx.rankSize, ctx.dataType, ctx.reduceOp);
    CCU_CHK_RET(InitPeerOnlyResource(ctx));
    CCU_CHK_RET(
        LoadPeerOnlyArgs(ctx, inputAddr, outputAddr, tokenInfo, currentRankSliceInputOffset, tailSize, chunkLoopNum));
    CCU_CHK_RET(PeerOnlyPreSync(ctx));
    CCU_CHK_RET(DoPeerOnlyReduceScatter(ctx));
    CCU_CHK_RET(PeerOnlyPostSync(ctx));
    HCCL_INFO("[CcuSchedReduceScatterSoleMeshPeerOnly] end.");
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
