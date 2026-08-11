/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_v2_reduce_scatter_sequence_executor.h"
#include "ins_temp_reduce_scatter_mesh_1D.h"
#include "ins_temp_reduce_scatter_mesh_1d_dpu.h"

namespace mc2_ops_hccl {

// 序列执行器需要的层级数
constexpr u32 SEQUENCE_EXECUTOR_LEVEL_NUM = 2;

// ! 已经编码完成
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
InsV2ReduceScatterSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InsV2ReduceScatterSequenceExecutor()
{}

// ! 已编码完成
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InitCommInfo(
    const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    devType_ = topoInfo->deviceType;
    reduceOp_ = param.reduceType;
    dataType_ = param.DataDes.dataType;
    dataCount_ = param.DataDes.count;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];

    algHierarchyInfo_ = algHierarchyInfo;
    HCCL_INFO(
        "[InsV2ReduceScatterSequenceExecutor][InitCommInfo] myRank [%u], rankSize [%u], devType [%u], redOp [%u], "
        "dataType [%u] dataTypeSize [%u]",
        myRank_, rankSize_, devType_, reduceOp_, dataType_, dataTypeSize_);
    return HCCL_SUCCESS;
}

// ! 已编码完成，实例化实际执行以来AutoMatchMeshNhr这个类的实现
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    devType_ = topoInfo->deviceType;
    // 使用topo match计算AlgHierarchyInfoForAllLevel
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

// ! 已编码完成
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    HCCL_DEBUG("[InsV2ReduceScatterSequenceExecutor]CalcRes start");
    // 初始化一些基本成员变量
    InitCommInfo(param, topoInfo, algHierarchyInfo);
    if (algHierarchyInfo.infos.size() != SEQUENCE_EXECUTOR_LEVEL_NUM) {
        HCCL_ERROR("algHierarchyInfo size should be %u", SEQUENCE_EXECUTOR_LEVEL_NUM);
        return HCCL_E_INTERNAL;
    }
    std::shared_ptr<InsAlgTemplate0> interTempAlg =
        std::make_shared<InsAlgTemplate0>(param, myRank_, algHierarchyInfo.infos[0]);
    std::shared_ptr<InsAlgTemplate1> intraTempAlg =
        std::make_shared<InsAlgTemplate1>(param, myRank_, algHierarchyInfo.infos[1]);

    AlgResourceRequest resReqInter;
    AlgResourceRequest resReqIntra;
    CHK_RET(interTempAlg->CalcRes(comm, param, topoInfo, resReqInter));
    CHK_RET(intraTempAlg->CalcRes(comm, param, topoInfo, resReqIntra));

    // step1在完成后，完成后同步后展开step2，因此slaveThread和对应notify可以复用
    resourceRequest.slaveThreadNum = std::max(resReqInter.slaveThreadNum, resReqIntra.slaveThreadNum);
    resourceRequest.notifyNumPerThread.clear();
    resourceRequest.notifyNumPerThread.resize(resourceRequest.slaveThreadNum);
    for (u32 i = 0; i < resourceRequest.slaveThreadNum; ++i) {
        if (i < resReqInter.notifyNumPerThread.size()) {
            resourceRequest.notifyNumPerThread[i] =
                std::max(resourceRequest.notifyNumPerThread[i], resReqInter.notifyNumPerThread[i]);
        }
        if (i < resReqIntra.notifyNumPerThread.size()) {
            resourceRequest.notifyNumPerThread[i] =
                std::max(resourceRequest.notifyNumPerThread[i], resReqIntra.notifyNumPerThread[i]);
        }
    }
    resourceRequest.notifyNumOnMainThread =
        std::max(resReqInter.notifyNumOnMainThread, resReqIntra.notifyNumOnMainThread);
    HCCL_INFO("notifyNumOnMainThread is %u", resourceRequest.notifyNumOnMainThread);
    resourceRequest.channels.resize(SEQUENCE_EXECUTOR_LEVEL_NUM);
    resourceRequest.channels[0] = resReqInter.channels[0];
    resourceRequest.channels[1] = resReqIntra.channels[0];
    HCCL_INFO(
        "slaveThreadNum is [%u], notifyNumOnMainThread is [%u], level 1 chanel size [%u], level 2 channel size [%u]",
        resourceRequest.slaveThreadNum, resourceRequest.notifyNumPerThread, resourceRequest.channels[0].size(),
        resourceRequest.channels[1].size());
    return HCCL_SUCCESS;
}

