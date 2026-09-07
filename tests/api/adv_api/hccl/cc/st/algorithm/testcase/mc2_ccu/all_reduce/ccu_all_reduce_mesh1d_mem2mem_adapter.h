/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_ALL_REDUCE_MESH1D_MEM2MEM_ADAPTER_H
#define HCCL_CCU_ALL_REDUCE_MESH1D_MEM2MEM_ADAPTER_H

#include "ccu_kernel_proxy.h"
#include "ccu_fixture.h"

#include <array>

namespace HcclSim {
namespace CcuSt {

constexpr uint32_t ALL_REDUCE_TASK_ARG_COUNT = 26;

struct AllReduceKernelConfig {
    uint32_t rankId{0};
    uint32_t rankSize{0};
    HcclDataType dataType{HCCL_DATA_TYPE_FP16};
    HcclDataType outputType{HCCL_DATA_TYPE_RESERVED};
    HcclReduceOp reduceOp{HCCL_REDUCE_SUM};
    std::vector<ChannelHandle> channels;
};

struct AllReduceLaunchConfig {
    uint64_t inputAddress{0};
    uint64_t outputAddress{0};
    uint64_t token{0};
    uint64_t scratchAddress{0};
    uint64_t currentRankSliceInputOffset{0};
    uint64_t currentRankSliceOutputOffset{0};
    uint64_t normalSliceSize{0};
    uint64_t lastSliceSize{0};
    uint64_t mySliceSize{0};
    uint64_t sliceOffset{0};
    uint64_t isInputOutputEqual{0};
    std::array<uint64_t, 4> goSize{};
    uint64_t chunkSize{0};
    uint64_t tailSize{0};
    uint64_t chunkLoopNum{0};
    std::array<uint64_t, 4> fullGoSize{};
    std::array<uint64_t, 4> tailGoSize{};
};

Result CaptureAllReduceMeshMem2Mem1DKernel(void* kernelArg);
std::vector<uint64_t> PrepareAllReduceTaskArgs(const AllReduceLaunchConfig& config);
ScenarioData BuildAllReduceMesh1DScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles);

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_ALL_REDUCE_MESH1D_MEM2MEM_ADAPTER_H
