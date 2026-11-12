/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_TEMP_KFC_ALL_GATHER_NHR_1D_MEM2MEM_H
#define HCCL_CCU_TEMP_KFC_ALL_GATHER_NHR_1D_MEM2MEM_H

#include "utils.h"
#include "ccu_alg_template_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

// CcuSchedAllGatherSoleNHR 的 KFC template。kernelFuncName 占位符由 CcuTempKfcServer::CalcRes
// 替换为 CcuKernelKfcServer；stepInfoVector/rank2ChannelIdx 经 algArg 透明传递机制下发。
struct CcuKernelArgKfcAllGatherNHR1DMem2Mem : CcuKernelArgBase {
    uint64_t rankSize = 0;
    uint32_t rankId = 0;
    // D4 双 die：die 维编号/总数（双 die 时每 die 一份 kernelInfo，axisId 区分实例）
    uint32_t axisId = 0;
    uint32_t axisSize = 1;
    OpParam opParam;
    std::vector<KfcNhrStepInfo> stepInfoVector;
    std::map<uint32_t, uint32_t> rank2ChannelIdx;
    std::vector<std::vector<uint32_t>> subCommRanks;
};

class CcuTempKfcAllGatherNHR1DMem2Mem : public CcuAlgTemplateBase {
public:
    CcuTempKfcAllGatherNHR1DMem2Mem() = default;
    CcuTempKfcAllGatherNHR1DMem2Mem(
        const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~CcuTempKfcAllGatherNHR1DMem2Mem() override = default;

    std::string Describe() const override
    {
        return StringFormat("Template of KFC AllGather NHR1D Mem2Mem with tempRankSize [%u].", subCommRanks_[0].size());
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
    // 按 step 序列逐个选 from/to 的 channel（单 die 用 enableDieId 的链路；双 die 加两个 die
    // 的链路，供跨框 die0 连 die1 场景两条链路并存）——RS 2die ProcessNHRStepInfo 同构。
    HcclResult ProcessNHRStepInfo(
        HcclComm comm, std::vector<KfcNhrStepInfo>& stepInfoVector, std::map<u32, u32>& rank2ChannelIdx,
        u32 enableDieNum, u32 enableDieId, std::vector<std::vector<HcclChannelDesc>>& channelsPerDie) const;
    HcclResult GetStepInfo(u32 step, u32 stepNum, KfcNhrStepInfo& stepInfo) const;

    uint32_t mySubCommRank_ = 0;
    uint32_t tempRankSize_ = 0;
    // 对端 rank -> 通道描述列表（RestoreChannelMap 填充，ProcessNHRStepInfo 消费；与 hccl/RS 2die 模板同构）
    std::map<u32, std::vector<HcclChannelDesc>> rankIdToChannelDesc_;
};

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_TEMP_KFC_ALL_GATHER_NHR_1D_MEM2MEM_H
