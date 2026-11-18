/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_KERNEL_ALL_TO_ALL_V_MESH_1D_H
#define HCCL_CCU_KERNEL_ALL_TO_ALL_V_MESH_1D_H

#include "ccu_kernel_alg_base.h"

namespace mc2_ops_hccl {

struct A2AsingleSendRecvInfo {
    ccu::variable tailSize;
    ccu::variable loopNum;
    ccu::variable sendOffset;
    ccu::variable recvOffset;
    GroupOpSizeVars tailGoSize;
};

struct AlltoAllVMesh1DContext : CcuKernelCtxBase {
    const ChannelHandle* channels;
    uint32_t channelCount;
    uint32_t rankSize;
    uint32_t rankId;
    bool loadFromMem = false;

    std::vector<ccu::variable> input;
    std::vector<ccu::variable> output;
    std::vector<ccu::variable> token;
    ccu::variable sliceSize;
    ccu::variable srcStride;
    ccu::variable srcOffset;
    ccu::variable dstOffset;
    ccu::variable groupOpSize;
    ccu::variable a2avXnAddr;
    ccu::variable xnLength;
    ccu::variable completedRankCount;
    ccu::variable xnMaxTransportSize;
    GroupOpSizeVars xnMaxTransportGoSize;
    std::vector<A2AsingleSendRecvInfo> sendRecvInfo;
    uint16_t selfBit{0};
    uint16_t allBit{0};
    uint16_t allOtherBit{0};
    ccu::local_addr myDst;
    std::vector<ccu::remote_addr> dst;
    std::vector<ccu::local_addr> src;
    // rankSize>16 时 uint16_t event mask 不够：按 16 rank 一组拆多个 event，
    // 每 rank 预计算 (event 序号, 组内 mask)，避免 kernel 主循环里重复除法/移位
    std::vector<ccu::event> events;
    std::vector<uint16_t> eventMaskOfRank;
    std::vector<uint32_t> eventIdxOfRank;
    ccu::variable flag; // 用以判断是否是第一次重复
    ccu::variable xnConst1;
};

CcuResult CcuAlltoAllVMesh1DKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable srcOffset,
    ccu::variable dstOffset, ccu::variable extAddr, ccu::variable goSize1, ccu::variable goSize2, ccu::variable goSize3,
    ccu::variable goSize4, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId);

} // namespace mc2_ops_hccl
#endif // HCCL_CCU_KERNEL_ALL_TO_ALL_V_MESH_1D_H
