/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_RES_DEFS_DL_H
#define CCU_RES_DEFS_DL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t HcommCcuResDescHandle;

typedef enum {
    HCOMM_CCU_RES_TYPE_INVALID = -1,
    HCOMM_CCU_RES_TYPE_LOOP = 0,
    HCOMM_CCU_RES_TYPE_CCU_BUF = 1,
    HCOMM_CCU_RES_TYPE_VARIABLE = 2,
    HCOMM_CCU_RES_TYPE_ADDRESS = 3,
    HCOMM_CCU_RES_TYPE_EVENT = 4,
    HCOMM_CCU_RES_TYPE_CCU_THREAD = 5,
    HCOMM_CCU_RES_TYPE_INSTRUCTION = 6
} HcommCcuResType;

#ifdef __cplusplus
}
#endif

#endif // CCU_RES_DEFS_DL_H
