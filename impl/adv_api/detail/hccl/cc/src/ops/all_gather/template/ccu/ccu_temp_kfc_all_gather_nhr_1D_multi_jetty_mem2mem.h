/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_TEMP_KFC_ALL_GATHER_NHR_1D_MULTI_JETTY_MEM2MEM_H
#define HCCL_CCU_TEMP_KFC_ALL_GATHER_NHR_1D_MULTI_JETTY_MEM2MEM_H

#include "ccu_alg_template_base.h"
#include "kernel/ccu_kernel_kfc_all_gather_nhr1d_multi_jetty_mem2mem.h"

namespace mc2_ops_hccl {

struct CcuKernelArgKfcAllGatherNHR1DMultiJettyMem2Mem : CcuKernelArgBase {
    uint64_t rankSize = 0;
    uint32_t rankId = 0;
    uint32_t jettyNum = 4;
    OpParam opParam;
    std::vector<KfcNhrStepInfo> stepInfoVector;
    std::map<uint32_t, uint32_t> rank2ChannelIdx;
    std::vector<std::vector<uint32_t>> subCommRanks;
};

class CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem : public CcuAlgTemplateBase {
public:
    CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem() = default;
    CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem(
        const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem() override = default;

    std::string Describe() const override
    {
        return StringFormat(
            "Template of KFC AllGather NHR1D MultiJetty Mem2Mem with tempRankSize [%u].", tempRankSize_);
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
    HcclResult CalcNhrInfo(std::vector<KfcNhrStepInfo>& stepInfoVector) const;
    HcclResult GetStepInfo(u32 step, u32 stepNum, KfcNhrStepInfo& stepInfo) const;
    uint32_t RemoteRankIdToSubRank(uint32_t remoteRankId) const;

    uint32_t mySubCommRank_ = 0;
    uint32_t tempRankSize_ = 0;
};

} // namespace mc2_ops_hccl

#endif
