/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_ALL_TO_ALL_MESH1D_ADAPTER_H
#define HCCL_CCU_ALL_TO_ALL_MESH1D_ADAPTER_H

#include "ccu_kernel_proxy.h"
#include "ccu_fixture.h"

#include <array>

namespace HcclSim {
namespace CcuSt {

constexpr uint32_t ALL_TO_ALL_TASK_ARG_COUNT = 11;

struct AllToAllKernelConfig {
    uint32_t rankId{0};
    uint32_t rankSize{0};
    bool loadFromMem{true};
    std::vector<ChannelHandle> channels;
};

struct AllToAllLaunchConfig {
    uint64_t inputAddress{0};
    uint64_t outputAddress{0};
    uint64_t token{0};
    uint64_t sliceSize{0};
    uint64_t srcStride{0};
    uint64_t srcOffset{0};
    uint64_t dstOffset{0};
    std::array<uint64_t, 4> goSize{};
    uint32_t rankId{0};
};

Result CaptureAllToAllMesh1DKernel(void* kernelArg);
std::vector<uint64_t> PrepareAllToAllTaskArgs(const AllToAllLaunchConfig& config);
ScenarioData BuildMesh1DScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles);

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_ALL_TO_ALL_MESH1D_ADAPTER_H
