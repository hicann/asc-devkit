/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_TOPO_MODEL_H
#define SIM_TOPO_MODEL_H

#include "hccl_sim_pub.h"
#include "hccl_common.h"
#include "hccl_rank_graph.h"

namespace HcclSim {
class TopoModel {
public:
    TopoModel() = delete;
    TopoModel(const TopoMeta& topoMeta);
    ~TopoModel() = default;
    uint32_t GetRankSize() const;
    void GetNetLayers(uint32_t** netLayers, uint32_t* netLayerNum);
    void GetInstSizeByNetLayer(uint32_t curRank, uint32_t netLayer, uint32_t* rankNum);
    void GetLinks(
        DevType devType, uint32_t netLayer, uint32_t srcRank, uint32_t dstRank, CommLink** linkList,
        uint32_t* listSize);
    void GetInstSizeListByNetLayer(uint32_t netLayer, uint32_t** instSizeList, uint32_t* listSize);
    void GetInstTopoTypeByNetLayer(DevType devType, uint32_t netLayer, CommTopo* topoType);
    void GetInstRanksByNetLayer(uint32_t curRank, uint32_t netLayer, uint32_t** ranks, uint32_t* rankNum);
    // A5 topo
    void GetTopoInstsByLayer(uint32_t curRank, uint32_t netLayer, uint32_t** topoInsts, uint32_t* topoInstNum);
    void GetRanksByTopoInst(
        uint32_t curRank, uint32_t netLayer, uint32_t topoInstId, uint32_t** ranks, uint32_t* rankNum);
    void GetTopoType(uint32_t curRank, uint32_t netLayer, uint32_t topoInstId, CommTopo* topoType);
    void GetEndpointNum(uint32_t curRank, uint32_t layer, uint32_t topoInstId, uint32_t* num);
    void GetEndpointDesc(
        uint32_t curRank, uint32_t layer, uint32_t topoInstId, uint32_t* descNum, EndpointDesc* endpointDesc);

private:
    void InitNetLayerInfo(uint32_t serverNum, uint32_t podNum);
    void InitEndpointMap(uint32_t rankId, uint32_t phyId, uint32_t serverId, uint32_t superpodId, uint32_t& devIpStart);
    void Init910BLinkMap();
    void Init910CLinkMap();
    void Init910DLinkMap();
    void InitTopoInstsMap(uint32_t serverId, uint32_t rankId, const std::vector<uint32_t>& phyIds);
    std::vector<uint32_t> GetAllRanks(const std::vector<uint32_t>& phyIds, uint32_t rankId);
    std::vector<uint32_t> CheckRowPeerDevice(const std::vector<uint32_t>& phyIds, uint32_t rankId, uint32_t rowId);
    std::vector<uint32_t> CheckColPeerDevice(const std::vector<uint32_t>& phyIds, uint32_t rankId, uint32_t colId);
    bool IsSamePod(uint32_t srcRank, uint32_t dstRank);
    bool IsSameServer(uint32_t srcRank, uint32_t dstRank);
    void Create910BLinks(uint32_t srcRank, uint32_t dstRank);
    void Create910CLinks(uint32_t srcRank, uint32_t dstRank);
    void Create910DLinks(uint32_t srcRank, uint32_t dstRank);
    void InitL1L2TopoInsts(uint32_t podNum);
    void InitHostDpuInfo(uint32_t serverNum);

private:
    bool is2D{false};
    bool isDpuEnable{false};
    std::vector<uint32_t> allRankList_;
    std::map<uint32_t, std::vector<uint32_t>> serverId2RankList_;
    std::map<uint32_t, std::vector<uint32_t>> podId2RankList_;
    std::vector<uint32_t> netLayerList_;
    std::map<uint32_t, EndpointDesc> rankId2Endpoint_;
    std::map<uint32_t, uint32_t> rankId2PhyId_;
    std::map<uint32_t, uint32_t> rankId2ServerId_;
    std::map<uint32_t, uint32_t> rankId2PodId_;

    std::vector<uint32_t> podServersGroup_;
    std::vector<uint32_t> podRanksGroup_;
    std::vector<uint32_t> allRankNum_;

    // A5 topo
    std::map<uint32_t, std::map<uint32_t, std::vector<uint32_t>>> dev2TopoInsts_; // serverId->每个dev对应的实例Id
    std::map<uint32_t, std::map<uint32_t, std::vector<uint32_t>>> instId2RankIds_; // serverId->每个实例对应的rankId集合
    std::map<uint32_t, std::vector<uint32_t>> level1TopoInsts_;
    std::vector<uint32_t> level2TopoInsts_;
    std::vector<EndpointDesc> dpuDesc_; // 模拟dpu网卡

    // links map
    std::map<std::pair<uint32_t, uint32_t>, CommProtocol> link910b_;
    std::map<std::pair<uint32_t, uint32_t>, CommProtocol> link910c_;
    std::map<std::pair<uint32_t, uint32_t>, CommProtocol> link910d_;
    //                srcRabkId  dstRankId           level     links
    std::map<std::pair<uint32_t, uint32_t>, std::map<uint32_t, std::vector<CommLink>>> allLinkMap_;
}; // TopoModel

}; // namespace HcclSim

#endif // SIM_TOPO_MODEL_H
