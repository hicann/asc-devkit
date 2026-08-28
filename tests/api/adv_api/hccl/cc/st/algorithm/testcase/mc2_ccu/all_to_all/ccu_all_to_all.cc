/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_to_all.h"

#include <vector>
#include "hccl_types.h"
#include "alg_param.h"

namespace HcclSim {
namespace CcuSt {

std::map<std::string, CcuStAllToAll::ScenarioBuilder>& CcuStAllToAll::GetVariantRegistry()
{
    static std::map<std::string, ScenarioBuilder> registry;
    return registry;
}

void CcuStAllToAll::RegisterVariant(const std::string& algName, ScenarioBuilder builder)
{
    GetVariantRegistry()[algName] = std::move(builder);
}

void CcuStAllToAll::PrepareOpParam(OpParam& opParam, uint32_t rankSize, const CcuStScenario& scenario)
{
    const uint32_t typeSize = DATATYPE_SIZE_TABLE[scenario.dataType];
    static thread_local std::vector<uint64_t> sendCounts;
    sendCounts.assign(rankSize, 0);
    for (uint32_t i = 0; i < rankSize; ++i) {
        sendCounts[i] = (typeSize > 0) ? scenario.sizes[0][i] / typeSize : scenario.sizes[0][i];
    }
    opParam.all2AllVDataDes.sendCounts = reinterpret_cast<void*>(sendCounts.data());
}

ScenarioData CcuStAllToAll::BuildScenarioData(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    algName_ = scenario.expectedAlgName;
    auto& registry = GetVariantRegistry();
    auto it = registry.find(algName_);
    if (it != registry.end()) {
        return it->second(scenario, handles);
    }
    return {};
}

} // namespace CcuSt
} // namespace HcclSim
