/**
 *  * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *  * This program is free software: you can redistribute it and/or modify it under the terms and conditions of
 *  * CANN Open Software License Agreement Version 2.0 (the "License").
 *  * Please refer to the License for details. You may not use this file except in compliance with the License.
 *  * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 *  * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *  * See LICENSE in the root of the software repository for the full text of the License.
 *  */
#ifndef HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_PEER_ONLY_A6_H
#define HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_PEER_ONLY_A6_H

#ifndef CCU_DATAPLANE_HCOMM
#define CCU_DATAPLANE_HCOMM
#endif

#include <ios>
#include "utils.h"
#include "ccu_kernel.h"
#include "ccu_kernel_utils.h"
#include "ccu_kernel_alg_base.h"

namespace mc2_ops_hccl {
using namespace hcomm;

// hcomm（V2/A6）数据面版 PeerOnly ReduceScatter kernel（第 6 个 twin）：与 A5 snake 版
// 同名不同 mangling（形参 ccu::Variable vs ccu::variable），实现为拆分前基线的逐行拷贝。
CcuResult CcuReduceScatterMeshMem2Mem1DPeerOnlyKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo,
    ccu::Variable currentRankSliceInputOffset, ccu::Variable skipLocalRankCopy, ccu::Variable tailSize,
    ccu::Variable chunkLoopNum, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize,
    uint32_t rankId, const HcclDataType& dataType, const HcclReduceOp& reduceType);

} // namespace mc2_ops_hccl
#endif // HCCL_CCU_KERNEL_KFC_REDUCE_SCATTER_MESH_1D_MEM2MEM_PEER_ONLY_A6_H
