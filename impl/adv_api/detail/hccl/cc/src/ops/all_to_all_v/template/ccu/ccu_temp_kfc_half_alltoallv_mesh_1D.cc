/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "channel.h"
#include "hccl_ccu_res.h"
#include "hccl_res.h"
#include "ccu_assist_pub.h"
#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"
#include "ccu_temp_kfc_half_alltoallv_mesh_1D.h"

namespace mc2_ops_hccl {

// varData 布局：[sizeA|sizeB] 与 [offsetA|offsetB] 各 2*rankSize 个 u64
constexpr uint32_t HA2AV_SLICE_SEG_NUM = 2;
// CalGoSize 返回四元组的槽位（与 kernel 端 LoadArgsFromMem 的 goSize0..3 一一对应）
constexpr uint32_t HA2AV_GO_IDX_ADDR_OFFSET = 0;
constexpr uint32_t HA2AV_GO_IDX_LOOP_ITER_NUM = 1;
constexpr uint32_t HA2AV_GO_IDX_PARALLEL_PARAM = 2;
constexpr uint32_t HA2AV_GO_IDX_RESIDUAL = 3;

// xn 参数帧填充：sendSize/sendOffset 地址 + goSize 四元组 + recvOffset
// （与 kernel 端 LoadArgsFromMem 的形参顺序一致）
static void FillKfcSubmitArgs(
    CcuKernelSubmitInfo& submitInfo, uint64_t userInAddr, uint64_t sendSizeAddr, uint64_t sendOffsetAddr,
    const std::vector<uint64_t>& goSize, uint64_t recvOffset)
{
    (void)FillCachedArgs(
        submitInfo, userInAddr, sendSizeAddr, sendOffsetAddr, goSize[HA2AV_GO_IDX_ADDR_OFFSET],
        goSize[HA2AV_GO_IDX_LOOP_ITER_NUM], goSize[HA2AV_GO_IDX_PARALLEL_PARAM], goSize[HA2AV_GO_IDX_RESIDUAL],
        recvOffset);
}

CcuTempKfcHalfAlltoAllVMesh1D::CcuTempKfcHalfAlltoAllVMesh1D(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuTempKfcMesh1DMem2MemBase(param, rankId, subCommRanks)
{}

CcuTempKfcHalfAlltoAllVMesh1D::~CcuTempKfcHalfAlltoAllVMesh1D() {}

HcclResult CcuTempKfcHalfAlltoAllVMesh1D::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));
    // 双 mission：两个同构 kernel 各映射为一个 mission，channel 复用同一份 Mesh1D 描述
    resourceRequest.ccuKernelNum.push_back(KFC_MAX_MISSION_NUM);

    CcuKernelInfo kernelInfo;
    CHK_SAFETY_FUNC_RET(
        strcpy_s(kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelHalfAlltoAllVMesh1D"));

    std::vector<HcclChannelDesc> channelDescs;
    if (topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS) {
        CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, channelDescs));
    } else {
        CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
            comm, param, topoInfo, subCommRanks_, channelDescs, CommTopo::COMM_TOPO_1DMESH));
        for (const auto& channel : channelDescs) {
            if (channel.channelProtocol != COMM_PROTOCOL_UBC_CTP) {
                HCCL_ERROR("[CcuTempKfcHalfAlltoAllVMesh1D][CalcRes] channelProtocol: %u", channel.channelProtocol);
                return HCCL_E_INTERNAL;
            }
        }
    }

    // CcuKernelKfcServer 在注册前替换该占位并复用这些 channel
    kernelInfo.channels = channelDescs;

    // 本端 window 基址：AlltoAllvWrite 契约不传 recvBuf，自留分片落点需建图期确定。
    // 必须取与 token 同源的 cclBuffer（token = GetTokenFromBuffInfo(cclBuffer)），
    // addr 与 token 不配对时 CCU 侧访问校验不通过；该地址还会广播给对端作 Write 目标。
    void* cclBufferAddr = nullptr;
    uint64_t cclBufferSize = 0;
    CHK_RET(HcclGetHcclBuffer(comm, &cclBufferAddr, &cclBufferSize));
    if (cclBufferAddr == nullptr) {
        HCCL_ERROR("[CcuTempKfcHalfAlltoAllVMesh1D][CalcRes] cclBuffer is null, local window base unavailable.");
        return HCCL_E_PARA;
    }
    kernelInfo.localWinAddr = PointerToAddr(cclBufferAddr);
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);

    CcuKernelInfo kernelInfo1 = kernelInfo;
    resourceRequest.ccuKernelInfos.push_back(kernelInfo1);

    HCCL_RUN_INFO(
        "[CcuTempKfcHalfAlltoAllVMesh1D::CalcRes] localWinAddr[0x%llx], cclBufferSize[%llu]",
        static_cast<unsigned long long>(kernelInfo.localWinAddr), static_cast<unsigned long long>(cclBufferSize));
    HCCL_DEBUG(
        "[CcuTempKfcHalfAlltoAllVMesh1D::CalcRes] channelDescs.size()=%llu, dimsize=%llu, ccuKernelInfos.size()=%llu",
        channelDescs.size(), subCommRanks_[0].size(), resourceRequest.ccuKernelInfos.size());
    return HcclResult::HCCL_SUCCESS;
}

