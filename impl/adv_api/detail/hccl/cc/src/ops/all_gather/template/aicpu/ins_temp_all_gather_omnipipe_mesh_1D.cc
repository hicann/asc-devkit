/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_gather_omnipipe_mesh_1D.h"
#include <sstream>
#include "alg_data_trans_wrapper.h"
#include "template_utils.h"
#include "hcomm_primitives_dl.h"

namespace mc2_ops_hccl {
// devkit 适配：GetSymWinRemoteMem 的本地封装（hccl 侧由 hccl_sym_win_dl.h 提供，devkit 侧
// hcomm_primitives_dl.h 已有 HcclSymWinGetPeerPointer 弱符号，此处直接调用）
static HcclResult GetSymWinRemoteMem(void* winHandle, size_t offset, uint32_t peerRank, void** ptr)
{
    return HcclSymWinGetPeerPointer(winHandle, offset, peerRank, ptr);
}

InsTempAllGatherOmniPipeMesh1D::InsTempAllGatherOmniPipeMesh1D(
    const OpParam& param,
    const u32 rankId, // 传通信域的rankId，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : InsTempAllGatherMesh1D(param, rankId, subCommRanks)
{}
InsTempAllGatherOmniPipeMesh1D::~InsTempAllGatherOmniPipeMesh1D() {}

HcclResult InsTempAllGatherOmniPipeMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO(
        "[InsTempAllGatherOmniPipeMesh1D][KernelRun] start Mesh all-gather template, "
        "rank[%u], symmetric[%d].",
        myRank_, param.supportSymmetricMemory);
    if (templateRankSize_ == 1) {
        HCCL_INFO(
            "[InsTempAllGatherOmniPipeMesh1D][KernelRun] skip communication for single-rank template, "
            "rank[%u].",
            myRank_);
        return HcclResult::HCCL_SUCCESS;
    }
    threadNum_ = templateResource.threads.size();
    tempAlgParams_ = tempAlgParams;
    tempAlgParams_.buffInfo.outputPtr = param.outputPtr;
    omniLastStepRead_ = tempAlgParams.omniLastStepRead_;
    dataType_ = param.DataDes.dataType;
    inputSymWindow_ = param.inputSymWindow;
    outputSymWindow_ = param.outputSymWindow;
    inputOffset_ = param.inputOffset;
    outputOffset_ = param.outputOffset;
    enableRemoteMemAccess_ = param.supportSymmetricMemory;
    HCCL_DEBUG(
        "[InsTempAllGatherOmniPipeMesh1D][KernelRun] communication threads are ready, "
        "rank[%u], threadNum[%u].",
        myRank_, threadNum_);

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }

    CHK_RET(RunAllGatherMesh(templateResource.threads, templateResource.channels));

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    HCCL_INFO("[InsTempAllGatherOmniPipeMesh1D][KernelRun] Mesh all-gather template completed, rank[%u].", myRank_);
    return HcclResult::HCCL_SUCCESS;
}

