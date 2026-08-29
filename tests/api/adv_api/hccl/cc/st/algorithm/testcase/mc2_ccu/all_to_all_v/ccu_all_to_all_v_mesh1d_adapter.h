/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_ALL_TO_ALL_V_MESH1D_ADAPTER_H
#define HCCL_CCU_ALL_TO_ALL_V_MESH1D_ADAPTER_H

#include "ccu_fixture.h"
#include "ccu_kernel_proxy.h"

#include <array>

namespace HcclSim {
namespace CcuSt {

constexpr uint32_t ALL_TO_ALL_V_TASK_ARG_COUNT = 10;
constexpr uint32_t ALL_TO_ALL_V_EXT_FIELD_COUNT = 4;

struct AllToAllVKernelConfig {
    uint32_t rankId{0};
    uint32_t rankSize{0};
    std::vector<ChannelHandle> channels;
};

struct AllToAllVLaunchConfig {
    uint64_t inputAddress{0};
    uint64_t outputAddress{0};
    uint64_t token{0};
    uint64_t sourceBaseOffset{0};
    uint64_t destinationBaseOffset{0};
    uint64_t parameterAddress{0};
    std::array<uint64_t, 4> goSize{};
};

Result CaptureAllToAllVMesh1DKernel(void* kernelArg);
std::array<uint64_t, 4> CalculateAllToAllVGoSize(uint64_t size);
std::vector<uint64_t> PrepareAllToAllVTaskArgs(const AllToAllVLaunchConfig& config);
ScenarioData BuildAllToAllVMesh1DScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles);

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_ALL_TO_ALL_V_MESH1D_ADAPTER_H
