/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_KERNEL_ALL_TO_ALL_V_MESH1D_MULTI_JETTY_H
#define HCCL_CCU_KERNEL_ALL_TO_ALL_V_MESH1D_MULTI_JETTY_H

#include "ccu_kernel_alg_base.h"

namespace mc2_ops_hccl {

// 照 hccl CcuKernelArgAllToAllVMesh1DMultiJetty 移植；jettyNums 为注册期 host 值（emission 期读取）
struct CcuKernelArgAllToAllVMesh1DMultiJetty : CcuKernelArgBase {
    uint64_t rankSize = 0;
    uint32_t rankId = 0;
    OpParam opParam;
    std::vector<std::vector<uint32_t>> subCommRanks;
    std::vector<uint32_t> jettyNums;
};

// GM ext 区每 rank 字段数（11 个 u64，与 AIV 侧 CCUMsgExtMultiJetty 布局一致）
constexpr uint32_t A2AV_MJ_EXT_FIELD_NUM = 11U;

struct A2AVMjSendRecvInfo {
    ccu::variable sliceSize;         // 数据块切片大小
    ccu::variable tailSliceSize;     // 数据块切片尾片大小
    ccu::variable lastSliceSize;     // 尾数据块切片大小
    ccu::variable lastTailSliceSize; // 尾数据块切片尾片大小
    ccu::variable loopNum;
    ccu::variable sendOffset;
    ccu::variable recvOffset;
    GroupOpSizeVars tailGoSize;
};

struct AllToAllVMesh1DMultiJettyContext : CcuKernelCtxBase {
    const ChannelHandle* channels = nullptr;
    uint32_t channelCount = 0;
    uint32_t rankSize = 0;
    uint32_t rankId = 0;
    const uint32_t* jettyNums = nullptr;

    std::vector<ccu::variable> input;
    std::vector<ccu::variable> output;
    std::vector<ccu::variable> token;
    ccu::variable srcOffset;
    ccu::variable dstOffset;
    std::vector<A2AVMjSendRecvInfo> sendRecvInfo;

    ccu::variable completedRankCount;
    ccu::variable xnMaxTransportSize;
    GroupOpSizeVars xnMaxTransportGoSize; // 本地整块拷贝 goSize
    ccu::variable xnConst1;

    std::vector<ccu::local_addr> src;
    ccu::local_addr myDst;
    std::vector<ccu::remote_addr> remoteDst;
    std::vector<ccu::event> eventList;
};

// KFC 适配：hccl CcuAllToAllVMesh1DMultiJettyKernel 的 CcuKernelArg 单参数改为显式参数；
// per-rank sendRecvInfo（11 字段）从 GM ext 区读取（AIV prepare 写入），其余逐行对照 hccl。
CcuResult CcuAllToAllVMesh1DMultiJettyKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable srcOffset,
    ccu::variable dstOffset, ccu::variable goSize1, ccu::variable goSize2, ccu::variable goSize3, ccu::variable goSize4,
    ccu::variable extAddr, const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId,
    const uint32_t jettyNums[]);

} // namespace mc2_ops_hccl

#endif // HCCL_CCU_KERNEL_ALL_TO_ALL_V_MESH1D_MULTI_JETTY_H
