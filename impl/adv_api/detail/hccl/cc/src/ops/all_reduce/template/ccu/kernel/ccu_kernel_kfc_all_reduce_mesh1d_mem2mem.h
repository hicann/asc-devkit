/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_KERNEL_KFC_ALL_REDUCE_MESH_1D_MEM2MEM_H
#define HCCL_CCU_KERNEL_KFC_ALL_REDUCE_MESH_1D_MEM2MEM_H

#include <ios>
#include "utils.h"
#include "ccu_kernel.h"
#include "ccu_kernel_utils.h"
#include "ccu_kernel_alg_base.h"

namespace mc2_ops_hccl {
using namespace hcomm;

struct CcuKernelArgAllReduceKfc : CcuKernelArgBase {
    uint64_t rankSize;
    uint32_t rankId;
    OpParam opParam;
    std::vector<std::vector<uint32_t>> subCommRanks;
};

struct KfcAllReduceMesh1DMem2MemContext : CcuKernelCtxBase {
    const ChannelHandle* channels{nullptr};
    uint32_t channelCount{0};
    uint32_t rankSize{0};
    uint32_t rankId{0};

    HcclDataType dataType;
    HcclDataType outputDataType;
    HcclReduceOp reduceOp;

    std::vector<ccu::variable> input;
    std::vector<ccu::variable> output;
    std::vector<ccu::variable> token;
    ccu::variable myScratch;
    ccu::variable currentRankSliceInputOffset;
    ccu::variable currentRankSliceOutputOffset;
    ccu::variable normalSliceSize;
    ccu::variable lastSliceSize;
    ccu::variable mySliceSize;
    ccu::variable sliceOffset;
    ccu::variable isInputOutputEqual;
    ccu::variable sliceSize;
    GroupOpSizeVars goSize;
    ccu::variable chunkSize;
    ccu::variable currentSliceSize;
    ccu::variable tailSize;
    ccu::variable chunkLoopNum;
    GroupOpSizeVars fullGoSize;
    GroupOpSizeVars tailGoSize;

    std::vector<ccu::event> events;

    ccu::local_addr srcMem;
    ccu::local_addr localDstMem;
    ccu::remote_addr remoteDstMem;
    std::vector<ccu::remote_addr> reduceScatterSrc;
    std::vector<ccu::local_addr> reduceScatterDst;
};

CcuResult CcuKfcAllReduceMesh1DMem2MemKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable scratch,
    ccu::variable currentRankSliceInputOffset, ccu::variable currentRankSliceOutputOffset,
    ccu::variable normalSliceSize, ccu::variable lastSliceSize, ccu::variable mySliceSize, ccu::variable sliceOffset,
    ccu::variable isInputOutputEqual, ccu::variable goSize0, ccu::variable goSize1, ccu::variable goSize2,
    ccu::variable goSize3, ccu::variable chunkSize, ccu::variable tailSize, ccu::variable chunkLoopNum,
    ccu::variable fullGoSize0, ccu::variable fullGoSize1, ccu::variable fullGoSize2, ccu::variable fullGoSize3,
    ccu::variable tailGoSize0, ccu::variable tailGoSize1, ccu::variable tailGoSize2, ccu::variable tailGoSize3,
    const ChannelHandle channels[], uint32_t channelCount, uint32_t rankSize, uint32_t rankId,
    const HcclDataType& dataType, const HcclDataType& outputType, const HcclReduceOp& reduceType);

CcuResult ReduceLoopGroupV1(KfcAllReduceMesh1DMem2MemContext& ctx, ccu::local_addr srcOrg);
} // namespace mc2_ops_hccl
#endif // HCCL_CCU_KERNEL_KFC_ALL_REDUCE_MESH_1D_MEM2MEM_H
