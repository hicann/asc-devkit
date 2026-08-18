/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef COMMON_H
#define COMMON_H

#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "adapter_hccp_common.h"
#include "hccl_ip_address.h"
#include "hccl_common.h"
#include "transport_pub.h"
#include "externalinput_pub.h"

// sub stream 相关
constexpr s64 HCCL_SUB_STREAM_NUM_ZERO = 0;        // subStream 数量为0
constexpr s64 HCCL_SUB_STREAM_NUM_DOUBLE_RING = 1; // subStream 数量为1
constexpr s64 HCCL_SUB_STREAM_NUM_4P_MESH = 2;     // subStream 数量为2
constexpr s64 HCCL_SUB_STREAM_NUM_8P_RING = 3;     // subStream 数量为3
constexpr s64 HCCL_SUB_STREAM_NP_MESH = 2;         // NP MESH场景下subStream数应为ranksize - 2

// HCCL通信算法类型
enum class HcclAlgoType {
    HCCL_ALGO_TYPE_DEFAULT = 0, // 默认算法，配置为此时，使用HCCL内藏算法选择逻辑
    HCCL_ALGO_TYPE_RING,
    HCCL_ALGO_TYPE_PIPELINE,
    HCCL_ALGO_TYPE_FULLMESH,
    HCCL_ALGO_TYPE_HDR,
    HCCL_ALGO_TYPE_PAIRWISE,
    HCCL_ALGO_TYPE_NHR,
    HCCL_ALGO_TYPE_NHR_V1,
    HCCL_ALGO_TYPE_NB,
    HCCL_ALGO_TYPE_NULL,
    HCCL_ALGO_TYPE_NA,
    HCCL_ALGO_TYPE_AHC,
    HCCL_ALGO_TYPE_AHC_BROKE,
};

// 对内拓扑算法枚举
enum class AlgTypeLevel0 {
    ALG_LEVEL0_WHOLE_RING = 0, // 单层拓扑, 所有level均为Whole ring时，组成一个大环
    ALG_LEVEL0_8P_RING,        // 拓扑组合0层, Ring 节点内4个固定stream
    ALG_LEVEL0_4P_MESH,        // 拓扑组合0层, Mesh 节点内3个固定stream
    ALG_LEVEL0_2P_MESH,        // 拓扑组合0层, Mesh
    ALG_LEVEL0_1P_MESH,        // 拓扑组合0层, Mesh
    ALG_LEVEL0_4P_RING,        // 拓扑组合0层, Ring
    ALG_LEVEL0_NP_SINGLE_RING, // 拓扑组合0层, Ring
    ALG_LEVEL0_NP_DOUBLE_RING, // 拓扑组合0层, double Ring
    ALG_LEVEL0_NP_MESH,        // 拓扑组合0层, 服务器内3~8p rank组成MESH
    ALG_LEVEL0_NP_HD,          // 拓扑组合0层, HD
    ALG_LEVEL0_NP_STAR,
    ALG_LEVEL0_PAIRWISE,
    ALG_LEVEL0_RESERVED
};

enum class AlgTypeLevel1 {
    ALG_LEVEL1_WHOLE_RING = 0, // 单层拓扑, 所有level均为Whole ring时，组成一个大环
    ALG_LEVEL1_HD,             // 拓扑组合1层, HDR
    ALG_LEVEL1_RING,           // 拓扑组合1层, Ring
    ALG_LEVEL1_PIPELINE,       // 拓扑组合1层, Pipeline
    ALG_LEVEL1_STAR,
    ALG_LEVEL1_NHR,       // 拓扑组合1层，NHR
    ALG_LEVEL1_NHR_V1,    // 拓扑组合1层，NHR_V1
    ALG_LEVEL1_NB,        // 拓扑组合1层，NB
    ALG_LEVEL1_AHC,       // 拓扑组合1层，AHC
    ALG_LEVEL1_AHC_BROKE, // 拓扑组合1层，AHC_BROKE
    ALG_LEVEL1_RESERVED
};

