/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_ALG_TEMPLATE_BASE
#define HCCL_CCU_ALG_TEMPLATE_BASE

#include "template_utils.h"
#include "alg_param.h"

namespace mc2_ops_hccl {

constexpr uint32_t CCU_DIE_NUM_MAX_2 = 2;

class CcuAlgTemplateBase {
public:
    explicit CcuAlgTemplateBase();
    explicit CcuAlgTemplateBase(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);

    virtual ~CcuAlgTemplateBase();

    virtual std::string Describe() const = 0;

    virtual HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest);
    virtual HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource);
    virtual HcclResult FastLaunch(const OpParam& param, const TemplateFastLaunchCtx& tempFastLaunchCtx);

    virtual HcclResult GetRes(AlgResourceRequest& resourceRequest) const;
    virtual u64 GetThreadNum() const;

    virtual u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType);

    uint64_t PointerToAddr(void* pointer) const;

    HcclResult GetToken(const BuffInfo& buffinfo, uint64_t& token) const;

    HcclResult GetChannelDieId(
        HcclComm comm, uint32_t rankId, const HcclChannelDesc& channelDesc, uint32_t& dieId) const;

protected:
    OpMode opMode_ = OpMode::OPBASE;
    u32 myRank_ = INVALID_VALUE_RANKID;
    u32 root_ = 0;
    u32 templateRankSize_ = 0;
    uint64_t scratchBufferSize_ = 0;
    HcclDataType dataType_ = HcclDataType::HCCL_DATA_TYPE_RESERVED;
    HcclReduceOp reduceOp_ = HcclReduceOp::HCCL_REDUCE_RESERVED;
    BuffInfo buffInfo_{};
    std::vector<std::vector<u32>> subCommRanks_;
    HcclResult RestoreChannelMap(
        const std::vector<HcclChannelDesc>& channelDescs,
        std::map<u32, std::vector<HcclChannelDesc>>& rankIdToChannelDesc) const;

    HcclResult SelectChannelToVec(
        const HcclComm comm, const u32 myRankId, const u32 rmtRankId,
        const std::map<u32, std::vector<HcclChannelDesc>>& rankIdToChannelDesc, const u32 dieId,
        std::map<u32, u32>& rank2ChannelIdx, std::vector<HcclChannelDesc>& channels) const;

    HcclResult GetDieInfoFromChannelDescs(
        HcclComm comm, const std::map<u32, std::vector<HcclChannelDesc>>& rankIdToChannelDesc, u32 myRankId,
        uint32_t& dieNum, uint32_t& dieId);
};
} // namespace mc2_ops_hccl
#endif // HCCLV2_CCU_ALG_TEMPLATE_BASE
