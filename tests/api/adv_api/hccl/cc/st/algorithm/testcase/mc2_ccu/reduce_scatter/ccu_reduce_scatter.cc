/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_reduce_scatter.h"

#include "alg_param.h"

#include <vector>

namespace HcclSim {
namespace CcuSt {

std::map<std::string, CcuStReduceScatter::ScenarioBuilder>& CcuStReduceScatter::GetVariantRegistry()
{
    static std::map<std::string, ScenarioBuilder> registry;
    return registry;
}

void CcuStReduceScatter::RegisterVariant(const std::string& algName, ScenarioBuilder builder)
{
    GetVariantRegistry()[algName] = std::move(builder);
}

void CcuStReduceScatter::PrepareOpParam(OpParam& opParam, uint32_t rankSize, const CcuStScenario& scenario)
{
    (void)rankSize;
    // KFC路径下count在tiling期不可知（PrepareParamForReduceScatter置0），此处用scenario补齐真实尺寸，
    // 模拟AIV侧CcuPrepareForReduceScatterM2M运行时才拿到的per-rank数据量。
    opParam.DataDes.count = scenario.count;
    opParam.DataDes.dataType = scenario.dataType;
    opParam.DataDes.outputType =
        scenario.outputType == HCCL_DATA_TYPE_RESERVED ? scenario.dataType : scenario.outputType;
    opParam.DataDes.strideCount = 0;
    opParam.reduceType = scenario.reduceType;
}

ScenarioData CcuStReduceScatter::BuildScenarioData(
    const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    auto it = GetVariantRegistry().find(scenario.expectedAlgName);
    return it == GetVariantRegistry().end() ? ScenarioData{} : it->second(scenario, handles);
}

} // namespace CcuSt
} // namespace HcclSim
