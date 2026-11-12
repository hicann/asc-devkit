/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_gather_mesh_1D.h"
#include <algorithm>
#include "alg_data_trans_wrapper.h"
#include "channel.h"
#include "template_utils.h"
#include "hcomm_primitives_dl.h"
namespace mc2_ops_hccl {
InsTempAllGatherMesh1D::InsTempAllGatherMesh1D(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks),
      isConcurrent_(std::string(param.algName) == "AicpuAllGatherConcurMeshNHR")
{}
InsTempAllGatherMesh1D::~InsTempAllGatherMesh1D() {}

HcclResult InsTempAllGatherMesh1D::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    HCCL_INFO("[InsTempAllGatherMesh1D][CalcRes] start");
    std::vector<HcclChannelDesc> level0Channels;
    CHK_PRT_RET(topoInfo == nullptr, HCCL_ERROR("[InsTempAllGatherMesh1D][CalcRes] topoInfo is nullptr"), HCCL_E_PARA);
    if (isConcurrent_ && concurrentTopo_ == COMM_TOPO_CLOS) {
        // 为所有peer申请CLOS物理链路上的共享binding通道。
        CHK_RET(
            CalcChannelRequestMeshClosMultiJetty(comm, param, topoInfo, subCommRanks_, level0Channels, false, false));
    } else if (isConcurrent_) {
        // 优先选择Mesh物理层上的直连通道。
        CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
            comm, param, topoInfo, subCommRanks_, level0Channels, COMM_TOPO_1DMESH));
    } else {
        CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, level0Channels));
    }
    channelsPerRank_ = CalcChannelsPerRankMin(level0Channels);
    maxChannelsPerRank_ = CalcChannelsPerRank(level0Channels);
    HCCL_INFO(
        "[InsTempAllGatherMesh1D][CalcRes] channelsPerRank_[%u], maxChannelsPerRank_[%u]", channelsPerRank_,
        maxChannelsPerRank_);
    CHK_RET(GetRes(resourceRequest));
    resourceRequest.channels.push_back(level0Channels);
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherMesh1D::GetRes(AlgResourceRequest& resourceRequest) const
{
    u32 threadNum = GetThreadNum();
    resourceRequest.slaveThreadNum = threadNum - 1;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    resourceRequest.notifyNumOnMainThread = threadNum - 1;
    return HCCL_SUCCESS;
}

u64 InsTempAllGatherMesh1D::GetThreadNum() const
{
    if (isConcurrent_) {
        // 主流处理本卡拷贝，每个peer/channel使用独立从流。
        return 1 + (templateRankSize_ > 1 ? templateRankSize_ - 1 : 0) * maxChannelsPerRank_;
    }
    // 普通Mesh每个peer固定使用一条通道，其余通道不参与本次算法。
    return templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
}

HcclResult InsTempAllGatherMesh1D::SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    CHK_PRT_RET(
        channels.empty(), HCCL_ERROR("[InsTempAllGatherMesh1D][SetchannelsPerRank] channels is empty."),
        HCCL_E_INTERNAL);
    channelsPerRank_ = CalcChannelsPerRankMin(channels);
    maxChannelsPerRank_ = CalcChannelsPerRank(channels);
    HCCL_INFO(
        "[InsTempAllGatherMesh1D][SetchannelsPerRank] channelsPerRank_[%u], maxChannelsPerRank_[%u]", channelsPerRank_,
        maxChannelsPerRank_);
    return HCCL_SUCCESS;
}

u64 InsTempAllGatherMesh1D::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    u64 scratchMultiple = 0;
    // 为单算子或Concurrent的CCL暂存区按rank预留分片空间。
    if (opMode_ == OpMode::OPBASE || isConcurrent_) {
        scratchMultiple = templateRankSize_;
    }
    return scratchMultiple;
}

