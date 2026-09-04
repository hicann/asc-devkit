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
 * @brief  Alloc Mc2OpArgs memory
 * @param opArgs A pointer to the allocated Mc2OpArgs memory.
 */
extern HcclResult Mc2KfcAllocOpArgs(void** opArgs);

/**
 * @brief  Free Mc2OpArgs memory
 * @param opArgs A pointer to the Mc2OpArgs memory.
 */
extern HcclResult Mc2KfcFreeOpArgs(void* opArgs);

/**
 * @brief  Set the source data type param of Mc2OpArgs
 * @param opArgs A pointer to the Mc2OpArgs.
 * @param srcDataType The source data type to set.
 */
extern HcclResult Mc2KfcOpArgsSetSrcDataType(void* opArgs, uint8_t srcDataType);

/**
 * @brief  Set the destination data type param of Mc2OpArgs
 * @param opArgs A pointer to the Mc2OpArgs.
 * @param dstDataType The destination data type to set.
 */
extern HcclResult Mc2KfcOpArgsSetDstDataType(void* opArgs, uint8_t dstDataType);

/**
 * @brief  Set the reduce type param of Mc2OpArgs
 * @param opArgs A pointer to the Mc2OpArgs.
 * @param reduceType The reduce type to set.
 */
extern HcclResult Mc2KfcOpArgsSetReduceType(void* opArgs, uint32_t reduceType);

/**
 * @brief  Set the data count param of Mc2OpArgs
 * @param opArgs A pointer to the Mc2OpArgs.
 * @param count The data count to set.
 */
extern HcclResult Mc2KfcOpArgsSetCount(void* opArgs, uint64_t count);

/**
 * @brief  Set the algConfig param of Mc2OpArgs
 * @param opArgs A pointer to the Mc2OpArgs.
 * @param algConfig The algConfig to set.
 */
extern HcclResult Mc2KfcOpArgsSetAlgConfig(void* opArgs, char* algConfig);

/**
 * @brief  Set the comm engine param of Mc2OpArgs
 * @param opArgs A pointer to the Mc2OpArgs.
 * @param commEngine The comm engine type to set.
 */
extern HcclResult Mc2KfcOpArgsSetCommEngine(void* opArgs, uint8_t commEngine);

/**
 * @brief  Create the OpResCtx for communication
 * @param comm A pointer identifying the communication resource based on.
 * @param opType The opType param.
 * @param opArgs A pointer to the Mc2OpArgs.
 * @param opResCtx A pointer to the created OpResCtx.
 */
extern HcclResult Mc2CreateOpResCtx(HcclComm comm, uint8_t opType, void* opArgs, void** opResCtx);

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

/**
 * @brief Check whether communication resource can be allocated for the given MC2 tiling
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @param mc2Tiling MC2 tiling data structure containing algorithm configuration.
 * @return HcclResult - HCCL_SUCCESS on success, error code otherwise.
 *
 * @note CheckOnly pre-check: reuses the same argument parsing / algorithm selection / resource
 *       calculation / resource acquisition logic as the real allocation path to probe whether
 *       resources are sufficient; HCCL_E_UNAVAIL is translated to HCCL_E_RES_NOT_SUFFICIENT,
 *       and the final OpResCtx is not persisted to device.
 */
extern HcclResult __attribute__((visibility("default"))) CheckOpResSufficient(
    HcclComm comm, void* stream, void* mc2Tiling);

extern CcuResult CcuKernelLaunch(HcclComm comm, void* opResCtx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HCCL_MC2_H
