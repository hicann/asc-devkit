/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_TEMP_KFC_MESH_1D_MEM2MEM_BASE_H
#define HCCL_CCU_TEMP_KFC_MESH_1D_MEM2MEM_BASE_H

#include <algorithm>

#include "ccu_alg_template_base.h"

namespace mc2_ops_hccl {

class CcuTempKfcMesh1DMem2MemBase : public CcuAlgTemplateBase {
public:
    CcuTempKfcMesh1DMem2MemBase() = default;
    explicit CcuTempKfcMesh1DMem2MemBase(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
        : CcuAlgTemplateBase(param, rankId, subCommRanks)
    {
        const std::vector<u32>& ranks = subCommRanks[0];
        templateRankSize_ = ranks.size();
        auto it = std::find(ranks.begin(), ranks.end(), rankId);
        if (it != ranks.end()) {
            mySubCommRank_ = std::distance(ranks.begin(), it);
        }
    }

    ~CcuTempKfcMesh1DMem2MemBase() override = default;

    HcclResult GetRes(AlgResourceRequest& resourceRequest) const override
    {
        resourceRequest.slaveThreadNum = 0;
        resourceRequest.notifyNumOnMainThread = 0;
        return HCCL_SUCCESS;
    }

    u64 GetThreadNum() const override { return 1; }

protected:
    uint32_t mySubCommRank_{0};
};

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_TEMP_KFC_MESH_1D_MEM2MEM_BASE_H
