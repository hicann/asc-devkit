/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_REDUCE_SCATTER_NHR_2DIE_ADAPTER_H
#define HCCL_CCU_REDUCE_SCATTER_NHR_2DIE_ADAPTER_H

#include "ccu_fixture.h"
#include "ccu_kernel_proxy.h"
#include "ccu_kernel_kfc_reduce_scatter_nhr1d_2die_mem2mem.h"

#include <array>
#include <map>
#include <vector>

namespace HcclSim {
namespace CcuSt {

// 与 CcuKfcReduceScatterNHR1D2DieMem2MemKernel 的 21 个 ccu::Variable 形参一一对应
// （token 亦占一槽，KFC 直调模式下由 taskArgs 提供）。
constexpr uint32_t REDUCE_SCATTER_NHR_2DIE_TASK_ARG_COUNT = 29;

// goSize 口径与 hccl 模板/AIV prepare 一致：(loopCount=8, memSlice=4096*8=32768)
constexpr uint64_t NHR_2DIE_MEM_SLICE = 32768ULL;
constexpr uint64_t NHR_2DIE_LOOP_SIZE = 8ULL * NHR_2DIE_MEM_SLICE;

struct ReduceScatterNhr2DieKernelConfig {
    uint32_t rankId{0}; // mySubCommRank
    uint32_t rankSize{0};
    uint32_t axisId{0}; // die 维编号
    uint32_t axisSize{1};
    HcclDataType dataType{HCCL_DATA_TYPE_FP16};
    HcclDataType outputType{HCCL_DATA_TYPE_FP16};
    HcclReduceOp reduceType{HCCL_REDUCE_SUM};
    std::vector<ChannelHandle> channels;
    std::vector<mc2_ops_hccl::KfcNhrStepInfo> stepInfoVector;
    std::map<uint32_t, uint32_t> rank2ChannelIdx;
};

struct ReduceScatterNhr2DieLaunchConfig {
    uint64_t inputAddress{0};
    uint64_t outputAddress{0};
    uint64_t token{0};
    uint64_t die0Size{0};
    uint64_t die1Size{0};
    uint64_t die0LastSliceSize{0};
    uint64_t die1LastSliceSize{0};
    uint64_t inputSliceStride{0};
    uint64_t currentRankSliceOutputOffset{0}; // hccl executor outputStride=0，恒 0
    uint64_t inputRepeatStride{0};            // sole executor 恒 0
    uint64_t outputRepeatStride{0};           // sole executor 恒 0
    uint64_t isInputOutputEqual{0};
    std::array<uint64_t, 4> goSizeNormal{}; // die0 组
    std::array<uint64_t, 4> goSizeLast{};   // die0 组
    std::array<uint64_t, 4> die1GoSizeNormal{};
    std::array<uint64_t, 4> die1GoSizeLast{};
};

Result CaptureCcuKfcReduceScatterNhr2DieKernel(void* kernelArg);
std::vector<uint64_t> PrepareReduceScatterNhr2DieTaskArgs(const ReduceScatterNhr2DieLaunchConfig& config);
ScenarioData BuildReduceScatterNhr2DieScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles);

} // namespace CcuSt
} // namespace HcclSim

#endif
