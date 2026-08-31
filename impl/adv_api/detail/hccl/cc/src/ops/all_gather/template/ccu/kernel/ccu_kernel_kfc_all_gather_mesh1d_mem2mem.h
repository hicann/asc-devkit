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

// 每个 repeat 批次最多展开 16 次 Write/LocalCopy/Wait 三阶段流水。
constexpr uint32_t AG_M2M_UNROLL_NUM = 1;

CcuResult CcuKfcAllGatherMesh1DMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable outputOffset,
    ccu::Variable sliceSize, ccu::Variable goSize0, ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3,
    ccu::Variable currentRankSliceInputOffset, ccu::Variable tmpRepeatNum, ccu::Variable inputRepeatStride,
    ccu::Variable outputRepeatStride, ccu::Variable lastSliceSize, ccu::Variable isInputOutputEqual,
    const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId);

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_KERNEL_KFC_ALL_GATHER_MESH_1D_MEM2MEM_H
