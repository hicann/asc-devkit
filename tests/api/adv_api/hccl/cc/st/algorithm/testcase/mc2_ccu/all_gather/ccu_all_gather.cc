/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_gather.h"

#include "alg_param.h"

namespace HcclSim {
namespace CcuSt {

std::map<std::string, CcuStAllGather::ScenarioBuilder>& CcuStAllGather::GetVariantRegistry()
{
    static std::map<std::string, ScenarioBuilder> registry;
    return registry;
}

void CcuStAllGather::RegisterVariant(const std::string& algName, ScenarioBuilder builder)
{
    GetVariantRegistry()[algName] = std::move(builder);
}

void CcuStAllGather::PrepareOpParam(OpParam& opParam, uint32_t rankSize, const CcuStScenario& scenario)
{
    (void)rankSize;
    opParam.DataDes.count = scenario.count;
    opParam.DataDes.strideCount = 0;
    // CcuSchedAllGatherSoleMesh is dispatched by the persistent KFC server.
    opParam.isKfc = true;
}

ScenarioData CcuStAllGather::BuildScenarioData(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    const auto it = GetVariantRegistry().find(scenario.expectedAlgName);
    return it == GetVariantRegistry().end() ? ScenarioData{} : it->second(scenario, handles);
}

} // namespace CcuSt
} // namespace HcclSim