enum class AlgTypeLevel2 {
    ALG_LEVEL2_WHOLE_RING = 0, // 单层拓扑, 所有level2均为Whole ring时，组成一个大环
    ALG_LEVEL2_HD,             // 拓扑组合2层, HDR
    ALG_LEVEL2_RING,           // 拓扑组合2层, Ring
    ALG_LEVEL2_NHR,            // 拓扑组合2层, NHR
    ALG_LEVEL2_NB,             // 拓扑组合2层, NB
    ALG_LEVEL2_PIPELINE,       // 拓扑组合2层, Pipeline
    ALG_LEVEL2_RESERVED
};

using AlgType = struct TagAlgType {
    AlgTypeLevel0 algoLevel0;
    AlgTypeLevel1 algoLevel1;
    AlgTypeLevel2 algoLevel2;
    TagAlgType()
        : algoLevel0(AlgTypeLevel0::ALG_LEVEL0_WHOLE_RING),
          algoLevel1(AlgTypeLevel1::ALG_LEVEL1_WHOLE_RING),
          algoLevel2(AlgTypeLevel2::ALG_LEVEL2_WHOLE_RING)
    {}

    explicit TagAlgType(AlgTypeLevel0 algoLevel0)
        : algoLevel0(algoLevel0),
          algoLevel1(AlgTypeLevel1::ALG_LEVEL1_WHOLE_RING),
          algoLevel2(AlgTypeLevel2::ALG_LEVEL2_WHOLE_RING)
    {}
    TagAlgType(AlgTypeLevel0 algoLevel0, AlgTypeLevel1 algoLevel1)
        : algoLevel0(algoLevel0), algoLevel1(algoLevel1), algoLevel2(AlgTypeLevel2::ALG_LEVEL2_WHOLE_RING)
    {}

    TagAlgType(AlgTypeLevel0 algoLevel0, AlgTypeLevel2 algoLevel2)
        : algoLevel0(algoLevel0), algoLevel1(AlgTypeLevel1::ALG_LEVEL1_WHOLE_RING), algoLevel2(algoLevel2)
    {}

    TagAlgType(AlgTypeLevel0 algoLevel0, AlgTypeLevel1 algoLevel1, AlgTypeLevel2 algoLevel2)
        : algoLevel0(algoLevel0), algoLevel1(algoLevel1), algoLevel2(algoLevel2)
    {}
    explicit TagAlgType(AlgTypeLevel1 algoLevel1)
        : algoLevel0(AlgTypeLevel0::ALG_LEVEL0_WHOLE_RING),
          algoLevel1(algoLevel1),
          algoLevel2(AlgTypeLevel2::ALG_LEVEL2_WHOLE_RING)
    {}

    TagAlgType(AlgTypeLevel1 algoLevel1, AlgTypeLevel2 algoLevel2)
        : algoLevel0(AlgTypeLevel0::ALG_LEVEL0_WHOLE_RING), algoLevel1(algoLevel1), algoLevel2(algoLevel2)
    {}

    explicit TagAlgType(AlgTypeLevel2 algoLevel2)
        : algoLevel0(AlgTypeLevel0::ALG_LEVEL0_WHOLE_RING),
          algoLevel1(AlgTypeLevel1::ALG_LEVEL1_WHOLE_RING),
          algoLevel2(algoLevel2)
    {}

    static TagAlgType Reserved()
    {
        return TagAlgType(
            AlgTypeLevel0::ALG_LEVEL0_RESERVED, AlgTypeLevel1::ALG_LEVEL1_RESERVED, AlgTypeLevel2::ALG_LEVEL2_RESERVED);
    }

    TagAlgType(const TagAlgType& that)
        : algoLevel0(that.algoLevel0), algoLevel1(that.algoLevel1), algoLevel2(that.algoLevel2)
    {}

    TagAlgType& operator=(const TagAlgType& that)
    {
        if (&that != this) {
            algoLevel0 = that.algoLevel0;
            algoLevel1 = that.algoLevel1;
            algoLevel2 = that.algoLevel2;
        }
        return *this;
    }
};

