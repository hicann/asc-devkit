/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_ALL_REDUCE_PARALLEL_EXECUTOR
#define INS_ALL_REDUCE_PARALLEL_EXECUTOR

#include "alg_param.h"
#include "topo_host.h"
#include "channel.h"
#include "alg_v2_template_base.h"
#include "utils.h"
#include "log.h"
#include "workflow.h"
#include "sal.h"
#include "config_log.h"
#include "executor_v2_base.h"
#include "coll_alg_v2_exec_registry.h"
#include "topo_match_pcie_mix.h"
#include "topo_match_base.h"
#include "topo_match_1d.h"

namespace mc2_ops_hccl {

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
class InsAllReduceParallelExecutor : public InsCollAlgBase {
public:
    explicit InsAllReduceParallelExecutor();
    ~InsAllReduceParallelExecutor() override = default;

    std::string Describe() const override { return "Instruction based AllReduce Parallel Executor."; }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override;
    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override;
    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;

private:
    static u32 CalcRankIdInter(u32 i, u32 j, u32 part2);
    static u32 CalcRankIdIntra(u32 i, u32 j, u32 part1);
    void FillDataMap(
        const u64 sliceCount, const u32 part1, const u32 part2, std::map<u32, std::pair<u64, u64>>& dataMap,
        bool isInter);
    void FillOffsetMaps(
        const std::map<u32, std::pair<u64, u64>>& dataMap, const u32 part1, const u32 part2, std::map<u32, u64>& agMap,
        bool isInter);
    void GetParallelDataSplit(std::vector<float>& splitDataSize) const;
    uint64_t GetRankSize(const std::vector<std::vector<u32>>& vTopo);
    HcclResult PrepareResForTemplate(
        InsAlgTemplate0& tempAlgIntra, InsAlgTemplate1& tempAlgInter, InsAlgTemplate2& tempAlgIntra1);
    HcclResult PrepareResForTemplate23(
        InsAlgTemplate0& tempAlgIntra, InsAlgTemplate2& tempAlgIntra1, InsAlgTemplate3& tempAlgInter1);
    HcclResult PrepareResForTemplateResource(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, TemplateResource& intraTempAlgRes,
        TemplateResource& interTempAlgRes, bool isRsStage);
    void GenDataParamsBufferType(
        const BufferType inBuffType, const BufferType outBuffType, const BufferType hcclBuffType,
        TemplateDataParams& dataParams) const;
    void GenDataParamstempAlg(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 sliceCount,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, const u32 LocalRankSize, const u64 inputOffset,
        const u64 outputOffset, const u64 hcclBuffOffset) const;
    void CalcInterDataAllRank(
        const u64 sliceCount, const u32 LocalRankSizePart1, const u32 LocalRankSizePart2,
        std::map<u32, std::pair<u64, u64>>& dataMap);
    void CalcIntraDataAllRank(
        const u64 sliceCount, const u32 LocalRankSizePart1, const u32 LocalRankSizePart2,
        std::map<u32, std::pair<u64, u64>>& dataMap);
    void PrePareDataParamstempAlgIntra(const u64 dataOffset, const u64 currCountPart, const u64 scratchOffsetCount);
    void PrePareDataParamstempAlgInter(const u64 dataOffset, const u64 currCountPart, const u64 scratchOffsetCount);
    void GenDataParamsAllRank(const u64 sliceCount, const u32 LocalRankSize, TemplateDataParams& dataParams) const;
    HcclResult RunTemplateIntra0(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate0& tempAlgInter);
    HcclResult RunTemplateIntra1(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate0& tempAlgInter);
    HcclResult RunTemplateInter0(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate1& tempAlgInter);
    HcclResult RunTemplateInter1(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate1& tempAlgInter);
    HcclResult RunTemplateInter01(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate3& tempAlgInter1);
    HcclResult RunTemplateIntra01(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate2& tempAlgIntra1);
    HcclResult RunTemplateIntra11(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate2& tempAlgInter1);
    HcclResult RunTemplateInter11(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate3& tempAlgInter1);
    HcclResult GenInsQues(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, InsAlgTemplate0& tempAlgIntra0,
        InsAlgTemplate1& tempAlgInter0, InsAlgTemplate2& tempAlgIntra1, InsAlgTemplate3& tempAlgInter1);
    inline u64 RoundDown(u64 dividend, u64 divisor) const
    {
        if (divisor == 0) {
            HCCL_WARNING("[InsAllReduceParallelExecutor][RoundDown] divisor is 0!");
            return dividend;
        }
        return dividend / divisor;
    }
    std::map<u32, u32> tempVirtRankMapInter_;
    std::map<u32, u32> tempVirtRankMapIntra_;
    std::vector<u64> allRankSliceSizeInter_;
    std::vector<u64> allRankDisplsInter_;
    std::vector<u64> allRankSliceSizeIntra_;
    std::vector<u64> allRankDisplsIntra_;
    u32 intraLocalRankSize_{0};
    u32 interLocalRankSize_{0};

    u64 dataOffset0Inter_;
    u64 currCountPart0_;
    u64 scratchOffsetCountInterStage1_;
    u64 outputPtrOffsetInter_;

    u64 dataOffset0Intra_;
    u64 currCountPart1_;
    u64 scratchOffsetCountIntraStage1_;
    u64 outputPtrOffsetIntra_;

    std::vector<std::vector<std::vector<u32>>> vTopo_;

    std::vector<ThreadHandle> intraThreads_;
    std::vector<ThreadHandle> interThreads_;

    ThreadHandle mainThread_;
    std::vector<ThreadHandle> templateMainThreads_;
    std::vector<u32> syncNotifyOnTemplates_;
    std::vector<u32> syncNotifyOnMain_;
    std::map<u32, std::vector<ChannelInfo>> intraLinks_;
    std::map<u32, std::vector<ChannelInfo>> interLinks_;

    std::vector<std::map<u32, std::vector<ChannelInfo>>> remoteRankToChannelInfo_;
    std::vector<ThreadHandle> threads_;

    std::map<u32, u64> rankBaseOffInterRSMap_;
    std::map<u32, u64> rankBaseOffIntraRSMap_;
    std::map<u32, u64> rankBaseOffInterAGMap_;
    std::map<u32, u64> rankBaseOffIntraAGMap_;

    std::map<u32, std::pair<u64, u64>> nhrPartDataMap_;
    std::map<u32, std::pair<u64, u64>> meshPartDataMap_;
    std::vector<std::vector<u32>> temp0HierarchyInfo_;
    std::vector<std::vector<u32>> temp1HierarchyInfo_;
};

} // namespace mc2_ops_hccl

#endif // INS_ALL_REDUCE_PARALLEL_EXECUTOR
