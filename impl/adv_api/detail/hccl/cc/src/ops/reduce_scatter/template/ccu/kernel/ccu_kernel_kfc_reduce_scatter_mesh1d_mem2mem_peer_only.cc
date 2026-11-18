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
    std::vector<ccu::variable> input;
    std::vector<ccu::variable> token;
    ccu::variable output;
    ccu::variable currentRankSliceInputOffset;
    ccu::variable skipLocalRankCopy;
    ccu::variable tailSize;
    ccu::variable chunkLoopNum;
    ccu::event event;
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
    ctx.input[peerRank] = ccu::get_res_by_channel<ccu::variable>(ctx.channels[0], PEER_ONLY_INPUT_XN_ID);
    ctx.token[peerRank] = ccu::get_res_by_channel<ccu::variable>(ctx.channels[0], PEER_ONLY_TOKEN_XN_ID);
    return CCU_SUCCESS;
}

static CcuResult LoadPeerOnlyArgs(
    KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx, ccu::variable inputAddr, ccu::variable outputAddr,
    ccu::variable tokenInfo, ccu::variable currentRankSliceInputOffset, ccu::variable skipLocalRankCopy,
    ccu::variable tailSize, ccu::variable chunkLoopNum)
{
    ctx.input[ctx.rankId] = inputAddr;
    ctx.token[ctx.rankId] = tokenInfo;
    ctx.output = outputAddr;
    ctx.currentRankSliceInputOffset = currentRankSliceInputOffset;
    ctx.skipLocalRankCopy = skipLocalRankCopy;
    ctx.tailSize = tailSize;
    ctx.chunkLoopNum = chunkLoopNum;
    return CCU_SUCCESS;
}

static CcuResult PeerOnlyPreSync(KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx)
{
    CCU_CHK_RET(ccu::write_variable_with_notify(
        ctx.channels[0], ctx.input[ctx.rankId], PEER_ONLY_INPUT_XN_ID, PEER_ONLY_CKE_IDX, 1U << PEER_ONLY_INPUT_XN_ID));
    CCU_CHK_RET(ccu::write_variable_with_notify(
        ctx.channels[0], ctx.token[ctx.rankId], PEER_ONLY_TOKEN_XN_ID, PEER_ONLY_CKE_IDX, 1U << PEER_ONLY_TOKEN_XN_ID));
    constexpr uint32_t allBit = (1U << PEER_ONLY_INPUT_XN_ID) | (1U << PEER_ONLY_TOKEN_XN_ID);
    ccu::notify_wait(ctx.channels[0], PEER_ONLY_CKE_IDX, allBit);
    return CCU_SUCCESS;
}

static CcuResult PeerOnlyPostSync(KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx)
{
    ccu::notify_record(ctx.channels[0], PEER_ONLY_CKE_IDX, 1U << PEER_ONLY_POST_SYNC_ID);
    ccu::notify_wait(ctx.channels[0], PEER_ONLY_CKE_IDX, 1U << PEER_ONLY_POST_SYNC_ID);
    return CCU_SUCCESS;
}

static CcuResult DoPeerOnlyReduceScatter(KfcReduceScatterMesh1DMem2MemPeerOnlyContext& ctx)
{
    const uint32_t peerRank = ctx.rankId ^ 1U;
    ccu::remote_addr remoteInput;
    remoteInput.addr = ctx.input[peerRank];
    remoteInput.addr += ctx.currentRankSliceInputOffset;
    remoteInput.token = ctx.token[peerRank];

    ccu::local_addr output;
    output.addr = ctx.output;
    output.token = ctx.token[ctx.rankId];

    ccu::local_addr localInput;
    localInput.addr = ctx.input[ctx.rankId];
    localInput.addr += ctx.currentRankSliceInputOffset;
    localInput.token = ctx.token[ctx.rankId];

    ccu::variable chunkSize{};
    chunkSize = UB_MAX_TRANS_SIZE;
    ccu::variable chunkLoopNumAdd{};
    chunkLoopNumAdd = 1;
    CCU_WHILE(ctx.chunkLoopNum != UINT64_MAX)
    {
        CCU_IF(ctx.chunkLoopNum == UINT64_MAX - 1) { chunkSize = ctx.tailSize; }
        ctx.chunkLoopNum += chunkLoopNumAdd;
        CCU_IF(chunkSize != 0)
        {
            // MMRS skips the copy only after Matmul has supplied the local contribution.
            CCU_IF(ctx.skipLocalRankCopy == 0)
            {
                CCU_CHK_RET(ccu::local_copy(output, localInput, chunkSize, ctx.event, 1U << ctx.rankId));
                ccu::event_wait(ctx.event, 1U << ctx.rankId);
            }
            ccu::read_reduce(
                ctx.channels[0], output, remoteInput, chunkSize, ctx.dataType, ctx.reduceOp, ctx.event, 1U << peerRank);
            ccu::event_wait(ctx.event, 1U << peerRank);
        }
        remoteInput.addr += chunkSize;
        localInput.addr += chunkSize;
        output.addr += chunkSize;
    }
    return CCU_SUCCESS;
}

CcuResult CcuReduceScatterMeshMem2Mem1DPeerOnlyKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo,
    ccu::variable currentRankSliceInputOffset, ccu::variable skipLocalRankCopy, ccu::variable tailSize,
    ccu::variable chunkLoopNum, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize,
    uint32_t rankId, const HcclDataType& dataType, const HcclReduceOp& reduceType)
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
    CCU_CHK_RET(LoadPeerOnlyArgs(
        ctx, inputAddr, outputAddr, tokenInfo, currentRankSliceInputOffset, skipLocalRankCopy, tailSize, chunkLoopNum));
    CCU_CHK_RET(PeerOnlyPreSync(ctx));
    CCU_CHK_RET(DoPeerOnlyReduceScatter(ctx));
    CCU_CHK_RET(PeerOnlyPostSync(ctx));
    HCCL_INFO("[CcuSchedReduceScatterSoleMeshPeerOnly] end.");
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
