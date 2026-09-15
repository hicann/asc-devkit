/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_ALL_TO_ALL_MJ_ADAPTER_H
#define HCCL_CCU_ALL_TO_ALL_MJ_ADAPTER_H

#include "ccu_kernel_proxy.h"
#include "ccu_fixture.h"

#include <array>

namespace HcclSim {
namespace CcuSt {

// 基础 11 个下发参数 + sliceSizePerJetty + lastSliceSizePerJetty（hccl_ccu_v0_prepare.h
// CcuPrepareForAllToAllMultiJetty）
constexpr uint32_t ALL_TO_ALL_MJ_TASK_ARG_COUNT = 13;

struct AllToAllMjKernelConfig {
    uint32_t rankId{0};
    uint32_t rankSize{0};
    uint32_t jettyNum{1};
    std::vector<ChannelHandle> channels;
};

struct AllToAllMjLaunchConfig {
    uint64_t inputAddress{0};
    uint64_t outputAddress{0};
    uint64_t token{0};
    uint64_t sliceSize{0};
    uint64_t srcStride{0};
    uint64_t srcOffset{0};
    uint64_t dstOffset{0};
    std::array<uint64_t, 4> goSize{};
    uint64_t sliceSizePerJetty{0};
    uint64_t lastSliceSizePerJetty{0};
};

Result CaptureAllToAllMjKernel(void* kernelArg);
std::vector<uint64_t> PrepareAllToAllMjTaskArgs(const AllToAllMjLaunchConfig& config);
ScenarioData BuildMjScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles);

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_ALL_TO_ALL_MJ_ADAPTER_H
