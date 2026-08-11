/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_RES_DL_H
#define CCU_RES_DL_H

#include "dlsym_common.h"
#include "ccu_types.h"
#include "ccu/ccu_res_defs_dl.h"

#ifdef __cplusplus
extern "C" {
#endif

DECL_WEAK_FUNC(CcuResult, HcommCcuGetMemToken, uint64_t srcVa, uint64_t size, uint64_t* tokenInfo);
DECL_WEAK_FUNC(CcuResult, HcommCcuInsResDescCreate, uint32_t dieId, HcommCcuResDescHandle* resDesc);
DECL_WEAK_FUNC(CcuResult, HcommCcuInsResDescDestroy, HcommCcuResDescHandle resDesc);
DECL_WEAK_FUNC(
    CcuResult, HcommCcuInsResDescSetNum, HcommCcuResDescHandle resDesc, HcommCcuResType resType, uint32_t resNum);
DECL_WEAK_FUNC(
    CcuResult, HcommCcuInsResDescQueryNum, HcommCcuResDescHandle resDesc, HcommCcuResType resType, uint32_t* resNum);
DECL_WEAK_FUNC(
    CcuResult, HcommCcuInsCreate, const HcommCcuResDescHandle* resDescs, uint32_t resDescNum,
    CcuInsHandle* ccuInsHandle);
DECL_WEAK_FUNC(CcuResult, HcommCcuInsDestroy, CcuInsHandle ccuInsHandle);
DECL_WEAK_FUNC(CcuResult, HcommCcuInsQueryResDesc, CcuInsHandle ccuInsHandle, HcommCcuResDescHandle resDesc);
DECL_WEAK_FUNC(CcuResult, HcommCcuQueryRemainResDesc, HcommCcuResDescHandle resDesc);
DECL_WEAK_FUNC(
    CcuResult, HcommCcuKernelQueryResReq, const void* kernelFunc, const void** kernelArgs, uint32_t argNum,
    HcommCcuResDescHandle resDesc);

DECL_SUPPORT_FLAG(HcommCcuInsResDescCreate);
DECL_SUPPORT_FLAG(HcommCcuInsResDescDestroy);
DECL_SUPPORT_FLAG(HcommCcuInsResDescSetNum);
DECL_SUPPORT_FLAG(HcommCcuInsResDescQueryNum);
DECL_SUPPORT_FLAG(HcommCcuInsCreate);
DECL_SUPPORT_FLAG(HcommCcuInsDestroy);
DECL_SUPPORT_FLAG(HcommCcuInsQueryResDesc);
DECL_SUPPORT_FLAG(HcommCcuQueryRemainResDesc);
DECL_SUPPORT_FLAG(HcommCcuKernelQueryResReq);

void CcuResDlInit(void* libHcommHandle);

#ifdef __cplusplus
}
#endif

#endif // CCU_RES_DL_H
