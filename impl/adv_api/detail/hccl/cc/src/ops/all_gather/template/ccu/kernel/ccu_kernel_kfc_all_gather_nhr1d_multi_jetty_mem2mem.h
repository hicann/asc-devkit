/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_KERNEL_KFC_ALL_GATHER_NHR_1D_MULTI_JETTY_MEM2MEM_H
#define HCCL_CCU_KERNEL_KFC_ALL_GATHER_NHR_1D_MULTI_JETTY_MEM2MEM_H

#include <map>
#include <vector>
#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

CcuResult CcuKfcAllGatherNHR1DMultiJettyMem2MemKernel(
    ccu::Variable inputAddr, ccu::Variable outputAddr, ccu::Variable tokenInfo, ccu::Variable sliceSize,
    ccu::Variable sliceSizePerJetty, ccu::Variable lastSliceSizePerJetty, ccu::Variable repeatNumInv,
    ccu::Variable inputSliceStride, ccu::Variable outputSliceStride, ccu::Variable inputRepeatStride,
    ccu::Variable outputRepeatStride, ccu::Variable isInputOutputEqual, ccu::Variable goSize0, ccu::Variable goSize1,
    ccu::Variable goSize2, ccu::Variable goSize3, const ChannelHandle channels[], uint32_t channelCount,
    uint32_t rankSize, uint32_t rankId, uint32_t jettyNum, const std::vector<KfcNhrStepInfo>& stepInfoVector,
    const std::map<uint32_t, uint32_t>& rank2ChannelIdx);

CcuResult CcuKfcParallelAllGatherNHR1DMultiJettyMem2MemKernel(
    ccu::Variable inputBase, ccu::Variable outputBase, ccu::Variable tokenInfo, ccu::Variable outputStride,
    ccu::Variable part0Size, ccu::Variable part1Size, ccu::Variable part1Offset, ccu::Variable meshPhaseDoneAddr,
    ccu::Variable nhrPhaseDoneAddr, ccu::Variable part0SliceSizePerJetty, ccu::Variable part0LastSliceSizePerJetty,
    ccu::Variable part1SliceSizePerJetty, ccu::Variable part1LastSliceSizePerJetty, ccu::Variable part0GoSize0,
    ccu::Variable part0GoSize1, ccu::Variable part0GoSize2, ccu::Variable part0GoSize3, ccu::Variable part1GoSize0,
    ccu::Variable part1GoSize1, ccu::Variable part1GoSize2, ccu::Variable part1GoSize3, const ChannelHandle channels[],
    uint32_t channelCount, uint32_t rankSizeLevel0, uint32_t rankIdxLevel0, uint32_t rankSizeLevel1,
    uint32_t rankIdxLevel1, uint32_t jettyNum, const std::vector<KfcNhrStepInfo>& stepInfoVector,
    const std::map<uint32_t, uint32_t>& rank2ChannelIdx);

} // namespace mc2_ops_hccl

#endif
