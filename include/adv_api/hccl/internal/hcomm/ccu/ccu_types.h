/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_TYPES_H
#define CCU_TYPES_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef __cplusplus
typedef void* CcuKernelArg;
typedef CcuResult (*CcuKernelFunc)(CcuKernelArg arg);
#endif

/**
 * @brief CCU return value definition
 */
typedef enum {
    CCU_SUCCESS = 0,       /**< success */
    CCU_E_PARA = 1,        /**< parameter error */
    CCU_E_PTR = 2,         /**< empty pointer */
    CCU_E_INTERNAL = 4,    /**< internal error */
    CCU_E_NOT_SUPPORT = 5, /**< not support feature */
    CCU_E_NOT_FOUND = 6,   /**< not found specific resource */
    CCU_E_UNAVAIL = 7,     /**< resource unavailable */

    CCU_E_DRV_START = 4096,

    CCU_E_DRV_INIT_FAILED = 4097,
    CCU_E_DRV_BUSY = 4098,

    CCU_E_DRV_END = 4224,

    CCU_E_RESERVED = 9216
} CcuResult;

/**
 * @brief CCU condition type for conditional jump
 */
typedef enum {
    CCU_CONDITION_EQ = 0,
    CCU_CONDITION_NE = 1,
} CcuConditionType;

typedef uint64_t CcuLoop;
typedef uint64_t CcuLoopGroup;
typedef uint64_t CcuLoopExecutors;

typedef struct {
    uint64_t addrOffset;
    uint64_t iterNum;
} CcuLoopConfig;

typedef struct {
    uint32_t cloneNum;
    uint32_t cloneLoopOffset;
    uint32_t addrOffset;
    uint32_t ccuBufferOffset;
    uint32_t eventOffset;

} CcuLoopGroupConfig;

typedef uint64_t CcuInsHandle;

typedef uint64_t CcuKernelHandle;

typedef uint64_t CcuVariableHandle;

typedef uint64_t CcuAddressHandle;

typedef uint64_t CcuEventHandle;

typedef uint64_t CcuBufferHandle;

typedef uint64_t CcuLocalAddrHandle;

typedef uint64_t CcuRemoteAddrHandle;

typedef void* CcuKernelArg;

typedef CcuResult (*CcuKernelFunc)(CcuKernelArg arg);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CCU_TYPES_H
