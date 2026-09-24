/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software; you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_v2_all_reduce_omnipipe_executor.h"

#include "ins_temp_reduce_scatter_omnipipe_mesh_1D.h"
#include "ins_temp_reduce_scatter_omnipipe_nhr.h"
#include "ins_temp_all_gather_omnipipe_mesh_1D.h"
#include "ins_temp_all_gather_omnipipe_nhr.h"
#include "omnipipe_data_slice_calc.h"
#include "alg_meta_registry.h"
#include "external_alg_rules.h"

namespace mc2_ops_hccl {
constexpr u32 RANK_LEVEL_2 = 2;
constexpr u32 RANK_LEVEL_4 = 4;

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::InsV2AllReduceOmniPipeExecutor()
{}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    InitCommInfo(
        HcclComm comm, const OpParam& param, TopoInfoWithNetLayerDetails* topoInfo,
        AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    (void)comm;
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    devType_ = topoInfo->deviceType;
    reduceOp_ = param.reduceType;
    dataType_ = param.DataDes.dataType;
    dataCount_ = param.DataDes.count;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];
    algHierarchyInfo_ = algHierarchyInfo;
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    devType_ = topoInfo->deviceType;

    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    CalcResLevel(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        std::shared_ptr<InsAlgTemplateBase> tempAlg, AlgResourceRequest& resourceRequest, bool addChannel)
{
    AlgResourceRequest resReqlevel;
    CHK_RET(tempAlg->CalcRes(comm, param, topoInfo, resReqlevel));
    resourceRequest.slaveThreadNum += resReqlevel.slaveThreadNum + 1;
    resourceRequest.notifyNumOnMainThread += 1;
    resourceRequest.notifyNumPerThread.emplace_back(resReqlevel.notifyNumOnMainThread + 1);
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), resReqlevel.notifyNumPerThread.begin(),
        resReqlevel.notifyNumPerThread.end());

    if (addChannel) {
        resourceRequest.channels.emplace_back(resReqlevel.channels[0]);
    }
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    devType_ = topoInfo->deviceType;
    reduceOp_ = param.reduceType;
    dataType_ = param.DataDes.dataType;
    dataCount_ = param.DataDes.count;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];
    algHierarchyInfo_ = algHierarchyInfo;

    std::vector<std::vector<u32>> subCommRanks0;
    std::vector<std::vector<u32>> subCommRanks1;
    std::vector<std::vector<u32>> subCommRanks2;
    CHK_RET(InitSubCommRanks(subCommRanks0, subCommRanks1, subCommRanks2, topoInfo));
    rankSizeLevel0_ = subCommRanks0[0].size();
    rankSizeLevel1_ = subCommRanks1[0].size();
    rankSizeLevel2_ = subCommRanks2[0].size();

    uint32_t intraSuperpodDeviceNum = rankSizeLevel0_ * rankSizeLevel1_;
    rankIdxLevel0_ = (myRank_ % intraSuperpodDeviceNum) % rankSizeLevel0_;
    rankIdxLevel1_ = (myRank_ % intraSuperpodDeviceNum) / rankSizeLevel0_;
    rankIdxLevel2_ = myRank_ / intraSuperpodDeviceNum;

    std::map<u32, std::shared_ptr<InsAlgTemplateBase>> tempMap;
    if (rankSizeLevel0_ > 1) {
        tempMap[OMNIPIPE_RS_LEVEL0] = std::make_shared<InsRsAlgTemplateX>(param, myRank_, subCommRanks0);
        tempMap[OMNIPIPE_AG_LEVEL0] = std::make_shared<InsAgAlgTemplateX>(param, myRank_, subCommRanks0);
    }
    if (rankSizeLevel1_ > 1) {
        tempMap[OMNIPIPE_RS_LEVEL1] = std::make_shared<InsRsAlgTemplateY>(param, myRank_, subCommRanks1);
        tempMap[OMNIPIPE_AG_LEVEL1] = std::make_shared<InsAgAlgTemplateY>(param, myRank_, subCommRanks1);
    }
    if (rankSizeLevel2_ > 1) {
        tempMap[OMNIPIPE_RS_LEVEL2] = std::make_shared<InsRsAlgTemplateZ>(param, myRank_, subCommRanks2);
        tempMap[OMNIPIPE_AG_LEVEL2] = std::make_shared<InsAgAlgTemplateZ>(param, myRank_, subCommRanks2);
    }

    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;
    for (int level = 0; level < OMNIPIPE_AR_LEVEL_NUM; level++) {
        if (tempMap.count(level) > 0) {
            CHK_RET(CalcResLevel(comm, param, topoInfo, tempMap[level], resourceRequest, level < OMNIPIPE_AG_LEVEL0));
        }
    }
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsV2AllReduceOmniPipeExecutor][Orchestrate] Orchestrate Start");
    CHK_RET(InitExectorInfo(param, resCtx));
    CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));

    HcclResult ret = OrchestrateLoop(param, resCtx);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsV2AllReduceOmniPipeExecutor][Orchestrate]errNo[0x%016llx] Reduce scatter executor kernel run failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    HCCL_INFO("[InsV2AllReduceOmniPipeExecutor][Orchestrate] Orchestrate END");
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::InitExectorInfo(const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    myRank_ = resCtx.topoInfo.userRank;
    rankSize_ = resCtx.topoInfo.userRankSize;
    algHierarchyInfo_ = resCtx.algHierarchyInfo;
    dataCount_ = param.DataDes.count;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];
    dataSize_ = dataCount_ * dataTypeSize_;
    dataType_ = param.DataDes.dataType;
    reduceOp_ = param.reduceType;
    threads_ = resCtx.threads;

    std::vector<std::vector<u32>> subCommRanks0;
    std::vector<std::vector<u32>> subCommRanks1;
    std::vector<std::vector<u32>> subCommRanks2;
    CHK_RET(InitSubCommRanks(subCommRanks0, subCommRanks1, subCommRanks2, &(resCtx.topoInfo)));
    rankSizeLevel0_ = subCommRanks0[0].size();
    rankSizeLevel1_ = subCommRanks1[0].size();
    rankSizeLevel2_ = subCommRanks2[0].size();

    uint32_t intraSuperpodDeviceNum = rankSizeLevel0_ * rankSizeLevel1_;
    rankIdxLevel0_ = (myRank_ % intraSuperpodDeviceNum) % rankSizeLevel0_;
    rankIdxLevel1_ = (myRank_ % intraSuperpodDeviceNum) / rankSizeLevel0_;
    rankIdxLevel2_ = myRank_ / intraSuperpodDeviceNum;

    HCCL_INFO(
        "[InsV2AllReduceOmniPipeExecutor][InitExectorInfo] threads_[%u], dataTypeSize_[%u], dataCount_[%d], "
        "L0[%u], L1[%u], L2[%u]",
        threads_.size(), dataTypeSize_, dataCount_, rankSizeLevel0_, rankSizeLevel1_, rankSizeLevel2_);
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::GenTemplateAlgParamsByDimData(TemplateDataParams& tempAlgParams, StepSliceInfo& stepSliceInfo)
{
    tempAlgParams.buffInfo.inBuffType = BufferType::HCCL_BUFFER;
    tempAlgParams.buffInfo.outBuffType = BufferType::HCCL_BUFFER;
    tempAlgParams.buffInfo.inBuffBaseOff = stepSliceInfo.buffInfo.inBuffBaseOff;
    tempAlgParams.buffInfo.outBuffBaseOff = stepSliceInfo.buffInfo.outBuffBaseOff;
    tempAlgParams.buffInfo.hcclBuffBaseOff = stepSliceInfo.buffInfo.hcclBuffBaseOff;
    tempAlgParams.stepSliceInfo = stepSliceInfo;
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::PrepareResForTemplateLevelRS(u32 level, std::shared_ptr<InsAlgTemplateBase>& tempBase)
{
    HCCL_INFO("[InsV2AllReduceOmniPipeExecutor][%s], level[%u]", __func__, level);
    u64 levelThreadNum = tempBase->GetThreadNum();
    if (level == OMNIPIPE_LEVEL0) {
        levelThreadsRS_[OMNIPIPE_LEVEL0].assign(threads_.begin() + 1, threads_.begin() + 1 + levelThreadNum);
        tempMainThreadsLevel01RS_.push_back(levelThreadsRS_[0].at(0));
    } else if (level == OMNIPIPE_LEVEL1) {
        levelThreadsRS_[OMNIPIPE_LEVEL1].assign(
            threads_.begin() + 1 + levelThreadsRS_[0].size(),
            threads_.begin() + 1 + levelThreadsRS_[0].size() + levelThreadNum);
        tempMainThreadsLevel01RS_.push_back(levelThreadsRS_[1].at(0));
    } else if (level == OMNIPIPE_LEVEL2) {
        levelThreadsRS_[OMNIPIPE_LEVEL2].assign(
            threads_.begin() + 1 + levelThreadsRS_[OMNIPIPE_LEVEL0].size() + levelThreadsRS_[OMNIPIPE_LEVEL1].size(),
            threads_.begin() + 1 + levelThreadsRS_[OMNIPIPE_LEVEL0].size() + levelThreadsRS_[OMNIPIPE_LEVEL1].size() +
                levelThreadNum);
        tempMainThreadsLevel2RS_.push_back(levelThreadsRS_[OMNIPIPE_LEVEL2].at(0));
    }

    AlgResourceRequest levelTempRequest;
    CHK_RET(tempBase->GetRes(levelTempRequest));
    if (level < OMNIPIPE_LEVEL2) {
        ntfIdxCtrlToTempLevel01RS_.push_back(levelTempRequest.notifyNumOnMainThread);
        ntfIdxTempToCtrlLevel01RS_.push_back(tempMainThreadsLevel01RS_.size() + tempMainThreadsLevel2RS_.size() - 1);
    } else {
        ntfIdxCtrlToTempLevel2RS_.push_back(levelTempRequest.notifyNumOnMainThread);
        ntfIdxTempToCtrlLevel2RS_.push_back(tempMainThreadsLevel01RS_.size() + tempMainThreadsLevel2RS_.size() - 1);
    }
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::PrepareResForTemplateLevelAG(u32 level, std::shared_ptr<InsAlgTemplateBase>& tempBase)
{
    HCCL_INFO("[InsV2AllReduceOmniPipeExecutor][%s], level[%u]", __func__, level);
    u64 levelThreadNum = tempBase->GetThreadNum();
    u64 threadsNumStart = levelThreadsRS_[OMNIPIPE_LEVEL0].size() + levelThreadsRS_[OMNIPIPE_LEVEL1].size() +
                          levelThreadsRS_[OMNIPIPE_LEVEL2].size();
    if (level == OMNIPIPE_LEVEL0) {
        levelThreadsAG_[OMNIPIPE_LEVEL0].assign(
            threads_.begin() + threadsNumStart + 1, threads_.begin() + threadsNumStart + 1 + levelThreadNum);
        tempMainThreadsLevel01AG_.push_back(levelThreadsAG_[0].at(0));
    } else if (level == OMNIPIPE_LEVEL1) {
        levelThreadsAG_[OMNIPIPE_LEVEL1].assign(
            threads_.begin() + threadsNumStart + 1 + levelThreadsAG_[0].size(),
            threads_.begin() + threadsNumStart + 1 + levelThreadsAG_[0].size() + levelThreadNum);
        tempMainThreadsLevel01AG_.push_back(levelThreadsAG_[1].at(0));
    } else if (level == OMNIPIPE_LEVEL2) {
        levelThreadsAG_[OMNIPIPE_LEVEL2].assign(
            threads_.begin() + threadsNumStart + 1 + levelThreadsAG_[0].size() + levelThreadsAG_[1].size(),
            threads_.end());
        tempMainThreadsLevel2AG_.push_back(levelThreadsAG_[2].at(0));
    }

    AlgResourceRequest levelTempRequest;
    CHK_RET(tempBase->GetRes(levelTempRequest));
    if (level < OMNIPIPE_LEVEL2) {
        ntfIdxCtrlToTempLevel01AG_.push_back(levelTempRequest.notifyNumOnMainThread);
        ntfIdxTempToCtrlLevel01AG_.push_back(tempMainThreadsLevel01AG_.size() + tempMainThreadsLevel2AG_.size() - 1);
    } else {
        ntfIdxCtrlToTempLevel2AG_.push_back(levelTempRequest.notifyNumOnMainThread);
        ntfIdxTempToCtrlLevel2AG_.push_back(tempMainThreadsLevel01AG_.size() + tempMainThreadsLevel2AG_.size() - 1);
    }
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    RestoreChannelMap(
        const AlgResourceCtxSerializable& resCtx,
        std::vector<std::map<u32, std::vector<ChannelInfo>>>& rankIdToChannelInfo)
{
    rankIdToChannelInfo.resize(OMNIPIPE_LEVEL_NUM);
    u32 level = 0;
    if (rankSizeLevel0_ > 1) {
        for (auto& channel : resCtx.channels[level]) {
            rankIdToChannelInfo[OMNIPIPE_LEVEL0][channel.remoteRank].push_back(channel);
        }
        level++;
    }
    if (rankSizeLevel1_ > 1) {
        for (auto& channel : resCtx.channels[level]) {
            rankIdToChannelInfo[OMNIPIPE_LEVEL1][channel.remoteRank].push_back(channel);
        }
        level++;
    }
    if (rankSizeLevel2_ > 1) {
        for (auto& channel : resCtx.channels[level]) {
            rankIdToChannelInfo[OMNIPIPE_LEVEL2][channel.remoteRank].push_back(channel);
        }
    }
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    InitOmniPipeScratchParam(
        OmniPipeScratchParam& scratchParam, const OpParam& param, const std::vector<double>& endpointAttrBwNew,
        std::map<u32, std::shared_ptr<InsAlgTemplateBase>>& tempMap)
{
    scratchParam.levelRankSize = {rankSizeLevel0_, rankSizeLevel1_, rankSizeLevel2_};
    scratchParam.endpointAttrBw = endpointAttrBwNew;
    std::vector<u64> levelAlgType;
    levelAlgType.push_back(
        tempMap.count(OMNIPIPE_RS_LEVEL0) > 0 ?
            tempMap[OMNIPIPE_RS_LEVEL0]->CalcScratchMultiple(BufferType::DEFAULT, BufferType::DEFAULT) :
            0);
    levelAlgType.push_back(
        tempMap.count(OMNIPIPE_RS_LEVEL1) > 0 ?
            tempMap[OMNIPIPE_RS_LEVEL1]->CalcScratchMultiple(BufferType::DEFAULT, BufferType::DEFAULT) :
            0);
    levelAlgType.push_back(
        tempMap.count(OMNIPIPE_RS_LEVEL2) > 0 ?
            tempMap[OMNIPIPE_RS_LEVEL2]->CalcScratchMultiple(BufferType::DEFAULT, BufferType::DEFAULT) :
            0);
    scratchParam.levelAlgType = levelAlgType;
    scratchParam.dataTypeSize = dataTypeSize_;
    scratchParam.opMode = param.opMode;
    scratchParam.engine = param.engine;
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    InitOmniPipeSliceParam(
        OmniPipeSliceParam& sliceParam, const OpParam& param, const std::vector<double>& endpointAttrBwNew,
        std::map<u32, std::shared_ptr<InsAlgTemplateBase>>& tempMap, u64 maxCountPerLoop)
{
    (void)maxCountPerLoop;
    sliceParam.levelRankSize = {rankSizeLevel0_, rankSizeLevel1_, rankSizeLevel2_};
    sliceParam.levelRankId = {rankIdxLevel0_, rankIdxLevel1_, rankIdxLevel2_};
    sliceParam.endpointAttrBw = endpointAttrBwNew;
    std::vector<u64> levelAlgType;
    levelAlgType.push_back(
        tempMap.count(OMNIPIPE_RS_LEVEL0) > 0 ?
            tempMap[OMNIPIPE_RS_LEVEL0]->CalcScratchMultiple(BufferType::DEFAULT, BufferType::DEFAULT) :
            0);
    levelAlgType.push_back(
        tempMap.count(OMNIPIPE_RS_LEVEL1) > 0 ?
            tempMap[OMNIPIPE_RS_LEVEL1]->CalcScratchMultiple(BufferType::DEFAULT, BufferType::DEFAULT) :
            0);
    levelAlgType.push_back(
        tempMap.count(OMNIPIPE_RS_LEVEL2) > 0 ?
            tempMap[OMNIPIPE_RS_LEVEL2]->CalcScratchMultiple(BufferType::DEFAULT, BufferType::DEFAULT) :
            0);
    sliceParam.levelAlgType = levelAlgType;
    sliceParam.dataTypeSize = dataTypeSize_;
    sliceParam.opMode = param.opMode;
    sliceParam.engine = param.engine;
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    InitTemplate(
        const OpParam& param, std::map<u32, std::shared_ptr<InsAlgTemplateBase>>& tempMap,
        const std::vector<std::vector<u32>>& subCommRanks0, const std::vector<std::vector<u32>>& subCommRanks1,
        const std::vector<std::vector<u32>>& subCommRanks2)
{
    if (rankSizeLevel0_ > 1) {
        tempMap[OMNIPIPE_RS_LEVEL0] = std::make_shared<InsRsAlgTemplateX>(param, myRank_, subCommRanks0);
        tempMap[OMNIPIPE_AG_LEVEL0] = std::make_shared<InsAgAlgTemplateX>(param, myRank_, subCommRanks0);
    }
    if (rankSizeLevel1_ > 1) {
        tempMap[OMNIPIPE_RS_LEVEL1] = std::make_shared<InsRsAlgTemplateY>(param, myRank_, subCommRanks1);
        tempMap[OMNIPIPE_AG_LEVEL1] = std::make_shared<InsAgAlgTemplateY>(param, myRank_, subCommRanks1);
    }
    if (rankSizeLevel2_ > 1) {
        tempMap[OMNIPIPE_RS_LEVEL2] = std::make_shared<InsRsAlgTemplateZ>(param, myRank_, subCommRanks2);
        tempMap[OMNIPIPE_AG_LEVEL2] = std::make_shared<InsAgAlgTemplateZ>(param, myRank_, subCommRanks2);
    }

    levelThreadsRS_.resize(OMNIPIPE_LEVEL_NUM);
    levelThreadsAG_.resize(OMNIPIPE_LEVEL_NUM);
    HCCL_DEBUG("[InsV2AllReduceOmniPipeExecutor][InitTemplate] tempMap.size()[%u]", tempMap.size());
    controlThread_ = threads_.at(0);
    for (int level = 0; level < OMNIPIPE_AR_LEVEL_NUM; level++) {
        if (tempMap.count(level) > 0) {
            if (level < OMNIPIPE_AG_LEVEL0) {
                CHK_RET(PrepareResForTemplateLevelRS(level, tempMap[level]));
            } else {
                CHK_RET(PrepareResForTemplateLevelAG(level - OMNIPIPE_AG_LEVEL0, tempMap[level]));
            }
        }
    }
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    InitTemplateParams(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx,
        const std::map<u32, std::shared_ptr<InsAlgTemplateBase>>& tempMap, std::map<u32, TemplateResource>& tempResMap,
        std::map<u32, TemplateDataParams>& tempAlgParamMap)
{
    for (int level = 0; level < OMNIPIPE_AR_LEVEL_NUM; level++) {
        if (tempMap.count(level) > 0) {
            if (level < OMNIPIPE_AG_LEVEL0) {
                tempResMap[level].threads = levelThreadsRS_[level];
                tempResMap[level].channels = remoteRankToChannelInfo_[level];
            } else {
                tempResMap[level].threads = levelThreadsAG_[level - OMNIPIPE_AG_LEVEL0];
                tempResMap[level].channels = remoteRankToChannelInfo_[level - OMNIPIPE_AG_LEVEL0];
            }
            tempResMap[level].npu2DpuShmemPtr = resCtx.npu2DpuShmemPtr;
            tempResMap[level].dpu2NpuShmemPtr = resCtx.dpu2NpuShmemPtr;
            tempAlgParamMap[level].buffInfo.inputPtr = param.inputPtr;
            tempAlgParamMap[level].buffInfo.outputPtr = param.outputPtr;
            tempAlgParamMap[level].buffInfo.hcclBuff = resCtx.cclMem;
        }
    }
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    InitSubCommRanks(
        std::vector<std::vector<u32>>& subCommRanks0, std::vector<std::vector<u32>>& subCommRanks1,
        std::vector<std::vector<u32>>& subCommRanks2, const TopoInfoWithNetLayerDetails* topoInfo)
{
    subCommRanks0.clear();
    subCommRanks1.clear();
    subCommRanks2.clear();

    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix) {
        subCommRanks0 = {algHierarchyInfo_.infos[0][0]};
        std::vector<u32> closRanks;
        u32 meshSize = algHierarchyInfo_.infos[0][0].size();
        for (auto rank : algHierarchyInfo_.infos[0][1]) {
            if (rank % meshSize == topoInfo->userRank % meshSize) {
                closRanks.push_back(rank);
            }
        }
        subCommRanks1 = {closRanks};
        subCommRanks2 = algHierarchyInfo_.infos[1];
    } else {
        subCommRanks0 = algHierarchyInfo_.infos[0];
        subCommRanks1 = algHierarchyInfo_.infos[1];
        subCommRanks2.emplace_back(std::vector<u32>{myRank_});
    }

    HCCL_INFO(
        "[InsV2AllReduceOmniPipeExecutor][InitSubCommRanks] subCommRanks0.size()[%u], "
        "subCommRanks1.size()[%u], subCommRanks2.size()[%u]",
        subCommRanks0.size(), subCommRanks1.size(), subCommRanks2.size());
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::OrchestrateLoop(const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    std::vector<std::vector<u32>> subCommRanks0;
    std::vector<std::vector<u32>> subCommRanks1;
    std::vector<std::vector<u32>> subCommRanks2;
    CHK_RET(InitSubCommRanks(subCommRanks0, subCommRanks1, subCommRanks2, &(resCtx.topoInfo)));

    std::map<u32, std::shared_ptr<InsAlgTemplateBase>> tempMap;
    CHK_RET(InitTemplate(param, tempMap, subCommRanks0, subCommRanks1, subCommRanks2));

    std::map<u32, TemplateResource> tempResMap;
    std::map<u32, TemplateDataParams> tempAlgParamMap;
    CHK_RET(InitTemplateParams(param, resCtx, tempMap, tempResMap, tempAlgParamMap));

    double bw_ag_l0 = BW_OMNI_DEFAULT;
    double bw_ag_l1 = BW_OMNI_DEFAULT;
    double bw_ag_l2 = BW_OMNI_DEFAULT;
    double bw_rs_l0 = BW_OMNI_DEFAULT;
    double bw_rs_l1 = BW_OMNI_DEFAULT;
    double bw_rs_l2 = BW_OMNI_DEFAULT;
    if (resCtx.topoInfo.level0PcieMix) {
        if (rankSizeLevel1_ == RANK_LEVEL_2) {
            bw_ag_l1 = BW_OMNI_PCIE_EIGHT_AG_CLOS;
            bw_rs_l1 = BW_OMNI_PCIE_EIGHT_RS_CLOS;
        } else if (rankSizeLevel1_ == RANK_LEVEL_4) {
            bw_ag_l1 = BW_OMNI_PCIE_SIXTEEN_AG_CLOS;
            bw_rs_l1 = BW_OMNI_PCIE_SIXTEEN_RS_CLOS;
        }
    }

    double eqBw0 = bw_ag_l0;
    double eqBw1 = bw_ag_l1;
    double eqBw2 = bw_ag_l2;
    eqBw1 = rankSizeLevel1_ > 1 ? eqBw1 / (rankSizeLevel1_ - 1) : eqBw1;
    eqBw2 = rankSizeLevel2_ > 1 ? eqBw2 / (rankSizeLevel2_ - 1) : eqBw2;
    std::vector<double> endpointAttrBwAG{eqBw0, eqBw1, eqBw2};

    double eqBw3 = bw_rs_l0;
    double eqBw4 = bw_rs_l1;
    double eqBw5 = bw_rs_l2;
    eqBw4 = rankSizeLevel1_ > 1 ? eqBw4 / (rankSizeLevel1_ - 1) : eqBw4;
    eqBw5 = rankSizeLevel2_ > 1 ? eqBw5 / (rankSizeLevel2_ - 1) : eqBw5;
    std::vector<double> endpointAttrBwNew{eqBw3, eqBw4, eqBw5};

    OmniPipeScratchParam scratchParam;
    CHK_RET(InitOmniPipeScratchParam(scratchParam, param, endpointAttrBwNew, tempMap));
    scratchParam.maxTmpMemSize = resCtx.cclMem.size;
    auto allRankSplitData = OmniPipeSplitData(rankSize_, dataCount_, dataTypeSize_);
    scratchParam.dataSize = CalcCountToDataSize(allRankSplitData, dataTypeSize_);
    std::vector<u64> loopInfo = CalcOmniPipeScratchInfo(scratchParam);
    u64 maxCountPerLoop = loopInfo[0];
    u64 loopTimes = loopInfo[1];
    HCCL_DEBUG("maxCountPerLoop[%u], loopTimes[%u]", maxCountPerLoop, loopTimes);

    auto multiLoopAllRankSplitData =
        OmniPipeSplitRankDataLoop(allRankSplitData, maxCountPerLoop, loopTimes, dataTypeSize_);

    OmniPipeSliceParam sliceParam;
    CHK_RET(InitOmniPipeSliceParam(sliceParam, param, endpointAttrBwNew, tempMap, maxCountPerLoop));
    u64 processedDataCount = 0;
    OmniPipeSliceInfo omniPipeSliceInfoRS;
    OmniPipeSliceInfo omniPipeSliceInfoAG;

    TemplateDataParams tempParamLocalcopy;
    tempParamLocalcopy.buffInfo.hcclBuff = resCtx.cclMem;
    tempParamLocalcopy.buffInfo.inputPtr = param.inputPtr;
    tempParamLocalcopy.buffInfo.outputPtr = param.outputPtr;

    for (u64 loop = 0; loop < loopTimes; loop++) {
        CHK_PRT_RET(
            multiLoopAllRankSplitData.size() <= loop,
            HCCL_ERROR("[InsV2AllReduceOmniPipeExecutor][Orchestrate] multiLoopAllRankSplitData.size() <= loop"),
            HCCL_E_PARA);

        if (loop == 0 || !isSameLoop(multiLoopAllRankSplitData[loop - 1], multiLoopAllRankSplitData[loop])) {
            sliceParam.dataSizePerLoop = CalcCountToDataSize(multiLoopAllRankSplitData[loop], dataTypeSize_);
            sliceParam.dataWholeSize = sliceParam.dataSizePerLoop;
            sliceParam.endpointAttrBw = endpointAttrBwNew;
            omniPipeSliceInfoRS = CalcRSOmniPipeSliceInfo(sliceParam);
            sliceParam.endpointAttrBw = endpointAttrBwAG;
            omniPipeSliceInfoAG = CalcAGOmniPipeSliceInfo(sliceParam);
        }

        u64 currDataCount = multiLoopAllRankSplitData[loop][myRank_];
        tempParamLocalcopy.buffInfo.inBuffType = BufferType::INPUT;
        tempParamLocalcopy.buffInfo.inBuffBaseOff = processedDataCount * dataTypeSize_;
        tempParamLocalcopy.buffInfo.outBuffBaseOff = 0;
        tempParamLocalcopy.repeatNum = rankSize_;

        CHK_RET(PreSyncInterThreads(controlThread_, tempMainThreadsLevel01RS_, ntfIdxCtrlToTempLevel01RS_));
        CHK_RET(DoLocalCopy(tempParamLocalcopy, controlThread_, allRankSplitData, multiLoopAllRankSplitData[loop]));
        CHK_RET(PostSyncInterThreads(controlThread_, tempMainThreadsLevel01RS_, ntfIdxTempToCtrlLevel01RS_));

        u32 interPodStepNum = omniPipeSliceInfoRS.dataSliceLevel2.size();
        u32 intraPodStepNum = omniPipeSliceInfoRS.dataSliceLevel0.size() / interPodStepNum;
        for (u32 stepZ = 0; stepZ < interPodStepNum; stepZ++) {
            if (rankSizeLevel2_ > 1) {
                HCCL_INFO("rankSizeLevel2_ > 1-----RS-Z");
                CHK_RET(GenTemplateAlgParamsByDimData(
                    tempAlgParamMap[OMNIPIPE_RS_LEVEL2], omniPipeSliceInfoRS.dataSliceLevel2[stepZ]));
                CHK_RET(PreSyncInterThreads(controlThread_, tempMainThreadsLevel2RS_, ntfIdxCtrlToTempLevel2RS_));
                CHK_RET(tempMap[OMNIPIPE_RS_LEVEL2]->KernelRun(
                    param, tempAlgParamMap[OMNIPIPE_RS_LEVEL2], tempResMap[OMNIPIPE_RS_LEVEL2]));
            }
            for (u32 stepXY = 0; stepXY < intraPodStepNum; stepXY++) {
                CHK_RET(PreSyncInterThreads(controlThread_, tempMainThreadsLevel01RS_, ntfIdxCtrlToTempLevel01RS_));
                if (rankSizeLevel0_ > 1) {
                    HCCL_INFO("rankSizeLevel0_ > 1-----RS-X");
                    CHK_RET(GenTemplateAlgParamsByDimData(
                        tempAlgParamMap[OMNIPIPE_RS_LEVEL0],
                        omniPipeSliceInfoRS.dataSliceLevel0[stepZ * intraPodStepNum + stepXY]));
                    CHK_RET(tempMap[OMNIPIPE_RS_LEVEL0]->KernelRun(
                        param, tempAlgParamMap[OMNIPIPE_RS_LEVEL0], tempResMap[OMNIPIPE_RS_LEVEL0]));
                }
                if (rankSizeLevel1_ > 1) {
                    HCCL_INFO("rankSizeLevel1_ > 1-----RS-Y");
                    CHK_RET(GenTemplateAlgParamsByDimData(
                        tempAlgParamMap[OMNIPIPE_RS_LEVEL1],
                        omniPipeSliceInfoRS.dataSliceLevel1[stepZ * intraPodStepNum + stepXY]));
                    CHK_RET(tempMap[OMNIPIPE_RS_LEVEL1]->KernelRun(
                        param, tempAlgParamMap[OMNIPIPE_RS_LEVEL1], tempResMap[OMNIPIPE_RS_LEVEL1]));
                }
                CHK_RET(PostSyncInterThreads(controlThread_, tempMainThreadsLevel01RS_, ntfIdxTempToCtrlLevel01RS_));
            }
            if (rankSizeLevel2_ > 1) {
                CHK_RET(PostSyncInterThreads(controlThread_, tempMainThreadsLevel2RS_, ntfIdxTempToCtrlLevel2RS_));
                HCCL_INFO("PostSyncInterThreads z success.");
            }
        }

        interPodStepNum = omniPipeSliceInfoAG.dataSliceLevel2.size();
        intraPodStepNum = omniPipeSliceInfoAG.dataSliceLevel0.size() / interPodStepNum;
        for (u32 stepZ = 0; stepZ < interPodStepNum; stepZ++) {
            if (rankSizeLevel2_ > 1) {
                HCCL_INFO("rankSizeLevel2_ > 1-----AG-Z");
                CHK_RET(PreSyncInterThreads(controlThread_, tempMainThreadsLevel2AG_, ntfIdxCtrlToTempLevel2AG_));
                CHK_RET(GenTemplateAlgParamsByDimData(
                    tempAlgParamMap[OMNIPIPE_AG_LEVEL2], omniPipeSliceInfoAG.dataSliceLevel2[stepZ]));
                CHK_RET(tempMap[OMNIPIPE_AG_LEVEL2]->KernelRun(
                    param, tempAlgParamMap[OMNIPIPE_AG_LEVEL2], tempResMap[OMNIPIPE_AG_LEVEL2]));
            }
            for (u32 stepXY = 0; stepXY < intraPodStepNum; stepXY++) {
                CHK_RET(PreSyncInterThreads(controlThread_, tempMainThreadsLevel01AG_, ntfIdxCtrlToTempLevel01AG_));
                if (rankSizeLevel0_ > 1) {
                    HCCL_INFO("rankSizeLevel0_ > 1-----AG-X");
                    CHK_RET(GenTemplateAlgParamsByDimData(
                        tempAlgParamMap[OMNIPIPE_AG_LEVEL0],
                        omniPipeSliceInfoAG.dataSliceLevel0[stepZ * intraPodStepNum + stepXY]));
                    CHK_RET(tempMap[OMNIPIPE_AG_LEVEL0]->KernelRun(
                        param, tempAlgParamMap[OMNIPIPE_AG_LEVEL0], tempResMap[OMNIPIPE_AG_LEVEL0]));
                }
                if (rankSizeLevel1_ > 1) {
                    HCCL_INFO("rankSizeLevel1_ > 1-----AG-Y");
                    CHK_RET(GenTemplateAlgParamsByDimData(
                        tempAlgParamMap[OMNIPIPE_AG_LEVEL1],
                        omniPipeSliceInfoAG.dataSliceLevel1[stepZ * intraPodStepNum + stepXY]));
                    CHK_RET(tempMap[OMNIPIPE_AG_LEVEL1]->KernelRun(
                        param, tempAlgParamMap[OMNIPIPE_AG_LEVEL1], tempResMap[OMNIPIPE_AG_LEVEL1]));
                }
                CHK_RET(PostSyncInterThreads(controlThread_, tempMainThreadsLevel01AG_, ntfIdxTempToCtrlLevel01AG_));
            }
            if (rankSizeLevel2_ > 1) {
                CHK_RET(PostSyncInterThreads(controlThread_, tempMainThreadsLevel2AG_, ntfIdxTempToCtrlLevel2AG_));
            }
        }

        tempParamLocalcopy.buffInfo.inBuffType = BufferType::HCCL_BUFFER;
        tempParamLocalcopy.buffInfo.inBuffBaseOff = 0;
        tempParamLocalcopy.buffInfo.outBuffBaseOff = processedDataCount * dataTypeSize_;
        tempParamLocalcopy.repeatNum = rankSize_;
        CHK_RET(PreSyncInterThreads(controlThread_, tempMainThreadsLevel01AG_, ntfIdxCtrlToTempLevel01AG_));
        CHK_RET(DoLocalCopy(tempParamLocalcopy, controlThread_, allRankSplitData, multiLoopAllRankSplitData[loop]));
        CHK_RET(PostSyncInterThreads(controlThread_, tempMainThreadsLevel01AG_, ntfIdxTempToCtrlLevel01AG_));
        processedDataCount += currDataCount;
    }
    HCCL_INFO("[InsV2AllReduceOmniPipeExecutor][OrchestrateLoop] End.");
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsRsAlgTemplateX, typename InsRsAlgTemplateY, typename InsRsAlgTemplateZ,
    typename InsAgAlgTemplateX, typename InsAgAlgTemplateY, typename InsAgAlgTemplateZ>
HcclResult InsV2AllReduceOmniPipeExecutor<
    AlgTopoMatch, InsRsAlgTemplateX, InsRsAlgTemplateY, InsRsAlgTemplateZ, InsAgAlgTemplateX, InsAgAlgTemplateY,
    InsAgAlgTemplateZ>::
    DoLocalCopy(
        const TemplateDataParams& tempAlgParams, const ThreadHandle& thread, const std::vector<u64>& allRankSplitData,
        const std::vector<u64>& curLoopAllRankSplitData)
{
    std::vector<DataSlice> srcDataSlice;
    std::vector<DataSlice> dstDataSlice;
    CHK_RET(CalLocalCopySlice(
        tempAlgParams, allRankSplitData, curLoopAllRankSplitData, srcDataSlice, dstDataSlice, dataTypeSize_));
    CHK_PRT_RET(
        srcDataSlice.size() != dstDataSlice.size(),
        HCCL_ERROR("[InsV2AllReduceOmniPipeExecutor][DoLocalCopy] srcDataSlice.size != dstDataSlice.size"),
        HCCL_E_PARA);
    for (u32 i = 0; i < srcDataSlice.size(); ++i) {
        CHK_RET(static_cast<HcclResult>(LocalCopy(thread, srcDataSlice[i], dstDataSlice[i])));
    }
    return HCCL_SUCCESS;
}

REGISTER_EXEC_V2_MULTI(
    HcclCMDType::HCCL_CMD_ALLREDUCE, AicpuAllReducePipeLinePcie, InsV2AllReduceOmniPipeExecutor, TopoMatchPcieMix,
    InsTempReduceScatterOmniPipeMesh1D, InsTempReduceScatterOmniPipeNHR, InsTempReduceScatterOmniPipeNHR,
    InsTempAllGatherOmniPipeMesh1D, InsTempAllGatherOmniPipeNHR, InsTempAllGatherOmniPipeNHR);
REGISTER_ALG_META(
    HcclCMDType::HCCL_CMD_ALLREDUCE, AicpuAllReducePipeLinePcie, AlgEngine::AICPU, "pipeline[mesh,nhr]", COND_NONE,
    FLAG_NONE, 0);

} // namespace mc2_ops_hccl
