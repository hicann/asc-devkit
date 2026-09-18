/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCOMM_DLSYM_H
#define HCOMM_DLSYM_H

#include "hccl_types.h"
#include "dtype_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// 动态库管理接口（大驼峰命名）
void HcommDlInit(void);
int GetHcommVersion(void);

// 功能支持情况查询
bool HcommIsProfilingSupported();
bool HcommIsExportThreadSupported();

#ifdef __cplusplus
}
#endif

#endif // HCOMM_DLSYM_H
