/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_TEMP_ALL_TO_ALL_V_MESH_1D_H
#define HCCL_CCU_TEMP_ALL_TO_ALL_V_MESH_1D_H

#include "utils.h"
#include "ccu_alg_template_base.h"

namespace mc2_ops_hccl {

struct CcuKernelArgAlltoAllVMesh1D : CcuKernelArgBase {
    uint64_t rankSize;
    uint32_t rankId;
    bool loadFromMem;
    OpParam opParam;
    std::vector<std::vector<uint32_t>> subCommRanks;
};

class CcuTempAlltoAllVMesh1D : public CcuAlgTemplateBase {
public:
    CcuTempAlltoAllVMesh1D() = default;
    explicit CcuTempAlltoAllVMesh1D(
        const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);

    ~CcuTempAlltoAllVMesh1D() override = default;

    std::string Describe() const override
    {
        return StringFormat("Template of MC2 AllToAllV CCU mesh 1D with tempRankSize [%u].", tempRankSize_);
    }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;

    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& templateDataParams,
        TemplateResource& templateResource) override;

    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;

private:
    uint32_t mySubCommRank_ = 0;
    uint32_t tempRankSize_ = 0;
};

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_TEMP_ALL_TO_ALL_V_MESH_1D_H