HcclResult CcuTempKfcHalfAlltoAllVMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    buffInfo_ = templateDataParams.buffInfo;

    const uint64_t userInAddr = PointerToAddr(buffInfo_.inputPtr) + buffInfo_.inBuffBaseOff;
    const uint64_t myBufferAddr = PointerToAddr(buffInfo_.hcclBuff.addr) + buffInfo_.hcclBuffBaseOff;
    uint64_t token = 0;
    CHK_RET(GetToken(buffInfo_, token));

    // varData 起始为 sendSize 段（2*rankSize 个 u64），紧随 sendOffset 段
    const uint64_t varBaseAddr = PointerToAddr(const_cast<void*>(static_cast<const void*>(param.varData)));
    const uint64_t sliceSegBytes = static_cast<uint64_t>(templateRankSize_) * HA2AV_SLICE_SEG_NUM * sizeof(u64);
    const uint64_t sendSizeAddr = varBaseAddr;
    const uint64_t sendOffsetAddr = varBaseAddr + sliceSegBytes;

    const uint64_t recvOffset = templateDataParams.outputSliceStride * mySubCommRank_;

    uint64_t localSliceSize = templateDataParams.sliceSize;
    if (templateDataParams.tailSize != 0 && mySubCommRank_ == templateRankSize_ - 1) {
        localSliceSize = templateDataParams.tailSize;
    }
    if (localSliceSize == 0) {
        return HcclResult::HCCL_SUCCESS;
    }

    LoopGroupConfig config{};
    config.msInterleave = CCU_MS_INTERLEAVE;
    config.loopCount = CCU_MS_LOCAL_COPY_LOOP_COUNT;
    config.memSlice = CCU_MS_SIZE * LOCAL_COPY_MS_PER_LOOP;
    const auto goSize = CalGoSize(localSliceSize, config);

    // 双 mission 各推一个 submitInfo（kernelHandle 不同，xn 参数帧共用）
    CcuKernelSubmitInfo submitInfo{};
    submitInfo.kernelHandle = templateResource.ccuKernels[0];
    // xn 布局见 kernel 头注释；token 走 kfcServerArgs[5]，不占 xn 槽位
    (void)myBufferAddr;
    (void)token; // token 由 KFC Server 从 ctx.token 获取
    FillKfcSubmitArgs(submitInfo, userInAddr, sendSizeAddr, sendOffsetAddr, goSize, recvOffset);
    templateResource.submitInfos.push_back(submitInfo);
    if (templateResource.ccuKernels.size() > 1U) {
        CcuKernelSubmitInfo submitInfo1 = submitInfo;
        submitInfo1.kernelHandle = templateResource.ccuKernels[1];
        templateResource.submitInfos.push_back(submitInfo1);
    }
    return HcclResult::HCCL_SUCCESS;
}

u64 CcuTempKfcHalfAlltoAllVMesh1D::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}
} // namespace mc2_ops_hccl
