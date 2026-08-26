/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_reduce_scatter_omnipipe_mesh_1D.h"

namespace mc2_ops_hccl {
InsTempReduceScatterOmniPipeMesh1D::InsTempReduceScatterOmniPipeMesh1D(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempReduceScatterOmniPipeMesh1D::~InsTempReduceScatterOmniPipeMesh1D() {}

HcclResult InsTempReduceScatterOmniPipeMesh1D::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    resourceRequest.slaveThreadNum = threadNum - 1;
    for (u32 index = 0; index < threadNum - 1; index++) {
        resourceRequest.notifyNumPerThread.push_back(1);
    }
    resourceRequest.notifyNumOnMainThread = threadNum - 1;

    std::vector<HcclChannelDesc> level0Channels;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, level0Channels));
    HCCL_INFO("InsTempReduceScatterOmniPipeMesh1D--CalcRes],level0Channels.size()=[%u]", level0Channels.size());
    resourceRequest.channels.push_back(level0Channels);
    HCCL_WARNING("Resource calculation is temporarily not performed in the template.");
    return HCCL_SUCCESS;
}

u64 InsTempReduceScatterOmniPipeMesh1D::GetThreadNum() const
{
    return templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
}

HcclResult InsTempReduceScatterOmniPipeMesh1D::GetRes(AlgResourceRequest& resourceRequest) const
{
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    resourceRequest.slaveThreadNum = threadNum - 1;
    for (u32 index = 0; index < threadNum - 1; index++) {
        resourceRequest.notifyNumPerThread.push_back(1);
    }
    resourceRequest.notifyNumOnMainThread = threadNum - 1;
    return HCCL_SUCCESS;
}

// 语义改为返回当前template的类型，mesh返回1，nhr返回0
u64 InsTempReduceScatterOmniPipeMesh1D::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) { return 1; }

// 这个也不用，计算scratch、对齐、loop信息封装在雪松接口里
u64 InsTempReduceScatterOmniPipeMesh1D::CalcScratchSlice(u64 dataSize) const
{
    // mesh直接乘rankSize
    u64 scratchMultiple = templateRankSize_ * dataSize;
    return scratchMultiple;
}

void InsTempReduceScatterOmniPipeMesh1D::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(0);
    }
}

void InsTempReduceScatterOmniPipeMesh1D::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

