/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "hccl_ccu_res_dl.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

DEFINE_WEAK_FUNC(HcclResult, HcclCommQueryCcuIns, HcclComm comm, CcuInsHandle* insHandles, uint32_t* insNum);
DEFINE_WEAK_FUNC(HcclResult, HcclCommQueryAssignedCcuIns, HcclComm comm, CcuInsHandle* insHandles, uint32_t* insNum);
DEFINE_WEAK_FUNC(HcclResult, HcclCommAssignCcuIns, HcclComm comm, CcuInsHandle insHandle);

// 初始化
void HcclCcuResDlInit(void* libHcommHandle)
{
    INIT_SUPPORT_FLAG(libHcommHandle, HcclCommQueryCcuIns);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclCommQueryAssignedCcuIns);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclCommAssignCcuIns);
}
