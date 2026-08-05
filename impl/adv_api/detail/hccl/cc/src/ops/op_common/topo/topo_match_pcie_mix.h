/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TOPO_MATCH_PCIE_MIX
#define TOPO_MATCH_PCIE_MIX

#include "topo_match_base.h"

namespace mc2_ops_hccl {
class TopoMatchPcieMix : public TopoMatchBase {
public:
    explicit TopoMatchPcieMix();
    ~TopoMatchPcieMix() override;
    std::string Describe() const override { return "Topo Match for pcie mix topo: layer 0 (Mesh + Clos)."; }
    HcclResult MatchTopo(
        const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo,
        AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;

private:
    HcclResult TopoForLayer0(const HcclComm comm, const uint32_t myRank, AlgHierarchyInfoForAllLevel& algHierarchyInfo);
    HcclResult CheckVecElementAllSame(const uint32_t* instSizeList, uint32_t listSize) const;
    template <typename T>
    std::string PrintCArray(const T* values, const u32 valueNum) const
    {
        std::ostringstream oss;
        for (u32 i = 0; i < valueNum; i++) {
            oss << values[i] << " ";
        }
        return oss.str();
    }
    HcclResult LoadTopoInstRanks(
        const HcclComm comm, uint32_t netLayer, uint32_t topoInstId, std::vector<uint32_t>& rankList) const;
    HcclResult DeduplicateLevelRanks(
        const uint32_t myRank, std::vector<uint32_t>& level0Ranks, std::vector<uint32_t>& level1Ranks) const;
};
} // namespace mc2_ops_hccl
#endif // !TOPO_MATCH_PCIE_MIX
