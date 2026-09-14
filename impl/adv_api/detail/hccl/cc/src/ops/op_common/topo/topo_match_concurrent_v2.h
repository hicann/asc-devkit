/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TOPO_MATCH_CONCURRENT_V2
#define TOPO_MATCH_CONCURRENT_V2

#include "topo_match_base.h"

namespace mc2_ops_hccl {

// 与 hccl TopoMatchConcurrentV2 同语义：mesh 组与 clos 组为同一组 rank（全通信域），
// 两流只是走不同物理链路（mesh 链路 / clos 链路），用于 Mesh+NHR 并发算法。
// 区别于 TopoMatchUBX（layer1 为跨 mesh 同序号卡组）。
class TopoMatchConcurrentV2 : public TopoMatchBase {
public:
    TopoMatchConcurrentV2() = default;
    ~TopoMatchConcurrentV2() override = default;
    std::string Describe() const override
    {
        return "Topo Match for concurrent Algorithm: mesh group and clos group share the same full comm rank set.";
    }
    HcclResult MatchTopo(
        const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo,
        AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;
};

} // namespace mc2_ops_hccl
#endif // TOPO_MATCH_CONCURRENT_V2
