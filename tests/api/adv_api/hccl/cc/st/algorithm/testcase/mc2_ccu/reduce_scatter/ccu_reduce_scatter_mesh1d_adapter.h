/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_REDUCE_SCATTER_MESH1D_ADAPTER_H
#define HCCL_CCU_REDUCE_SCATTER_MESH1D_ADAPTER_H

#include "ccu_fixture.h"
#include "ccu_kernel_proxy.h"

#include <array>
#include <vector>

namespace HcclSim {
namespace CcuSt {

// 与 CcuReduceScatterMesh1DMem2MemKernel 的16个 ccu::Variable 形参一一对应
// （token 亦占一槽，KFC 直调模式下由 taskArgs 提供）。
constexpr uint32_t REDUCE_SCATTER_MESH_TASK_ARG_COUNT = 16;

struct ReduceScatterMeshKernelConfig {
    uint32_t rankId{0};
    uint32_t rankSize{0};
    HcclDataType dataType{HCCL_DATA_TYPE_FP16};
    HcclDataType outputType{HCCL_DATA_TYPE_FP16};
    HcclReduceOp reduceType{HCCL_REDUCE_SUM};
    std::vector<ChannelHandle> channels;
};

struct ReduceScatterMeshLaunchConfig {
    uint64_t inputAddress{0};
    uint64_t outputAddress{0};
    uint64_t token{0};
    uint64_t scratchAddress{0};
    uint64_t currentRankSliceInputOffset{0};
    uint64_t chunkSize{0};
    uint64_t chunkLoopNum{0};
    uint64_t tailSize{0};
    std::array<uint64_t, 4> fullGoSize{};
    std::array<uint64_t, 4> tailGoSize{};
};

Result CaptureCcuKfcReduceScatterMesh1DKernel(void* kernelArg);
std::array<uint64_t, 4> CalculateReduceScatterMeshGoSize(uint64_t size);
std::vector<uint64_t> PrepareReduceScatterMeshTaskArgs(const ReduceScatterMeshLaunchConfig& config);
ScenarioData BuildReduceScatterMeshScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles);

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_REDUCE_SCATTER_MESH1D_ADAPTER_H
