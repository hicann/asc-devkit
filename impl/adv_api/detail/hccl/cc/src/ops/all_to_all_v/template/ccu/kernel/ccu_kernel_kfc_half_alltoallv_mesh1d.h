/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_KERNEL_KFC_HALF_ALL_TO_ALL_V_MESH_1D_H
#define HCCL_CCU_KERNEL_KFC_HALF_ALL_TO_ALL_V_MESH_1D_H

#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

CcuResult CcuKfcHalfAlltoAllVMesh1DKernel(
    ccu::Variable userInAddr, ccu::Variable sendSizeAddr, ccu::Variable sendOffsetAddr, ccu::Variable goSize0,
    ccu::Variable goSize1, ccu::Variable goSize2, ccu::Variable goSize3, ccu::Variable recvOffset,
    ccu::Variable tokenInfo, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId,
    uint64_t localWinAddr, ccu::Variable missionNum, ccu::Variable missionIndex, uint32_t hostMissionIndex,
    uint32_t hostMissionNum);

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_KERNEL_KFC_HALF_ALL_TO_ALL_V_MESH_1D_H
