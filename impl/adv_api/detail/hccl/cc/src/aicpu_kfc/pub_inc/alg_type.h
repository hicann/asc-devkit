/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MC2_OPS_HCCL_SRC_COMMON_ALG_TYPE
#define MC2_OPS_HCCL_SRC_COMMON_ALG_TYPE

#include <string>
#include <map>

namespace mc2_ops_hccl {

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
    HCCL_ALGO_TYPE_AHC_BROKE
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
    ALG_LEVEL2_WHOLE_RING = 0, // 单层拓扑, 所有leve2均为Whole ring时，组成一个大环
    ALG_LEVEL2_HD,             // 拓扑组合2层, HDR
    ALG_LEVEL2_RING,           // 拓扑组合2层, Ring
    ALG_LEVEL2_NHR,            // 拓扑组合2层, NHR
    ALG_LEVEL2_NB,             // 拓扑组合2层, NB
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
    {AlgTypeLevel2::ALG_LEVEL2_NB, "NB"},           {AlgTypeLevel2::ALG_LEVEL2_RESERVED, "null"},
};

std::string AlgTypeToStr(const AlgType algType);

std::string TransferAlgTypeStr(AlgType algType);

std::string TransferAlgType(AlgType algType);
} // namespace mc2_ops_hccl

#endif
