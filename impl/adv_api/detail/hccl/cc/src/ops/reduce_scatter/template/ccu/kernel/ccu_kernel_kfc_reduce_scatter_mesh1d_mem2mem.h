/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_H
#define HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_H

#include <vector>
#include <ios>
#include "utils.h"
#include "ccu_kernel.h"
#include "ccu_kernel_utils.h"
#include "ccu_kernel_alg_base.h"

namespace mc2_ops_hccl {
using namespace hcomm;

CcuResult CcuReduceScatterMesh1DMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable scratch,
    ccu::Variable currentRankSliceInputOffset, ccu::Variable chunkSize, ccu::Variable chunkLoopNum,
    ccu::Variable tailSize, ccu::Variable fullGoAddrOffset, ccu::Variable fullGoLoopParam,
    ccu::Variable fullGoParallelParam, ccu::Variable fullGoResidual, ccu::Variable tailGoAddrOffset,
    ccu::Variable tailGoLoopParam, ccu::Variable tailGoParallelParam, ccu::Variable tailGoResidual,
    const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId,
    const HcclDataType& dataType, const HcclDataType& outputType, const HcclReduceOp& reduceType);

} // namespace mc2_ops_hccl
#endif // HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_H
