/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TOPO_MATCH_UBX_1D
#define TOPO_MATCH_UBX_1D

#include "topo_match_ubx.h"

namespace mc2_ops_hccl {
class TopoMatchUBX1d : public TopoMatchUBX {
public:
    explicit TopoMatchUBX1d();
    ~TopoMatchUBX1d() override;
    std::string Describe() const override
    {
        return "Topo Match for combined Algorithm: layer 0 Mesh + Clos, layer 3 Clos.";
    }
    HcclResult MatchTopo(
        const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo,
        AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;

protected:
    HcclResult TopoForLayer0(
        const HcclComm comm, uint32_t& layer0Size, const uint32_t myRank,
        AlgHierarchyInfoForAllLevel& algHierarchyInfo) const;
    HcclResult TopoForLayer3(
        const HcclComm comm, uint32_t layer0Size, const uint32_t myRank,
        AlgHierarchyInfoForAllLevel& algHierarchyInfo) const;
};
} // namespace mc2_ops_hccl
#endif // !TOPO_MATCH_UBX_MESH_1D
