/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_KFC_SERVER_PROTOCOL_H
#define HCCL_KFC_SERVER_PROTOCOL_H

#include <cstdint>
#include <cstring>
#include <vector>
#include "../../../common/kfc_server_message.h"

namespace mc2_ops_hccl {

using namespace HcclKfcProtocol;

constexpr uint32_t KFC_SERVER_PROTOCOL_VERSION = 2U;
constexpr uint32_t KFC_SERVER_ARG_NUM = 6U;
constexpr uint32_t KFC_SERVER_XN_ADDR_ARG_INDEX = 0U;
constexpr uint32_t KFC_SERVER_CKE_ADDR_ARG_INDEX = 1U;
constexpr uint32_t KFC_SERVER_MISSION_NUM_ARG_INDEX = 3U;
constexpr uint32_t KFC_SERVER_MISSION_INDEX_ARG_INDEX = 4U;
constexpr uint32_t KFC_SERVER_TOKEN_ARG_INDEX = 5U;
constexpr uint32_t KFC_SERVER_MAX_MISSION_NUM = KFC_MAX_MISSION_NUM;
constexpr uint32_t KFC_SERVER_SIGNAL_REGION_NUM = KFC_SIGNAL_REGION_NUM;

constexpr char KFC_CONCURRENT_ALL_GATHER_ALG_NAME[] = "CcuSchedAllGatherConcurMeshNHRMultiLink";

struct KfcNhrStepInfo {
    uint32_t step = 0;
    uint32_t myRank = 0;
    uint32_t nSlices = 0;
    uint32_t toRank = 0;
    uint32_t fromRank = 0;
    std::vector<uint32_t> txSliceIdxs;
    std::vector<uint32_t> rxSliceIdxs;
};

inline uint32_t GetKfcServerMissionNum(const char* algName)
{
    return algName != nullptr && std::strcmp(algName, KFC_CONCURRENT_ALL_GATHER_ALG_NAME) == 0 ? KFC_MAX_MISSION_NUM :
                                                                                                 1U;
}

enum class KfcServerRole : uint32_t {
    DEFAULT = 0,
    ALL_GATHER_MESH = 1,
    ALL_GATHER_NHR = 2,
};

inline KfcServerRole GetKfcServerRole(const char* algName, uint32_t missionIndex)
{
    if (GetKfcServerMissionNum(algName) == KFC_MAX_MISSION_NUM) {
        if (missionIndex == 0U) {
            return KfcServerRole::ALL_GATHER_MESH;
        }
        if (missionIndex == 1U) {
            return KfcServerRole::ALL_GATHER_NHR;
        }
    }
    return KfcServerRole::DEFAULT;
}

} // namespace mc2_ops_hccl

#endif
