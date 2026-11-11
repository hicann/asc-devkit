/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_TEMP_KFC_ALL_REDUCE_NHR_1D_MEM2MEM_H
#define HCCL_CCU_TEMP_KFC_ALL_REDUCE_NHR_1D_MEM2MEM_H

#include "utils.h"
#include "ccu_alg_template_base.h"
#include "kernel/ccu_kernel_kfc_all_reduce_nhr1d_mem2mem.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

// SoleNHR（CcuSchedAllReduceSoleNHR）的注册期 kernelArg：
// axisId/axisSize 为 die 维编号与总数（双 die 时每 die 一份 kernelInfo）；step/rank2ChannelIdx
// 来自 AllReduce NHR 的 reduce-scatter + all-gather 两段 step 序列（hccl 同源）。
struct CcuKernelArgKfcAllReduceNHR1D : CcuKernelArgBase {
    uint64_t rankSize = 0;                  // 子通信域 rank 数
    uint32_t rankId = 0;                    // 子通信域虚拟 rankid
    uint32_t axisId = 0;                    // die 维编号：0=die0, 1=die1
    uint32_t axisSize = 0;                  // die 维总数（=dieNum）
    uint64_t dieSplitRatioPermille = 1000U; // 1000=不切分；(0,1000)=die0 千分比（AIV prepare 消费）
    OpParam opParam;
    std::vector<KfcNhrStepInfo> stepInfoVector;
    std::map<uint32_t, uint32_t> rank2ChannelIdx;
    std::vector<std::vector<uint32_t>> subCommRanks;
};

// KFC 版 AllReduce NHR 1D Mem2Mem 模板（CcuSchedAllReduceSoleNHR）：
// CalcRes 与 hccl CcuTempAllReduceNHRMem2Mem1D::CalcRes 对照（两级拓扑通道/dieNum 探测/
// channelsPerDie/dieSplitRatio/kernelNum=dieNum 份 kernelInfo）；KernelRun 的 host 侧 die 切分与
// 分片尺寸计算（SplitDataFor2Dies + CalcSlice）搬至 AIV prepare
// （CcuPrepareForAllReduceSoleNhrM2M），由 KFC server 消费。
class CcuTempKfcAllReduceNHR1DMem2Mem : public CcuAlgTemplateBase {
public:
    CcuTempKfcAllReduceNHR1DMem2Mem() = default;
    CcuTempKfcAllReduceNHR1DMem2Mem(
        const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~CcuTempKfcAllReduceNHR1DMem2Mem() override = default;

    std::string Describe() const override
    {
        return StringFormat("Template of KFC AllReduce NHR1D Mem2Mem with tempRankSize [%u].", tempRankSize_);
    }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& templateDataParams,
        TemplateResource& templateResource) override;
    HcclResult GetRes(AlgResourceRequest& resourceRequest) const override;
    u64 GetThreadNum() const override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;

private:
    // 与 hccl CcuTempAllReduceNHRMem2Mem1D::ProcessNHRStepInfo 对照：单 die 按 enableDieId 选
    // from/to 的 channel；双 die 为 from/to 各加两个 die 的链路（channelsPerDie 分组）。
    HcclResult ProcessNHRStepInfo(
        const HcclComm comm, std::vector<KfcNhrStepInfo>& stepInfoVector, std::map<u32, u32>& rank2ChannelIdx,
        u32 enableDieNum, u32 enableDieId, std::vector<std::vector<HcclChannelDesc>>& channelsPerDie) const;
    // step 序列 = 前一半 reduce-scatter，后一半 all-gather（hccl GetStepInfo 同源）。
    HcclResult GetStepInfo(u32 step, u32 nSteps, KfcNhrStepInfo& stepInfo) const;
    HcclResult GetReduceScatterStepInfo(u32 step, KfcNhrStepInfo& stepInfo) const;
    HcclResult GetAllGatherStepInfo(u32 step, u32 nSteps, KfcNhrStepInfo& stepInfo) const;

    uint32_t mySubCommRank_ = 0;
    uint32_t tempRankSize_ = 0;
    // 对端 rank -> 通道描述列表（RestoreChannelMap 填充，ProcessNHRStepInfo 消费；与 hccl 模板同构）
    std::map<u32, std::vector<HcclChannelDesc>> rankIdToChannelDesc_;
};

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_TEMP_KFC_ALL_REDUCE_NHR_1D_MEM2MEM_H
