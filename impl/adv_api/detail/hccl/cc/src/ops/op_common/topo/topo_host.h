/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MC2_OPS_HCCL_SRC_OPS_TOPO_HOST
#define MC2_OPS_HCCL_SRC_OPS_TOPO_HOST

#include <hccl/hccl_types.h>
#include "hccl/base.h"
#include "log.h"
#include "alg_param.h"
#include "hccl_rank_graph.h"
#include "hccl_res.h"
#include "dlsym_common.h"

namespace mc2_ops_hccl {

constexpr u32 BIG_CLOS_RANGE = 8;
constexpr s32 DEVICE_PER_MODULE_A2 = 8;
enum class HcclNetLayer {
    HCCL_NetLayer_L0 = 0,
    HCCL_NetLayer_L1,
    HCCL_NetLayer_L2,
    HCCL_NetLayer_L3,
    HCCL_NetLayer_MAX,
};

HcclResult InitRankInfo(HcclComm comm, TopoInfo* topoInfo);
HcclResult InitRankInfo(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo);

HcclResult CalcMyRankInfo(HcclComm comm, TopoInfo* topoInfo);
HcclResult SetServerModuleInfo(HcclComm comm, TopoInfo* topoInfo, const std::unordered_map<u32, u32>& pairLinkCounter);
HcclResult SetSuperPodInfo(HcclComm comm, TopoInfo* topoInfo);
bool IsDiffDeviceModule(const TopoInfo* topoInfo, const std::unordered_map<u32, u32>& pairLinkCounter);

HcclResult CalcLinkInfo(TopoInfo* topoInfo, const std::unordered_map<u32, u32>& pairLinkCounter);
HcclResult CalcLevel0MeshType(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo);

/**
 * Calculates the group index of the current process in the specified network layer.
 * This function determines the position index of the group that the current process belongs to
 * by obtaining the list of rank counts for all groups in the current network layer and comparing
 * the cumulative sum with the global rank value of the current process.
 *
 * @param comm Hccl communication domain, representing a process group
 * @param topoInfo Pointer to the topology information structure
 * @param netLayer Network layer identifier, specifying the communication level to calculate
 * @return HCCL_SUCCESS on success, other values indicate failure
 */
HcclResult CalcGroupIdx(HcclComm comm, TopoInfo* topoInfo, uint32_t netLayer);
/**
 * Counts the number of links between all pairs of ranks in the communication domain.
 * This function iterates through all pairs of user ranks and identifies the communication links
 * between them, then categorizes and counts links by their protocol types.
 *
 * @param comm Hccl communication domain
 * @param topoInfo Pointer to the topology information structure
 * @param pairLinkCounter Reference to an unordered map that will store the count of links by protocol type
 * @return HCCL_SUCCESS on success, other values indicate failure
 */
HcclResult GetPairLinkCounter(HcclComm comm, TopoInfo* topoInfo, std::unordered_map<u32, u32>& pairLinkCounter);
/**
 * Calculates the module index based on sever index and device type.
 * For certain device types (like 910B) with different device modules, the module index
 * is calculated differently than the standard case.
 *
 * @param topoInfo Pointer to the topology information structure
 * @return HCCL_SUCCESS on success, other values indicate failure
 */
HcclResult GetModuleIdx(HcclComm comm, TopoInfo* topoInfo);
HcclResult GetModuleIdxByRank(HcclComm comm, uint32_t rank, const TopoInfo* topoInfo, uint32_t& moduleIdx);
HcclResult GetModuleMap(HcclComm comm, TopoInfo* topoInfo, std::map<u32, std::vector<u32>>& moduleMap);
uint32_t GetCurrentServerStartRank(HcclComm comm, const TopoInfo* topoInfo);
uint32_t GetCurrentServerEndRank(HcclComm comm, const TopoInfo* topoInfo);
HcclResult GetDeviceNumPerModule(HcclComm comm, TopoInfo* topoInfo, std::map<u32, std::vector<u32>>& moduleMap);
/**
 * Calculates the number of servers per superpod based on netlayer L0 and L1 rank information.
 *
 * @param l0Sizes Vector containing the number of ranks in each server (L0 level)
 * @param l1Sizes Vector containing the number of ranks in each superpod (L1 level)
 * @param serversPerSuperPod Vector containing the number of servers in each superpod
 * @return HCCL_SUCCESS on success, other values indicate failure
 */
HcclResult CalculateServersPerSuperPod(
    const std::vector<uint32_t>& l0Sizes, const std::vector<uint32_t>& l1Sizes,
    std::vector<uint32_t>& serversPerSuperPod);

HcclResult CalcTopoShape(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo);

HcclResult ExtractNetLayerDetails(const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo);

HcclResult ExtractTopoDetails(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo);

HcclResult Is2DieFullMesh(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo);

HcclResult CalAllLevelEndpointAttrBwCoeff(
    HcclComm comm, uint32_t rankId, uint32_t levelSize, std::vector<std::vector<EndpointAttrBwCoeff>>& endpointAttrBw);

HcclResult IsLevel0PcieMix(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo);

template <typename T>
bool is_uniform(const std::vector<T>& vec);
} // namespace mc2_ops_hccl

#endif
