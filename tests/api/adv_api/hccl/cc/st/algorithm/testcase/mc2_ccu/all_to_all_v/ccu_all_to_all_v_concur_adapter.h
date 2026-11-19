/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_ALL_TO_ALL_V_CONCUR_ADAPTER_H
#define HCCL_CCU_ALL_TO_ALL_V_CONCUR_ADAPTER_H

#include "ccu_all_to_all_v.h"
#include "ccu_kernel_proxy.h"

#include <array>

namespace HcclSim {
namespace CcuSt {

// taskArgs 布局 = kernel 形参前 10 槽（模拟 AIV CcuPrepareForConcurrentAllToAllV 写入的 xnData 段）：
// input/output/token/srcOffset/dstOffset/goSize×4/extAddr；两 mission 各一份（ext 指针不同）
constexpr uint32_t CONCUR_A2AV_TASK_ARG_COUNT = 10;
// ext 区每 rank 字段数（与 CCUMsgExtMultiJetty 布局一致）
constexpr uint32_t CONCUR_A2AV_EXT_FIELD_COUNT = 11;

struct ConcurA2avKernelConfig {
    uint32_t rankId{0};
    uint32_t rankSize{0};
    std::vector<ChannelHandle> channels;
    std::vector<uint32_t> jettyNums;
};

struct ConcurA2avMissionLaunchConfig {
    uint64_t inputAddress{0};
    uint64_t outputAddress{0};
    uint64_t token{0};
    uint64_t sourceBaseOffset{0};
    uint64_t destinationBaseOffset{0};
    uint64_t parameterAddress{0};
    std::array<uint64_t, 4> goSize{};
};

Result CaptureConcurrentAllToAllVMultiJettyKernel(void* kernelArg);
std::vector<uint64_t> PrepareConcurA2avTaskArgs(const ConcurA2avMissionLaunchConfig& config);
ScenarioData BuildConcurrentAllToAllVScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles);

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_ALL_TO_ALL_V_CONCUR_ADAPTER_H
