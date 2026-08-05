/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
  * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
  * CANN Open Software License Agreement Version 2.0 (the "License").
  * Please refer to the License for details. You may not use this file except in compliance with the License.
  * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
  * See LICENSE in the root of the software repository for the full text of the License.
  */

#ifndef CCU_LAUNCH_DL_H
#define CCU_LAUNCH_DL_H

#include "dlsym_common.h"
#include "ccu_types.h"
#include "hccl_res.h"

#ifdef __cplusplus
extern "C" {
#endif

DECL_WEAK_FUNC(CcuResult, HcommCcuKernelRegisterStart, CcuInsHandle insHandle);
DECL_WEAK_FUNC(
    CcuResult, HcommCcuKernelRegister, CcuInsHandle insHandle, uint32_t dieId, const char* kernelFuncName,
    const void* kernelFunc, const void** kernelArgs, uint32_t argNum, CcuKernelHandle* kernelHandle);
DECL_WEAK_FUNC(CcuResult, HcommCcuKernelRegisterEnd, CcuInsHandle insHandle);
DECL_WEAK_FUNC(
    CcuResult, HcommCcuKernelLaunch, ThreadHandle threadHandle, CcuKernelHandle kernelHandle, const void* taskArgs,
    uint32_t argSize);

void CcuLaunchDlInit(void* libHcommHandle);

#ifdef __cplusplus
}
#endif

#endif // CCU_LAUNCH_DL_H
