/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "comm_domain.h"
#include "log.h"
#include "exception_util.h"
#include "invalid_params_exception.h"

namespace HcclSim {
uint32_t CommDomain::GetCommRankCount() { return rankId2Pos_.size(); }

NpuPos CommDomain::GetNpuPosByRankId(RankId rankId)
{
    if (rankId2Pos_.count(rankId) == 0) {
        Hccl::THROW<Hccl::InvalidParamsException>(
            "[CommDomain::GetNpuPosByRankId] rankId[%u] is not in current comm domain", rankId);
    }
    return rankId2Pos_[rankId];
}

void CommDomain::Init(const TopoMeta& topoMeta)
{
    rankId2Pos_.clear();
    RankId rankId = 0;
    PodId podId = 0;
    for (const auto& superPod : topoMeta) {
        SerId serId = 0;
        for (const auto& server : superPod) {
            for (const auto& phyId : server) {
                HCCL_INFO("[CommDomain::Init] rankId[%u] -> npuPos[%u, %u, %u]", rankId, podId, serId, phyId);
                rankId2Pos_[rankId] = NpuPos{podId, serId, phyId};
                rankId++;
            }
            serId++;
        }
        podId++;
    }
}

void CommDomain::Clear() { rankId2Pos_.clear(); }
} // namespace HcclSim
