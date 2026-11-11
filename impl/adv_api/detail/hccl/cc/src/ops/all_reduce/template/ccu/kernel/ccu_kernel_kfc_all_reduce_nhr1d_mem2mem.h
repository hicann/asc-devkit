/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_KERNEL_KFC_ALL_REDUCE_NHR_1D_MEM2MEM_H
#define HCCL_CCU_KERNEL_KFC_ALL_REDUCE_NHR_1D_MEM2MEM_H

#include <map>
#include <vector>
#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

// KFC 版 AllReduce NHR 1D Mem2Mem kernel（CcuSchedAllReduceSoleNHR）：算法逻辑与
// hccl CcuAllReduceNHR1DKernel 逐行对照，参数改为 dispatch 显式传参。
// 双 die 时每个 die 一个 kernel 实例（axisId 区分），数据按 dieSplitRatio 不相交切分；
// die0/die1 尺寸与分片尺寸为 AIV prepare 运行期计算（MC2 device-driven），经 xnData 传入。
// outputType 缺省（RESERVED）时归一化为 dataType；NHR 远端 reduce 仅携带单一类型，故要求两者一致。
CcuResult CcuKernelKfcAllReduceNHR1DMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable isInputOutputEqual,
    ccu::Variable die0Size, ccu::Variable die1Size, ccu::Variable die0SliceSize, ccu::Variable die1SliceSize,
    ccu::Variable die0LastSliceSize, ccu::Variable die1LastSliceSize, const ChannelHandle channels[],
    uint32_t channelCount, uint32_t rankSize, uint32_t rankId, uint32_t axisId, uint32_t axisSize,
    const HcclDataType& dataType, const HcclDataType& outputType, const HcclReduceOp& reduceType,
    const std::vector<KfcNhrStepInfo>& stepInfoVector, const std::map<uint32_t, uint32_t>& rank2ChannelIdx);

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_KERNEL_KFC_ALL_REDUCE_NHR_1D_MEM2MEM_H
