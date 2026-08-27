/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_ubx_all_to_all_v_mesh_1D.h"

#include <limits>

namespace mc2_ops_hccl {
namespace {
bool AddOverflows(u64 lhs, u64 rhs) { return lhs > std::numeric_limits<u64>::max() - rhs; }

bool MultiplyOverflows(u64 lhs, u64 rhs) { return lhs != 0U && rhs > std::numeric_limits<u64>::max() / lhs; }

HcclResult CalcUserSliceOffset(u64 displacement, u64 dataTypeSize, u64 splitOffset, u64& byteOffset)
{
    CHK_PRT_RET(
        MultiplyOverflows(displacement, dataTypeSize),
        HCCL_ERROR("[InsTempUBXAllToAllVMesh1D] user buffer displacement overflows."), HCCL_E_PARA);
    const u64 displacementBytes = displacement * dataTypeSize;
    CHK_PRT_RET(
        AddOverflows(displacementBytes, splitOffset),
        HCCL_ERROR("[InsTempUBXAllToAllVMesh1D] user buffer offset overflows."), HCCL_E_PARA);
    byteOffset = displacementBytes + splitOffset;
    return HCCL_SUCCESS;
}

HcclResult CalcCclSliceOffset(u32 bufferIndex, u64 sliceStride, u64 splitOffset, u64& byteOffset)
{
    CHK_PRT_RET(
        MultiplyOverflows(bufferIndex, sliceStride),
        HCCL_ERROR("[InsTempUBXAllToAllVMesh1D] CCL buffer stride overflows."), HCCL_E_PARA);
    const u64 bufferOffset = static_cast<u64>(bufferIndex) * sliceStride;
    CHK_PRT_RET(
        AddOverflows(bufferOffset, splitOffset), HCCL_ERROR("[InsTempUBXAllToAllVMesh1D] CCL buffer offset overflows."),
        HCCL_E_PARA);
    byteOffset = bufferOffset + splitOffset;
    return HCCL_SUCCESS;
}

HcclResult CalcDataSize(u64 count, u64 dataTypeSize, u64& dataSize)
{
    CHK_PRT_RET(
        MultiplyOverflows(count, dataTypeSize), HCCL_ERROR("[InsTempUBXAllToAllVMesh1D] data size overflows."),
        HCCL_E_PARA);
    dataSize = count * dataTypeSize;
    return HCCL_SUCCESS;
}
} // namespace

constexpr u32 UBX_BOARD_PAIR_SIZE = 2;
InsTempUBXAllToAllVMesh1D::InsTempUBXAllToAllVMesh1D(
    const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempUBXAllToAllVMesh1D::~InsTempUBXAllToAllVMesh1D() {}

HcclResult InsTempUBXAllToAllVMesh1D::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    std::vector<HcclChannelDesc> level0Channels;
    std::vector<HcclChannelDesc> myChannelDescs;
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix) {
        bool isIsolation = !(IsAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH));
        CHK_RET(
            CalcChannelRequestMeshClosMultiJetty(comm, param, topoInfo, subCommRanks_, myChannelDescs, isIsolation));
        for (auto channel : myChannelDescs) {
            if (channel.channelProtocol == COMM_PROTOCOL_UBC_CTP) {
                level0Channels.push_back(channel);
            }
        }
        HCCL_DEBUG("[InsTempUBXAllToAllVMesh1D::CalcRes] Get Channel Success!");
    } else {
        CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, level0Channels));
    }
    resourceRequest.channels.push_back(level0Channels);
    channelsPerRank_ = CalcChannelsPerRank(level0Channels);
    HCCL_INFO("[InsTempUBXAllToAllVMesh1D][CalcRes] channelsPerRank_ is [%u]", channelsPerRank_);
    // 按照当前来看，channelsPerRank_ 就是maxPathNum_
    if (channelsPerRank_ > maxPathNum_) {
        HCCL_ERROR(
            "[InsTempUBXAllToAllVMesh1D][CalcRes] channelsPerRank_[%u] is more than [%u]", channelsPerRank_,
            maxPathNum_);
        return HCCL_E_NOT_SUPPORT;
    }

    // UBX机型，fullmesh内最多四卡
    // 跨框的时候，最多4 jetty
    // fullmesh 和 clos 并发
    resourceRequest.slaveThreadNum = maxPathNum_ + maxRankNumPerBoard_;
    HCCL_INFO("[InsTempUBXAllToAllVMesh1D::CalcRes] slaveThreadNum is [%u]", resourceRequest.slaveThreadNum);
    for (u32 index = 0; index < resourceRequest.slaveThreadNum; index++) {
        // 从流的notify数量以rank间channel数的最大值为准，用于和主流同步以及同一个rank多条链路间的同步
        resourceRequest.notifyNumPerThread.push_back(channelsPerRank_);
    }
    resourceRequest.notifyNumOnMainThread = resourceRequest.slaveThreadNum;
    return HCCL_SUCCESS;
}

