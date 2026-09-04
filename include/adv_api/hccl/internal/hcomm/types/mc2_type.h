/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_MC2_TYPE_H
#define HCCL_MC2_TYPE_H

#include <string>

namespace Hccl { // 仅保留了a5 ccu kfc_server算子需要编译态常量

constexpr uint32_t MC2_RES_CTX_MAX = 3;
constexpr uint8_t MC2_DEBUG_ONLY_AICPU = 4;
constexpr uint32_t CCU_PARAM_NUM_PER_DIE = 32;
constexpr uint32_t CCU_PARAM_NUM_MAX = CCU_PARAM_NUM_PER_DIE * 2;
constexpr uint32_t CCU_ONE_PARAM_SIZE = 8;
constexpr uint32_t CCU_TASK_NUM_MAX = 64;
constexpr uint32_t MAX_RANK_NUM = 64;  // 最大卡数
constexpr uint32_t MC2_MAX_OP_NUM = 8; // MC2最大通信算子数

constexpr uint32_t UNKNOWN_TILING_V1 = 3;                 // 旧版本 MC2 Tiling version = 3
constexpr uint32_t UNKNOWN_TILING_V2 = 100;               // 新版本 MC2 Tiling version = 100
constexpr uint64_t MC2_WORKSPACE_SIZE = 16 * 1024 * 1024; // aic与ccu的交互空间workspace大小为16*1024*1024B

} // namespace Hccl

#endif // HCCL_MC2_TYPE_H