namespace hccl {
struct HcclTopoInfo {
    u32 userRank{0};
    u32 userRankSize{0};
    u32 devicePhyId{0};
    s32 deviceLogicId{0};
    std::vector<u32> nicList{};
    bool isSingleMeshAggregation{false};
    u32 deviceNumPerAggregation{0};
    u32 superPodNum{0};
    DevType deviceType{DevType::DEV_TYPE_COUNT};
    bool is310P3Common{false};
    u32 serverNum{0};
    u32 meshAggregationRankSize{0};
    u32 multiModuleDiffDeviceNumMode{0};
    u32 multiSuperPodDiffServerNumMode{0};
    u32 multiSuperPodDiffDeviceNumMode{0};
    bool isDiffDeviceType{false};
    u32 gcdDeviceNumPerAggregation{0};
    u32 realUserRank{0};
    bool isDiffDeviceModule{false};
    u32 moduleNum{0};
    bool useSuperPodMode{false};
    std::unordered_map<u32, bool> isUsedRdmaMap{};
    std::unordered_map<u32, u32> pairLinkCounter{};
    bool isARSDoubleRing{true};
};

// 通信域内rank信息
using RankInfo = struct TagRankInfo {
public:
    /* * 用户的原始属性 */
    u32 userRank{INVALID_UINT};                  // 当前rank的user rank ID
    u32 worldRank{INVALID_UINT};                 // 当前rank的global user rank ID
    u32 localRank{INVALID_UINT};                 // 当前rank在server内的rank ID
    s32 devicePhyId{-1};                         // 当前rank在操作的设备物理编号
    DevType deviceType{DevType::DEV_TYPE_NOSOC}; // 当前rank在操作的设备属性
    std::vector<HcclIpAddress> nicIp;            // 当前rank所归属网卡的IP值(实际建链所用网卡IP)
    std::vector<HcclIpAddress> backupNicIp;      // 当前rank所在卡备用网卡的IP值
    u32 deviceNicPort{HCCL_INVALID_PORT};        // 当前rank使用的device socket port
    u32 deviceVnicPort{HCCL_INVALID_PORT};       // 当前rank使用的device socket port
    u32 backupDevicePort{HCCL_INVALID_PORT};     // 当前rank使用的备用网卡的device socket port
    std::vector<u32> nicIdx;                     // 当前rank在server内的所有device网卡位置（devicePhyId）
    NICDeployment nicDeploy{NICDeployment::NIC_DEPLOYMENT_DEVICE}; // 参数平面位置 0:host 1:device other:reserve
    HcclIpAddress hostIp;                                          // 当前rank使用的host socket addr
    u32 hostPort{HCCL_INVALID_PORT};                               // 当前rank使用的host socket port
    std::string serverId{""};        // 当前rank所在服务器的唯一标识值 (rank_table中的server id)
    u32 serverIdx{INVALID_UINT};     // Server在ranktable中的自然顺序（用户指定）
    u32 superDeviceId{INVALID_UINT}; // 当前rank所在的超节点内的device id（sdid）
    std::string superPodId{""};      // 当前rank所在超节点id
    u32 superPodIdx{INVALID_UINT};   // SuperPod在ranktable中的自然顺序（用户指定)
    HcclIpAddress deviceVnicIp; // 当前rank所归属网卡的虚拟ip                                                //
                                // 当前rank所在通信域的server数
};

constexpr s64 HCCL_SMALL_COUNT_128_KB = 128 * 1024; // hccl 910B/310P duo卡单算子小数据量标准，暂定128kb
constexpr s64 HCCL_SMALL_COUNT_GRAPH_64_KB = 64 * 1024;       // hccl图模式小数据量标准，暂定64KB
constexpr s64 HCCL_MEDIUM_COUNT_GRAPH_4_MB = 4 * 1024 * 1024; // hccl图模式中数据量标准，暂定4MB
constexpr s64 HCCL_SMALL_COUNT_256_KB = 256 * 1024;           // 910B/310p V卡hccl小数据量标准，暂定256KB

constexpr s64 HCCL_ALIGN_COUNT_32_B = 32;           // 910B aiv+rdma，中数据量情况下按照32B对齐
constexpr s64 HCCL_SMALL_COUNT_190_KB = 190 * 1024; // 910B aiv+rdma，暂定从OutAIVBuffer的512K位置开始存放标记
constexpr s64 HCCL_SMALL_COUNT_512_KB = 512 * 1024; // 910B aiv+rdma，暂定从OutAIVBuffer的512K位置开始存放标记
constexpr s64 HCCL_SMALL_COUNT_1_MB = 1024 * 1024; // 910BB aiv+rdma，AIVIN的最后1M空间用作标记区
constexpr s64 HCCL_SMALL_COUNT_2_MB = 2048 * 1024;
constexpr s64 HCCL_SMALL_COUNT_4_MB = 4096 * 1024;
constexpr s64 HCCL_SMALL_COUNT_8_MB = 8192 * 1024;
constexpr s64 HCCL_SMALL_COUNT_16_KB = 16 * 1024;      // A3打平ROCE
constexpr s64 HCCL_MID_COUNT_16_MB = 16 * 1024 * 1024; // 910B aiv+rdma 中数据量支持上限
constexpr s64 HCCL_MID_COUNT_32_MB = 32 * 1024 * 1024;

constexpr u32 HCCL_INTER_SERVER_RING_ALGO_MAX_SUPPORT_SERVER_NUM = 8; // server 间 ring 算法支持的最大server数: 8

constexpr u32 HCCL_ALLTOALLV_P2P_SIZE = 2; // alltoallv不区分全连接、分级的最大ranksize

constexpr u32 HCCL_MEMSIZE_HD_FACTOR = 4;

// a2a前置allgather和a2a注册的notify资源要隔离, tag不能包含HCCL_ALLTOALL字符串
const std::string HCCL_ALLTOALL_PARA_ALLGATHER = "AllgatherForCollectA2APara";

// 标识MC2多机的tag后缀，用于区分单机和多机MC2场景，避免资源复用出错
const std::string HCCL_MC2_MULTISERVER_SUFFIX = "_MC2MultiServer";

const std::map<AlgTypeLevel0, std::string> HCCL_ALGO_LEVEL0_NAME_MAP = {
    {AlgTypeLevel0::ALG_LEVEL0_NP_DOUBLE_RING, "ring"}, {AlgTypeLevel0::ALG_LEVEL0_WHOLE_RING, "ring"},
    {AlgTypeLevel0::ALG_LEVEL0_8P_RING, "ring"},        {AlgTypeLevel0::ALG_LEVEL0_4P_MESH, "fullmesh"},
    {AlgTypeLevel0::ALG_LEVEL0_2P_MESH, "fullmesh"},    {AlgTypeLevel0::ALG_LEVEL0_1P_MESH, "fullmesh"},
    {AlgTypeLevel0::ALG_LEVEL0_4P_RING, "ring"},        {AlgTypeLevel0::ALG_LEVEL0_NP_SINGLE_RING, "ring"},
    {AlgTypeLevel0::ALG_LEVEL0_NP_MESH, "fullmesh"},    {AlgTypeLevel0::ALG_LEVEL0_NP_HD, "HD"},
    {AlgTypeLevel0::ALG_LEVEL0_NP_STAR, "star"},        {AlgTypeLevel0::ALG_LEVEL0_RESERVED, "null"},
};

const std::map<AlgTypeLevel1, std::string> HCCL_ALGO_LEVEL1_NAME_MAP = {
    {AlgTypeLevel1::ALG_LEVEL1_WHOLE_RING, "ring"}, {AlgTypeLevel1::ALG_LEVEL1_HD, "H-D"},
    {AlgTypeLevel1::ALG_LEVEL1_RING, "ring"},       {AlgTypeLevel1::ALG_LEVEL1_PIPELINE, "pipeline"},
    {AlgTypeLevel1::ALG_LEVEL1_NHR, "NHR"},         {AlgTypeLevel1::ALG_LEVEL1_NHR_V1, "NHR_V1"},
    {AlgTypeLevel1::ALG_LEVEL1_AHC, "AHC"},         {AlgTypeLevel1::ALG_LEVEL1_AHC_BROKE, "AHC_BROKE"},
    {AlgTypeLevel1::ALG_LEVEL1_NB, "NB"},           {AlgTypeLevel1::ALG_LEVEL1_RESERVED, "null"},
};

const std::map<AlgTypeLevel2, std::string> HCCL_ALGO_LEVEL2_NAME_MAP = {
    {AlgTypeLevel2::ALG_LEVEL2_WHOLE_RING, "ring"}, {AlgTypeLevel2::ALG_LEVEL2_HD, "H-D"},
    {AlgTypeLevel2::ALG_LEVEL2_RING, "ring"},       {AlgTypeLevel2::ALG_LEVEL2_NHR, "NHR"},
    {AlgTypeLevel2::ALG_LEVEL2_NB, "NB"},           {AlgTypeLevel2::ALG_LEVEL2_PIPELINE, "pipeline"},
    {AlgTypeLevel2::ALG_LEVEL2_RESERVED, "null"},
};

const std::map<AlgTypeLevel0, std::string> HCCL_ALGO_LEVEL0_MAP = {
    {AlgTypeLevel0::ALG_LEVEL0_NP_DOUBLE_RING, "RING"}, {AlgTypeLevel0::ALG_LEVEL0_WHOLE_RING, "RING"},
    {AlgTypeLevel0::ALG_LEVEL0_8P_RING, "RING"},        {AlgTypeLevel0::ALG_LEVEL0_4P_MESH, "MESH"},
    {AlgTypeLevel0::ALG_LEVEL0_2P_MESH, "MESH"},        {AlgTypeLevel0::ALG_LEVEL0_1P_MESH, "MESH"},
    {AlgTypeLevel0::ALG_LEVEL0_4P_RING, "RING"},        {AlgTypeLevel0::ALG_LEVEL0_NP_SINGLE_RING, "RING"},
    {AlgTypeLevel0::ALG_LEVEL0_NP_MESH, "MESH"},        {AlgTypeLevel0::ALG_LEVEL0_NP_HD, "HD"},
    {AlgTypeLevel0::ALG_LEVEL0_NP_STAR, "STAR"},        {AlgTypeLevel0::ALG_LEVEL0_PAIRWISE, "PAIRWISE"},
    {AlgTypeLevel0::ALG_LEVEL0_RESERVED, "NA"},
};

const std::map<AlgTypeLevel1, std::string> HCCL_ALGO_LEVEL1_MAP = {
    {AlgTypeLevel1::ALG_LEVEL1_WHOLE_RING, "RING"}, {AlgTypeLevel1::ALG_LEVEL1_HD, "HD"},
    {AlgTypeLevel1::ALG_LEVEL1_RING, "RING"},       {AlgTypeLevel1::ALG_LEVEL1_PIPELINE, "PIPELINE"},
    {AlgTypeLevel1::ALG_LEVEL1_NHR, "NHR"},         {AlgTypeLevel1::ALG_LEVEL1_NHR_V1, "NHR_V1"},
    {AlgTypeLevel1::ALG_LEVEL1_AHC, "AHC"},         {AlgTypeLevel1::ALG_LEVEL1_AHC_BROKE, "AHC_BROKE"},
    {AlgTypeLevel1::ALG_LEVEL1_NB, "NB"},           {AlgTypeLevel1::ALG_LEVEL1_STAR, "STAR"},
    {AlgTypeLevel1::ALG_LEVEL1_RESERVED, "NA"},
};

const std::map<AlgTypeLevel2, std::string> HCCL_ALGO_LEVEL2_MAP = {
    {AlgTypeLevel2::ALG_LEVEL2_WHOLE_RING, "RING"}, {AlgTypeLevel2::ALG_LEVEL2_HD, "HD"},
    {AlgTypeLevel2::ALG_LEVEL2_RING, "RING"},       {AlgTypeLevel2::ALG_LEVEL2_NHR, "NHR"},
    {AlgTypeLevel2::ALG_LEVEL2_NB, "NB"},           {AlgTypeLevel2::ALG_LEVEL2_PIPELINE, "PIPELINE"},
    {AlgTypeLevel2::ALG_LEVEL2_RESERVED, "NA"},
};

inline std::string TransferAlgType(AlgType algType)
{
    std::string algTypeStr;
    auto level0Iter = HCCL_ALGO_LEVEL0_MAP.find(algType.algoLevel0);
    auto level1Iter = HCCL_ALGO_LEVEL1_MAP.find(algType.algoLevel1);
    auto level2Iter = HCCL_ALGO_LEVEL2_MAP.find(algType.algoLevel2);
    if (level0Iter == HCCL_ALGO_LEVEL0_MAP.end() || level1Iter == HCCL_ALGO_LEVEL1_MAP.end() ||
        level2Iter == HCCL_ALGO_LEVEL2_MAP.end()) {
        algTypeStr = "not found";
        HCCL_WARNING("No matching algLevel0 or algLevel1 or algLevel2 is found.");
    } else {
        algTypeStr.append(static_cast<std::string>(level0Iter->second))
            .append("-")
            .append(static_cast<std::string>(level1Iter->second))
            .append("-")
            .append(static_cast<std::string>(level2Iter->second));
        HCCL_INFO("The algtype for profiling is [%s]", algTypeStr.c_str());
    }

    return algTypeStr;
}

inline bool TransferStrToAlgType(const std::string& algTypeStr, AlgType& algType)
{
    // 分割字符串
    std::vector<std::string> parts;
    std::stringstream ss(algTypeStr);
    std::string part;
    constexpr u32 algTypeLevelNum = 3;

    while (std::getline(ss, part, '-')) {
        parts.push_back(part);
    }

    // 检查格式
    if (parts.size() != algTypeLevelNum) {
        HCCL_ERROR("Invalid algType string format: %s", algTypeStr.c_str());
        return false;
    }
    int level0Val = std::stoi(parts[0]);
    int level1Val = std::stoi(parts[1]);
    int level2Val = std::stoi(parts[2]);
    if (level0Val >= static_cast<int>(AlgTypeLevel0::ALG_LEVEL0_RESERVED) || level0Val < 0 ||
        level1Val >= static_cast<int>(AlgTypeLevel1::ALG_LEVEL1_RESERVED) || level1Val < 0 ||
        level2Val >= static_cast<int>(AlgTypeLevel2::ALG_LEVEL2_RESERVED) || level2Val < 0) {
        HCCL_ERROR("Invalid algType string format: %s", algTypeStr.c_str());
        return false;
    }
    algType.algoLevel0 = static_cast<AlgTypeLevel0>(level0Val);
    algType.algoLevel1 = static_cast<AlgTypeLevel1>(level1Val);
    algType.algoLevel2 = static_cast<AlgTypeLevel2>(level2Val);

    HCCL_INFO("Successfully parsed algType from string: %s", algTypeStr.c_str());
    return true;
}

} // namespace hccl

