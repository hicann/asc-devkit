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
    OpParam opParam;
    std::vector<std::vector<uint32_t>> subCommRanks;
    std::vector<KfcNhrStepInfo> nhrStepInfoVector;
    std::map<uint32_t, uint32_t> nhrRank2ChannelIdx;
};

// 多mission由Host使用独立start/done区域调度；Server之间不在CCU侧同步。
struct KfcServerContext : CcuKernelCtxBase {
    const CcuKernelArgKfcServer* arg;

    ccu::Variable xnAddr; // HBM上的算子执行参数首地址
    ccu::Variable ckeAddr;
    ccu::Variable waitAddr;   // HBM上的每轮开始信号的首地址
    ccu::Variable recordAddr; // HBM上的每轮完成信号的首地址

    ccu::Variable token;
    ccu::Variable dieNum; // Die数量，用于判断单双Die，默认为1即单Die

    ccu::Variable missionNum;
    ccu::Variable missionIndex;

    // 用于Die间同步信号[missionId=0]
    const char* exportDieSig;
    const char* importDieSig;

    // TODO：当前missionNum_=1，missionIndex_=0---
    // 用于Mission间同步信号
    std::vector<const char*> exportMissoinSig;
    std::vector<const char*> importMissionSig;
    // 用于Mission间同步变量
    std::vector<ccu::Variable> exportMissionVar;
    std::vector<ccu::Variable> importMissionVar;
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

} // namespace mc2_ops_hccl

#endif