u64 InsTempUBXAllToAllVMesh1D::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    // 直接切成rankSize份
    return templateRankSize_;
}

HcclResult InsTempUBXAllToAllVMesh1D::GetBoardSendRecvMatrix(u32 n, std::vector<std::vector<u32>>& sendRecvMatrix)
{
    // n必须是
    if (n < UBX_BOARD_PAIR_SIZE || n % UBX_BOARD_PAIR_SIZE != 0) {
        HCCL_ERROR("n is [%u]", n);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }

    // 创建环形排列：0固定，其余1到n-1
    std::vector<int> ring(n);
    ring[0] = 0;
    for (int i = 1; i < n; i++) {
        ring[i] = i;
    }

    // 生成n-1轮调度
    for (int round = 0; round < n - 1; round++) {
        // 生成本轮配对：首尾对称配对
        for (int i = 0; i < n / UBX_BOARD_PAIR_SIZE; i++) {
            int a = ring[i];
            int b = ring[n - 1 - i];

            // 写入调度矩阵
            sendRecvMatrix[a][round] = b;
            sendRecvMatrix[b][round] = a;
        }

        // 旋转环形排列：固定第一个元素0，其余旋转一位
        // 保存最后一个元素
        int last = ring[n - 1];
        // 从倒数第二个元素开始向前移动
        for (int i = n - 1; i > 1; i--) {
            ring[i] = ring[i - 1];
        }
        // 将原最后一个元素放到第二个位置
        ring[1] = last;
    }
    // 打印矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1; j++) {
            HCCL_DEBUG("sendRecvMatrix[%d][%d] = %d", i, j, sendRecvMatrix[i][j]);
        }
    }
    // 举个例子：n = 8 的时候，矩阵如下
    // board	step1	step2	step3	step4	step5	step6	step7
    //     0	    7	    6	    5	    4	    3	    2	    1
    //     1	    6	    4	    2	    7	    5	    3	    0
    //     2	    5	    3	    1	    6	    4	    0	    7
    //     3	    4	    2	    7	    5	    0	    1	    6
    //     4	    3	    1	    6	    0	    2	    7	    5
    //     5	    2	    7	    0	    3	    1	    6	    4
    //     6	    1	    0	    4	    2	    7	    5	    3
    //     7	    0	    5	    3	    1	    6	    4	    2
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::GetRankSendRecvMatrix(
    u32 board1, u32 board2, std::vector<std::vector<u32>>& rankSendRecvMatrix)
{
    u32 boardSmall = std::min(board1, board2);
    u32 boardBig = std::max(board1, board2);

    for (u32 rankIndex = boardSmall * rankNumPerBoard_; rankIndex < (boardSmall + 1) * rankNumPerBoard_; rankIndex++) {
        for (u32 step = 0; step < rankNumPerBoard_; step++) {
            u32 targetRank = boardBig * rankNumPerBoard_ + (rankIndex + step) % rankNumPerBoard_;
            rankSendRecvMatrix[rankIndex][step] = targetRank;
            rankSendRecvMatrix[targetRank][step] = rankIndex;
        }
    }
    // 打印矩阵
    for (int i = 0; i < rankSendRecvMatrix.size(); i++) {
        for (int j = 0; j < rankNumPerBoard_; j++) {
            HCCL_DEBUG(
                "myRank is [%u], board1 is[%u], board2 is [%u], rankSendRecvMatrix[%d][%d] = [%d]", myAlgRank_, board1,
                board2, i, j, rankSendRecvMatrix[i][j]);
        }
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::GetRankNumPerBoard(const TemplateResource& templateResource)
{
    // 根据channel，和远端rank只有一条链路的，说明是fullmesh内的
    // 遍历所有卡
    u32 fullMeshRankNum = 1;
    const std::map<u32, std::vector<ChannelInfo>>& channels = templateResource.channels;
    for (u32 rank = 0; rank < templateRankSize_; rank++) {
        if (rank == myAlgRank_) {
            continue;
        }
        u32 linkNumSendRecv = channels.at(rank).size();
        if (linkNumSendRecv == 1) { // 说明是fullmesh内
            HCCL_INFO("myRank[%u] and curRank[%u] is fullMesh link", myAlgRank_, rank);
            fullMeshRankNum++;
        }
    }

    rankNumPerBoard_ = fullMeshRankNum;
    if (rankNumPerBoard_ > maxRankNumPerBoard_) {
        // fullmesh 内最多4P
        HCCL_ERROR(
            "[InsTempUBXAllToAllVMesh1D][GetRankNumPerBoard] rankNumPerBoard_[%u] is more than [%u]", rankNumPerBoard_,
            maxRankNumPerBoard_);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }
    // 纯clos不支持
    if (rankNumPerBoard_ == 1) {
        HCCL_ERROR(
            "[InsTempUBXAllToAllVMesh1D][GetRankNumPerBoard] rankNumPerBoard_ is [%u], "
            "templateRankSize_ is [%u], only clos is not support",
            rankNumPerBoard_, templateRankSize_);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }
    // 纯fullmesh暂时先支持
    if (rankNumPerBoard_ == templateRankSize_) {
        HCCL_WARNING(
            "[InsTempUBXAllToAllVMesh1D][GetRankNumPerBoard] rankNumPerBoard_ is [%u], "
            "templateRankSize_ is [%u], here is only fullmesh",
            rankNumPerBoard_, templateRankSize_);
    }
    // rankSize要是rankNumPerBoard_的整数倍
    if (templateRankSize_ % rankNumPerBoard_ != 0) {
        HCCL_ERROR(
            "[InsTempUBXAllToAllVMesh1D][GetRankNumPerBoard] rankNumPerBoard_ is [%u], "
            "templateRankSize_ is [%u]",
            rankNumPerBoard_, templateRankSize_);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::CheckPathNum(const TemplateResource& templateResource)
{
    // 校验是不是最多4jetty
    const std::map<u32, std::vector<ChannelInfo>>& channels = templateResource.channels;
    for (u32 rank = 0; rank < templateRankSize_; rank++) {
        if (rank == myAlgRank_) {
            continue;
        }
        u32 linkNumSendRecv = channels.at(rank).size();
        HCCL_INFO("myRank is [%u], targetRank is [%u], linkNum is [%u] ", myAlgRank_, rank, linkNumSendRecv);
        if (linkNumSendRecv > maxPathNum_) {
            HCCL_ERROR(
                "myRank is [%u], targetRank is [%u], linkNum is [%u], linkNum should not bigger than [%u]", myAlgRank_,
                rank, linkNumSendRecv, maxPathNum_);
            return HcclResult::HCCL_E_NOT_SUPPORT;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::RunFullMeshSelfCopy(
    const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 targetRank,
    u32 fullMeshThreadId)
{
    const std::vector<ThreadHandle>& threads = templateResource.threads;
    u64 inputOffset = 0U;
    u64 outputOffset = 0U;
    u64 inputSize = 0U;
    u64 outputSize = 0U;
    CHK_RET(CalcUserSliceOffset(tempAlgParams.sdispls[targetRank], dataTypeSize_, 0U, inputOffset));
    CHK_RET(CalcUserSliceOffset(tempAlgParams.rdispls[targetRank], dataTypeSize_, 0U, outputOffset));
    CHK_RET(CalcDataSize(tempAlgParams.sendCounts[targetRank], dataTypeSize_, inputSize));
    CHK_RET(CalcDataSize(tempAlgParams.recvCounts[targetRank], dataTypeSize_, outputSize));
    DataSlice usrInSlices(
        tempAlgParams.buffInfo.inputPtr, inputOffset, inputSize, tempAlgParams.sendCounts[targetRank]);
    DataSlice usrOutSlices(
        tempAlgParams.buffInfo.outputPtr, outputOffset, outputSize, tempAlgParams.recvCounts[targetRank]);
    return LocalCopy(threads[fullMeshThreadId], usrInSlices, usrOutSlices);
}

HcclResult InsTempUBXAllToAllVMesh1D::RunFullMeshPeer(
    const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 targetRank,
    u32 fullMeshThreadId)
{
    const std::map<u32, std::vector<ChannelInfo>>& channels = templateResource.channels;
    const std::vector<ThreadHandle>& threads = templateResource.threads;
    const ChannelInfo& channelSendRecv = channels.at(targetRank)[0];
    const u64 sendCount = tempAlgParams.sendCounts[targetRank];
    const u64 recvCount = tempAlgParams.recvCounts[targetRank];
    u64 txSourceOffset = 0U;
    u64 txDestinationOffset = 0U;
    u64 rxSourceOffset = 0U;
    u64 rxDestinationOffset = 0U;
    u64 sendSize = 0U;
    u64 recvSize = 0U;
    CHK_RET(CalcUserSliceOffset(tempAlgParams.sdispls[targetRank], dataTypeSize_, 0U, txSourceOffset));
    CHK_RET(CalcCclSliceOffset(myAlgRank_, scratchBufferSizePerRank_, 0U, txDestinationOffset));
    CHK_RET(CalcCclSliceOffset(targetRank, scratchBufferSizePerRank_, 0U, rxSourceOffset));
    CHK_RET(CalcCclSliceOffset(targetRank, scratchBufferSizePerRank_, 0U, rxDestinationOffset));
    CHK_RET(CalcDataSize(sendCount, dataTypeSize_, sendSize));
    CHK_RET(CalcDataSize(recvCount, dataTypeSize_, recvSize));
    u64 userOutputOffset = 0U;
    if (recvCount > 0U) {
        CHK_RET(CalcUserSliceOffset(tempAlgParams.rdispls[targetRank], dataTypeSize_, 0U, userOutputOffset));
    }
    DataSlice txSrcSlice(tempAlgParams.buffInfo.inputPtr, txSourceOffset, sendSize, sendCount);
    DataSlice txDstSlice(channelSendRecv.remoteCclMem.addr, txDestinationOffset, sendSize, sendCount);
    DataSlice rxSrcSlice(channelSendRecv.remoteCclMem.addr, rxSourceOffset, recvSize, recvCount);
    DataSlice rxDstSlice(tempAlgParams.buffInfo.hcclBuff.addr, rxDestinationOffset, recvSize, recvCount);
    std::vector<DataSlice> txSrcSlices{txSrcSlice};
    std::vector<DataSlice> txDstSlices{txDstSlice};
    std::vector<DataSlice> rxSrcSlices{rxSrcSlice};
    std::vector<DataSlice> rxDstSlices{rxDstSlice};
    SendRecvInfo sendRecvInfo{
        {channelSendRecv, channelSendRecv}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}};
    if (sendCount > 0 && recvCount > 0) {
        CHK_RET(SendRecvWrite(sendRecvInfo, threads[fullMeshThreadId]));
    } else if (sendCount > 0) {
        DataInfo sendInfo{channelSendRecv, {txSrcSlices, txDstSlices}, dataType_};
        CHK_RET(SendWrite(sendInfo, threads[fullMeshThreadId]));
    } else if (recvCount > 0) {
        DataInfo recvInfo{channelSendRecv, {rxSrcSlices, rxDstSlices}, dataType_};
        CHK_RET(RecvWrite(recvInfo, threads[fullMeshThreadId]));
    }
    if (recvCount > 0) {
        DataSlice usrOutSlices(tempAlgParams.buffInfo.outputPtr, userOutputOffset, recvSize, recvCount);
        localCopyInfoFullMesh_.push_back(std::vector<DataSlice>{rxDstSlice, usrOutSlices});
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::RunFullMesh(
    const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    // 用后面的 maxRankNumPerBoard_ 条流做fullmesh内的收发
    localCopyInfoFullMesh_.clear();
    for (u32 i = 0; i < rankNumPerBoard_; i++) {
        u32 targetRank = currBoard_ * rankNumPerBoard_ + i;
        HCCL_INFO("[InsTempUBXAllToAllVMesh1D] targetRank is [%u]", targetRank);
        u32 fullMeshThreadId = maxPathNum_ + 1 + i;
        if (targetRank == myAlgRank_) {
            CHK_RET(RunFullMeshSelfCopy(tempAlgParams, templateResource, targetRank, fullMeshThreadId));
        } else {
            CHK_RET(RunFullMeshPeer(tempAlgParams, templateResource, targetRank, fullMeshThreadId));
        }
    }
    needDealWithFullMeshInfo_ = true;
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::RunPairwiseStep(const std::vector<ThreadHandle>& threads)
{
    if (threadNum_ > 1) {
        GetNotifyIdxMainToClos(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(threads[0], subThreadsBoard_, notifyIdxMainToSub_));
    }
    for (u32 i = 0; i < localCopyInfo_.size(); i++) {
        CHK_RET(LocalCopy(threads[0], localCopyInfo_[i][0], localCopyInfo_[i][1]));
    }
    if (needDealWithFullMeshInfo_) {
        for (u32 i = 0; i < localCopyInfoFullMesh_.size(); i++) {
            CHK_RET(LocalCopy(threads[0], localCopyInfoFullMesh_[i][0], localCopyInfoFullMesh_[i][1]));
        }
        needDealWithFullMeshInfo_ = false;
        localCopyInfoFullMesh_.clear();
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::RunPairwiseChannel(
    const TemplateDataParams& tempAlgParams, const std::vector<ChannelInfo>& channelSendRecv,
    const std::vector<ThreadHandle>& threads, u32 targetRank, u32 channelIndex, u64& innerSendOffset,
    u64& innerRecvOffset, u64 curSendDataCount, u64 curRecvDataCount)
{
    const u32 linkNumSendRecv = channelSendRecv.size();
    const float dataSplitRate = (float)1.0 / (float)linkNumSendRecv;
    u64 innerCurrSendDataCount = curSendDataCount * dataSplitRate;
    u64 innerCurrRecvDataCount = curRecvDataCount * dataSplitRate;
    if (channelIndex == linkNumSendRecv - 1) {
        innerCurrSendDataCount = curSendDataCount - innerSendOffset;
        innerCurrRecvDataCount = curRecvDataCount - innerRecvOffset;
    }
    u64 innerSendOffsetBytes = 0U;
    u64 innerRecvOffsetBytes = 0U;
    u64 innerCurrSendDataSize = 0U;
    u64 innerCurrRecvDataSize = 0U;
    u64 txSourceOffset = 0U;
    u64 txDestinationOffset = 0U;
    u64 rxSourceOffset = 0U;
    u64 rxDestinationOffset = 0U;
    CHK_RET(CalcDataSize(innerSendOffset, dataTypeSize_, innerSendOffsetBytes));
    CHK_RET(CalcDataSize(innerRecvOffset, dataTypeSize_, innerRecvOffsetBytes));
    CHK_RET(CalcDataSize(innerCurrSendDataCount, dataTypeSize_, innerCurrSendDataSize));
    CHK_RET(CalcDataSize(innerCurrRecvDataCount, dataTypeSize_, innerCurrRecvDataSize));
    CHK_RET(
        CalcUserSliceOffset(tempAlgParams.sdispls[targetRank], dataTypeSize_, innerSendOffsetBytes, txSourceOffset));
    CHK_RET(CalcCclSliceOffset(myAlgRank_, scratchBufferSizePerRank_, innerSendOffsetBytes, txDestinationOffset));
    CHK_RET(CalcCclSliceOffset(targetRank, scratchBufferSizePerRank_, innerRecvOffsetBytes, rxSourceOffset));
    CHK_RET(CalcCclSliceOffset(targetRank, scratchBufferSizePerRank_, innerRecvOffsetBytes, rxDestinationOffset));
    CHK_PRT_RET(
        AddOverflows(innerSendOffset, innerCurrSendDataCount) || AddOverflows(innerRecvOffset, innerCurrRecvDataCount),
        HCCL_ERROR("[InsTempUBXAllToAllVMesh1D] pairwise data offset overflows."), HCCL_E_PARA);
    const u32 queId = channelIndex + 1; // 主流用来后同步了
    DataSlice txSrcSlice(
        tempAlgParams.buffInfo.inputPtr, txSourceOffset, innerCurrSendDataSize, innerCurrSendDataCount);
    DataSlice txDstSlice(
        channelSendRecv[channelIndex].remoteCclMem.addr, txDestinationOffset, innerCurrSendDataSize,
        innerCurrSendDataCount);
    DataSlice rxSrcSlice(
        channelSendRecv[channelIndex].remoteCclMem.addr, rxSourceOffset, innerCurrRecvDataSize, innerCurrRecvDataCount);
    DataSlice rxDstSlice(
        tempAlgParams.buffInfo.hcclBuff.addr, rxDestinationOffset, innerCurrRecvDataSize, innerCurrRecvDataCount);
    std::vector<DataSlice> txSrcSlices{txSrcSlice};
    std::vector<DataSlice> txDstSlices{txDstSlice};
    std::vector<DataSlice> rxSrcSlices{rxSrcSlice};
    std::vector<DataSlice> rxDstSlices{rxDstSlice};
    CHK_RET(RunPairwiseTransfer(
        channelSendRecv[channelIndex], threads, queId, txSrcSlices, txDstSlices, rxSrcSlices, rxDstSlices,
        innerCurrSendDataSize, innerCurrRecvDataSize));
    innerSendOffset += innerCurrSendDataCount;
    innerRecvOffset += innerCurrRecvDataCount;
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::RunPairwiseTransfer(
    const ChannelInfo& channel, const std::vector<ThreadHandle>& threads, u32 queId,
    const std::vector<DataSlice>& txSrcSlices, const std::vector<DataSlice>& txDstSlices,
    const std::vector<DataSlice>& rxSrcSlices, const std::vector<DataSlice>& rxDstSlices, u64 sendDataSize,
    u64 recvDataSize)
{
    if (sendDataSize > 0 && recvDataSize > 0) {
        SendRecvInfo sendRecvInfo{{channel, channel}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}};
        CHK_RET(SendRecvWrite(sendRecvInfo, threads[queId]));
    } else if (sendDataSize > 0) {
        DataInfo sendInfo{channel, {txSrcSlices, txDstSlices}, dataType_};
        CHK_RET(SendWrite(sendInfo, threads[queId]));
    } else if (recvDataSize > 0) {
        DataInfo recvInfo{channel, {rxSrcSlices, rxDstSlices}, dataType_};
        CHK_RET(RecvWrite(recvInfo, threads[queId]));
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::SavePairwiseLocalCopy(
    const TemplateDataParams& tempAlgParams, u32 targetRank, u64 curRecvDataCount)
{
    u64 scratchOffset = 0U;
    u64 outputOffset = 0U;
    u64 recvSize = 0U;
    CHK_RET(CalcCclSliceOffset(targetRank, scratchBufferSizePerRank_, 0U, scratchOffset));
    CHK_RET(CalcUserSliceOffset(tempAlgParams.rdispls[targetRank], dataTypeSize_, 0U, outputOffset));
    CHK_RET(CalcDataSize(curRecvDataCount, dataTypeSize_, recvSize));
    DataSlice scratchSlices(tempAlgParams.buffInfo.hcclBuff.addr, scratchOffset, recvSize, curRecvDataCount);
    DataSlice usrOutSlices(tempAlgParams.buffInfo.outputPtr, outputOffset, recvSize, curRecvDataCount);
    localCopyInfo_.clear();
    if (curRecvDataCount > 0) {
        localCopyInfo_.push_back(std::vector<DataSlice>{scratchSlices, usrOutSlices});
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::RunPairwise(
    const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 targetBoard)
{
    const std::map<u32, std::vector<ChannelInfo>>& channels = templateResource.channels;
    const std::vector<ThreadHandle>& threads = templateResource.threads;

    // curBoard 和 targetBoard 两两收发，要按照编排好的次序
    std::vector<std::vector<u32>> rankSendRecvMatrix(
        algBoardNum_ * rankNumPerBoard_, std::vector<u32>(rankNumPerBoard_, 0));
    CHK_RET(GetRankSendRecvMatrix(currBoard_, targetBoard, rankSendRecvMatrix));
    // 按照一定的顺序，遍历board内的卡
    for (u32 step = 0; step < rankNumPerBoard_; step++) {
        u32 targetRank = rankSendRecvMatrix[myAlgRank_][step];

        // 跳过虚拟board 的收发
        if (targetRank >= templateRankSize_) {
            HCCL_INFO(
                "[InsTempUBXAllToAllVMesh1D] myRank is [%u], targetRank is [%u], "
                "targetRank is out of rankSize, skip",
                myAlgRank_, targetRank);
            continue;
        }

        CHK_RET(RunPairwiseStep(threads));
        u32 linkNumSendRecv = channels.at(targetRank).size();
        HCCL_INFO(
            "[InsTempUBXAllToAllVMesh1D] myRank is [%u], targetRank is [%u], currBoard is [%u], "
            "targetBoard is [%u], linkNumSendRecv is[%u]",
            myAlgRank_, targetRank, currBoard_, targetBoard, linkNumSendRecv);
        const std::vector<ChannelInfo>& channelSendRecv = channels.at(targetRank);
        u64 innerSendOffset = 0;
        u64 innerRecvOffset = 0;
        u64 curSendDataCount = tempAlgParams.sendCounts[targetRank];
        u64 curRecvDataCount = tempAlgParams.recvCounts[targetRank];
        for (u32 j = 0; j < linkNumSendRecv; j++) {
            CHK_RET(RunPairwiseChannel(
                tempAlgParams, channelSendRecv, threads, targetRank, j, innerSendOffset, innerRecvOffset,
                curSendDataCount, curRecvDataCount));
        }

        // recv 完成之后，后同步信息保留下来
        CHK_RET(SavePairwiseLocalCopy(tempAlgParams, targetRank, curRecvDataCount));

        if (threadNum_ > 1) {
            GetNotifyIdxClosToMain(notifyIdxSubToMain_);
            CHK_RET(PostSyncInterThreads(threads[0], subThreadsBoard_, notifyIdxSubToMain_));
        }
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::InitRankAndDataType(const OpParam& param)
{
    dataType_ = param.all2AllVDataDes.sendType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];
    // myAlgRank_应该是逻辑rank，myRank_ 是物理rank，如果全打平来看，应该是一样的
    auto iter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    if (iter != subCommRanks_[0].end()) {
        myAlgRank_ = std::distance(subCommRanks_[0].begin(), iter);
    } else {
        HCCL_ERROR("[InsTempUBXAllToAllVMesh1D][InitParam] subCommRanks_ or myRank_ is error.");
        return HCCL_E_INTERNAL;
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::InitBoardState(const TemplateResource& templateResource)
{
    CHK_RET(GetRankNumPerBoard(templateResource));
    CHK_RET(CheckPathNum(templateResource));
    currBoard_ = myAlgRank_ / rankNumPerBoard_;
    currRankIndex_ = myAlgRank_ % rankNumPerBoard_;
    boardNum_ = templateRankSize_ / rankNumPerBoard_;
    // 考虑boardNum 不是偶数的场景,先配置成偶数board，实际收发再跳过
    algBoardNum_ = boardNum_;
    if (boardNum_ % UBX_BOARD_PAIR_SIZE == 1) {
        algBoardNum_ = algBoardNum_ + 1;
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::InitDataState(const TemplateDataParams& tempAlgParams)
{
    scratchBufferSizePerRank_ = tempAlgParams.inputSliceStride;
    dataStridePerRank_ = tempAlgParams.outputSliceStride;
    curProcessedDataCount_ = tempAlgParams.processedDataCount;
    curDataCount_ = tempAlgParams.count;
    CHK_RET(CalcDataSize(curDataCount_, dataTypeSize_, curDataSize_));
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::InitThreadState(const TemplateResource& templateResource)
{
    threadNum_ = templateResource.threads.size();
    if (threadNum_ != maxPathNum_ + maxRankNumPerBoard_ + 1) {
        HCCL_ERROR(
            "[InsTempUBXAllToAllVMesh1D] tempInsQues.size() is [%u], but it should be [%u]", threadNum_,
            maxPathNum_ + maxRankNumPerBoard_ + 1);
        return HcclResult::HCCL_E_PARA;
    }
    return HcclResult::HCCL_SUCCESS;
}

void InsTempUBXAllToAllVMesh1D::LogInitState()
{
    HCCL_INFO(
        "[InsTempUBXAllToAllVMesh1D] myRank_ is [%u], myAlgRank_ is [%u], rankNumPerBoard_ is [%u], "
        "templateRankSize_ is [%u], currBoard_ is [%u], currRankIndex_ is [%u], boardNum_ is [%u], "
        "algBoardNum_ is [%u], threadNum_ is [%u], scratchBufferSizePerRank_ is [%llu], dataStridePerRank_ is [%llu], "
        "curProcessedDataCount_ is [%llu], curDataCount_ is [%llu], curDataSize_ is [%llu]",
        myRank_, myAlgRank_, rankNumPerBoard_, templateRankSize_, currBoard_, currRankIndex_, boardNum_, algBoardNum_,
        threadNum_, scratchBufferSizePerRank_, dataStridePerRank_, curProcessedDataCount_, curDataCount_, curDataSize_);
}

void InsTempUBXAllToAllVMesh1D::LogDataSlices(const TemplateDataParams& tempAlgParams)
{
    for (u32 i = 0; i < templateRankSize_; i++) {
        HCCL_DEBUG(
            "[InsTempUBXAllToAllVMesh1D] rank is [%u], sendCounts[%u] is [%llu], recvCounts[%u] is [%llu], "
            "sdispls[%u] is [%llu], rdispls[%u] is [%llu]",
            myAlgRank_, i, tempAlgParams.sendCounts[i], i, tempAlgParams.recvCounts[i], i, tempAlgParams.sdispls[i], i,
            tempAlgParams.rdispls[i]);
    }
}

HcclResult InsTempUBXAllToAllVMesh1D::InitParam(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempUBXAllToAllVMesh1D][InitParam] Run Start");
    CHK_RET(InitRankAndDataType(param));
    CHK_RET(InitBoardState(templateResource));
    CHK_RET(InitDataState(tempAlgParams));
    CHK_RET(InitThreadState(templateResource));
    LogInitState();
    LogDataSlices(tempAlgParams);
    const std::vector<ThreadHandle>& threads = templateResource.threads;
    if (threadNum_ > 1) {
        subThreadsBoard_.assign(threads.begin() + 1, threads.begin() + 1 + maxPathNum_); // clos用
        subThreadsFullMesh_.assign(
            threads.begin() + 1 + maxPathNum_,
            threads.begin() + 1 + maxPathNum_ + maxRankNumPerBoard_); // fullmesh用
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempUBXAllToAllVMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempUBXAllToAllVMesh1D][KernelRun] Run Start");

    CHK_RET(InitParam(param, tempAlgParams, templateResource));

    localCopyInfo_.clear();
    localCopyInfoFullMesh_.clear();

    const std::vector<ThreadHandle>& threads = templateResource.threads;
    std::vector<std::vector<u32>> sendRecvMatrix(algBoardNum_, std::vector<u32>(algBoardNum_ - 1, 0));
    CHK_RET(GetBoardSendRecvMatrix(algBoardNum_, sendRecvMatrix));

    // 这里是fullmesh的前同步
    if (threadNum_ > 1) {
        GetNotifyIdxMainToFullMesh(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(threads[0], subThreadsFullMesh_, notifyIdxMainToSub_));
    }

    // 前 maxPathNum_条从流用来做clos通信
    // 第一轮的pairwise和fullmesh并行
    u32 targetBoard = sendRecvMatrix[currBoard_][0];
    CHK_RET(RunPairwise(tempAlgParams, templateResource, targetBoard));

    // fullmesh的前后流同步要取出来，不然就变串行了
    CHK_RET(RunFullMesh(tempAlgParams, templateResource));

    // 这里是fullmesh的后同步
    if (threadNum_ > 1) {
        GetNotifyIdxFullMeshToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(threads[0], subThreadsFullMesh_, notifyIdxSubToMain_));
    }

    for (u32 boardIndex = 1; boardIndex < algBoardNum_ - 1; boardIndex++) {
        targetBoard = sendRecvMatrix[currBoard_][boardIndex];
        CHK_RET(RunPairwise(tempAlgParams, templateResource, targetBoard));
    }

    // 最后做一次后同步
    for (u32 i = 0; i < localCopyInfo_.size(); i++) {
        CHK_RET(LocalCopy(threads[0], localCopyInfo_[i][0], localCopyInfo_[i][1]));
    }
    for (u32 i = 0; i < localCopyInfoFullMesh_.size(); i++) {
        CHK_RET(LocalCopy(threads[0], localCopyInfoFullMesh_[i][0], localCopyInfoFullMesh_[i][1]));
    }

    HCCL_INFO("[InsTempUBXAllToAllVMesh1D] AllToAll full mesh rank[%d] finish.", myAlgRank_);
    return HcclResult::HCCL_SUCCESS;
}

void InsTempUBXAllToAllVMesh1D::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMianToSub)
{
    (void)notifyIdxMianToSub;
    return;
}

void InsTempUBXAllToAllVMesh1D::GetNotifyIdxMainToClos(std::vector<u32>& notifyIdxMianToSub)
{
    notifyIdxMianToSub.clear();
    if (threadNum_ <= 1) {
        return;
    }
    u32 slaveThreadNum = threadNum_ - 1 - maxRankNumPerBoard_;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMianToSub.push_back(0);
    }
}

void InsTempUBXAllToAllVMesh1D::GetNotifyIdxMainToFullMesh(std::vector<u32>& notifyIdxMianToSub)
{
    notifyIdxMianToSub.clear();
    if (threadNum_ <= 1) {
        return;
    }
    u32 slaveThreadNum = threadNum_ - 1 - maxPathNum_;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMianToSub.push_back(0);
    }
}

void InsTempUBXAllToAllVMesh1D::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    (void)notifyIdxSubToMain;
    return;
}

void InsTempUBXAllToAllVMesh1D::GetNotifyIdxClosToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 notifyNum = threadNum_ - 1 - maxRankNumPerBoard_;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

void InsTempUBXAllToAllVMesh1D::GetNotifyIdxFullMeshToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 notifyNum = threadNum_ - 1;
    for (u32 notifyIdx = maxPathNum_; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

} // namespace mc2_ops_hccl