// ! 已编码完成
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    // 参数填充
    myRank_ = resCtx.topoInfo.userRank;
    rankSize_ = resCtx.topoInfo.userRankSize;

    dataCount_ = param.DataDes.count;
    strideCount_ = param.DataDes.strideCount;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];
    dataSize_ = dataCount_ * dataTypeSize_;
    dataType_ = param.DataDes.dataType;
    reduceOp_ = param.reduceType;
    algHierarchyInfo_ = resCtx.algHierarchyInfo;
    threads_ = resCtx.threads;

    rankIdxLevel0_ = myRank_ % algHierarchyInfo_.infos[0][0].size();
    rankIdxLevel1_ = myRank_ / algHierarchyInfo_.infos[0][0].size();

    rankSizeLevel0_ = algHierarchyInfo_.infos[0][0].size();
    rankSizeLevel1_ = algHierarchyInfo_.infos[1][0].size();
    CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));

    rankIdxLevel0_ = myRank_ % algHierarchyInfo_.infos[0][0].size();
    rankIdxLevel1_ = myRank_ / algHierarchyInfo_.infos[0][0].size();

    rankSizeLevel0_ = algHierarchyInfo_.infos[0][0].size();
    rankSizeLevel1_ = algHierarchyInfo_.infos[1][0].size();

    // 算法展开
    HcclResult ret = OrchestrateLoop(param, resCtx);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsV2ReduceScatterSequenceExecutor][Orchestrate]errNo[0x%016llx] Reduce scatter executor kernel run "
            "failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    return HCCL_SUCCESS;
}

