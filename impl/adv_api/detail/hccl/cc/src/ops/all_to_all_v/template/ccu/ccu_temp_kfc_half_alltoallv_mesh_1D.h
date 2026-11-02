/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_TEMP_KFC_HALF_ALL_TO_ALL_V_MESH_1D_H
#define HCCL_CCU_TEMP_KFC_HALF_ALL_TO_ALL_V_MESH_1D_H

#include "utils.h"
#include "ccu_temp_kfc_mesh_1D_mem2mem_base.h"

namespace mc2_ops_hccl {

/*
 MC2 场景的变长 AllToAll。双 mission 并发：mission0 搬 A 切片、mission1 搬 B 切片，
 拆半设计为 64P 双 die unions 全互联算法预留扩展。后同步采用源端保序方式，
 当前最大支持 8P 通信域。
 */
class CcuTempKfcHalfAlltoAllVMesh1D : public CcuTempKfcMesh1DMem2MemBase {
public:
    explicit CcuTempKfcHalfAlltoAllVMesh1D(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);

    ~CcuTempKfcHalfAlltoAllVMesh1D() override;

    std::string Describe() const override
    {
        return StringFormat("Template of KFC HalfAlltoAllV Mesh1D with tempRankSize [%u].", subCommRanks_[0].size());
    }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& templateDataParams,
        TemplateResource& templateResource) override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
};

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_TEMP_KFC_HALF_ALL_TO_ALL_V_MESH_1D_H
