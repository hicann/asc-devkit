/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_MC2_H
#define HCCL_MC2_H

#include <hccl/hccl_types.h>
#include <hccl/hccl_res.h>
#include <ccu/ccu_types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief  Alloc HcclOpArgs memory
 * @param opArgs A pointer to the allocated HcclOpArgs memory.
 */
extern HcclResult HcclKfcAllocOpArgs(void** opArgs);

/**
 * @brief  Free HcclOpArgs memory
 * @param opArgs A pointer to the HcclOpArgs memory.
 */
extern HcclResult HcclKfcFreeOpArgs(void* opArgs);

/**
 * @brief  Set the source data type param of HcclOpArgs
 * @param opArgs A pointer to the HcclOpArgs.
 * @param srcDataType The source data type to set.
 */
extern HcclResult HcclKfcOpArgsSetSrcDataType(void* opArgs, uint8_t srcDataType);

/**
 * @brief  Set the destination data type param of HcclOpArgs
 * @param opArgs A pointer to the HcclOpArgs.
 * @param dstDataType The destination data type to set.
 */
extern HcclResult HcclKfcOpArgsSetDstDataType(void* opArgs, uint8_t dstDataType);

/**
 * @brief  Set the reduce type param of HcclOpArgs
 * @param opArgs A pointer to the HcclOpArgs.
 * @param reduceType The reduce type to set.
 */
extern HcclResult HcclKfcOpArgsSetReduceType(void* opArgs, uint32_t reduceType);

/**
 * @brief  Set the data count param of HcclOpArgs
 * @param opArgs A pointer to the HcclOpArgs.
 * @param count The data count to set.
 */
extern HcclResult HcclKfcOpArgsSetCount(void* opArgs, uint64_t count);

/**
 * @brief  Set the algConfig param of HcclOpArgs
 * @param opArgs A pointer to the HcclOpArgs.
 * @param algConfig The algConfig to set.
 */
extern HcclResult HcclKfcOpArgsSetAlgConfig(void* opArgs, char* algConfig);

/**
 * @brief  Set the comm engine param of HcclOpArgs
 * @param opArgs A pointer to the HcclOpArgs.
 * @param commEngine The comm engine type to set.
 */
extern HcclResult HcclKfcOpArgsSetCommEngine(void* opArgs, uint8_t commEngine);

/**
 * @brief  Create the OpResCtx for communication
 * @param comm A pointer identifying the communication resource based on.
 * @param opType The opType param.
 * @param opArgs A pointer to the HcclOpArgs.
 * @param opResCtx A pointer to the created OpResCtx.
 */
extern HcclResult HcclCreateOpResCtx(HcclComm comm, uint8_t opType, void* opArgs, void** opResCtx);

/**
 * @brief Allocate communication resource by MC2 Tiling data
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @param mc2Tiling MC2 tiling data structure containing algorithm configuration.
 * @param opResCtx Output pointer to store the created communication context.
 * @return HcclResult - HCCL_SUCCESS on success, error code otherwise.
 *
 * @note This interface is used for MC2 compilation scenario to pre-allocate
 *       communication resources based on tiling information.
 */
extern HcclResult __attribute__((visibility("default"))) HcclAllocComResourceByTiling(
    HcclComm comm, void* stream, void* mc2Tiling, void** opResCtx);

extern CcuResult CcuKernelLaunch(HcclComm comm, void* opResCtx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HCCL_MC2_H