HcclResult InsTempReduceScatterOmniPipeMesh1D::DoLocalCopy(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads)
{
    HCCL_INFO("[InsTempReduceScatterOmniPipeMesh1D][DoLocalCopy] DoLocalCopy myRank_ = [%u]", myRank_);
    if (tempAlgParams.sliceSize == 0) {
        HCCL_INFO("Rank [%d], get slicesize zero. skip localcopy", myRank_);
        return HcclResult::HCCL_SUCCESS;
    }
    u32 rankIdx = 0;
    auto iter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    if (iter != subCommRanks_[0].end()) {
        rankIdx = std::distance(subCommRanks_[0].begin(), iter);
    } else {
        HCCL_ERROR("[%s]subCommRanks_ or myRank_ is error.", __func__);
        return HCCL_E_INTERNAL;
    }

    // 区分前后搬运
    void* srcAddr;
    void* dstAddr;
    if (tempAlgParams.buffInfo.inBuffType == BufferType::INPUT) {
        // 头拷贝
        srcAddr = tempAlgParams.buffInfo.inputPtr;
        dstAddr = tempAlgParams.buffInfo.hcclBuff.addr;
    } else if (tempAlgParams.buffInfo.inBuffType == BufferType::HCCL_BUFFER) {
        // 尾拷贝
        srcAddr = tempAlgParams.buffInfo.hcclBuff.addr;
        dstAddr = tempAlgParams.buffInfo.outputPtr;
    } else {
        HCCL_ERROR("[%s]InputBufferType Error.", __func__);
        return HCCL_E_PARA;
    }
    for (auto i = 0; i < tempAlgParams.repeatNum; ++i) {
        // sliceSize，count按照rank从0~i的顺序给
        auto srcSlice = DataSlice(
            srcAddr, tempAlgParams.buffInfo.inBuffBaseOff + i * tempAlgParams.inputSliceStride, tempAlgParams.sliceSize,
            tempAlgParams.count);
        auto dstSlice = DataSlice(
            dstAddr, tempAlgParams.buffInfo.outBuffBaseOff + i * tempAlgParams.outputSliceStride,
            tempAlgParams.sliceSize, tempAlgParams.count);
        HCCL_INFO(
            "myRank[%u], i[%u], srcSlice:%s, dstSlice:%s", myRank_, i, srcSlice.Describe().c_str(),
            dstSlice.Describe().c_str());
        CHK_RET(static_cast<HcclResult>(LocalCopy(threads[0], srcSlice, dstSlice)));
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterOmniPipeMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    if (templateRankSize_ == 1) {
        HCCL_INFO("templateRankSize_ ==1");
        return HcclResult::HCCL_SUCCESS;
    }
    threadNum_ = templateResource.threads.size();
    dataType_ = param.DataDes.dataType;
    HCCL_INFO("[%s]Run Start", __func__);
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
    // 这个PostReduce处理的是当前轴的规约任务
    CHK_RET(PostReduce(tempAlgParams, templateResource.threads));
    HCCL_INFO("[%s]Run End", __func__);
    return HcclResult::HCCL_SUCCESS;
}

// 通信结束之后，数据都在 cclBuffer 上，需要搬运到对应的输出位置（斜对角算法仍然是规约到ccl上某片位置）
HcclResult InsTempReduceScatterOmniPipeMesh1D::PostReduce(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads)
{
    u32 rankIdx = 0;
    auto iter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    if (iter != subCommRanks_[0].end()) {
        rankIdx = std::distance(subCommRanks_[0].begin(), iter);
    } else {
        HCCL_ERROR("[InsTempReduceScatterOmniPipeMesh1D][RunReduceScatter] subCommRanks_ or myRank_ is error.");
        return HCCL_E_INTERNAL;
    }

    HCCL_INFO("[InsTempReduceScatterOmniPipeMesh1D][PostReduce], copy from cclBuffer to cclBuffer");
    // 本卡的数据在executor中，loop刚开始时就完成了localcopy
    // 地址指针全用ccl的
    void* cclBuffAddr = tempAlgParams.buffInfo.hcclBuff.addr;
    HCCL_INFO("[InsTempReduceScatterOmniPipeMesh1D][PostReduce]do first slice reduce.");
    // 从param中ccl的地址往param中out的地址规约，
    for (u32 repeatIdx = 0; repeatIdx < tempAlgParams.stepSliceInfo.outputOmniPipeSliceStride[rankIdx].size();
         repeatIdx++) {
        for (u32 tmpRank = 0; tmpRank < templateRankSize_; tmpRank++) {
            if (tmpRank != rankIdx) {
                u64 srcCurrent = tempAlgParams.buffInfo.hcclBuffBaseOff +
                                 tempAlgParams.stepSliceInfo.stepOutputSliceStride[tmpRank] +
                                 tempAlgParams.stepSliceInfo.outputOmniPipeSliceStride[tmpRank][repeatIdx];
                u64 dstCurrent = tempAlgParams.buffInfo.outBuffBaseOff +
                                 tempAlgParams.stepSliceInfo.stepInputSliceStride[rankIdx] +
                                 tempAlgParams.stepSliceInfo.inputOmniPipeSliceStride[rankIdx][repeatIdx];
                auto srcSlice = DataSlice(
                    cclBuffAddr, srcCurrent, tempAlgParams.stepSliceInfo.stepSliceSize[rankIdx][repeatIdx],
                    tempAlgParams.stepSliceInfo.stepCount[rankIdx][repeatIdx]);
                auto dstSlice = DataSlice(
                    cclBuffAddr, dstCurrent, tempAlgParams.stepSliceInfo.stepSliceSize[rankIdx][repeatIdx],
                    tempAlgParams.stepSliceInfo.stepCount[rankIdx][repeatIdx]);
                HCCL_DEBUG(
                    "srcSlice=[%s], dstSlice=[%s], tmpRank=[%u], rankIdx=[%u], repeatIdx=[%u]",
                    srcSlice.Describe().c_str(), dstSlice.Describe().c_str(), tmpRank, rankIdx, repeatIdx);
                CHK_RET(static_cast<HcclResult>(LocalReduce(threads[0], srcSlice, dstSlice, dataType_, reduceOp_)));
            }
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

// 进行数据从ccl搬运到ccl。不reduce，只涉及receive/write
HcclResult InsTempReduceScatterOmniPipeMesh1D::RunReduceScatter(
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
    const TemplateDataParams& tempAlgParam)
{
    HCCL_INFO("MT start to RunReduceScatter, channels.size()=%u", channels.size());
    u32 myAlgRank = 0;
    auto iter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    if (iter != subCommRanks_[0].end()) {
        myAlgRank = std::distance(subCommRanks_[0].begin(), iter);
    } else {
        HCCL_ERROR("[InsTempReduceScatterOmniPipeMesh1D][RunReduceScatter] subCommRanks_ or myRank_ is error.");
        return HCCL_E_INTERNAL;
    }
    HCCL_DEBUG("MT threadNum_=%u, myAlgRank=%u", threadNum_, myAlgRank);
    for (u32 queIdx = 0; queIdx < threadNum_; queIdx++) {
        u32 nextRank =
            (myAlgRank + 1 + queIdx) % templateRankSize_; // 这里取的虚拟rankId , z轴的时候templateRankSize_=2
        u32 remoteRank = subCommRanks_[0][nextRank];

        HCCL_DEBUG(
            "[InsTempReduceScatterOmniPipeMesh1D][RunReduceScatter] myRank[%d], remoteRank[%d]", myRank_, remoteRank);
        const ChannelInfo& linkRemote = channels.at(remoteRank)[0];
        std::vector<DataSlice> txSrcSlices;
        std::vector<DataSlice> txDstSlices;
        std::vector<DataSlice> rxSrcSlices;
        std::vector<DataSlice> rxDstSlices;
        void* localCclBuffAddr = tempAlgParam.buffInfo.hcclBuff.addr;
        void* remoteCclBuffAddr = linkRemote.remoteCclMem.addr;
        // 按照偏移数组边计算位置
        for (u32 repeatIdx = 0; repeatIdx < tempAlgParam.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank].size();
             repeatIdx++) {
            u64 txSrcCurrent = tempAlgParam.buffInfo.inBuffBaseOff +
                               tempAlgParam.stepSliceInfo.stepInputSliceStride[nextRank] +
                               tempAlgParam.stepSliceInfo.inputOmniPipeSliceStride[nextRank][repeatIdx];
            u64 txDstCurrent = tempAlgParam.buffInfo.hcclBuffBaseOff +
                               tempAlgParam.stepSliceInfo.stepOutputSliceStride[myAlgRank] +
                               tempAlgParam.stepSliceInfo.outputOmniPipeSliceStride[myAlgRank][repeatIdx];

            u64 rxSrcCurrent = tempAlgParam.buffInfo.inBuffBaseOff +
                               tempAlgParam.stepSliceInfo.stepInputSliceStride[myAlgRank] +
                               tempAlgParam.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank][repeatIdx];
            u64 rxDstCurrent = tempAlgParam.buffInfo.hcclBuffBaseOff +
                               tempAlgParam.stepSliceInfo.stepOutputSliceStride[nextRank] +
                               tempAlgParam.stepSliceInfo.outputOmniPipeSliceStride[nextRank][repeatIdx];
            // 换成数组
            DataSlice txSrcSlice = DataSlice(
                localCclBuffAddr, txSrcCurrent, tempAlgParam.stepSliceInfo.stepSliceSize[nextRank][repeatIdx],
                tempAlgParam.stepSliceInfo.stepCount[nextRank][repeatIdx]); // 发送源
            DataSlice txDstSlice = DataSlice(
                remoteCclBuffAddr, txDstCurrent, tempAlgParam.stepSliceInfo.stepSliceSize[nextRank][repeatIdx],
                tempAlgParam.stepSliceInfo.stepCount[nextRank][repeatIdx]); // 发送目标
            DataSlice rxSrcSlice = DataSlice(
                remoteCclBuffAddr, rxSrcCurrent, tempAlgParam.stepSliceInfo.stepSliceSize[myAlgRank][repeatIdx],
                tempAlgParam.stepSliceInfo.stepCount[myAlgRank][repeatIdx]); // 接收源
            DataSlice rxDstSlice = DataSlice(
                localCclBuffAddr, rxDstCurrent, tempAlgParam.stepSliceInfo.stepSliceSize[myAlgRank][repeatIdx],
                tempAlgParam.stepSliceInfo.stepCount[myAlgRank][repeatIdx]); // 接收目标
            rxSrcSlices.push_back(rxSrcSlice);
            rxDstSlices.push_back(rxDstSlice);
            txSrcSlices.push_back(txSrcSlice);
            txDstSlices.push_back(txDstSlice);
        }
        SendRecvInfo sendRecvInfo{{linkRemote, linkRemote}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}};

        CHK_PRT_RET(
            SendRecvWrite(sendRecvInfo, threads[queIdx]),
            HCCL_ERROR("[InsTempReduceScatterOmniPipeMesh1D] RunReduceScatter Send failed"),
            HcclResult::HCCL_E_INTERNAL);
    }
    return HcclResult::HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