// 普通路径在 ccl scratch 间通信；对称路径直接在本端与对端的 user output 窗口间通信。
HcclResult InsTempAllGatherOmniPipeMesh1D::RunAllGatherMesh(
    const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    HCCL_INFO("[InsTempAllGatherOmniPipeMesh1D][RunAllGatherMesh] start exchanging Mesh slices, rank[%u].", myRank_);
    const u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];

    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));

    for (u32 threadIdx = 0; threadIdx < subCommRanks_[0].size() - 1; threadIdx++) {
        CHK_RET(RunMeshPeer(threads, channels, myAlgRank, threadIdx, dataTypeSize));
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherOmniPipeMesh1D::RunMeshPeer(
    const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels, u32 myAlgRank,
    u32 threadIdx, u32 dataTypeSize)
{
    const u32 connectedRank = subCommRanks_[0][(myAlgRank + 1 + threadIdx) % subCommRanks_[0].size()];
    u32 connectedAlgRank = 0;
    CHK_RET(GetAlgRank(connectedRank, subCommRanks_[0], connectedAlgRank));
    HCCL_INFO(
        "[InsTempAllGatherOmniPipeMesh1D][RunAllGatherMesh] prepare peer slice exchange, "
        "localRank[%u], remoteRank[%u], remoteAlgRank[%u].",
        myRank_, connectedRank, connectedAlgRank);
    CHK_PRT_RET(
        threadIdx >= threads.size() || (channels.count(connectedRank) == 0) || channels.at(connectedRank).empty(),
        HCCL_ERROR(
            "[InsTempAllGatherOmniPipeMesh1D][RunAllGatherMesh] thread or channel resource is "
            "missing, localRank[%u], remoteRank[%u], threadIdx[%u], threadNum[%zu], "
            "channelCount[%zu].",
            myRank_, connectedRank, threadIdx, threads.size(), channels.size()),
        HcclResult::HCCL_E_INTERNAL);

    MeshPeerSlices slices;
    const ChannelInfo& linkRemote = channels.at(connectedRank)[0];
    void* remoteOut = nullptr;
    if (enableRemoteMemAccess_) {
        CHK_RET(GetPeerSymmetricPointers(connectedRank, remoteOut));
        BuildSymmetricSlices(myAlgRank, connectedAlgRank, connectedRank, dataTypeSize, remoteOut, slices);
    } else {
        BuildScratchSlices(
            myAlgRank, connectedAlgRank, connectedRank, dataTypeSize, linkRemote.remoteCclMem.addr, slices);
    }
    return ExchangeMeshSlices(linkRemote, threads[threadIdx], connectedRank, threadIdx, slices);
}

HcclResult InsTempAllGatherOmniPipeMesh1D::GetPeerSymmetricPointers(u32 connectedRank, void*& remoteOut) const
{
    HcclResult ret = GetSymWinRemoteMem(outputSymWindow_, outputOffset_, connectedRank, &remoteOut);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS || remoteOut == nullptr,
        HCCL_ERROR(
            "[InsTempAllGatherOmniPipeMesh1D][RunAllGatherMesh] failed to get peer output "
            "pointer for data transfer, remoteRank[%u], ret[%d], ptr[%p].",
            connectedRank, ret, remoteOut),
        HcclResult::HCCL_E_INTERNAL);
    HCCL_INFO(
        "[InsTempAllGatherOmniPipeMesh1D][RunAllGatherMesh] peer symmetric pointers are ready, "
        "remoteRank[%u], outputPtr[%p].",
        connectedRank, remoteOut);
    return HcclResult::HCCL_SUCCESS;
}

void InsTempAllGatherOmniPipeMesh1D::BuildSymmetricSlices(
    u32 myAlgRank, u32 connectedAlgRank, u32 connectedRank, u32 dataTypeSize, void* const remoteOut,
    MeshPeerSlices& slices)
{
    for (u32 rpt = 0; rpt < tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank].size(); ++rpt) {
        const u64 txBaseOff = tempAlgParams_.buffInfo.inBuffBaseOff +
                              tempAlgParams_.omniReadDstStepSliceInfo.inputOmniPipeSliceStride[myAlgRank][rpt];
        const u64 rxBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff +
                              tempAlgParams_.omniReadDstStepSliceInfo.outputOmniPipeSliceStride[connectedAlgRank][rpt];
        const u64 txOffset = tempAlgParams_.omniReadDstStepSliceInfo.stepInputSliceStride[myAlgRank] + txBaseOff +
                             tempAlgParams_.processedDataCount * dataTypeSize;
        const u64 rxOffset = tempAlgParams_.omniReadDstStepSliceInfo.stepOutputSliceStride[connectedAlgRank] +
                             rxBaseOff + tempAlgParams_.processedDataCount * dataTypeSize;
        const u64 txSize = tempAlgParams_.omniReadDstStepSliceInfo.stepSliceSize[myAlgRank][rpt];
        const u64 rxSize = tempAlgParams_.omniReadDstStepSliceInfo.stepSliceSize[connectedAlgRank][rpt];
        const u64 txCount = tempAlgParams_.stepSliceInfo.stepCount[myAlgRank][rpt];
        const u64 rxSrcCount = tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt];
        const u64 rxDstCount = omniLastStepRead_ ? rxSize : rxSrcCount;
        const char* mode = omniLastStepRead_ ? "last-step-read" : "symmetric-output";
        const MeshSliceInfo txSrc{tempAlgParams_.buffInfo.outputPtr, txOffset, txSize, txCount};
        const MeshSliceInfo txDst{remoteOut, txOffset, txSize, txCount};
        const MeshSliceInfo rxSrc{remoteOut, rxOffset, rxSize, rxSrcCount};
        const MeshSliceInfo rxDst{tempAlgParams_.buffInfo.outputPtr, rxOffset, rxSize, rxDstCount};
        AppendMeshSlices(txSrc, txDst, rxSrc, rxDst, mode, connectedRank, slices);
    }
}

