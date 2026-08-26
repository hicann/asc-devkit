/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_REDUCESCATTTER_AUTO_SELECTOR
#define HCCLV2_REDUCESCATTTER_AUTO_SELECTOR

#include "auto_selector_base.h"

namespace mc2_ops_hccl {

class ReduceScatterAutoSelector : public AutoSelectorBase {
private:
    SelectorStatus SelectCcuMsAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap,
        std::string& selectAlgName) const override;
    SelectorStatus SelectAicpuAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap,
        std::string& selectAlgName) const override;
    SelectorStatus SelectAivAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap,
        std::string& selectAlgName) const override;
    SelectorStatus SelectMeshAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const;
    SelectorStatus SelectCcuScheduleAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap,
        std::string& selectAlgName) const override;
    SelectorStatus SelectMeshAlgoAicpuMeshClos(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const;
    SelectorStatus SelectMeshAlgoAicpu(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const;
    SelectorStatus SelectMeshAlgoCcums(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const;
    SelectorStatus SelectMeshAlgoCcuSchedule(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const;
    SelectorStatus SelectMeshAlgoAicpuForMesh1D(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName, u64 dataSize,
        double ratio) const;
    SelectorStatus SelectMeshAlgoAicpuForMesh1DClos(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, u64 dataSize, double ratio,
        bool isClosNumMultipleOfMeshNum, std::string& selectAlgName) const;
    SelectorStatus SelectDPUAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap,
        std::string& selectAlgName) const override;
};

} // namespace mc2_ops_hccl
#endif
