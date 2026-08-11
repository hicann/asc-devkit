/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_res_dl.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

DEFINE_WEAK_FUNC(CcuResult, HcommCcuGetMemToken, uint64_t srcVa, uint64_t size, uint64_t* tokenInfo);
DEFINE_WEAK_FUNC(CcuResult, HcommCcuInsResDescCreate, uint32_t dieId, HcommCcuResDescHandle* resDesc);
DEFINE_WEAK_FUNC(CcuResult, HcommCcuInsResDescDestroy, HcommCcuResDescHandle resDesc);
DEFINE_WEAK_FUNC(
    CcuResult, HcommCcuInsResDescSetNum, HcommCcuResDescHandle resDesc, HcommCcuResType resType, uint32_t resNum);
DEFINE_WEAK_FUNC(
    CcuResult, HcommCcuInsResDescQueryNum, HcommCcuResDescHandle resDesc, HcommCcuResType resType, uint32_t* resNum);
DEFINE_WEAK_FUNC(
    CcuResult, HcommCcuInsCreate, const HcommCcuResDescHandle* resDescs, uint32_t resDescNum,
    CcuInsHandle* ccuInsHandle);
DEFINE_WEAK_FUNC(CcuResult, HcommCcuInsDestroy, CcuInsHandle ccuInsHandle);
DEFINE_WEAK_FUNC(CcuResult, HcommCcuInsQueryResDesc, CcuInsHandle ccuInsHandle, HcommCcuResDescHandle resDesc);
DEFINE_WEAK_FUNC(CcuResult, HcommCcuQueryRemainResDesc, HcommCcuResDescHandle resDesc);
DEFINE_WEAK_FUNC(
    CcuResult, HcommCcuKernelQueryResReq, const void* kernelFunc, const void** kernelArgs, uint32_t argNum,
    HcommCcuResDescHandle resDesc);

// 初始化
void CcuResDlInit(void* libHcommHandle)
{
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuGetMemToken);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuInsResDescCreate);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuInsResDescDestroy);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuInsResDescSetNum);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuInsResDescQueryNum);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuInsCreate);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuInsDestroy);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuInsQueryResDesc);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuQueryRemainResDesc);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommCcuKernelQueryResReq);
}