void InsTempAllGatherOmniPipeMesh1D::BuildScratchSlices(
    u32 myAlgRank, u32 connectedAlgRank, u32 connectedRank, u32 dataTypeSize, void* remoteCclBuffAddr,
    MeshPeerSlices& slices)
{
    for (u32 rpt = 0; rpt < tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank].size(); ++rpt) {
        if (omniLastStepRead_) {
            BuildScratchReadSlice(
                myAlgRank, connectedAlgRank, connectedRank, dataTypeSize, rpt, remoteCclBuffAddr, slices);
        } else {
            BuildScratchWriteSlice(myAlgRank, connectedAlgRank, connectedRank, rpt, remoteCclBuffAddr, slices);
        }
    }
}

void InsTempAllGatherOmniPipeMesh1D::BuildScratchWriteSlice(
    u32 myAlgRank, u32 connectedAlgRank, u32 connectedRank, u32 rpt, void* const remoteCclBuffAddr,
    MeshPeerSlices& slices)
{
    const u64 txBaseOff =
        tempAlgParams_.buffInfo.inBuffBaseOff + tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank][rpt];
    const u64 rxBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff +
                          tempAlgParams_.stepSliceInfo.outputOmniPipeSliceStride[connectedAlgRank][rpt];
    const u64 txOffset = tempAlgParams_.stepSliceInfo.stepInputSliceStride[myAlgRank] + txBaseOff;
    const u64 rxOffset = tempAlgParams_.stepSliceInfo.stepOutputSliceStride[connectedAlgRank] + rxBaseOff;
    const u64 txSize = tempAlgParams_.stepSliceInfo.stepSliceSize[myAlgRank][rpt];
    const u64 rxSize = tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt];
    const u64 txCount = tempAlgParams_.stepSliceInfo.stepCount[myAlgRank][rpt];
    const MeshSliceInfo txSrc{tempAlgParams_.buffInfo.hcclBuff.addr, txOffset, txSize, txCount};
    const MeshSliceInfo txDst{remoteCclBuffAddr, txOffset, txSize, txCount};
    const MeshSliceInfo rxSrc{remoteCclBuffAddr, rxOffset, rxSize, rxSize};
    const MeshSliceInfo rxDst{tempAlgParams_.buffInfo.hcclBuff.addr, rxOffset, rxSize, rxSize};
    AppendMeshSlices(txSrc, txDst, rxSrc, rxDst, "scratch-write", connectedRank, slices);
}

void InsTempAllGatherOmniPipeMesh1D::BuildScratchReadSlice(
    u32 myAlgRank, u32 connectedAlgRank, u32 connectedRank, u32 dataTypeSize, u32 rpt, void* const remoteCclBuffAddr,
    MeshPeerSlices& slices)
{
    const u64 txScratchBase =
        tempAlgParams_.buffInfo.inBuffBaseOff + tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank][rpt];
    const u64 rxScratchBase = tempAlgParams_.buffInfo.outBuffBaseOff +
                              tempAlgParams_.stepSliceInfo.outputOmniPipeSliceStride[connectedAlgRank][rpt];
    const u64 txScratchOff = tempAlgParams_.stepSliceInfo.stepInputSliceStride[myAlgRank] + txScratchBase;
    const u64 rxScratchOff = tempAlgParams_.stepSliceInfo.stepOutputSliceStride[connectedAlgRank] + rxScratchBase;
    const u64 txOutBase = tempAlgParams_.buffInfo.inBuffBaseOff +
                          tempAlgParams_.omniReadDstStepSliceInfo.inputOmniPipeSliceStride[myAlgRank][rpt];
    const u64 rxOutBase = tempAlgParams_.buffInfo.outBuffBaseOff +
                          tempAlgParams_.omniReadDstStepSliceInfo.outputOmniPipeSliceStride[connectedAlgRank][rpt];
    const u64 txOutOff = tempAlgParams_.omniReadDstStepSliceInfo.stepInputSliceStride[myAlgRank] + txOutBase +
                         tempAlgParams_.processedDataCount * dataTypeSize;
    const u64 rxOutOff = tempAlgParams_.omniReadDstStepSliceInfo.stepOutputSliceStride[connectedAlgRank] + rxOutBase +
                         tempAlgParams_.processedDataCount * dataTypeSize;
    const u64 txScratchSize = tempAlgParams_.stepSliceInfo.stepSliceSize[myAlgRank][rpt];
    const u64 rxScratchSize = tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt];
    const u64 txOutSize = tempAlgParams_.omniReadDstStepSliceInfo.stepSliceSize[myAlgRank][rpt];
    const u64 rxOutSize = tempAlgParams_.omniReadDstStepSliceInfo.stepSliceSize[connectedAlgRank][rpt];
    const u64 txCount = tempAlgParams_.stepSliceInfo.stepCount[myAlgRank][rpt];
    const MeshSliceInfo txSrc{tempAlgParams_.buffInfo.outputPtr, txOutOff, txOutSize, txCount};
    const MeshSliceInfo txDst{remoteCclBuffAddr, txScratchOff, txScratchSize, txCount};
    const MeshSliceInfo rxSrc{remoteCclBuffAddr, rxScratchOff, rxScratchSize, rxScratchSize};
    const MeshSliceInfo rxDst{tempAlgParams_.buffInfo.outputPtr, rxOutOff, rxOutSize, rxOutSize};
    AppendMeshSlices(txSrc, txDst, rxSrc, rxDst, "last-step-read", connectedRank, slices);
}

