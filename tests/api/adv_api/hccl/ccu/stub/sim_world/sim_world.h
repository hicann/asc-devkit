/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_WORLD_H
#define SIM_WORLD_H
#include <map>
#include <memory>
#include "sim_common.h"
#include "sim_npu.h"
#include "comm_domain.h"
#include "sim_task.h"
#include "data_slice.h"
#include "hccl_sim_pub.h"
#include "checker_def.h"

namespace HcclSim {
class SimWorld {
public:
    static SimWorld* Global();
    void Deinit();
    void Init(const TopoMeta& topoMeta, DevType devType);
    SimNpu& GetSimNpuByRankId(RankId rankId);
    NpuPos GetNpuPosByRankId(RankId rankId);
    TopoMeta GetTopoMetaInfo();

private:
    void InitSimNpuRes(const TopoMeta& topoMeta, DevType devType);
    SimNpu CreateSimNpu(const NpuPos& npuPos, DevType devType);

private:
    TopoMeta topoMeta_;
    CommDomain commDomain_;
    std::map<PodId, std::map<SerId, std::map<PhyId, SimNpu>>> simNpus_;
};
} // namespace HcclSim
#endif
