/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_HALF_ALLTOALLV_AUTO_SELECTOR_H
#define HCCLV2_HALF_ALLTOALLV_AUTO_SELECTOR_H

#include "auto_selector_base.h"

namespace mc2_ops_hccl {

// AllToAllvWrite（HCCL_CMD_HALF_ALLTOALLV）默认兜底选择器：level0 为 1D mesh
// （MESH_1D / MESH_1D_CLOS）即选 CcuHalfAll2AllVMesh1D。该算子只有 CCU 一条
// 执行路径，AICPU 分支保持基类默认 NOT_MATCH。
class HalfAlltoAllVAutoSelector : public AutoSelectorBase {
private:
    SelectorStatus SelectCcuScheduleAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap,
        std::string& selectAlgName) const override;
};

} // namespace mc2_ops_hccl
#endif