void InsTempAllGatherOmniPipeMesh1D::AppendMeshSlices(
    const MeshSliceInfo& txSrc, const MeshSliceInfo& txDst, const MeshSliceInfo& rxSrc, const MeshSliceInfo& rxDst,
    const char* mode, u32 connectedRank, MeshPeerSlices& slices)
{
    slices.rxSrcSlices_.emplace_back(rxSrc.addr_, rxSrc.offset_, rxSrc.size_, rxSrc.count_);
    slices.rxDstSlices_.emplace_back(rxDst.addr_, rxDst.offset_, rxDst.size_, rxDst.count_);
    slices.txSrcSlices_.emplace_back(txSrc.addr_, txSrc.offset_, txSrc.size_, txSrc.count_);
    slices.txDstSlices_.emplace_back(txDst.addr_, txDst.offset_, txDst.size_, txDst.count_);
    LogMeshSlice("send source", mode, connectedRank, txSrc);
    LogMeshSlice("send destination", mode, connectedRank, txDst);
    LogMeshSlice("receive source", mode, connectedRank, rxSrc);
    LogMeshSlice("receive destination", mode, connectedRank, rxDst);
}

void InsTempAllGatherOmniPipeMesh1D::LogMeshSlice(
    const char* sliceName, const char* mode, u32 connectedRank, const MeshSliceInfo& slice) const
{
    HCCL_DEBUG(
        "[InsTempAllGatherOmniPipeMesh1D][RunAllGatherMesh] build %s slice, "
        "mode[%s], localRank[%u], remoteRank[%u], offset[%llu], sliceSize[%llu], count[%llu].",
        sliceName, mode, myRank_, connectedRank, slice.offset_, slice.size_, slice.count_);
}

HcclResult InsTempAllGatherOmniPipeMesh1D::ExchangeMeshSlices(
    const ChannelInfo& linkRemote, const ThreadHandle& thread, u32 connectedRank, u32 threadIdx,
    const MeshPeerSlices& slices) const
{
    const TxRxSlicesList sendRecvSlicesList(
        {slices.txSrcSlices_, slices.txDstSlices_}, {slices.rxSrcSlices_, slices.rxDstSlices_});
    const TxRxChannels sendRecvChannels(linkRemote, linkRemote);
    const SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList);
    if (!omniLastStepRead_) {
        CHK_PRT_RET(
            SendRecvWrite(sendRecvInfo, thread),
            HCCL_ERROR(
                "[InsTempAllGatherOmniPipeMesh1D][RunAllGatherMesh] write communication "
                "failed, localRank[%u], remoteRank[%u], threadIdx[%u].",
                myRank_, connectedRank, threadIdx),
            HcclResult::HCCL_E_INTERNAL);
    } else {
        CHK_PRT_RET(
            SendRecvRead(sendRecvInfo, thread),
            HCCL_ERROR(
                "[InsTempAllGatherOmniPipeMesh1D][RunAllGatherMesh] last-step read "
                "communication failed, localRank[%u], remoteRank[%u], threadIdx[%u].",
                myRank_, connectedRank, threadIdx),
            HcclResult::HCCL_E_INTERNAL);
    }
    return HcclResult::HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
