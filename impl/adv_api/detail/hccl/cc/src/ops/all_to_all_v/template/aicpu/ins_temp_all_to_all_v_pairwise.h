/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_ALL_TO_ALL_V_PAIRWISE_H
#define INS_TEMP_ALL_TO_ALL_V_PAIRWISE_H

#include "alg_v2_template_base.h"
#include "executor_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempAllToAllVPairwise : public InsAlgTemplateBase {
public:
    InsTempAllToAllVPairwise() = default;
    explicit InsTempAllToAllVPairwise(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);

    ~InsTempAllToAllVPairwise() override;

    std::string Describe() const override
    {
        std::string info = "Template of alltoallv pairwise with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    // 设计常数：每板 8 卡 × 2 流集合（一套做前半 rank 空间，一套做后半），全链路口径一致
    static constexpr u32 RANK_NUM_PER_BOARD = 8;
    static constexpr u32 THREAD_SET_NUM = 2;

    static bool IsSupportedTopology(const TopoInfoWithNetLayerDetails* topoInfo);

    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

private:
    // 拓扑校验与参数推导：CalcRes/InitParam 两入口共用，保证校验口径一致
    HcclResult CheckAndDeriveTopology();

    HcclResult InitParam(
        const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource);

    // 板间配对：boardNumPerStreamSet_ 为 2 的幂走 XOR（i ^ t）；否则走反射 (t - i) mod N（对合，任意 N 成立）
    // 组内（isInterStreamSet=false）：本流集合内板对，自环轮由 fullMesh 填空
    // 组间（isInterStreamSet=true）：对侧流集合板对，每轮为双射，覆盖 N² 跨流集合对
    u32 GetBoardSendRecvMatrix(u32 round, bool isInterStreamSet) const;
    // 板内配对：step s target rank = targetBoard * RANK_NUM_PER_BOARD + (rankIndex XOR s)
    u32 GetRankSendRecvMatrix(u32 targetBoard, u32 step) const;
    // cclBuff 槽位 = 分区基址 + step×RING_MAX_CH_NUM，调用侧按 channel 加 ch 偏移（每 channel 独占槽）
    void CalcCclBuffIdx(bool isInterStreamSet, u32 partnerIdx, u32& myCclBuffIdx, u32& remoteCclBuffIdx) const;

    HcclResult RunFullMesh(const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource);
    HcclResult RunRing(
        const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 round,
        bool isInterStreamSet);
    HcclResult RunRingStep(
        const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 targetRank,
        u32 myCclBuffIdx, u32 remoteCclBuffIdx, const std::vector<ThreadHandle>& subThreads);
    HcclResult PostCopy(
        const TemplateDataParams& tempAlgParams, const ThreadHandle& thread, u32 myCclBuffIdx, u32 remoteRank,
        const std::vector<u64>& recvOffsetSplit, const std::vector<u64>& recvSizeSplit);

    u64 dataTypeSize_{0};
    u32 boardNumPerStreamSet_{0};  // 每套流集合负责的板数，由 templateRankSize_ 推导
    u32 ringStepNum_{0};           // 每 round ring 内步数 = RANK_NUM_PER_BOARD
    u32 ringInSubThreadNum_{0};    // 组内 ring 子流数 = RANK_NUM_PER_BOARD × 2
    u32 ringInterSubThreadNum_{0}; // 组间 ring 子流数 = RANK_NUM_PER_BOARD × 2
    u32 ringInCclBuffBase_{0};     // 组内 ring cclBuff 基址
    u32 fullmeshCclBuffBase_{0};   // fullMesh cclBuff 基址
    u32 ringInterCclBuffBase_{0};  // 组间 ring cclBuff 基址
    u32 cclBuffSlotNum_{0};        // cclBuff 总槽数 = (2×RING_MAX_CH_NUM + 1) × RANK_NUM_PER_BOARD
    u64 scratchMultiple_{0};       // scratch 2 + cclBuffSlotNum_
    u32 myAlgRank_{0};
    u32 currBoard_{0};
    u32 currRankIndex_{0};
    u32 currStreamSet_{0};            // 本 rank 所属流集合编号（0=前半 rank 空间，1=后半）
    u64 scratchBufferSizePerRank_{0}; // = inputSliceStride

    std::vector<ThreadHandle> subThreadsRingIn_;    // 组内 ring 子流，fullMesh 复用前 RANK_NUM_PER_BOARD 条
    std::vector<ThreadHandle> subThreadsRingInter_; // 组间 ring 子流

    std::vector<u64> sendCountsSplit_;
    std::vector<u64> sendSizeSplit_;
    std::vector<u64> sendOffsetSplit_;
    std::vector<u64> recvCountsSplit_;
    std::vector<u64> recvSizeSplit_;
    std::vector<u64> recvOffsetSplit_;
};
} // namespace mc2_ops_hccl
#endif // INS_TEMP_ALL_TO_ALL_V_PAIRWISE_H