enum class TopoType {
    TOPO_TYPE_COMMON = 0,         // 普通拓扑类型 ，default单层拓扑使用
    TOPO_TYPE_8P_RING = 1,        // 特殊场景, 服务器内8 rank组成一个ring，4个逻辑环
    TOPO_TYPE_4P_MESH = 2,        // 特殊场景, 服务器内4 rank组成MESH
    TOPO_TYPE_2P_MESH = 3,        // 特殊场景, 服务器内2 rank组成MESH。仅用于测试和自验证
    TOPO_TYPE_1P_MESH = 4,        // 特殊场景, 服务器内1 rank组成MESH。仅用于测试和自验证
    TOPO_TYPE_4P_RING = 5,        // 特殊场景，服务器内4 rank组成ring
    TOPO_TYPE_NP_SINGLE_RING = 6, // 特殊场景, 服务器内n rank组成单 ring。目前仅用于标卡
    TOPO_TYPE_8P_MESH = 7,        // 特殊场景, 服务器内8 rank通过RDMA组成MESH
    TOPO_TYPE_NP_MESH = 8,        // 特殊场景, 服务器内3~8p rank组成MESH
    TOPO_TYPE_NP_DOUBLE_RING = 9, // 特殊场景, 910_93场景
    TOPO_TYPE_HETEROG = 10,
    TOPO_TYPE_ES_MESH = 11,
    TOPO_TYPE_RESERVED
};

#endif /* COMMON_H */
