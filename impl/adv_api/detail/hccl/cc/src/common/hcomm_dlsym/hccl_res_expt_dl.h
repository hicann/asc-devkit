/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_RES_EXPT_DL_H
#define HCCL_RES_EXPT_DL_H

#include "dlsym_common.h"
#if CANN_VERSION_NUM >= 90000000
#include "hccl_res_expt.h"
#endif
#include "hccl_res.h"

#ifdef __cplusplus
extern "C" {
#endif

DECL_WEAK_FUNC(HcclResult, HcclCommAddExchangeInfo, HcclComm comm, const void* data, uint32_t length);
DECL_WEAK_FUNC(
    HcclResult, HcclCommGetExchangeInfo, HcclComm comm, uint32_t remoteRank, uint32_t length, void* data,
    uint32_t* actualLength);
DECL_WEAK_FUNC(HcclResult, HcclCommResetExchangeInfo, HcclComm comm);
DECL_SUPPORT_FLAG(HcclCommAddExchangeInfo);
DECL_SUPPORT_FLAG(HcclCommGetExchangeInfo);
DECL_SUPPORT_FLAG(HcclCommResetExchangeInfo);

void HcclResExptDlInit(void* libHcommHandle);

#ifdef __cplusplus
}
#endif

#endif // HCCL_RES_EXPT_DL_H
