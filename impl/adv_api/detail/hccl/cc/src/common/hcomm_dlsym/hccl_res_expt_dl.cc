/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "hccl_res_expt_dl.h"
#include <dlfcn.h>

DEFINE_WEAK_FUNC(HcclResult, HcclCommAddExchangeInfo, HcclComm comm, const void* data, uint32_t length);
DEFINE_WEAK_FUNC(
    HcclResult, HcclCommGetExchangeInfo, HcclComm comm, uint32_t remoteRank, uint32_t length, void* data,
    uint32_t* actualLength);
DEFINE_WEAK_FUNC(HcclResult, HcclCommResetExchangeInfo, HcclComm comm);

void HcclResExptDlInit(void* libHcommHandle)
{
    INIT_SUPPORT_FLAG(libHcommHandle, HcclCommAddExchangeInfo);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclCommGetExchangeInfo);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclCommResetExchangeInfo);
}
