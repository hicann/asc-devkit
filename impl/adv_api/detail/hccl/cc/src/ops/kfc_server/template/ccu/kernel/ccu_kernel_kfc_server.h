/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_KERNEL_KFC_SERVER_H
#define HCCL_CCU_KERNEL_KFC_SERVER_H

#include <vector>
#include <map>
#include <ios>
#include "utils.h"
#include "ccu_kernel_utils.h"
#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {
constexpr uint64_t INVALID_U64 = UINT64_MAX;
constexpr uint32_t CCU_PARAM_NUM_PER_DIE = 32;
constexpr uint32_t CCU_PARAM_NUM_MAX = CCU_PARAM_NUM_PER_DIE * 2;
constexpr uint32_t CCU_ONE_PARAM_SIZE = 8;
constexpr uint32_t CCU_TASK_NUM_MAX = 64;

// new---20260603
struct CcuKernelArgKfcServer : CcuKernelArgBase {
    uint64_t rankSize = 0;
    uint32_t rankId = 0;
    bool loadFromMem = false;
    KfcServerRole role = KfcServerRole::DEFAULT;
    uint32_t jettyNum = 4;
    // SoleNHR 2Die：die 维编号/总数（双 die 时每 mission 一个 die 实例，dispatch 转发给 kernel）
    uint32_t axisId = 0;
    uint32_t axisSize = 1;
    OpParam opParam;
    std::vector<std::vector<uint32_t>> subCommRanks;
    std::vector<KfcNhrStepInfo> nhrStepInfoVector;
    std::map<uint32_t, uint32_t> nhrRank2ChannelIdx;
    // 仅HALF_ALLTOALLV使用，见CcuKernelInfo::localWinAddr
    uint64_t localWinAddr = 0;
    // 构图期宿主常量（与运行时 kfcServerArgs[4] 同值）。MissionPreSync 按 mission 分支选择
    // tag 时必须用它：CCU_IF(Variable) 是运行时分支，构图期 IF/ELSE 两分支都会执行，
    // 会导致双 mission 两个 kernel 各自 export 同一对 tag，注册期 ProcessInterCtxRes
    // 合并 exported sharedNotifies 时同名冲突（HCCL_E_PARA）。
    uint32_t hostMissionIndex = 0;
    uint64_t opId = 0U;
    // Multi-tiling subgraphs have distinct opTypes and one mission each on the same die.
    std::vector<std::shared_ptr<CcuKernelArgKfcServer>> tilingArgs;
};

class CcuTaskArgKfcServer : public hcomm::CcuTaskArg {
public:
    explicit CcuTaskArgKfcServer(
        uint64_t xnAddr, uint64_t ckeAddr, uint64_t dieNum, uint64_t missionNum, uint64_t missionIndex, uint64_t token)
        : xnAddr_(xnAddr),
          ckeAddr_(ckeAddr),
          dieNum_(dieNum),
          missionNum_(missionNum),
          missionIndex_(missionIndex),
          token_(token)
    {
        HCCL_DEBUG(
            "[CcuTaskArgKfcServer] xnAddr: %lu, ckeAddr: %lu, dieNum: %lu, "
            "missionNum: %lu, missionIndex: %lu, token: %lu",
            xnAddr_, ckeAddr_, dieNum_, missionNum_, missionIndex_, token_);
    }

    uint64_t xnAddr_;       // HBM上的算子执行参数首地址
    uint64_t ckeAddr_;      // CKE地址
    uint64_t dieNum_;       // Die数量
    uint64_t missionNum_;   // Mission数量
    uint64_t missionIndex_; // Mission索引
    uint64_t token_;        // Token信息
};

CcuResult CcuKfcServerKernel(CcuKernelArg arg);

CcuResult CcuKfcServerKernelA6(CcuKernelArg arg);

} // namespace mc2_ops_hccl

#endif
