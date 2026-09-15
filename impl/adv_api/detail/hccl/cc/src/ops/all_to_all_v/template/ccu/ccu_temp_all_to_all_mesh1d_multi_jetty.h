/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_TEMP_ALL_TO_ALL_MESH1D_MULTI_JETTY_H
#define HCCL_CCU_TEMP_ALL_TO_ALL_MESH1D_MULTI_JETTY_H

#include "utils.h"
#include "ccu_alg_template_base.h"
#include "kernel/ccu_kernel_all_to_all_mesh1d_multi_jetty.h"

namespace mc2_ops_hccl {

struct CcuKernelArgAllToAllMesh1DMultiJetty : CcuKernelArgBase {
    uint64_t rankSize = 0;
    uint32_t rankId = 0;
    uint32_t jettyNum = 1U;
    OpParam opParam;
    std::vector<std::vector<uint32_t>> subCommRanks;
};

class CcuTempAllToAllMesh1DMultiJetty : public CcuAlgTemplateBase {
public:
    CcuTempAllToAllMesh1DMultiJetty() = default;
    CcuTempAllToAllMesh1DMultiJetty(
        const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~CcuTempAllToAllMesh1DMultiJetty() override = default;

    std::string Describe() const override
    {
        return StringFormat("Template of AllToAll Mesh1D MultiJetty with tempRankSize [%u].", tempRankSize_);
    }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    HcclResult GetRes(AlgResourceRequest& resourceRequest) const override;
    u64 GetThreadNum() const override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;

private:
    uint32_t mySubCommRank_ = 0;
    uint32_t tempRankSize_ = 0;
};

} // namespace mc2_ops_hccl

#endif
