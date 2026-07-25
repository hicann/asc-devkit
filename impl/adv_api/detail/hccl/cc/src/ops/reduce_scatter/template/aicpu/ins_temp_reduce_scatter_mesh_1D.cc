/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "aicpu/ins_temp_reduce_scatter_mesh_1D.h"

namespace mc2_ops_hccl {
InsTempReduceScatterMesh1D::InsTempReduceScatterMesh1D(
    const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempReduceScatterMesh1D::~InsTempReduceScatterMesh1D() {}

HcclResult InsTempReduceScatterMesh1D::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    resourceRequest.slaveThreadNum = threadNum - 1;
    for (u32 index = 0; index < threadNum - 1; index++) {
        resourceRequest.notifyNumPerThread.push_back(1);
    }
    resourceRequest.notifyNumOnMainThread = threadNum - 1;

    std::vector<HcclChannelDesc> level0Channels;
    // 校验topoInfo是否为空
    CHK_PRT_RET(
        topoInfo == nullptr, HCCL_ERROR("[InsTempReduceScatterMesh1D][CalcRes] topoInfo is nullptr"), HCCL_E_PARA);
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix) {
        std::vector<HcclChannelDesc> myChannelDescs;
        CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
            comm, param, topoInfo, subCommRanks_, myChannelDescs, CommTopo::COMM_TOPO_CLOS));
        for (auto channel : myChannelDescs) {
            if (channel.channelProtocol == COMM_PROTOCOL_UBC_CTP) {
                level0Channels.push_back(channel);
            }
        }
    } else {
        CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, level0Channels));
    }

    resourceRequest.channels.push_back(level0Channels);
    HCCL_DEBUG(
        "[InsTempReduceScatterMesh1D][CalcRes] myRank[%u], notifyNumOnMainThread[%u], slaveThreadNum[%u]", myRank_,
        resourceRequest.notifyNumOnMainThread, resourceRequest.slaveThreadNum);
    return HCCL_SUCCESS;
}

u64 InsTempReduceScatterMesh1D::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    u64 scratchMultiple = templateRankSize_;
    return scratchMultiple;
}