HcclResult InsTempAllGatherMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    enableRemoteMemAccess_ = tempAlgParams.enableRemoteMemAccess;
    supportSymmetricMemory_ = param.supportSymmetricMemory;
    HCCL_INFO("[InsTempAllGatherMesh1D] Run start");
    if (tempAlgParams.sliceSize == 0 && tempAlgParams.tailSize == 0) {
        HCCL_INFO("[InsTempAllGatherMesh1D] Rank [%d], get slicesize zero.", myRank_);
        return HCCL_SUCCESS;
    }
    inputSymWindow_ = param.inputSymWindow;
    outputSymWindow_ = param.outputSymWindow;
    inputOffset_ = param.inputOffset;
    outputOffset_ = param.outputOffset;
    threadNum_ = GetThreadNum();
    tempAlgParams_ = tempAlgParams;
    dataType_ = param.DataDes.dataType;
    HCCL_DEBUG(
        "[InsTempAllGatherMesh1D][KernelRun] rank[%u], threadNum[%u], concurrent[%d], concurrentTopo[%d], "
        "remoteMemAccess[%d], symmetric[%d].",
        myRank_, threadNum_, isConcurrent_, static_cast<int>(concurrentTopo_), enableRemoteMemAccess_,
        supportSymmetricMemory_);
    CHK_PRT_RET(
        threadNum_ > templateResource.threads.size(),
        HCCL_ERROR(
            "[InsTempAllGatherMesh1D][KernelRun] rank[%u] has insufficient threads: available[%zu], required[%u].",
            myRank_, templateResource.threads.size(), threadNum_),
        HCCL_E_INTERNAL);
    // 普通Mesh先完成本卡拷贝，Concurrent非对称路径仅暂存CCL。
    if (!isConcurrent_) {
        CHK_RET(LocalDataCopy(templateResource.threads));
    } else if (!supportSymmetricMemory_) {
        CHK_RET(LocalDataCopyConcurrent(templateResource.threads, false));
    }
    if (!isConcurrent_ && templateRankSize_ == 1) {
        return HcclResult::HCCL_SUCCESS;
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(
            templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }

    CHK_RET(RunAllGatherMesh(templateResource.threads, templateResource.channels));
    if (isConcurrent_) {
        // peer从流读取数据时，主流并行拷贝本卡输出分片。
        CHK_RET(LocalDataCopyConcurrent(templateResource.threads, true));
    }

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(
            templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    HCCL_INFO("[InsTempAllGatherMesh1D] Run End");
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherMesh1D::RunAllGatherMeshOnChannel(
    const ThreadHandle& thread, const ChannelInfo& linkRemote, void* remoteInputAddr, void* remoteCclBuffAddr,
    const u32 connectedRank, const u32 connectedAlgRank, const u32 channelIdx)
{
    std::vector<DataSlice> rxDstSlicesAll;
    std::vector<DataSlice> rxSrcSlicesAll;

    for (u32 rpt = 0; rpt < tempAlgParams_.repeatNum; ++rpt) {
        const u64 outBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
        const u64 scratchRepeatStride = tempAlgParams_.sliceSize * templateRankSize_;
        u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * scratchRepeatStride;
        if (tempAlgParams_.buffInfo.inBuffType == BufferType::HCCL_BUFFER) {
            scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * tempAlgParams_.inputRepeatStride;
        }

        u64 rxOutOffset =
            tempAlgParams_.outputSliceStride * connectedAlgRank + outBaseOff + chanElemOffset_[channelIdx];
        u64 rxSrcOffset = 0;
        void* rxSrcPtr = nullptr;
        void* rxDstPtr = tempAlgParams_.buffInfo.outputPtr;
        if (supportSymmetricMemory_) {
            // 对称内存路径直接读取peer的input窗口，按peer的分片布局计算读取偏移。
            rxSrcPtr = remoteInputAddr;
            rxSrcOffset = tempAlgParams_.buffInfo.inBuffBaseOff + rpt * tempAlgParams_.inputRepeatStride +
                          connectedAlgRank * tempAlgParams_.inputSliceStride + chanElemOffset_[channelIdx];
        } else if (enableRemoteMemAccess_) {
            rxSrcPtr = linkRemote.remoteOutputGraphMode.addr;
            rxSrcOffset = rxOutOffset;
        } else {
            u64 rxScratchOffset =
                scratchBase + tempAlgParams_.sliceSize * connectedAlgRank + chanElemOffset_[channelIdx];
            if (tempAlgParams_.buffInfo.inBuffType == BufferType::HCCL_BUFFER) {
                rxScratchOffset =
                    scratchBase + tempAlgParams_.inputSliceStride * connectedAlgRank + chanElemOffset_[channelIdx];
            }
            rxSrcPtr = remoteCclBuffAddr;
            rxSrcOffset = rxScratchOffset;
        }
        CHK_PRT_RET(
            isConcurrent_ && rxSrcPtr == nullptr,
            HCCL_ERROR(
                "[InsTempAllGatherMesh1D][RunAllGatherMeshOnChannel] peer[%u] buffer is unavailable.", connectedRank),
            HCCL_E_INTERNAL);

        rxDstSlicesAll.emplace_back(rxDstPtr, rxOutOffset, chanSize_[channelIdx], chanElemCount_[channelIdx]);
        rxSrcSlicesAll.emplace_back(rxSrcPtr, rxSrcOffset, chanSize_[channelIdx], chanElemCount_[channelIdx]);

        HCCL_DEBUG(
            "[InsTempAllGatherMesh1D][RunAllGatherMesh] rankId [%d] connectedRank [%d] rpt [%d] "
            "channelIdx[%u] offset[%llu] sliceSize[%llu] count[%llu] srcOffset[%llu] outOffset[%llu].",
            myRank_, connectedRank, rpt, channelIdx, chanElemOffset_[channelIdx], chanSize_[channelIdx],
            chanElemCount_[channelIdx], rxSrcOffset, rxOutOffset);
    }

    const std::vector<DataSlice> emptySlices;
    TxRxSlicesList sendRecvSlicesList({emptySlices, emptySlices}, {rxSrcSlicesAll, rxDstSlicesAll});
    TxRxChannels sendRecvChannels(linkRemote, linkRemote);
    SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList, dataType_);
    if (isConcurrent_ && concurrentTopo_ == COMM_TOPO_CLOS) {
        // 在CLOS轴的peer线程上提交普通Read任务。
        CHK_RET(SendRecvRead(sendRecvInfo, thread));
        return HCCL_SUCCESS;
    }
    CHK_PRT_RET(
        SendRecvBatchRead(sendRecvInfo, thread), HCCL_ERROR("[InsTempAllGatherMesh1D] RunAllGather Send failed"),
        HcclResult::HCCL_E_INTERNAL);
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherMesh1D::RunAllGatherMesh(
    const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    HCCL_INFO("[InsTempAllGatherMesh1D] RunAllGatherMesh RankIDs[%d].", myRank_);

    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    const u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    // Concurrent的0号线程执行本卡拷贝，peer读取从1号线程开始。
    u32 queIdx = isConcurrent_ ? 1 : 0;
    for (u32 rankIdx = 1; rankIdx < templateRankSize_; rankIdx++) {
        u32 connectedAlgRank = (myAlgRank + rankIdx) % templateRankSize_;
        u32 connectedRank = subCommRanks_[0][connectedAlgRank];

        HCCL_INFO(
            "[InsTempAllGatherMesh1D] RunAllGatherMesh RankIDs[%d], connectedRank[%d], connectedAlgRank[%d].", myRank_,
            connectedRank, connectedAlgRank);

        CHK_PRT_RET(
            channels.count(connectedRank) == 0 || channels.at(connectedRank).empty(),
            HCCL_ERROR(
                "[InsTempAllGatherMesh1D][RankID]=%u connectedRank=%d, channels.size=%zu", myRank_, connectedRank,
                channels.size()),
            HcclResult::HCCL_E_INTERNAL);

        const std::vector<ChannelInfo>& curChannels = channels.at(connectedRank);
        u64 sliceSize = tempAlgParams_.sliceSize;
        if (tempAlgParams_.tailSize != 0 && connectedAlgRank == templateRankSize_ - 1) {
            sliceSize = tempAlgParams_.tailSize;
        }
        u64 sliceCount = sliceSize / dataTypeSize;
        CHK_RET(CalcDataSplitByPortGroup(
            sliceCount, dataTypeSize, curChannels, chanElemCount_, chanSize_, chanElemOffset_));

        // 对称内存路径每个peer只需解析一次远端input地址，各jetty共享同一远端窗口。
        void* remoteInputAddr = nullptr;
        if (supportSymmetricMemory_) {
            HcclResult ret = HcclSymWinGetPeerPointer(inputSymWindow_, inputOffset_, connectedRank, &remoteInputAddr);
            CHK_PRT_RET(
                ret != HCCL_SUCCESS || remoteInputAddr == nullptr,
                HCCL_ERROR(
                    "[InsTempAllGatherMesh1D][RunAllGatherMesh] HcclSymWinGetPeerPointer failed, remoteRank[%u] "
                    "ret[%d] input[%p].",
                    connectedRank, ret, remoteInputAddr),
                HcclResult::HCCL_E_INTERNAL);
        }

        // Concurrent按port group消费多jetty；普通Mesh每个peer只用首条通道。
        const u32 curChannelNum = isConcurrent_ ? std::min(static_cast<u32>(curChannels.size()), channelsPerRank_) : 1U;
        for (u32 channelIdx = 0; channelIdx < curChannelNum; channelIdx++) {
            CHK_PRT_RET(
                queIdx >= threads.size(),
                HCCL_ERROR(
                    "[InsTempAllGatherMesh1D][RankID]=%u queIdx=%u, threads.size=%zu", myRank_, queIdx, threads.size()),
                HcclResult::HCCL_E_INTERNAL);
            const ChannelInfo& linkRemote = curChannels[channelIdx];
            CHK_RET(RunAllGatherMeshOnChannel(
                threads[queIdx], linkRemote, remoteInputAddr, linkRemote.remoteCclMem.addr, connectedRank,
                connectedAlgRank, channelIdx));
            queIdx++;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherMesh1D::LocalDataCopy(const std::vector<ThreadHandle>& threads)
{
    HCCL_INFO("[InsTempAllGatherMesh1D] LocalDataCopy.");
    if (threads.empty()) {
        return HcclResult::HCCL_E_INTERNAL;
    }

    u32 myAlgRank;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    const u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    u64 sliceSize = tempAlgParams_.sliceSize;
    if (tempAlgParams_.tailSize != 0 && myAlgRank == templateRankSize_ - 1) {
        sliceSize = tempAlgParams_.tailSize;
    }
    u64 sliceCount = sliceSize / dataTypeSize;
    for (u32 rpt = 0; rpt < tempAlgParams_.repeatNum; ++rpt) {
        const u64 inBaseOff = tempAlgParams_.buffInfo.inBuffBaseOff + rpt * tempAlgParams_.inputRepeatStride;
        const u64 outBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
        const u64 inOff = tempAlgParams_.inputSliceStride * myAlgRank + inBaseOff;
        const u64 outOff = tempAlgParams_.outputSliceStride * myAlgRank + outBaseOff;

        DataSlice srcSlice(tempAlgParams_.buffInfo.inputPtr, inOff, sliceSize, sliceCount);
        bool skipOutCopy = (tempAlgParams_.buffInfo.inputPtr == tempAlgParams_.buffInfo.outputPtr && inOff == outOff);
        if (!skipOutCopy) {
            DataSlice dstSlice(tempAlgParams_.buffInfo.outputPtr, outOff, sliceSize, sliceCount);
            HCCL_DEBUG(
                "[InsTempAllGatherMesh1D][LocalDataCopy] RankID [%d] AlgRank [%d] srcSlice: inBaseOff[%llu] "
                "inOff[%llu] "
                "sliceSize[%llu] count[%llu].",
                myRank_, myAlgRank, inBaseOff, inOff, sliceSize, sliceCount);
            HCCL_DEBUG(
                "[InsTempAllGatherMesh1D][LocalDataCopy] RankID [%d] AlgRank [%d] dstSlice: outBaseoff[%llu] "
                "outOff[%llu] sliceSize[%llu] count[%llu].",
                myRank_, myAlgRank, outBaseOff, outOff, sliceSize, sliceCount);
            CHK_RET(LocalCopy(threads[0], srcSlice, dstSlice));
        }

        if (!enableRemoteMemAccess_ && !supportSymmetricMemory_) {
            const u64 scratchRepeatStride = tempAlgParams_.sliceSize * templateRankSize_;
            const u64 cclBaseOff = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * scratchRepeatStride;
            u64 cclOff = cclBaseOff + tempAlgParams_.sliceSize * myAlgRank;
            DataSlice cclDstSlice(tempAlgParams_.buffInfo.hcclBuff.addr, cclOff, sliceSize, sliceCount);
            bool skipCclCopy =
                (tempAlgParams_.buffInfo.inputPtr == tempAlgParams_.buffInfo.hcclBuff.addr && inOff == cclOff);
            if (!skipCclCopy) {
                HCCL_DEBUG(
                    "[InsTempAllGatherMesh1D][LocalDataCopy] RankID [%d] AlgRank [%d] copy to ccl: "
                    "cclBaseOff[%llu] cclOff[%llu] sliceSize[%llu] count[%llu].",
                    myRank_, myAlgRank, cclBaseOff, cclOff, sliceSize, sliceCount);
                CHK_RET(LocalCopy(threads[0], srcSlice, cclDstSlice));
            }
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherMesh1D::LocalDataCopyConcurrent(const std::vector<ThreadHandle>& threads, bool outputOnly)
{
    HCCL_INFO(
        "[InsTempAllGatherMesh1D][LocalDataCopyConcurrent] rank[%u], outputOnly[%d], remoteMemAccess[%d], "
        "symmetric[%d].",
        myRank_, outputOnly, enableRemoteMemAccess_, supportSymmetricMemory_);
    if (threads.empty()) {
        return HcclResult::HCCL_E_INTERNAL;
    }

    u32 myAlgRank;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    const u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    u64 sliceSize = tempAlgParams_.sliceSize;
    if (tempAlgParams_.tailSize != 0 && myAlgRank == templateRankSize_ - 1) {
        sliceSize = tempAlgParams_.tailSize;
    }
    u64 sliceCount = sliceSize / dataTypeSize;
    for (u32 rpt = 0; rpt < tempAlgParams_.repeatNum; ++rpt) {
        const u64 inBaseOff = tempAlgParams_.buffInfo.inBuffBaseOff + rpt * tempAlgParams_.inputRepeatStride;
        const u64 outBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
        const u64 inOff = tempAlgParams_.inputSliceStride * myAlgRank + inBaseOff;
        const u64 outOff = tempAlgParams_.outputSliceStride * myAlgRank + outBaseOff;

        DataSlice srcSlice(tempAlgParams_.buffInfo.inputPtr, inOff, sliceSize, sliceCount);
        // 非对称路径下本卡分片先暂存到CCL，输出阶段再从CCL读取。
        if (!enableRemoteMemAccess_ && !supportSymmetricMemory_) {
            const u64 scratchRepeatStride = tempAlgParams_.sliceSize * templateRankSize_;
            const u64 cclOff = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * scratchRepeatStride +
                               tempAlgParams_.sliceSize * myAlgRank;
            if (outputOnly) {
                srcSlice = DataSlice(tempAlgParams_.buffInfo.hcclBuff.addr, cclOff, sliceSize, sliceCount);
            } else {
                bool skipCclCopy =
                    (tempAlgParams_.buffInfo.inputPtr == tempAlgParams_.buffInfo.hcclBuff.addr && inOff == cclOff);
                if (!skipCclCopy) {
                    DataSlice cclDstSlice(tempAlgParams_.buffInfo.hcclBuff.addr, cclOff, sliceSize, sliceCount);
                    HCCL_DEBUG(
                        "[InsTempAllGatherMesh1D][LocalDataCopyConcurrent] RankID [%d] AlgRank [%d] copy to ccl: "
                        "cclOff[%llu] sliceSize[%llu] count[%llu].",
                        myRank_, myAlgRank, cclOff, sliceSize, sliceCount);
                    CHK_RET(LocalCopy(threads[0], srcSlice, cclDstSlice));
                }
            }
        }

        if (outputOnly) {
            bool skipOutCopy =
                (tempAlgParams_.buffInfo.inputPtr == tempAlgParams_.buffInfo.outputPtr && inOff == outOff);
            if (!skipOutCopy) {
                DataSlice dstSlice(tempAlgParams_.buffInfo.outputPtr, outOff, sliceSize, sliceCount);
                HCCL_DEBUG(
                    "[InsTempAllGatherMesh1D][LocalDataCopyConcurrent] RankID [%d] AlgRank [%d] dstSlice: "
                    "outBaseOff[%llu] outOff[%llu] sliceSize[%llu] count[%llu].",
                    myRank_, myAlgRank, outBaseOff, outOff, sliceSize, sliceCount);
                CHK_RET(LocalCopy(threads[0], srcSlice, dstSlice));
            }
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherMesh1D::PostLocalCopy(const std::vector<ThreadHandle>& threads)
{
    HCCL_INFO("[InsTempAllGatherMesh1D] PostLocalCopy.");
    if (tempAlgParams_.buffInfo.outBuffType == BufferType::HCCL_BUFFER) {
        HCCL_INFO("[InsTempAllGatherMesh1D] PostLocalCopy skip because output is scratch");
        return HcclResult::HCCL_SUCCESS;
    }
    if (tempAlgParams_.buffInfo.inBuffType == BufferType::HCCL_BUFFER) {
        HCCL_INFO("[InsTempAllGatherMesh1D] PostLocalCopy skip because input is scratch and should be read to output");
        return HcclResult::HCCL_SUCCESS;
    }
    const u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    u64 sliceSize = tempAlgParams_.sliceSize;
    for (u32 rpt = 0; rpt < tempAlgParams_.repeatNum; ++rpt) {
        const u64 outBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
        const u64 scratchRepeatStride = tempAlgParams_.sliceSize * templateRankSize_;
        const u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * scratchRepeatStride;

        for (auto rank : subCommRanks_[0]) {
            if (rank == myRank_) {
                continue;
            }
            u32 algRank = 0;
            CHK_RET(GetAlgRank(rank, subCommRanks_[0], algRank));
            // 尾块模式
            if (tempAlgParams_.tailSize != 0 && algRank == templateRankSize_ - 1) {
                sliceSize = tempAlgParams_.tailSize;
            }
            u64 scratchOffset = tempAlgParams_.sliceSize * algRank + scratchBase;
            u64 outOffset = tempAlgParams_.outputSliceStride * algRank + outBaseOff;
            u64 sliceCount = sliceSize / dataTypeSize;
            DataSlice srcSlice(tempAlgParams_.buffInfo.hcclBuff.addr, scratchOffset, sliceSize, sliceCount);
            DataSlice dstSlice(tempAlgParams_.buffInfo.outputPtr, outOffset, sliceSize, sliceCount);
            HCCL_DEBUG(
                "[InsTempAllGatherMesh1D] LocalDataCopy RankID [%d] dataRank [%d] dataAlgRank[%d] "
                "scratchBase[%d] outBaseOff[%d] scratchOffset[%d] outOffset[%d].",
                myRank_, rank, algRank, outBaseOff, outBaseOff, scratchOffset, outOffset);
            CHK_RET(LocalCopy(threads[0], srcSlice, dstSlice));
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherMesh1D::CalcDataSplitByPortGroup(
    const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
    std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset)
{
    // 只有Concurrent才按port group把单个peer的数据切分到多条jetty上。
    const u32 splitChannelNum = isConcurrent_ ? channelsPerRank_ : 1U;
    return CalcDataSplitByPortGroupCommon(
        totalDataCount, dataTypeSize, channels, elemCountOut, sizeOut, elemOffset, splitChannelNum);
}

void InsTempAllGatherMesh1D::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = GetThreadNum();
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(0);
    }
}

void InsTempAllGatherMesh1D::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = GetThreadNum();
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

} // namespace mc2_ops_hccl
