/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_KERNEL_ALL_TO_ALL_MESH1D_MULTI_JETTY_H
#define HCCL_CCU_KERNEL_ALL_TO_ALL_MESH1D_MULTI_JETTY_H

#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

CcuResult CcuAllToAllMesh1DMultiJettyKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable sliceSize,
    ccu::variable srcStride, ccu::variable srcOffset, ccu::variable dstOffset, ccu::variable goSize0,
    ccu::variable goSize1, ccu::variable goSize2, ccu::variable goSize3, ccu::variable sliceSizePerJetty,
    ccu::variable lastSliceSizePerJetty, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize,
    uint32_t rankId, uint32_t jettyNum);
} // namespace mc2_ops_hccl

#endif
