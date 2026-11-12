/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_ALL_GATHER_OMNIPIPE_MESH_1D_H
#define INS_TEMP_ALL_GATHER_OMNIPIPE_MESH_1D_H

#include "ins_temp_all_gather_mesh_1D.h"
namespace mc2_ops_hccl {

class InsTempAllGatherOmniPipeMesh1D : public InsTempAllGatherMesh1D {
public:
    explicit InsTempAllGatherOmniPipeMesh1D(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    // Host侧调用
    ~InsTempAllGatherOmniPipeMesh1D() override;

    std::string Describe() const override
    {
        std::string info = "Template of all gather mesh (OmniPipe) with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;

private:
    struct MeshPeerSlices {
        std::vector<DataSlice> txSrcSlices_;
        std::vector<DataSlice> txDstSlices_;
        std::vector<DataSlice> rxSrcSlices_;
        std::vector<DataSlice> rxDstSlices_;
    };

    struct MeshSliceInfo {
        void* addr_;
        u64 offset_;
        u64 size_;
        u64 count_;
    };

    HcclResult RunAllGatherMesh(
        const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels) override;
    HcclResult RunMeshPeer(
        const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
        u32 myAlgRank, u32 threadIdx, u32 dataTypeSize);
    HcclResult GetPeerSymmetricPointers(u32 connectedRank, void*& remoteOut) const;
    void BuildSymmetricSlices(
        u32 myAlgRank, u32 connectedAlgRank, u32 connectedRank, u32 dataTypeSize, void* const remoteOut,
        MeshPeerSlices& slices);
    void BuildScratchSlices(
        u32 myAlgRank, u32 connectedAlgRank, u32 connectedRank, u32 dataTypeSize, void* remoteCclBuffAddr,
        MeshPeerSlices& slices);
    void BuildScratchWriteSlice(
        u32 myAlgRank, u32 connectedAlgRank, u32 connectedRank, u32 rpt, void* const remoteCclBuffAddr,
        MeshPeerSlices& slices);
    void BuildScratchReadSlice(
        u32 myAlgRank, u32 connectedAlgRank, u32 connectedRank, u32 dataTypeSize, u32 rpt,
        void* const remoteCclBuffAddr, MeshPeerSlices& slices);
    void AppendMeshSlices(
        const MeshSliceInfo& txSrc, const MeshSliceInfo& txDst, const MeshSliceInfo& rxSrc, const MeshSliceInfo& rxDst,
        const char* mode, u32 connectedRank, MeshPeerSlices& slices);
    void LogMeshSlice(const char* sliceName, const char* mode, u32 connectedRank, const MeshSliceInfo& slice) const;
    HcclResult ExchangeMeshSlices(
        const ChannelInfo& linkRemote, const ThreadHandle& thread, u32 connectedRank, u32 threadIdx,
        const MeshPeerSlices& slices) const;
    bool omniLastStepRead_ = false;
    u64 inputOffset_{0};
    u64 outputOffset_{0};
    void* inputSymWindow_;
    void* outputSymWindow_;
};

} // namespace mc2_ops_hccl

#endif // INS_TEMP_ALL_GATHER_OMNIPIPE_MESH_1D_H
