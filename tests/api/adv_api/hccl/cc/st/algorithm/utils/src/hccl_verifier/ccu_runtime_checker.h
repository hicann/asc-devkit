/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_RUNTIME_CHECKER_H
#define HCCL_CCU_RUNTIME_CHECKER_H

#include "ccu_sim_world.h"
#include "ccu_checker.h"
#include "ccu_scenario.h"

#include <string>
#include <vector>

namespace HcclSim {
namespace CcuSt {

struct ScenarioData {
    std::vector<RankMemory> memories;
    std::vector<RankLaunch> launches;

    std::vector<std::vector<uint64_t>> srcOffsets;
    std::vector<std::vector<uint64_t>> dstOffsets;

    HcclReduceOp reduceOp = HCCL_REDUCE_RESERVED;
};

struct RuntimeExpectation {
    std::vector<std::vector<uint8_t>> outputs;
};

CheckResult CaptureRuntimeExpectation(
    const ScenarioData& data, const CcuStScenario& scenario, RuntimeExpectation& expectation);

CheckResult CheckRuntimeOutput(
    const std::vector<RankMemory>& memories, const RuntimeExpectation& expectation, const RuntimeWorld& world);

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_RUNTIME_CHECKER_H
