/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef AIV_ALG_TEMPLATE_BASE
#define AIV_ALG_TEMPLATE_BASE

#include <memory>
#include <vector>
#include "alg_template_base.h"
#include "alg_param.h"
#include "template_utils.h"

namespace mc2_ops_hccl {
constexpr uint64_t TOPO_LEN_Y_OFFSET = 8;
constexpr uint64_t TOPO_LEN_Z_OFFSET = 16;
constexpr uint64_t MAX_DIM_NUM = 3;

class AivAlgTemplateBase {
public:
    explicit AivAlgTemplateBase();
    explicit AivAlgTemplateBase(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    virtual ~AivAlgTemplateBase();

    virtual std::string Describe() const = 0;
    virtual HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest);
    virtual u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType);
    virtual HcclResult CalNumBlocks(u32& numBlocks, u64 dataSize, u32 numBlocksLimit);
    virtual HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource);
    virtual HcclResult FastLaunch(const OpParam& param, const TemplateFastLaunchCtx& tempFastLaunchCtx);
    // Sync
    HcclResult PreSync(const u32 queIdx, const std::vector<ThreadHandle>& threads) const;
    HcclResult PostSync(const u32 queIdx, const std::vector<ThreadHandle>& threads) const;
    HcclResult PreSyncInterQueues(const std::vector<ThreadHandle>& threads) const;
    HcclResult PostSyncInterQueues(const std::vector<ThreadHandle>& threads) const;

protected:
    void IncSliceId();

    OpMode opMode_; // 单算子还是图模式
    u32 root_ = 0;  // 一般是scatter、broadcast需要
    u32 myRank_ = INVALID_VALUE_RANKID;
    u32 tempRankSize_ = 0;
    std::vector<std::vector<u32>> subCommRanks_;
    BuffInfo buffInfo_;
    u32 threadNum_ = 0;
    HcclReduceOp reduceOp_;
    HcclDataType dataType_;
    // 从OpParam中获取
    bool enableDetour_ = false;
    u32 sliceId_ = 0;
};

} // namespace mc2_ops_hccl

#endif // AIV_ALG_TEMPLATE_BASE
