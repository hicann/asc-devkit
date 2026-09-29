/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "alg_data_trans_wrapper.h"
#include "channel.h"
#include "ccu_temp_kfc_reduce_scatter_nhr_1D_2die_mem2mem.h"

namespace mc2_ops_hccl {

namespace {
constexpr u32 DIE_NUM_1 = 1;
constexpr u32 DIE_NUM_2 = 2;
} // namespace

CcuTempKfcReduceScatterNHR1D2DieMem2Mem::CcuTempKfcReduceScatterNHR1D2DieMem2Mem(
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

// 与 hccl CcuTempReduceScatterNHR1DMem2Mem::ProcessNHRStepInfo 逐行对照：
// 按 step 序列逐个选 from/to 的 channel（单 die 用 enableDieId 的链路；双 die 加两个 die 的链路，
// 供跨框 die0 连 die1 场景两条链路并存）。
HcclResult CcuTempKfcReduceScatterNHR1D2DieMem2Mem::ProcessNHRStepInfo(
    const HcclComm comm, std::vector<KfcNhrStepInfo>& stepInfoVector, std::map<u32, u32>& rank2ChannelIdx,
    u32 enableDieNum, u32 enableDieId, std::vector<std::vector<HcclChannelDesc>>& channelsPerDie) const
{
    u32 nSteps = 0;
    for (u32 ranks = tempRankSize_ - 1U; ranks != 0U; ranks >>= 1U) {
        ++nSteps;
    }
    for (u32 step = 0; step < nSteps; step++) {
        KfcNhrStepInfo stepInfo;
        CHK_RET(GetStepInfo(step, stepInfo));
        stepInfoVector.push_back(stepInfo);
        if (enableDieNum == DIE_NUM_1) {
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.fromRank, rankIdToChannelDesc_, enableDieId, rank2ChannelIdx,
                channelsPerDie[0]));
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.toRank, rankIdToChannelDesc_, enableDieId, rank2ChannelIdx, channelsPerDie[0]));
        } else if (enableDieNum == DIE_NUM_2) {
            // 加入fromRank 2个die的链路
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.fromRank, rankIdToChannelDesc_, 0, rank2ChannelIdx, channelsPerDie[0]));
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.fromRank, rankIdToChannelDesc_, 1, rank2ChannelIdx, channelsPerDie[1]));
            // 加入toRank 2个die的链路
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.toRank, rankIdToChannelDesc_, 0, rank2ChannelIdx, channelsPerDie[0]));
            CHK_RET(SelectChannelToVec(
                comm, myRank_, stepInfo.toRank, rankIdToChannelDesc_, 1, rank2ChannelIdx, channelsPerDie[1]));
        }
    }
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcReduceScatterNHR1D2DieMem2Mem::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));

    // 与 hccl 一致：SoleNHR 的通道走两级拓扑 Nhr 计算（非 UBX 的 NhrMultiJetty）
    std::vector<HcclChannelDesc> channelDescs;
    CHK_RET(CalcChannelRequestNhr(comm, param, topoInfo, subCommRanks_, channelDescs));
    CHK_RET(RestoreChannelMap(channelDescs, rankIdToChannelDesc_));

    // 1.从获得的channelDesc，判断kernel发送到几个die上
    uint32_t dieNum = 0;
    uint32_t enableDieId = 0;
    CHK_RET(GetDieInfoFromChannelDescs(comm, rankIdToChannelDesc_, myRank_, dieNum, enableDieId));
    CHK_PRT_RET(
        dieNum < DIE_NUM_1 || dieNum > DIE_NUM_2,
        HCCL_ERROR("[CcuTempKfcReduceScatterNHR1D2DieMem2Mem::CalcRes] get channelDescs fail, dieNum[%u]", dieNum),
        HCCL_E_INTERNAL);

    // 双 die 时每个 die 一个 kernel 实例（KFC 下映射为双 mission，由 kfc server 模板继承调度）
    uint32_t kernelNum = dieNum;
    resourceRequest.ccuKernelNum.push_back(kernelNum);

    // 2.将channelDescs分到die组
    std::vector<std::vector<HcclChannelDesc>> channelsPerDie;
    channelsPerDie.resize(dieNum);
    std::map<u32, u32> rank2ChannelIdx;
    std::vector<KfcNhrStepInfo> stepInfoVector;

    CHK_RET(ProcessNHRStepInfo(comm, stepInfoVector, rank2ChannelIdx, dieNum, enableDieId, channelsPerDie));
    if (dieNum > 1) { // 通过端口数划分channel，适配跨框die0连die1的场景，避免建链失败
        CHK_RET(ReverseChannelPerDieIfNeed(comm, myRank_, channelsPerDie));
    }

    // 3.双 die 按通道带宽比切分数据（AIV prepare 消费 dieSplitRatio 计算 die0/die1 尺寸）
    double ratio = 1.0;
    if (dieNum == DIE_NUM_2) {
        uint32_t p0 = 0, p1 = 0;
        CHK_RET(GetChannelBwCoeff(comm, myRank_, channelsPerDie[0][0], p0));
        CHK_RET(GetChannelBwCoeff(comm, myRank_, channelsPerDie[1][0], p1));
        if (p0 + p1 > 0) {
            ratio = static_cast<double>(p0) / (p0 + p1);
        }
    }
    resourceRequest.dieSplitRatio = ratio;

    // 4.构造kernelInfo：每个die一份（axisId 区分），channels 为该 die 的链路组
    for (uint32_t kernelIdx = 0; kernelIdx < kernelNum; kernelIdx++) {
        CcuKernelInfo kernelInfo;
        CHK_SAFETY_FUNC_RET(strcpy_s(
            kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelKfcReduceScatterNHR1D2DieMem2Mem"));
        kernelInfo.kernelFunc = reinterpret_cast<void*>(CcuKfcReduceScatterNHR1D2DieMem2MemKernel);

        auto kernelArg = std::make_shared<CcuKernelArgKfcReduceScatterNHR1D2Die>();
        kernelArg->rankSize = tempRankSize_;
        kernelArg->rankId = mySubCommRank_;
        kernelArg->axisId = kernelIdx;
        kernelArg->axisSize = dieNum;
        kernelArg->dieSplitRatioPermille = DieSplitRatioToPermille(ratio);
        kernelArg->stepInfoVector = stepInfoVector;
        kernelArg->rank2ChannelIdx = rank2ChannelIdx;
        kernelArg->opParam = param;
        kernelArg->subCommRanks = subCommRanks_;
        kernelInfo.setKernelArg(kernelArg);
        kernelInfo.channels = channelsPerDie[kernelIdx];
        resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    }

    HCCL_INFO(
        "[CcuTempKfcReduceScatterNHR1D2DieMem2Mem::CalcRes] channelDescs.size()=%zu, dimsize=%u, "
        "dieNum=%u, dieSplitRatio=%.3f, ccuKernelInfos(contributed)=%u",
        channelDescs.size(), tempRankSize_, dieNum, ratio, kernelNum);
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcReduceScatterNHR1D2DieMem2Mem::CalcNhrInfo(std::vector<KfcNhrStepInfo>& stepInfoVector) const
{
    u32 stepNum = 0;
    for (u32 ranks = tempRankSize_ - 1U; ranks != 0U; ranks >>= 1U) {
        ++stepNum;
    }
    for (u32 step = 0; step < stepNum; ++step) {
        KfcNhrStepInfo stepInfo;
        CHK_RET(GetStepInfo(step, stepInfo));
        stepInfoVector.push_back(stepInfo);
    }
    return HCCL_SUCCESS;
}

// 与 hccl CcuTempReduceScatterNHR1DMem2Mem::GetStepInfo 逐行对照（与 MultiLink 同源：
// RS 的 sendTo 为减 deltaRank，AG 相反，不可互抄）。
HcclResult CcuTempKfcReduceScatterNHR1D2DieMem2Mem::GetStepInfo(u32 step, KfcNhrStepInfo& stepInfo) const
{
    const u32 deltaRank = 1U << step;
    const u32 deltaSlice = 1U << (step + 1U);
    stepInfo.step = step;
    stepInfo.myRank = mySubCommRank_;
    stepInfo.toRank = (mySubCommRank_ + tempRankSize_ - deltaRank) % tempRankSize_;
    stepInfo.fromRank = (mySubCommRank_ + deltaRank) % tempRankSize_;
    stepInfo.nSlices = (tempRankSize_ - 1U + deltaRank) / deltaSlice;
    u32 txSliceIdx = stepInfo.toRank;
    u32 rxSliceIdx = mySubCommRank_;
    for (u32 i = 0; i < stepInfo.nSlices; ++i) {
        stepInfo.txSliceIdxs.push_back(txSliceIdx);
        stepInfo.rxSliceIdxs.push_back(rxSliceIdx);
        txSliceIdx = (txSliceIdx + tempRankSize_ - deltaSlice) % tempRankSize_;
        rxSliceIdx = (rxSliceIdx + tempRankSize_ - deltaSlice) % tempRankSize_;
    }
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcReduceScatterNHR1D2DieMem2Mem::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // Per-round 参数（die0/die1 尺寸 + goSize×8 + 基础参数）由 CcuPrepareForReduceScatterSoleNhr2DieM2M
    // 运行期写入 HBM xnData，KFC server dispatch 逐槽转发，host 侧不再计算。
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcReduceScatterNHR1D2DieMem2Mem::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    return HCCL_SUCCESS;
}

u64 CcuTempKfcReduceScatterNHR1D2DieMem2Mem::GetThreadNum() const
{
    // hccl 恒返回 2（双 die 双 thread）；KFC 下 mission 由 kfc server 模板统一继承调度，
    // 这里保持 1 与 MultiLink 一致（slaveThreadNum 由 kfc server 按 missionNum 设置）。
    return 1;
}

u64 CcuTempKfcReduceScatterNHR1D2DieMem2Mem::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    // 与 hccl 一致：NHR 流直接远端规约（WriteReduce），不占用 scratch。
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

} // namespace mc2_ops_hccl
