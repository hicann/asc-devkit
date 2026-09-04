/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TOPO_MATCH_MESH
#define TOPO_MATCH_MESH
#include <string>
#include <vector>
#include <hccl/hccl_types.h>
#include "alg_param.h"
#include "topo_match_base.h"

namespace mc2_ops_hccl {

class TopoMatch1D : public TopoMatchBase {
public:
    explicit TopoMatch1D();
    ~TopoMatch1D() override;

    std::string Describe() const override
    {
        return "Topo Match for Mesh Algorithm (CURRENTLY only 910_95 is supported).";
    }

    HcclResult MatchTopo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo,
        AlgHierarchyInfoForAllLevel& algHierarchyInfoExector) override;

private:
    u32 myRank_{0};
    std::vector<u32> rankIds_;
};
} // namespace mc2_ops_hccl

#endif // !HCCLV2_TOPO_MATCH_MESH
