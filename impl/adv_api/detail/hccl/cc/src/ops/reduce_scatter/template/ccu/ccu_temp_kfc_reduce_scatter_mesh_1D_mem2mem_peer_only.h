/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_TEMP_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_PEER_ONLY_H
#define HCCL_CCU_TEMP_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_PEER_ONLY_H

#include "utils.h"
#include "ccu_temp_kfc_mesh_1D_mem2mem_base.h"

namespace mc2_ops_hccl {

class CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly : public CcuTempKfcMesh1DMem2MemBase {
public:
    CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly() = default;
    explicit CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);

    ~CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly() override;

    std::string Describe() const override
    {
        return StringFormat(
            "Template of TP2 KFC ReduceScatter Mesh1D Mem2Mem PeerOnly with tempRankSize [%u].",
            subCommRanks_[0].size());
    }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& templateDataParams,
        TemplateResource& templateResource) override;
};

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_TEMP_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_PEER_ONLY_H
