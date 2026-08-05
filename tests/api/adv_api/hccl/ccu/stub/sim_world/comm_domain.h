/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef COMM_DOMAIN_H
#define COMM_DOMAIN_H
#include <map>
#include "sim_common.h"

namespace HcclSim {
class CommDomain {
public:
    void Init(const TopoMeta& topoMeta);
    void Clear();
    uint32_t GetCommRankCount();
    NpuPos GetNpuPosByRankId(RankId rankId);

private:
    std::map<RankId, NpuPos> rankId2Pos_;
};
} // namespace HcclSim
#endif
