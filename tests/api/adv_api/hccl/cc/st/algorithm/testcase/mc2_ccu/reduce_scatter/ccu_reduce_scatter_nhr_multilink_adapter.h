/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_REDUCE_SCATTER_NHR_MULTILINK_ADAPTER_H
#define HCCL_CCU_REDUCE_SCATTER_NHR_MULTILINK_ADAPTER_H

#include "ccu_fixture.h"
#include "ccu_kernel_proxy.h"
#include "ccu_kernel_kfc_reduce_scatter_nhr1d_multi_jetty_mem2mem.h"

#include <array>
#include <map>
#include <vector>

namespace HcclSim {
namespace CcuSt {

// 与 CcuKfcReduceScatterNHR1DMultiJettyMem2MemKernel 的 10 个 ccu::Variable 形参一一对应
// （token 亦占一槽，KFC 直调模式下由 taskArgs 提供）。
constexpr uint32_t REDUCE_SCATTER_NHR_ML_TASK_ARG_COUNT = 10;

struct ReduceScatterNhrMultiLinkKernelConfig {
    uint32_t rankId{0}; // mySubCommRank
    uint32_t rankSize{0};
    uint32_t jettyNum{1};
    HcclDataType dataType{HCCL_DATA_TYPE_FP16};
    HcclDataType outputType{HCCL_DATA_TYPE_FP16};
    HcclReduceOp reduceType{HCCL_REDUCE_SUM};
    std::vector<ChannelHandle> channels;
    std::vector<mc2_ops_hccl::KfcNhrStepInfo> stepInfoVector;
    std::map<uint32_t, uint32_t> rank2ChannelIdx;
};

struct ReduceScatterNhrMultiLinkLaunchConfig {
    uint64_t inputAddress{0};
    uint64_t outputAddress{0};
    uint64_t token{0};
    uint64_t sliceSize{0};          // per-rank slice 字节数
    uint64_t inputSliceStride{0};   // strideCount=0 时等于 sliceSize
    uint64_t sliceOneJettySize{0};  // portNum=1 时不被 kernel 消费，与 AIV prepare 同式计算
    uint64_t sliceLastJettySize{0}; // portNum=1 时恒等于 sliceSize
    uint64_t inputRepeatStride{0};  // sole executor 恒 0
    uint64_t outputRepeatStride{0}; // sole executor 恒 0
};

Result CaptureCcuKfcReduceScatterNhrMultiLinkKernel(void* kernelArg);
std::vector<uint64_t> PrepareReduceScatterNhrMultiLinkTaskArgs(const ReduceScatterNhrMultiLinkLaunchConfig& config);
ScenarioData BuildReduceScatterNhrMultiLinkScenario(
    const CcuStScenario& scenario, const std::vector<KernelHandle>& handles);

} // namespace CcuSt
} // namespace HcclSim

#endif