// ! 已编码完成
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    // 将ccl-buffer分成ccl-in和ccl-out 2部分区分使用
    void* cclInAddr = resCtx.cclMem.addr;
    HcclMem cclInMem = {resCtx.cclMem.type, cclInAddr, resCtx.cclMem.size / 2};
    void* cclOutAddr = static_cast<void*>(static_cast<s8*>(resCtx.cclMem.addr) + resCtx.cclMem.size / 2);
    HcclMem cclOutMem = {resCtx.cclMem.type, cclOutAddr, resCtx.cclMem.size / 2};
    // 声明框内templateargs，user in搬运到ccl in，最终规约到ccl in
    TemplateDataParams tempAlgParamsInter;
    tempAlgParamsInter.buffInfo.inputPtr = param.inputPtr;
    tempAlgParamsInter.buffInfo.outputPtr = cclOutMem.addr;
    tempAlgParamsInter.buffInfo.hcclBuff = cclInMem; // ! 待验证这样使用是否能正常输出到CCL-IN，或者这里改用CCL-OUT

    // 构建框内template
    std::shared_ptr<InsAlgTemplate0> algTemplateInter =
        std::make_shared<InsAlgTemplate0>(param, myRank_, algHierarchyInfo_.infos[0]);

    // 声明框间templateargs，ccl-in写到对端ccl-out，最终规约到outputPtr上
    TemplateDataParams tempAlgParamsIntra;
    tempAlgParamsIntra.buffInfo.inputPtr =
        cclOutMem.addr; // ! 如果上面验证有问题，这里改成用CCL-OUT做输入，CCL-IN做Buffer
    tempAlgParamsIntra.buffInfo.outputPtr = param.outputPtr;
    tempAlgParamsIntra.buffInfo.hcclBuff = cclInMem;

    // 构建框间template
    std::shared_ptr<InsAlgTemplate1> algTemplateIntra =
        std::make_shared<InsAlgTemplate1>(param, myRank_, algHierarchyInfo_.infos[1]);

    BufferType inBuffType = BufferType::INPUT;
    BufferType outBuffType = BufferType::OUTPUT;
    u32 templateScratchMultiplierInter = algTemplateInter->CalcScratchMultiple(inBuffType, outBuffType);
    u32 templateScratchMultiplierIntra = algTemplateIntra->CalcScratchMultiple(outBuffType, outBuffType);

    u32 templateScratchMultiplier =
        std::max(templateScratchMultiplierInter * rankSizeLevel1_, templateScratchMultiplierIntra);

    // 构造框内template资源
    TemplateResource templateResourceInter;
    templateResourceInter.channels = remoteRankToChannelInfo_[0];
    templateResourceInter.threads = resCtx.threads;
    templateResourceInter.npu2DpuShmemPtr = resCtx.npu2DpuShmemPtr;
    templateResourceInter.dpu2NpuShmemPtr = resCtx.dpu2NpuShmemPtr;
    // 构造框间template资源
    TemplateResource templateResourceIntra;
    templateResourceIntra.channels = remoteRankToChannelInfo_[1];
    templateResourceIntra.threads = resCtx.threads;
    templateResourceIntra.npu2DpuShmemPtr = resCtx.npu2DpuShmemPtr;
    templateResourceIntra.dpu2NpuShmemPtr = resCtx.dpu2NpuShmemPtr;

    // 中转内存单次最多能够接受的output count，注意是count不是size
    u64 maxCountPerLoop = tempAlgParamsInter.buffInfo.hcclBuff.size / 2 / templateScratchMultiplier /
                          HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN / dataTypeSize_;
    // 计算loopTimes
    u64 loopTimes = dataCount_ / maxCountPerLoop + static_cast<u64>(dataCount_ % maxCountPerLoop != 0);
    u64 processedDataCount = 0;
    for (u64 loop = 0; loop < loopTimes; loop++) {
        u64 currDataCount = (loop == loopTimes - 1) ? dataCount_ - processedDataCount : maxCountPerLoop;

        // ----------- 框内数据搬运 -----------
        // 框内的数据偏移和搬运计算
        tempAlgParamsInter.count = currDataCount;
        tempAlgParamsInter.buffInfo.inBuffBaseOff = processedDataCount * dataTypeSize_;
        tempAlgParamsInter.buffInfo.outBuffBaseOff = 0; // 从user-in搬运到ccl-in，最终输出到ccl-in上面
        tempAlgParamsInter.buffInfo.hcclBuffBaseOff = 0;

        tempAlgParamsInter.sliceSize = currDataCount * dataTypeSize_;
        tempAlgParamsInter.tailSize = tempAlgParamsInter.sliceSize;
        // 这里的stride当成传统意义上的sreide 间隔
        tempAlgParamsInter.inputSliceStride =
            (strideCount_ == 0) ? dataSize_ :
                                  strideCount_ * dataTypeSize_; // ccl-in按照rank偏移量，每次偏移是单次循环最大数据量
        tempAlgParamsInter.outputSliceStride =
            currDataCount * dataTypeSize_; // 如果是scratchbuffer，偏移是单次循环处理的最大数据量

        HCCL_INFO(
            "[InsV2ReduceScatterSequenceExecutor] loop [%u] tempAlgParamsInter.inputSliceStride [%u],"
            "tempAlgParamsInter.outputSliceStride [%u] tempAlgParamsInter.sliceSize [%u]",
            loop, tempAlgParamsInter.inputSliceStride, tempAlgParamsInter.outputSliceStride,
            tempAlgParamsInter.sliceSize);
        HCCL_INFO(
            "[InsV2ReduceScatterSequenceExecutor] loop [%u] tempAlgParamsInter.buffInfo.inBuffBaseOff [%u],"
            "tempAlgParamsInter.buffInfo.outBuffBaseOff [%u]",
            loop, tempAlgParamsInter.buffInfo.inBuffBaseOff, tempAlgParamsInter.buffInfo.outBuffBaseOff);
        // m*n组网框内需要做n次重复
        tempAlgParamsInter.repeatNum = algHierarchyInfo_.infos[1][0].size();
        HCCL_INFO("templateScratchMultiplierInter is %u", templateScratchMultiplierInter);
        tempAlgParamsInter.inputRepeatStride = (strideCount_ == 0) ?
                                                   templateScratchMultiplierInter * dataCount_ * dataTypeSize_ :
                                                   templateScratchMultiplierInter * strideCount_ * dataTypeSize_;
        tempAlgParamsInter.outputRepeatStride = templateScratchMultiplierInter * currDataCount * dataTypeSize_;
        HCCL_INFO(
            "[InsV2ReduceScatterSequenceExecutor] loop [%u] tempAlgParamsInter.repeatNum [%u],"
            "tempAlgParamsInter.inputRepeatStride [%u], tempAlgParamsInter.outputRepeatStride [%u]",
            loop, tempAlgParamsInter.repeatNum, tempAlgParamsInter.inputRepeatStride,
            tempAlgParamsInter.outputRepeatStride);
        // 因为只考虑执行0级算法，所以传进template里面的channels就是channels_的第一个vector
        CHK_RET(algTemplateInter->KernelRun(param, tempAlgParamsInter, templateResourceInter));

        // ----------- 框间数据搬运 -----------
        // 框间的数据偏移和搬运量计算
        tempAlgParamsIntra.count = currDataCount;
        tempAlgParamsIntra.buffInfo.inBuffBaseOff = 0; // ccl-out偏移量，每次更新，所以是0
        tempAlgParamsIntra.buffInfo.outBuffBaseOff = processedDataCount * dataTypeSize_;
        tempAlgParamsIntra.buffInfo.hcclBuffBaseOff = 0;

        tempAlgParamsIntra.sliceSize = currDataCount * dataTypeSize_;
        tempAlgParamsIntra.tailSize = tempAlgParamsIntra.sliceSize;
        // 这里的stride当成传统意义上的sreide 间隔

        tempAlgParamsIntra.inputSliceStride =
            templateScratchMultiplierInter * currDataCount * dataTypeSize_; // 框间从ccl-in拿数据，
        tempAlgParamsIntra.outputSliceStride =
            currDataCount * dataTypeSize_; // 如果是scratchbuffer，偏移是单次循环处理的最大数据量

        HCCL_INFO(
            "[InsV2ReduceScatterSequenceExecutor] loop [%u] tempAlgParamsIntra.inputSliceStride [%u],"
            "tempAlgParamsIntra.outputSliceStride [%u] tempAlgParamsIntra.sliceSize [%u]",
            loop, tempAlgParamsIntra.inputSliceStride, tempAlgParamsIntra.outputSliceStride,
            tempAlgParamsIntra.sliceSize);
        HCCL_INFO(
            "[InsV2ReduceScatterSequenceExecutor] loop [%u] tempAlgParamsIntra.buffInfo.inBuffBaseOff [%u],"
            "tempAlgParamsIntra.buffInfo.outBuffBaseOff [%u]",
            loop, tempAlgParamsIntra.buffInfo.inBuffBaseOff, tempAlgParamsIntra.buffInfo.outBuffBaseOff);
        // 不需要重复
        tempAlgParamsIntra.repeatNum = 1;
        tempAlgParamsIntra.inputRepeatStride = 0;
        tempAlgParamsIntra.outputRepeatStride = 0;
        // 因为只考虑执行0级算法，所以传进template里面的channels就是channels_的第一个vector
        CHK_RET(algTemplateIntra->KernelRun(param, tempAlgParamsIntra, templateResourceIntra));
        processedDataCount += currDataCount;
    }
    return HCCL_SUCCESS;
}

REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_REDUCE_SCATTER, InsReduceScatterSequenceMeshMeshDPU, InsV2ReduceScatterSequenceExecutor,
    TopoMatchMultilevel, InsTempReduceScatterMesh1D, InsTempReduceScatterMesh1dDpu);
} // namespace mc2_ops_hccl
