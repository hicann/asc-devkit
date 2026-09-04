/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <algorithm>
#include <stdexcept>

#include "ins_temp_all_reduce_aicpu_reduce_nhr.h"
#include "alg_data_trans_wrapper.h"
#include "template_utils.h"

namespace mc2_ops_hccl {
InsTempAllReduceAicpuReduceNHR::InsTempAllReduceAicpuReduceNHR(
    const OpParam& param,
    const u32 rankId, // 传通信域的rankId，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

u64 InsTempAllReduceAicpuReduceNHR::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    u64 scratchMultiple = templateRankSize_;
    return scratchMultiple;
}

InsTempAllReduceAicpuReduceNHR::~InsTempAllReduceAicpuReduceNHR() {}

HcclResult InsTempAllReduceAicpuReduceNHR::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    u32 threadNum = 1;
    resourceRequest.slaveThreadNum = threadNum - 1;
    resourceRequest.notifyNumPerThread = {};
    resourceRequest.notifyNumOnMainThread = threadNum - 1;

    std::vector<HcclChannelDesc> level1Channels;
    CHK_RET(CalcChannelRequestNhr(comm, param, topoInfo, subCommRanks_, level1Channels));
    resourceRequest.channels.push_back(level1Channels);
    HCCL_INFO(
        "[InsTempAllReduceAicpuReduceNHR][CalcRes]slaveThreadNum[%u] notifyNumPerThread[%u] notifyNumOnMainThread[%u]"
        " level1Channels[%u] .",
        resourceRequest.slaveThreadNum, resourceRequest.notifyNumPerThread.size(),
        resourceRequest.notifyNumOnMainThread, level1Channels.size());
    return HCCL_SUCCESS;
}

u64 InsTempAllReduceAicpuReduceNHR::GetThreadNum() const { return 1; }

void InsTempAllReduceAicpuReduceNHR::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    (void)notifyIdxMainToSub;
}

void InsTempAllReduceAicpuReduceNHR::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    (void)notifyIdxSubToMain;
}

