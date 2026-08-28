/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_TEMP_ALL_TO_ALL_MESH_1D_H
#define HCCL_CCU_TEMP_ALL_TO_ALL_MESH_1D_H

#include "utils.h"
#include "ccu_alg_template_base.h"

namespace mc2_ops_hccl {

class CcuTempAlltoAllMesh1D : public CcuAlgTemplateBase {
public:
    CcuTempAlltoAllMesh1D() = default;
    explicit CcuTempAlltoAllMesh1D(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~CcuTempAlltoAllMesh1D() override;

    std::string Describe() const override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;

private:
    uint32_t typeSize_ = 0;
    uint32_t mySubCommRank_ = 0;
    uint32_t tempRankSize_ = 0;
};

} // namespace mc2_ops_hccl
#endif // HCCL_CCU_TEMP_ALL_TO_ALL_MESH_1D_H
