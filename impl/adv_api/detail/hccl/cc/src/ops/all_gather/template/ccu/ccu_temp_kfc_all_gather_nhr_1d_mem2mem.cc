/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// 与 hccl src/ops/all_gather/algorithm/template/ccu/ccu_temp_all_gather_nhr_1D_mem2mem.cc 逐行对照移植。
// hccl -> KFC 适配：
//  1. CalcRes 按 dieNum(1-2) 建 kernelInfo（D4，RS SoleNHR 2Die 先例）：GetDieInfoFromChannelDescs
//     探测 die 数、链路按 die 分组（channelsPerDie，SelectChannelToVec）、GetChannelBwCoeff 带宽比
//     切分（dieSplitRatio）；KFC 下每 kernel 映射一个 mission（CcuTempKfcServer::CalcRes 按尾部
//     源 kernel 数修正 mission 数，op_common.cc 再生 kfcServerArgs）。
//  2. 通道走 CalcChannelRequestNhr（hccl 非 MESH_1D_CLOS 路径）；rank2ChannelIdx 以 sub-rank 为键，
//     每 peer 每 die 保留一条链路（SelectChannelToVec，RS 2die 同构）。
//  3. KernelRun 为空壳：每轮参数由 AIV 侧 CcuPrepareForAllGatherSoleNhrM2M 写入 xnData 队列。
//  4. GetStepInfo 的 toRank/fromRank/nSlices/tx/rx 公式与 hccl 逐行一致（sub-rank 口径）。
#include "alg_data_trans_wrapper.h"
#include "channel.h"
#include "ccu_temp_kfc_all_gather_nhr_1d_mem2mem.h"

namespace mc2_ops_hccl {

namespace {
constexpr u32 DIE_NUM_1 = 1;
constexpr u32 DIE_NUM_2 = 2;
} // namespace

CcuTempKfcAllGatherNHR1DMem2Mem::CcuTempKfcAllGatherNHR1DMem2Mem(
    const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuAlgTemplateBase(param, rankId, subCommRanks)
{
    const auto& ranks = subCommRanks_[0];
    const auto it = std::find(ranks.begin(), ranks.end(), rankId);
    if (it != ranks.end()) {
        mySubCommRank_ = static_cast<uint32_t>(std::distance(ranks.begin(), it));
    }
    tempRankSize_ = static_cast<uint32_t>(ranks.size());
}

HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));

    // 与 hccl 一致：通道走两级拓扑 Nhr 计算（非 UBX 的 NhrMultiJetty）
    std::vector<HcclChannelDesc> channelDescs;
    CHK_RET(CalcChannelRequestNhr(comm, param, topoInfo, subCommRanks_, channelDescs));
    CHK_RET(RestoreChannelMap(channelDescs, rankIdToChannelDesc_));

    // 1. 从 channelDesc 探测 die 数（RS SoleNHR 2Die 先例）
    uint32_t dieNum = 0;
    uint32_t enableDieId = 0;
    CHK_RET(GetDieInfoFromChannelDescs(comm, rankIdToChannelDesc_, myRank_, dieNum, enableDieId));
    CHK_PRT_RET(
        dieNum < DIE_NUM_1 || dieNum > DIE_NUM_2,
        HCCL_ERROR("[CcuTempKfcAllGatherNHR1DMem2Mem::CalcRes] get channelDescs fail, dieNum[%u]", dieNum),
        HCCL_E_INTERNAL);

    // 双 die 时每个 die 一个 kernel 实例（KFC 下映射为双 mission，由 kfc server 模板继承调度）
    uint32_t kernelNum = dieNum;
    resourceRequest.ccuKernelNum.push_back(kernelNum);

    // 2. 将 channelDescs 分到 die 组（每 step 的 from/to peer 各 die 选一条链路，
    //    rank2ChannelIdx 保持 peer → 该 die 链路组内索引）
    std::vector<std::vector<HcclChannelDesc>> channelsPerDie;
    channelsPerDie.resize(dieNum);
    std::map<u32, u32> rank2ChannelIdx;
    std::vector<KfcNhrStepInfo> stepInfoVector;
    CHK_RET(ProcessNHRStepInfo(comm, stepInfoVector, rank2ChannelIdx, dieNum, enableDieId, channelsPerDie));
    if (dieNum > DIE_NUM_1) { // 跨框 die0 连 die1 场景端口序修正
        CHK_RET(ReverseChannelPerDieIfNeed(comm, myRank_, channelsPerDie));
    }

    // 3. 双 die 按通道带宽比切分数据（AIV prepare 消费 dieSplitRatio 计算 die0/die1 尺寸）
    double ratio = 1.0;
    if (dieNum == DIE_NUM_2) {
        uint32_t p0 = 0;
        uint32_t p1 = 0;
        CHK_RET(GetChannelBwCoeff(comm, myRank_, channelsPerDie[0][0], p0));
        CHK_RET(GetChannelBwCoeff(comm, myRank_, channelsPerDie[1][0], p1));
        if (p0 + p1 > 0) {
            ratio = static_cast<double>(p0) / (p0 + p1);
        }
    }
    resourceRequest.dieSplitRatio = ratio;

    // 4. 构造 kernelInfo：每个 die 一份（axisId 区分），channels 为该 die 的链路组
    for (uint32_t kernelIdx = 0; kernelIdx < kernelNum; kernelIdx++) {
        CcuKernelInfo kernelInfo{};
        CHK_SAFETY_FUNC_RET(strcpy_s(
            kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelKfcAllGatherNHR1DMem2Mem"));
        auto kernelArg = std::make_shared<CcuKernelArgKfcAllGatherNHR1DMem2Mem>();
        kernelArg->rankSize = tempRankSize_;
        kernelArg->rankId = mySubCommRank_;
        kernelArg->axisId = kernelIdx;
        kernelArg->axisSize = dieNum;
        kernelArg->opParam = param;
        kernelArg->subCommRanks = subCommRanks_;
        kernelArg->stepInfoVector = stepInfoVector;
        kernelArg->rank2ChannelIdx = rank2ChannelIdx;
        kernelInfo.setKernelArg(kernelArg);
        kernelInfo.channels = channelsPerDie[kernelIdx];
        resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    }

    HCCL_INFO(
        "[CcuTempKfcAllGatherNHR1DMem2Mem::CalcRes] channelDescs.size()=%zu, dimsize=%u, dieNum=%u, "
        "dieSplitRatio=%.3f, ccuKernelInfos(contributed)=%u",
        channelDescs.size(), tempRankSize_, dieNum, ratio, kernelNum);
    return HCCL_SUCCESS;
}

// 按 step 序列逐个选 from/to 的 channel（单 die 用 enableDieId 的链路；双 die 加两个 die 的
// 链路，供跨框 die0 连 die1 场景两条链路并存）——RS 2die ProcessNHRStepInfo 同构。
HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::ProcessNHRStepInfo(
    HcclComm comm, std::vector<KfcNhrStepInfo>& stepInfoVector, std::map<u32, u32>& rank2ChannelIdx, u32 enableDieNum,
    u32 enableDieId, std::vector<std::vector<HcclChannelDesc>>& channelsPerDie) const
{
    u32 stepNum = 0;
    for (u32 ranks = tempRankSize_ - 1U; ranks != 0U; ranks >>= 1U) {
        ++stepNum;
    }
    for (u32 step = 0; step < stepNum; ++step) {
        KfcNhrStepInfo stepInfo;
        CHK_RET(GetStepInfo(step, stepNum, stepInfo));
        stepInfoVector.push_back(stepInfo);
        if (enableDieNum == DIE_NUM_1) {
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.fromRank, rankIdToChannelDesc_, enableDieId, rank2ChannelIdx,
                channelsPerDie[0]));
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.toRank, rankIdToChannelDesc_, enableDieId, rank2ChannelIdx, channelsPerDie[0]));
        } else if (enableDieNum == DIE_NUM_2) {
            // 加入 fromRank/toRank 两个 die 的链路
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.fromRank, rankIdToChannelDesc_, 0, rank2ChannelIdx, channelsPerDie[0]));
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.fromRank, rankIdToChannelDesc_, 1, rank2ChannelIdx, channelsPerDie[1]));
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.toRank, rankIdToChannelDesc_, 0, rank2ChannelIdx, channelsPerDie[0]));
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.toRank, rankIdToChannelDesc_, 1, rank2ChannelIdx, channelsPerDie[1]));
        }
    }
    return HCCL_SUCCESS;
}

