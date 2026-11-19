/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_TEMP_ALL_TO_ALL_V_MESH_1D_MULTI_JETTY_H
#define HCCL_CCU_TEMP_ALL_TO_ALL_V_MESH_1D_MULTI_JETTY_H

#include "utils.h"
#include "alg_param.h"
#include "ccu_alg_template_base.h"

namespace mc2_ops_hccl {

// 照 hccl CcuTempAllToAllVMesh1DMultiJetty 移植（CcuSchedAllToAllVSoleMeshConcurrent 的双 mission 模板）。
// KFC 适配：KernelRun 为空壳（逐 op 参数由 AIV CcuPrepareForConcurrentAllToAllV 写 HBM，
// KFC server 消费）；host 侧切分/参数烘焙逻辑（hccl AddTaskArgA2AInfo）不迁移。
class CcuTempAllToAllVMesh1DMultiJetty : public CcuAlgTemplateBase {
public:
    CcuTempAllToAllVMesh1DMultiJetty() = default;
    CcuTempAllToAllVMesh1DMultiJetty(
        const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~CcuTempAllToAllVMesh1DMultiJetty() override = default;

    std::string Describe() const override
    {
        return StringFormat(
            "Template of AlltoAllV ccu mesh 1D MultiJetty with tempRankSize [%zu].", subCommRanks_[0].size());
    }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    // KFC 并发入口：executor 计算 mesh/clos 通道后经此注入（mission0=mesh 优先链路，mission1=clos）
    HcclResult CalcResByChannelDescs(
        const OpParam& param, const std::vector<HcclChannelDesc>& channelDescs,
        AlgResourceRequest& resourceRequest) const;
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& templateDataParams,
        TemplateResource& templateResource) override;
    HcclResult GetRes(AlgResourceRequest& resourceRequest) const override;
    u64 GetThreadNum() const override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;

    HcclResult SetJettyNums(std::vector<uint32_t>& jettyNums, const bool multijetty) const;

private:
    HcclResult CalcResByChannelDescsImpl(
        const OpParam& param, const std::vector<HcclChannelDesc>& channelDescs,
        AlgResourceRequest& resourceRequest) const;
    uint32_t mySubCommRank_ = 0;
    uint32_t tempRankSize_ = 0;
};

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_TEMP_ALL_TO_ALL_V_MESH_1D_MULTI_JETTY_H
