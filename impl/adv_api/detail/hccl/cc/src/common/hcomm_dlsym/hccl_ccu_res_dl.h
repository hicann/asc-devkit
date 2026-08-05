/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
  * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
  * CANN Open Software License Agreement Version 2.0 (the "License").
  * Please refer to the License for details. You may not use this file except in compliance with the License.
  * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
  * See LICENSE in the root of the software repository for the full text of the License.
  */

#ifndef HCCL_CCU_RES_DL_H
#define HCCL_CCU_RES_DL_H

#include "dlsym_common.h"
#include "hccl_types.h"
#include "ccu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

DECL_WEAK_FUNC(HcclResult, HcclCommQueryCcuIns, HcclComm comm, CcuInsHandle* insHandles, uint32_t* insNum);

void HcclCcuResDlInit(void* libHcommHandle);

#ifdef __cplusplus
}
#endif

#endif // HCCL_CCU_RES_DL_H