// 与 hccl CcuTempAllGatherNHR1DMem2Mem::GetStepInfo 逐行对照：
// deltaRank = 1<<(nSteps-1-step)；sendTo = (r+delta)%size；recvFrom = (r+size-delta)%size；
// nSlices = (size-1+deltaRank)/deltaSlice；tx 从 mySubCommRank 起、rx 从 fromRank 起，按 deltaSlice 递减。
HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::GetStepInfo(u32 step, u32 stepNum, KfcNhrStepInfo& stepInfo) const
{
    const u32 deltaRank = 1U << (stepNum - 1U - step);
    const u32 deltaSlice = 1U << (stepNum - step);
    stepInfo.step = step;
    stepInfo.myRank = mySubCommRank_;
    stepInfo.toRank = (mySubCommRank_ + deltaRank) % tempRankSize_;
    stepInfo.fromRank = (mySubCommRank_ + tempRankSize_ - deltaRank) % tempRankSize_;
    stepInfo.nSlices = (tempRankSize_ - 1U + deltaRank) / deltaSlice;
    u32 txSliceIdx = mySubCommRank_;
    u32 rxSliceIdx = stepInfo.fromRank;
    for (u32 i = 0; i < stepInfo.nSlices; ++i) {
        stepInfo.txSliceIdxs.push_back(txSliceIdx);
        stepInfo.rxSliceIdxs.push_back(rxSliceIdx);
        txSliceIdx = (txSliceIdx + tempRankSize_ - deltaSlice) % tempRankSize_;
        rxSliceIdx = (rxSliceIdx + tempRankSize_ - deltaSlice) % tempRankSize_;
    }
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // 每轮参数由 CcuPrepareForAllGatherSoleNhrM2M 写入 xnData 队列，由 KFC server 消费。
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    return HCCL_SUCCESS;
}

u64 CcuTempKfcAllGatherNHR1DMem2Mem::GetThreadNum() const { return 1; }

u64 CcuTempKfcAllGatherNHR1DMem2Mem::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

} // namespace mc2_ops_hccl
