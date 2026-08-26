/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MC2_OPS_HCCL_SRC_OPS_CHANNEL
#define MC2_OPS_HCCL_SRC_OPS_CHANNEL

#include "hccl/base.h"
#include "alg_param.h"

namespace mc2_ops_hccl {

enum CommPlane { COMM_LEVEL0 = 0, COMM_LEVEL1, COMM_LEVEL2, COMM_LEVEL_RESERVED };
constexpr u32 NORMAL_NOTIFY_NUM = 3;

HcclResult CalcLevel0ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels);
HcclResult CalcLevel1ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels);
HcclResult CalcLevel2ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels);
HcclResult CalcChannelRequestMesh1D(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels);
HcclResult CalcChannelRequestMesh1DLevel0(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels);
HcclResult CalcChannelRequestMesh1DLevel1(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels);
HcclResult CalcChannelRequestNhr(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels);
HcclResult CalcChannelRequestMesh2D(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels);
HcclResult CreateChannelRequestByRankId(
    HcclComm comm, const OpParam& param, u32 myRank, u32 remoteRank, std::vector<HcclChannelDesc>& channels,
    u32 channelRepeatNum = 1);
HcclResult CalcChannelRequestMesh1DWithPriorityTopo(
    HcclComm comm, const OpParam& param, const TopoInfo* topoInfo, const std::vector<std::vector<u32>>& subcommInfo,
    std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo);
HcclResult CalcChannelRequestNHRWithPriorityTopo(
    HcclComm comm, const OpParam& param, const TopoInfo* topoInfo, const std::vector<std::vector<u32>>& subcommInfo,
    std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo);
HcclResult GetTopoTypeByLink(HcclComm comm, uint32_t netLayer, const CommLink& link, CommTopo& topoType);
HcclResult ProcessLinksForChannel(
    HcclComm comm, u32 myRank, u32 rank, std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo);
HcclResult ProcessLinksForChannelMutiJetty(
    HcclComm comm, CommProtocol expectedProtocol, const std::vector<CommLink>& linkList, u32 myRank, u32 remoteRank,
    uint32_t netLayer, std::vector<HcclChannelDesc>& channels, bool expectMesh, bool isIsolation = false);
HcclResult CalcChannelRequestMeshClosMultiJetty(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels, bool isIsolation = false,
    bool expectMesh = false);
HcclResult GetProtocolByEngine(const OpParam& param, std::vector<CommProtocol>& protocols);
} // namespace mc2_ops_hccl

#endif
