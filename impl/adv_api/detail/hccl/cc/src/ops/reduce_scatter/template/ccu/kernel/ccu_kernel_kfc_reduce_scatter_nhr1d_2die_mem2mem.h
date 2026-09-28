/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_NHR_1D_2DIE_MEM2MEM_H
#define HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_NHR_1D_2DIE_MEM2MEM_H

#include <map>
#include <vector>
#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

// KFC 版 ReduceScatter NHR 1D 2Die Mem2Mem kernel（CcuSchedReduceScatterSoleNHR）：算法逻辑与
// hccl CcuReduceScatterNHR1DMem2MemKernel 逐行对照，参数改为 dispatch 显式传参。
// 双 die 时每个 die 一个 kernel 实例（axisId 区分），数据按 dieSplitRatio 不相交切分；
// die0/die1 尺寸与 goSize 为 AIV prepare 运行期计算（MC2 device-driven），经 xnData 传入。
// 21 个 Variable 形参与 hccl 模板 KernelRun 的 taskArgs 布局逐槽一致。
CcuResult CcuKfcReduceScatterNHR1D2DieMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable die0Size,
    ccu::Variable die1Size, ccu::Variable die0LastSliceSize, ccu::Variable die1LastSliceSize,
    ccu::Variable inputSliceStride, ccu::Variable currentRankSliceOutputOffset, ccu::Variable inputRepeatStride,
    ccu::Variable outputRepeatStride, ccu::Variable repeatNumVar, ccu::Variable isInputOutputEqual,
    ccu::Variable goSizeNormalAddrOffset, ccu::Variable goSizeNormalLoopParam, ccu::Variable goSizeNormalParallelParam,
    ccu::Variable goSizeNormalResidual, ccu::Variable goSizeLastAddrOffset, ccu::Variable goSizeLastLoopParam,
    ccu::Variable goSizeLastParallelParam, ccu::Variable goSizeLastResidual, const ChannelHandle channels[],
    uint32_t channelCount, uint32_t rankSize, uint32_t rankId, uint32_t axisId, uint32_t axisSize,
    const HcclDataType& dataType, const HcclDataType& outputType, const HcclReduceOp& reduceType,
    const std::vector<KfcNhrStepInfo>& stepInfoVector, const std::map<uint32_t, uint32_t>& rank2ChannelIdx);

} // namespace mc2_ops_hccl

#endif
