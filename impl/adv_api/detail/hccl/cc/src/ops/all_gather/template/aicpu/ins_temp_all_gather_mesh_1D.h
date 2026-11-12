/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_ALL_GATHER_MESH_1D_H
#define INS_TEMP_ALL_GATHER_MESH_1D_H

#include "alg_v2_template_base.h"
#include "executor_base.h"

namespace mc2_ops_hccl {

class InsTempAllGatherMesh1D : public InsAlgTemplateBase {
public:
    InsTempAllGatherMesh1D() = default;
    explicit InsTempAllGatherMesh1D(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    // Host侧调用
    ~InsTempAllGatherMesh1D() override;

    std::string Describe() const override
    {
        std::string info = "Template of all gather Mesh with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    HcclResult GetRes(AlgResourceRequest& resourceRequest) const override;

    // 设置Concurrent模板的物理通信轴，决定建链方式与对端读取接口。
    void SetConcurrentTopo(CommTopo topo) { concurrentTopo_ = topo; }

    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
    u64 GetThreadNum() const override;
    HcclResult CalcDataSplitByPortGroup(
        const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
        std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset) override;
    HcclResult SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels) override;
    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

protected:
    virtual HcclResult RunAllGatherMesh(
        const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels);
    HcclResult RunAllGatherMeshOnChannel(
        const ThreadHandle& thread, const ChannelInfo& linkRemote, void* remoteInputAddr, void* remoteCclBuffAddr,
        const u32 connectedRank, const u32 connectedAlgRank, const u32 channelIdx);
    // 非Concurrent模板的前置拷贝：本卡分片写入输出与CCL暂存区。
    virtual HcclResult LocalDataCopy(const std::vector<ThreadHandle>& threads);
    // Concurrent模板的分阶段拷贝：outputOnly=false仅暂存CCL，true仅生成本卡输出。
    HcclResult LocalDataCopyConcurrent(const std::vector<ThreadHandle>& threads, bool outputOnly);
    HcclResult PostLocalCopy(const std::vector<ThreadHandle>& threads);
    TemplateDataParams tempAlgParams_;
    u64 inputOffset_{0};
    u64 outputOffset_{0};
    void* inputSymWindow_{nullptr};
    void* outputSymWindow_{nullptr};
    u32 maxChannelsPerRank_{1};
    // 单peer数据按port group切分后的分片信息
    std::vector<u64> chanElemCount_;
    std::vector<u64> chanSize_;
    std::vector<u64> chanElemOffset_;

private:
    bool isConcurrent_{false};                  // 启用Concurrent的独立主流和分阶段拷贝。
    CommTopo concurrentTopo_{COMM_TOPO_1DMESH}; // 当前模板使用的物理通信轴。
};

} // namespace mc2_ops_hccl

#endif // INS_TEMP_ALL_GATHER_MESH_1D_H
