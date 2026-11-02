/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TOPO_MATCH_ONE_LEVEL
#define TOPO_MATCH_ONE_LEVEL

#include "topo_match_base_v2.h"

namespace mc2_ops_hccl {

class TopoMatchOneLevel : public TopoMatchBaseV2 {
public:
    explicit TopoMatchOneLevel();
    ~TopoMatchOneLevel() override;

    std::string Describe() const override { return "Topo Match for One Level Algorithm."; }

    HcclResult MatchTopo(
        TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo,
        const AlgAttrs& algAttrs) override;

    // devkit 适配（对齐 TopoMatchTwoLevel 先例）：V1 签名兼容入口。
    // devkit 的调度链（HcclGetAlgRes → InsCollAlgBase::CalcAlgHierarchyInfo）以 V1 三参
    // MatchTopo(comm, topoInfo, algHierarchyInfo) 调用，且 devkit 无 hccl 侧注册框架的
    // algoTypes 注入链（hccl 由 algName 解析填充）。本重载内置本算法家族固化的
    // AlgAttrs（AICPU_TS + {PAIRWISE}，即 AicpuAllToAllVSolePairwise 的单模板形态），
    // 转发到 V2 MatchTopo。
    HcclResult MatchTopo(
        const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo);
};

} // namespace mc2_ops_hccl

#endif // !TOPO_MATCH_ONE_LEVEL
