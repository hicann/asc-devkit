/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_to_all_v.h"

#include "alg_param.h"

#include <vector>

namespace HcclSim {
namespace CcuSt {

std::map<std::string, CcuStAllToAllV::ScenarioBuilder>& CcuStAllToAllV::GetVariantRegistry()
{
    static std::map<std::string, ScenarioBuilder> registry;
    return registry;
}

void CcuStAllToAllV::RegisterVariant(const std::string& algName, ScenarioBuilder builder)
{
    GetVariantRegistry()[algName] = std::move(builder);
}

void CcuStAllToAllV::PrepareOpParam(OpParam& opParam, uint32_t rankSize, const CcuStScenario& scenario)
{
    int32_t currentRank = 0;
    ASSERT_EQ(aclrtGetDevice(&currentRank), ACL_SUCCESS);
    ASSERT_GE(currentRank, 0);
    ASSERT_LT(static_cast<uint32_t>(currentRank), rankSize);

    const uint32_t typeSize = DATATYPE_SIZE_TABLE[scenario.dataType];
    ASSERT_NE(typeSize, 0U);
    static thread_local std::vector<uint64_t> sendCounts;
    static thread_local std::vector<uint64_t> recvCounts;
    static thread_local std::vector<uint64_t> sendDispls;
    static thread_local std::vector<uint64_t> recvDispls;
    sendCounts.resize(rankSize);
    recvCounts.resize(rankSize);
    sendDispls.resize(rankSize);
    recvDispls.resize(rankSize);

    uint64_t sendOffset = 0;
    uint64_t recvOffset = 0;
    for (uint32_t peer = 0; peer < rankSize; ++peer) {
        sendCounts[peer] = scenario.sizes[currentRank][peer] / typeSize;
        recvCounts[peer] = scenario.sizes[peer][currentRank] / typeSize;
        sendDispls[peer] = sendOffset;
        recvDispls[peer] = recvOffset;
        sendOffset += sendCounts[peer];
        recvOffset += recvCounts[peer];
    }
    opParam.all2AllVDataDes.sendType = scenario.dataType;
    opParam.all2AllVDataDes.recvType = scenario.dataType;
    opParam.all2AllVDataDes.sendCounts = sendCounts.data();
    opParam.all2AllVDataDes.recvCounts = recvCounts.data();
    opParam.all2AllVDataDes.sdispls = sendDispls.data();
    opParam.all2AllVDataDes.rdispls = recvDispls.data();
    opParam.isKfc = true;
}

ScenarioData CcuStAllToAllV::BuildScenarioData(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    auto it = GetVariantRegistry().find(scenario.expectedAlgName);
    return it == GetVariantRegistry().end() ? ScenarioData{} : it->second(scenario, handles);
}

} // namespace CcuSt
} // namespace HcclSim
