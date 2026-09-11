/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_NHR_1D_MULTI_JETTY_MEM2MEM_H
#define HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_NHR_1D_MULTI_JETTY_MEM2MEM_H

#include <map>
#include <vector>
#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

// KFC 版 ReduceScatter NHR MultiJetty Mem2Mem kernel：算法逻辑与 hccl
// CcuReduceScatterNhrMem2Mem1DMultiJettyKernel 逐行对照，参数改为 dispatch 显式传参。
CcuResult CcuKfcReduceScatterNHR1DMultiJettyMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable sliceSize,
    ccu::Variable inputSliceStride, ccu::Variable sliceOneJettySize, ccu::Variable sliceLastJettySize,
    ccu::Variable repeatNumInv, ccu::Variable inputRepeatStride, ccu::Variable outputRepeatStride,
    const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId, uint32_t jettyNum,
    const HcclDataType& dataType, const HcclDataType& outputType, const HcclReduceOp& reduceType,
    const std::vector<KfcNhrStepInfo>& stepInfoVector, const std::map<uint32_t, uint32_t>& rank2ChannelIdx);

} // namespace mc2_ops_hccl

#endif