HcclResult InsTempAllReduceAicpuReduceNHR::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllReduceAicpuReduceNHR] rank[%d] KernelRun start", myRank_);
    // 处理数据量为0的场景
    CHK_PRT_RET(
        tempAlgParams.sliceSize == 0, HCCL_INFO("[InsTempAllReduceAicpuReduceNHR] sliceSize is 0, no need to process"),
        HCCL_SUCCESS);

    CHK_PRT_RET(
        templateRankSize_ == 0, HCCL_ERROR("[InsTempAllReduceAicpuReduceNHR] rankSize is 0"),
        HcclResult::HCCL_E_INTERNAL);
    CHK_PRT_RET(
        subCommRanks_.size() == 0, HCCL_ERROR("[InsTempAllReduceAicpuReduceNHR] subCommRanks_ is empty"),
        HcclResult::HCCL_E_INTERNAL);

    thread_ = templateResource.threads.at(0);
    buffInfo_ = tempAlgParams.buffInfo;

    myIdx_ = GetAlgRank(myRank_);
    CHK_PRT_RET(
        myIdx_ >= templateRankSize_,
        HCCL_ERROR(
            "[InsTempAllReduceAicpuReduceNHR] rank idx[%u] in virtRankMap is invalid, it should be less than "
            "rankSize[%u]",
            myIdx_, templateRankSize_),
        HcclResult::HCCL_E_INTERNAL);
    processSize_ = tempAlgParams.sliceSize;
    count_ = tempAlgParams.count;
    dataType_ = param.DataDes.dataType;
    HCCL_INFO(
        "[KernelRun] sliceSize: %u, count_: %u, typeSize: %u", tempAlgParams.sliceSize, count_,
        DATATYPE_SIZE_TABLE[dataType_]);

    const std::map<u32, std::vector<ChannelInfo>>& channels = templateResource.channels;
    HCCL_DEBUG(
        "[Kernel Run] myRank_[%u], channels.size():%u, channels first[%u]", myRank_, channels.size(),
        channels.begin()->first);

    bool isPcieProtocal = IsPcieProtocol(channels); // 判断是否存在pcie链路
    isDmaRead_ = isPcieProtocal;                    // 是否使用Read模式
    HCCL_DEBUG("[InsTempAllReduceAicpuReduceNHR] Use Dma Read[%d]", isDmaRead_);

    // 1. 切片
    CHK_RET(CalcSlice(tempAlgParams.sliceSize));

    // 2. PreCopy (OPBASE 模式下将 userIn -> ccl)
    CHK_RET(PreCopy(tempAlgParams));

    // 3. AllGather 阶段
    CHK_RET(RunGather(channels));

    // 4. LocalReduce 阶段
    CHK_RET(RunReduce(channels, tempAlgParams, param.algTag));

    HCCL_INFO("[InsTempAllReduceAicpuReduceNHR] rank[%d] KernelRun finished", myRank_);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceAicpuReduceNHR::CalcSlice(u64 chunkSize)
{
    HCCL_INFO("[InsTempAllReduceAicpuReduceNHR] rank[%d] CalcSlice start", chunkSize);
    // 按 rank 切分数据
    sliceInfoVec_ = RankSliceInfo(templateRankSize_);

    u64 unitAlignSize = DATATYPE_SIZE_TABLE[dataType_];
    u64 dataSize = chunkSize * templateRankSize_;

    u64 accumOff = 0;
    for (u32 rankIdx = 0; rankIdx < templateRankSize_; rankIdx++) {
        u64 currChunkSize = std::min<u64>(dataSize - accumOff, chunkSize);
        sliceInfoVec_[rankIdx].emplace_back(SliceInfo{accumOff, currChunkSize});
        HCCL_DEBUG(
            "[InsTempAllReduceAicpuReduceNHR] rankIdx [%d] CalcSlice accumOff[%u], currChunkSize[%u]", rankIdx,
            accumOff, currChunkSize);
        accumOff += currChunkSize;
    }

    CHK_PRT_RET(
        (sliceInfoVec_[templateRankSize_ - 1][0].offset + sliceInfoVec_[templateRankSize_ - 1][0].size != dataSize),
        HCCL_ERROR(
            "[InsTempAllReduceAicpuReduceNHR] chunkSize:[%llu], Rank:[%d], SliceInfo calculation error!", chunkSize,
            myRank_),
        HcclResult::HCCL_E_INTERNAL);

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceAicpuReduceNHR::PreCopy(const TemplateDataParams& tempAlgParams)
{
    HCCL_INFO("[InsTempAllReduceAicpuReduceNHR][PreCopy] start");
    const BuffInfo& buffInfo = tempAlgParams.buffInfo;
    const u64 sliceSize = tempAlgParams.sliceSize;

    const DataSlice srcSlice(buffInfo.inputPtr, buffInfo.inBuffBaseOff, sliceSize);
    const DataSlice dstSlice(buffInfo.hcclBuff.addr, buffInfo.hcclBuffBaseOff + myIdx_ * sliceSize, sliceSize);
    CHK_RET(static_cast<HcclResult>(LocalCopy(thread_, srcSlice, dstSlice)));

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceAicpuReduceNHR::RunGather(const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    HCCL_INFO("[InsTempAllReduceAicpuReduceNHR][RunGather] start");
    u64 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];

    u32 nSteps = GetNHRStepNum(templateRankSize_);
    for (u32 step = 0; step < nSteps; step++) {
        AicpuNHRStepInfo stepInfo;
        CHK_RET(GetStepInfo(step, nSteps, stepInfo));

        const ChannelInfo& channelRecv = channels.at(subCommRanks_.at(0).at(stepInfo.fromRank)).at(0);
        const ChannelInfo& channelSend = channels.at(subCommRanks_.at(0).at(stepInfo.toRank)).at(0);

        std::vector<DataSlice> txSrcSlices;
        std::vector<DataSlice> txDstSlices;
        std::vector<DataSlice> rxSrcSlices;
        std::vector<DataSlice> rxDstSlices;
        for (u32 i = 0; i < stepInfo.nSlices; i++) {
            u64 txOffset = sliceInfoVec_[stepInfo.txSliceIdxs[i]][0].offset + buffInfo_.hcclBuffBaseOff;
            u64 txSize = sliceInfoVec_[stepInfo.txSliceIdxs[i]][0].size;
            u64 rxOffset = sliceInfoVec_[stepInfo.rxSliceIdxs[i]][0].offset + buffInfo_.hcclBuffBaseOff;
            u64 rxSize = sliceInfoVec_[stepInfo.rxSliceIdxs[i]][0].size;

            txSrcSlices.push_back(DataSlice(buffInfo_.hcclBuff.addr, txOffset, txSize, txSize / dataTypeSize));
            txDstSlices.push_back(DataSlice(channelSend.remoteCclMem.addr, txOffset, txSize, txSize / dataTypeSize));
            rxSrcSlices.push_back(DataSlice(channelRecv.remoteCclMem.addr, rxOffset, rxSize, rxSize / dataTypeSize));
            rxDstSlices.push_back(DataSlice(buffInfo_.hcclBuff.addr, rxOffset, rxSize, rxSize / dataTypeSize));
        }

        TxRxChannels sendRecvChannels(channelSend, channelRecv);
        TxRxSlicesList sendRecvSlicesList({txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices});

        SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList);
        if (isDmaRead_) {
            CHK_PRT_RET(
                SendRecvRead(sendRecvInfo, thread_),
                HCCL_ERROR("[InsTempAllReduceAicpuReduceNHR] RunGather send/recv failed"), HcclResult::HCCL_E_INTERNAL);
        } else {
            CHK_PRT_RET(
                SendRecvWrite(sendRecvInfo, thread_),
                HCCL_ERROR("[InsTempAllReduceAicpuReduceNHR] RunGather send/recv failed"), HcclResult::HCCL_E_INTERNAL);
        }
    }

    HCCL_INFO("[InsTempAllReduceAicpuReduceNHR][RunGather] end");
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceAicpuReduceNHR::RunReduce(
    const std::map<u32, std::vector<ChannelInfo>>& channels, const TemplateDataParams& tempAlgParams,
    const std::string& algTag)
{
    HCCL_INFO("[InsTempAllReduceAicpuReduceNHR][RunReduce] start");
    const BuffInfo& buffInfo = tempAlgParams.buffInfo;
    const u64 sliceSize = tempAlgParams.sliceSize;
    const DataSlice srcSlice(buffInfo.hcclBuff.addr, buffInfo.hcclBuffBaseOff + myIdx_ * sliceSize, sliceSize, count_);
    const DataSlice dstSlice(buffInfo.outputPtr, buffInfo.outBuffBaseOff, sliceSize, count_);
    CHK_RET(static_cast<HcclResult>(LocalCopy(thread_, srcSlice, dstSlice)));

    CHK_RET(static_cast<HcclResult>(HcommBatchModeEnd(algTag.c_str())));
    CHK_RET(static_cast<HcclResult>(HcommBatchModeStart(algTag.c_str())));
    CHK_RET(static_cast<HcclResult>(HcommThreadJoin(thread_, CUSTOM_TIMEOUT)));

    for (u32 idx = 0; idx < subCommRanks_.at(0).size(); ++idx) {
        if (idx == myRank_) {
            continue;
        }
        const DataSlice srcSlice(buffInfo.hcclBuff.addr, buffInfo.hcclBuffBaseOff + sliceSize * idx, sliceSize, count_);

        CHK_RET(static_cast<HcclResult>(LocalReduce(thread_, srcSlice, dstSlice, dataType_, reduceOp_)));
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceAicpuReduceNHR::GetStepInfo(u32 step, u32 nSteps, AicpuNHRStepInfo& stepInfo)
{
    u32 rankIdx = myIdx_;
    stepInfo.txSliceIdxs.clear();
    stepInfo.rxSliceIdxs.clear();
    stepInfo.step = step;
    stepInfo.myRank = rankIdx;

    // AicpuReduceNHR 计算通信对象
    u32 deltaRank = 1 << (nSteps - 1 - step);
    u32 recvFrom = (rankIdx + templateRankSize_ - deltaRank) % templateRankSize_;
    u32 sendTo = (rankIdx + deltaRank) % templateRankSize_;

    // AicpuReduceNHR 数据份数和数据编号增量
    u32 nSlices = (templateRankSize_ - 1 + (1 << (nSteps - 1 - step))) / (1 << (nSteps - step));
    u32 deltaSliceIndex = 1 << (nSteps - step);
    u32 txSliceIdx = rankIdx;
    u32 rxSliceIdx = (rankIdx - (1 << (nSteps - 1 - step)) + templateRankSize_) % templateRankSize_;

    stepInfo.nSlices = nSlices;
    stepInfo.toRank = sendTo;
    stepInfo.fromRank = recvFrom;

    for (u32 i = 0; i < nSlices; i++) {
        stepInfo.txSliceIdxs.push_back(txSliceIdx);
        stepInfo.rxSliceIdxs.push_back(rxSliceIdx);

        HCCL_DEBUG(
            "[InsTempAllReduceAicpuReduceNHR][GetStepInfo] i[%u] txSliceIdx[%u] rxSliceIdx[%u]", i, txSliceIdx,
            rxSliceIdx);

        txSliceIdx = (txSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
        rxSliceIdx = (rxSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
    }
    return HcclResult::HCCL_SUCCESS;
}

u32 InsTempAllReduceAicpuReduceNHR::GetAlgRank(u32 rank) const
{
    auto iter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), rank);
    if (iter == subCommRanks_[0].end()) {
        throw std::runtime_error("Cannot find Rank = " + std::to_string(rank) + " in subCommRanks_[0]");
    }
    return static_cast<u32>(std::distance(subCommRanks_[0].begin(), iter));
}

} // namespace mc2_ops_hccl
