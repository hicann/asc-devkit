/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "calc_resource_graph_mode.h"
#include <cstddef>
#include <cstring>

HcclResult HcclCreateOpParamGraphMode(OpParamGraphMode** opParam)
{
    if (opParam == nullptr) {
        return HCCL_E_PARA;
    }
    // 将void**转换为OpParamGraphMode**
    OpParamGraphMode** paramPtr = reinterpret_cast<OpParamGraphMode**>(opParam);
    *paramPtr = new OpParamGraphMode();
    if (*paramPtr == nullptr) {
        return HCCL_E_MEMORY;
    }
    return HCCL_SUCCESS;
}

HcclResult HcclDestroyOpParamGraphMode(OpParamGraphMode* opParam)
{
    if (opParam == nullptr) {
        return HCCL_E_PARA;
    }
    // 将void*转换为OpParamGraphMode*
    OpParamGraphMode* paramPtr = reinterpret_cast<OpParamGraphMode*>(opParam);
    delete paramPtr;
    return HCCL_SUCCESS;
}

HcclResult HcclSetOpParamGraphModeOpType(OpParamGraphMode* opParam, const char* opType)
{
    if (opParam == nullptr || opType == nullptr) {
        return HCCL_E_PARA;
    }
    // 将void*转换为OpParamGraphMode*
    OpParamGraphMode* paramPtr = reinterpret_cast<OpParamGraphMode*>(opParam);
    CHK_SAFETY_FUNC_RET(strncpy_s(paramPtr->opType, sizeof(paramPtr->opType), opType, sizeof(paramPtr->opType) - 1));
    return HCCL_SUCCESS;
}

HcclResult HcclCalcOpResOnlineGraphMode(
    OpParamGraphMode* opParam, u64* opMemSize, u32* streamNum, u32* taskNum, u32* aivCoreNum)
{
    HCCL_INFO("Enter HcclCalcOpResOnlineGraphMode.");
    CHK_RET(CheckCalcResInputGraphMode(opParam, opMemSize, streamNum, taskNum, aivCoreNum));
    // 将void**转换为OpParamGraphMode**
    OpParamGraphMode* paramPtr = reinterpret_cast<OpParamGraphMode*>(opParam);
    if (paramPtr == nullptr) {
        return HCCL_E_PARA;
    }
    // 为了兼容，创建临时的 ResResponseGraphMode 结构
    ResResponseGraphMode resResponse = {0, 0, 0, 0};
    HCCL_INFO("Start to calc op resource online.");
    // aicpu引擎计算资源
    mc2_ops_hccl::HcclCalcAicpuResOffline(&resResponse);

    // 其他引擎补充在下面

    // 将结果复制到输出参数
    *opMemSize = resResponse.opMemSize;
    *streamNum = resResponse.streamNum;
    *taskNum = resResponse.taskNum;
    *aivCoreNum = resResponse.aivCoreNum;

    return HCCL_SUCCESS;
}

HcclResult HcclCalcOpResOfflineGraphMode(
    OpParamGraphMode* opParam, u64* opMemSize, u32* streamNum, u32* taskNum, u32* aivCoreNum)
{
    HCCL_INFO("Enter HcclCalcOpResOfflineGraphMode.");
    CHK_RET(CheckCalcResInputGraphMode(opParam, opMemSize, streamNum, taskNum, aivCoreNum));
    // 将void**转换为OpParamGraphMode**
    OpParamGraphMode* paramPtr = reinterpret_cast<OpParamGraphMode*>(opParam);
    if (paramPtr == nullptr) {
        return HCCL_E_PARA;
    }
    // 为了兼容，创建临时的 ResResponseGraphMode 结构
    ResResponseGraphMode resResponse = {0, 0, 0, 0};
    HCCL_INFO("Start to calc op resource offline.");
    // aicpu引擎计算资源
    mc2_ops_hccl::HcclCalcAicpuResOffline(&resResponse);

    // 其他引擎补充在下面

    // 将结果复制到输出参数
    *opMemSize = resResponse.opMemSize;
    *streamNum = resResponse.streamNum;
    *taskNum = resResponse.taskNum;
    *aivCoreNum = resResponse.aivCoreNum;

    return HCCL_SUCCESS;
}

namespace mc2_ops_hccl {
HcclResult HcclCalcAicpuResOffline(ResResponseGraphMode* resResponse)
{
    if (resResponse == nullptr) {
        return HCCL_E_PARA;
    }
    u64 aicpuOpMemSize = 0;
    u32 aicpuStreamNum = 0;
    u32 aicpuTaskNum = 3;

    resResponse->opMemSize = std::max(resResponse->opMemSize, aicpuOpMemSize);
    resResponse->streamNum = std::max(resResponse->streamNum, aicpuStreamNum);
    resResponse->taskNum = std::max(resResponse->taskNum, aicpuTaskNum);
    return HCCL_SUCCESS;
}

HcclResult CheckCalcResInputGraphMode(
    const OpParamGraphMode* opParam, const u64* opMemSize, const u32* streamNum, const u32* taskNum,
    const u32* aivCoreNum)
{
    CHK_PTR_NULL(opParam);
    CHK_PTR_NULL(opMemSize);
    CHK_PTR_NULL(streamNum);
    CHK_PTR_NULL(taskNum);
    CHK_PTR_NULL(aivCoreNum);
    return HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
