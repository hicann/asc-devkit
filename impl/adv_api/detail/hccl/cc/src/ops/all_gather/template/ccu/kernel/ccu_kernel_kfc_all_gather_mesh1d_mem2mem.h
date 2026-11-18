/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_KERNEL_KFC_ALL_GATHER_MESH_1D_MEM2MEM_H
#define HCCL_CCU_KERNEL_KFC_ALL_GATHER_MESH_1D_MEM2MEM_H

#include "ccu_kernel_alg_base.h"

namespace mc2_ops_hccl {

// 每个 repeat 批次最多展开 4 次 Write/LocalCopy/Wait 三阶段流水。
constexpr uint32_t AG_M2M_UNROLL_NUM = 4;

CcuResult CcuKfcAllGatherMesh1DMem2MemKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable outputOffset,
    ccu::variable sliceSize, ccu::variable goSize0, ccu::variable goSize1, ccu::variable goSize2, ccu::variable goSize3,
    ccu::variable currentRankSliceInputOffset, ccu::variable tmpRepeatNum, ccu::variable inputRepeatStride,
    ccu::variable outputRepeatStride, ccu::variable lastSliceSize, ccu::variable isInputOutputEqual,
    const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId);

CcuResult CcuKfcParallelAllGatherMesh1DMem2MemKernel(
    ccu::variable inputBase, ccu::variable outputBase, ccu::variable tokenInfo, ccu::variable outputStride,
    ccu::variable part0Size, ccu::variable part1Size, ccu::variable part1Offset, ccu::variable meshPhaseDoneAddr,
    ccu::variable nhrPhaseDoneAddr, ccu::variable part0GoSize0, ccu::variable part0GoSize1, ccu::variable part0GoSize2,
    ccu::variable part0GoSize3, ccu::variable part1GoSize0, ccu::variable part1GoSize1, ccu::variable part1GoSize2,
    ccu::variable part1GoSize3, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSizeLevel0,
    uint32_t rankIdxLevel0, uint32_t rankSizeLevel1, uint32_t rankIdxLevel1);

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_KERNEL_KFC_ALL_GATHER_MESH_1D_MEM2MEM_H
