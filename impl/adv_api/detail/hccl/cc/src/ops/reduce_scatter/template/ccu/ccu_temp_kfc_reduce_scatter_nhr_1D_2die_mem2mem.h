/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_TEMP_KFC_REDUCE_SCATTER_NHR_1D_2DIE_MEM2MEM_H
#define HCCL_CCU_TEMP_KFC_REDUCE_SCATTER_NHR_1D_2DIE_MEM2MEM_H

#include "utils.h"
#include "ccu_alg_template_base.h"
#include "kernel/ccu_kernel_kfc_reduce_scatter_nhr1d_2die_mem2mem.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

// SoleNHR（两级拓扑 L1 NHR 中继）的注册期 kernelArg：
// axisId/axisSize 为 die 维编号与总数（双 die 时每 die 一份 kernelInfo）；step/rank2ChannelIdx
// 与 MultiLink 同源（递归倍增中继），通道按两级拓扑经 CalcChannelRequestNhr 计算。
struct CcuKernelArgKfcReduceScatterNHR1D2Die : CcuKernelArgBase {
    uint64_t rankSize = 0; // dimSize：子通信域 rank 数
    uint32_t rankId = 0;   // 子通信域虚拟 rankid
    uint32_t axisId = 0;   // die 维编号：0=die0, 1=die1
    uint32_t axisSize = 0; // die 维总数（=dieNum）
    OpParam opParam;
    std::vector<KfcNhrStepInfo> stepInfoVector;
    std::map<uint32_t, uint32_t> rank2ChannelIdx;
    std::vector<std::vector<uint32_t>> subCommRanks;
};

// KFC 版 ReduceScatter NHR 1D 2Die Mem2Mem 模板（CcuSchedReduceScatterSoleNHR）：
// CalcRes 与 hccl CcuTempReduceScatterNHR1DMem2Mem::CalcRes 对照（两级拓扑通道/dieNum 探测/
// channelsPerDie/dieSplitRatio/kernelNum=dieNum 份 kernelInfo）；KernelRun 的 host 侧 die 切分与
// goSize 计算（SplitDataFor2Dies + CalGoSize）按 MC2 device-driven 铁律搬至 AIV prepare
// （CcuPrepareForReduceScatterSoleNhr2DieM2M）。
class CcuTempKfcReduceScatterNHR1D2DieMem2Mem : public CcuAlgTemplateBase {
public:
    CcuTempKfcReduceScatterNHR1D2DieMem2Mem() = default;
    CcuTempKfcReduceScatterNHR1D2DieMem2Mem(
        const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~CcuTempKfcReduceScatterNHR1D2DieMem2Mem() override = default;

    std::string Describe() const override
    {
        return StringFormat("Template of KFC ReduceScatter NHR1D 2Die Mem2Mem with tempRankSize [%u].", tempRankSize_);
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
    // 与 hccl CcuTempReduceScatterNHR1DMem2Mem::ProcessNHRStepInfo 对照：单 die 按 enableDieId 选
    // from/to 的 channel；双 die 为 from/to 各加两个 die 的链路（channelsPerDie 分组）。
    HcclResult ProcessNHRStepInfo(
        const HcclComm comm, std::vector<KfcNhrStepInfo>& stepInfoVector, std::map<u32, u32>& rank2ChannelIdx,
        u32 enableDieNum, u32 enableDieId, std::vector<std::vector<HcclChannelDesc>>& channelsPerDie) const;
    // step 生成与 MultiLink 同源（hccl SoleNHR/MultiJetty 的 GetStepInfo 为同一套递归倍增公式）
    HcclResult CalcNhrInfo(std::vector<KfcNhrStepInfo>& stepInfoVector) const;
    HcclResult GetStepInfo(u32 step, KfcNhrStepInfo& stepInfo) const;

    uint32_t mySubCommRank_ = 0;
    uint32_t tempRankSize_ = 0;
    // 对端 rank -> 通道描述列表（RestoreChannelMap 填充，ProcessNHRStepInfo 消费；与 hccl 模板同构）
    std::map<u32, std::vector<HcclChannelDesc>> rankIdToChannelDesc_;
};

} // namespace mc2_ops_hccl

#endif
