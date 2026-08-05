/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TOPO_MATCH_MESH_NHR
#define TOPO_MATCH_MESH_NHR

#include "topo_match_base.h"

namespace mc2_ops_hccl {
class TopoMatchMultilevel : public TopoMatchBase {
public:
    explicit TopoMatchMultilevel();
    ~TopoMatchMultilevel() override;

    std::string Describe() const override { return "Topo Match for combined Algorithm: layer 0 Mesh, layer 1 NHR."; }
    HcclResult MatchTopo(
        const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo,
        AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;

private:
    HcclResult TopoForLayer0(
        const HcclComm comm, uint32_t& layer0Size, const uint32_t myRank, AlgHierarchyInfoForAllLevel& algHierarchyInfo,
        uint32_t gcdInstSize = 0) const;
    HcclResult TopoForLayer1(
        const HcclComm comm, uint32_t netLayer, uint32_t& layer0Size, const uint32_t myRank,
        AlgHierarchyInfoForAllLevel& algHierarchyInfo) const;
    bool CheckVecElementAllSame(const uint32_t* instSizeList, uint32_t listSize) const;
    uint32_t GcdTwo(uint32_t a, uint32_t b) const;
    uint32_t GcdOfInstSizeList(const uint32_t* instSizeList, uint32_t listSize) const;

    template <typename T>
    std::string PrintCArray(const T* values, const u32 valueNum) const
    {
        std::ostringstream oss;
        for (u32 i = 0; i < valueNum; i++) {
            oss << values[i] << " ";
        }
        return oss.str();
    }
};
} // namespace mc2_ops_hccl

#endif // !TOPO_MATCH_MESH_NHR
