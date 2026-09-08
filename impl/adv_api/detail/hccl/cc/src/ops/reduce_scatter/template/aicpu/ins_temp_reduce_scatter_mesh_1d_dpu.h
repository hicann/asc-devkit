/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef INS_TEMP_REDUCE_SCATTER_1D_DPU
#define INS_TEMP_REDUCE_SCATTER_1D_DPU

#include "alg_v2_template_base.h"
#include "alg_v2_template_register.h"
#include "alg_param.h"
#include "executor_base.h"
#include "dpu_alg_data_trans_wrapper.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempReduceScatterMesh1dDpu : public InsAlgTemplateBase {
public:
    explicit InsTempReduceScatterMesh1dDpu();
    explicit InsTempReduceScatterMesh1dDpu(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempReduceScatterMesh1dDpu() override;

    std::string Describe() const override
    {
        std::string info = "Template of reduce scatter Mesh with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    // 现在的RunAsync就是之前的GenExtIns
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    u64 CalcScratchMultiple(BufferType inBufferType, BufferType outBufferType) override;
    HcclResult DPUKernelRun(
        const TemplateDataParams& tempAlgParams, const std::map<u32, std::vector<ChannelInfo>>& channels,
        const u32 myRank, const std::vector<std::vector<uint32_t>>& subCommRanks) override;
    HcclResult PostLocalReduce(const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads);

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub) override {}
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override {}

private:
    u64 count_{0};
    u64 processSize_{0};
};

} // namespace mc2_ops_hccl
#endif // OPEN_HCCL_INS_TEMP_REDUCE_SCATTER_MESH_1D_DPU