HcclResult InsTempReduceScatterMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    if (tempAlgParams.sliceSize == 0 && tempAlgParams.tailSize == 0) {
        HCCL_DEBUG(
            "[InsTempReduceScatterMesh1D] myRank[%u] sliceSize and tailSize are 0, skip reduce scatter.", myRank_);
        return HCCL_SUCCESS;
    }
    CHK_RET(SetchannelsPerRank(templateResource.channels));
    threadNum_ = GetThreadNum();
    dataType_ = param.DataDes.dataType;
    processSize_ = tempAlgParams.sliceSize;
    count_ = tempAlgParams.sliceSize / DATATYPE_SIZE_TABLE[dataType_];
    HCCL_INFO("[InsTempReduceScatterMesh1D] Run Start");
    HCCL_INFO(
        "[InsTempReduceScatterMesh1D] KernelRun threadNum_[%u], templateResource.threads.size()[%u]", threadNum_,
        templateResource.threads.size());
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }
    CHK_RET(RunReduceScatter(templateResource.channels, templateResource.threads, tempAlgParams));
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    if (dataType_ == HCCL_DATA_TYPE_INT64 || dataType_ == HCCL_DATA_TYPE_UINT64 || dataType_ == HCCL_DATA_TYPE_FP64 ||
        reduceOp_ == HcclReduceOp::HCCL_REDUCE_PROD) {
        CHK_RET(static_cast<HcclResult>(HcommBatchModeEnd(param.algTag)));
        CHK_RET(static_cast<HcclResult>(HcommBatchModeStart(param.algTag)));
        for (const auto& thread : templateResource.threads) {
            CHK_RET(static_cast<HcclResult>(HcommThreadJoin(thread, CUSTOM_TIMEOUT)));
        }
    }
    PostCopy(param, tempAlgParams, templateResource.threads);
    HCCL_INFO("[InsTempReduceScatterMesh1D] Run End");
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterMesh1D::PostCopy(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads)
{
    // 通信结束之后，数据都在 cclBuffer 上，需要搬运到对应的输出位置。
    u32 rankIdx = 0;
    auto iter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    if (iter != subCommRanks_[0].end()) {
        rankIdx = std::distance(subCommRanks_[0].begin(), iter);
    } else {
        HCCL_ERROR("[InsTempReduceScatterMesh1D][PostCopy] subCommRanks_ or myRank_ is error.");
        return HCCL_E_INTERNAL;
    }
    // 如果是单算子模式, 并且是最后一步算子，需要将数据从 cclBuffer 拷贝到 userOut
    // 先把本卡的数据从userIn搬运到userOut，然后再在userOut上做规约
    u32 myAlgRank = 0;
    u64 buffSliceStride = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    if (myAlgRank == templateRankSize_ - 1 && tempAlgParams.tailSize > 0) {
        processSize_ = tempAlgParams.tailSize;
        count_ = tempAlgParams.tailSize / DATATYPE_SIZE_TABLE[dataType_];
        buffSliceStride = tempAlgParams.tailSize;
    } else {
        processSize_ = tempAlgParams.sliceSize;
        count_ = tempAlgParams.sliceSize / DATATYPE_SIZE_TABLE[dataType_];
        buffSliceStride = tempAlgParams.sliceSize;
    }

    for (u32 repeatIdx = 0; repeatIdx < tempAlgParams.repeatNum; repeatIdx++) {
        if (tempAlgParams.buffInfo.inBuffType != tempAlgParams.buffInfo.outBuffType ||
            tempAlgParams.buffInfo.inBuffBaseOff != tempAlgParams.buffInfo.outBuffBaseOff) {
            DataSlice srcSlice = DataSlice(
                tempAlgParams.buffInfo.inputPtr,
                tempAlgParams.buffInfo.inBuffBaseOff + repeatIdx * tempAlgParams.inputRepeatStride +
                    myAlgRank * tempAlgParams.inputSliceStride,
                processSize_, count_);
            DataSlice dstSlice = DataSlice(
                tempAlgParams.buffInfo.outputPtr,
                tempAlgParams.buffInfo.outBuffBaseOff + repeatIdx * tempAlgParams.outputRepeatStride +
                    myAlgRank * tempAlgParams.outputSliceStride,
                processSize_, count_);
            CHK_RET(static_cast<HcclResult>(LocalCopy(threads[0], srcSlice, dstSlice)));
        }
        if (dataType_ == HCCL_DATA_TYPE_INT64 || dataType_ == HCCL_DATA_TYPE_UINT64 ||
            dataType_ == HCCL_DATA_TYPE_FP64 || reduceOp_ == HcclReduceOp::HCCL_REDUCE_PROD) {
            CHK_RET(static_cast<HcclResult>(HcommBatchModeEnd(param.algTag)));
            CHK_RET(static_cast<HcclResult>(HcommBatchModeStart(param.algTag)));
            for (const auto& thread : threads) {
                CHK_RET(static_cast<HcclResult>(HcommThreadJoin(thread, CUSTOM_TIMEOUT)));
            }
        }
        // 把其他卡的数据input累加到output
        for (u32 tmpRank = 0; tmpRank < templateRankSize_; tmpRank++) {
            if (tmpRank != rankIdx) {
                DataSlice srcSlice = DataSlice(
                    tempAlgParams.buffInfo.hcclBuff.addr,
                    tempAlgParams.buffInfo.hcclBuffBaseOff + repeatIdx * tempAlgParams.outputRepeatStride +
                        tmpRank * buffSliceStride,
                    processSize_, count_);
                DataSlice dstSlice = DataSlice(
                    tempAlgParams.buffInfo.outputPtr,
                    tempAlgParams.buffInfo.outBuffBaseOff + repeatIdx * tempAlgParams.outputRepeatStride +
                        rankIdx * tempAlgParams.outputSliceStride,
                    processSize_, count_);
                CHK_RET(static_cast<HcclResult>(LocalReduce(threads[0], srcSlice, dstSlice, dataType_, reduceOp_)));
            }
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterMesh1D::RunReduceScatter(
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
    const TemplateDataParams& tempAlgParam)
{
    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    u32 queIdx = 1;
    for (u32 rankIdx = 1; rankIdx < templateRankSize_; rankIdx++) {
        u32 nextRank = (myAlgRank + rankIdx) % templateRankSize_; // 这里取的虚拟rankId
        u64 sliceSize = processSize_;
        u64 sliceCount = count_;
        u64 outputSliceStride = tempAlgParam.sliceSize;
        if ((nextRank == templateRankSize_ - 1) && (tempAlgParam.tailSize > 0)) {
            sliceSize = tempAlgParam.tailSize;
            sliceCount = tempAlgParam.tailSize / DATATYPE_SIZE_TABLE[dataType_];
            outputSliceStride = tempAlgParam.tailSize;
        }
        u32 remoteRank = subCommRanks_[0][nextRank];
        HCCL_DEBUG(
            "[InsTempReduceScatterMesh1D][RunReduceScatter] myRank[%d], toRank[%d], fromRank[%d]", myRank_, remoteRank,
            remoteRank);
        const std::vector<ChannelInfo>& curChannels = channels.at(remoteRank);
        CHK_RET(CalcDataSplitByPortGroup(
            sliceCount, DATATYPE_SIZE_TABLE[dataType_], curChannels, elemCountOut_, sizeOut_, elemOffset_));
        for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
            const ChannelInfo& linkSend = curChannels[channelIdx];
            const ChannelInfo& linkRecv = curChannels[channelIdx];
            sliceSize = sizeOut_[channelIdx];
            sliceCount = elemCountOut_[channelIdx];
            std::vector<DataSlice> txSrcSlices;
            std::vector<DataSlice> txDstSlices;
            std::vector<DataSlice> rxSrcSlices;
            std::vector<DataSlice> rxDstSlices;

            // 在 HcclBuffer 上进行 ReduceScatter 操作
            // 由于进程只能访问远端的HcclBuffer，所以只能通过write的方式将自己userIn上的数据写到远端HcclBuffer上
            for (u32 repeatIdx = 0; repeatIdx < tempAlgParam.repeatNum; repeatIdx++) {
                // 在reduce_scatter_op.cc的创建channels的环节中获取到了remote的HcclBuff的地址
                void* remoteCclBuffAddr = linkSend.remoteCclMem.addr;
                // 在接收的时候接收源应该是远端地址，但是由于rs的mesh算法用的是write，所以rx不用care
                DataSlice rxSrcSlice = DataSlice(
                    remoteCclBuffAddr,
                    tempAlgParam.buffInfo.inBuffBaseOff + repeatIdx * tempAlgParam.inputRepeatStride +
                        myAlgRank * tempAlgParam.inputSliceStride + elemOffset_[channelIdx],
                    sliceSize, sliceCount); // 接收源
                DataSlice rxDstSlice = DataSlice(
                    tempAlgParam.buffInfo.hcclBuff.addr,
                    tempAlgParam.buffInfo.hcclBuffBaseOff + repeatIdx * tempAlgParam.outputRepeatStride +
                        nextRank * outputSliceStride + elemOffset_[channelIdx],
                    sliceSize, sliceCount); // 接收目标
                DataSlice txSrcSlice = DataSlice(
                    tempAlgParam.buffInfo.inputPtr,
                    tempAlgParam.buffInfo.inBuffBaseOff + repeatIdx * tempAlgParam.inputRepeatStride +
                        nextRank * tempAlgParam.inputSliceStride + elemOffset_[channelIdx],
                    sliceSize, sliceCount); // 发送源
                DataSlice txDstSlice = DataSlice(
                    remoteCclBuffAddr,
                    tempAlgParam.buffInfo.hcclBuffBaseOff + repeatIdx * tempAlgParam.outputRepeatStride +
                        myAlgRank * outputSliceStride + elemOffset_[channelIdx],
                    sliceSize, sliceCount); // 发送目标

                rxSrcSlices.push_back(rxSrcSlice);
                rxDstSlices.push_back(rxDstSlice);
                txSrcSlices.push_back(txSrcSlice);
                txDstSlices.push_back(txDstSlice);
            }
            SendRecvInfo sendRecvInfo{
                {linkSend, linkRecv}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}, dataType_};
            CHK_PRT_RET(
                SendRecvBatchWrite(sendRecvInfo, threads[queIdx]),
                HCCL_ERROR("[InsTempReduceScatterMesh1D] RunReduceScatter Send failed"), HcclResult::HCCL_E_INTERNAL);
            queIdx++;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterMesh1D::CalcDataSplitByPortGroup(
    const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
    std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset)
{
    return CalcDataSplitByPortGroupCommon(
        totalDataCount, dataTypeSize, channels, elemCountOut, sizeOut, elemOffset, channelsPerRank_);
}

HcclResult InsTempReduceScatterMesh1D::SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    CHK_PRT_RET(
        channels.empty(), HCCL_ERROR("[InsTempReduceScatterMesh1D][SetchannelsPerRank] channels is empty."),
        HCCL_E_INTERNAL);
    channelsPerRank_ = CalcChannelsPerRank(channels);
    return HCCL_SUCCESS;
}

void InsTempReduceScatterMesh1D::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = GetThreadNum();
    HCCL_DEBUG("[InsTempReduceScatterMesh1D][GetNotifyIdxMainToSub] threadNum[%u]", threadNum);
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(0);
    }
}

void InsTempReduceScatterMesh1D::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = GetThreadNum();
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

u64 InsTempReduceScatterMesh1D::GetThreadNum() const
{
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    return threadNum;
}

HcclResult InsTempReduceScatterMesh1D::GetRes(AlgResourceRequest& resourceRequest) const
{
    u32 threadNum = GetThreadNum();
    resourceRequest.slaveThreadNum = threadNum - 1;
    for (u32 index = 0; index < threadNum - 1; index++) {
        resourceRequest.notifyNumPerThread.push_back(1);
    }
    resourceRequest.notifyNumOnMainThread = threadNum - 1;

    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
