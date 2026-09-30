/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file ascendc_runtime.h
 * \brief
 */
#ifndef __ASCENDC_RUNTIME_H__
#define __ASCENDC_RUNTIME_H__

#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASCENDC_SUCCESS 0
#define ASCENDC_ERR_RUNTIME_ERROR 1

#define ASCENDC_ASSERT_RETVAL(exp, ret)       \
    do {                                      \
        if (!(exp)) {                         \
            printf("Assert %s failed", #exp); \
            return (ret);                     \
        }                                     \
    } while (0)

#define CHECK_COND(cond, ret, fmt, ...) \
    do {                                \
        if (!(cond)) {                  \
            printf(fmt, ##__VA_ARGS__); \
            return ret;                 \
        }                               \
    } while (0)

#define ASCENDC_ASSERT_RTOK_RETVAL(v) ASCENDC_ASSERT_RETVAL(((v) == 0), (ASCENDC_ERR_RUNTIME_ERROR))
#define ASCENDC_ASSERT_NOTNULL_RETVAL(v) ASCENDC_ASSERT_RETVAL(((v) != nullptr), (ASCENDC_ERR_RUNTIME_ERROR))

#ifdef __cplusplus
}
#endif

using rtStream_t = void*;
using rtFuncHandle = void*;
extern "C" void ReportAscendProf(const char* name, uint32_t numBlocks, uint32_t taskType, const uint64_t startTime);
extern "C" uint32_t AllocAscendMemDevice(void** devMem, uint64_t size);
extern "C" uint32_t FreeAscendMemDevice(void* devMem);
extern "C" uint32_t RegisterAscendBinary(const char* fileBuf, size_t fileSize, uint32_t type, void** handle);
extern "C" bool GetAscendProfStatus();
extern "C" uint32_t GetAscendCoreSyncAddr(void** addr);
extern "C" int UnregisterAscendBinary(void* hdl);
extern "C" void AscendProfRegister();
extern "C" void AscendProfUnRegister();
extern "C" bool AscendCheckSoCVersion(const char* socVersion, char* errMsg);
extern "C" uint32_t GetCoreNumForMixVectorCore(uint32_t* aiCoreNum, uint32_t* vectorCoreNum);
extern "C" int32_t AscendDevBinaryLazyRegister(const char* binBuf, size_t binSize, void** handle);
extern "C" int32_t AscendGetFuncFromBinary(void* const binHandle, const char* kernelName, void** funcHandle);
extern "C" int32_t AscendLaunchKernelWithHostArgs(
    void* funcHandle, uint32_t numBlocks, void* stream, void* hostArgs, size_t argsSize, uint32_t ubufDynamicSize);
extern "C" uint32_t AscendCFunctionGetMetaInfoKtype(const rtFuncHandle funcHandle, unsigned int* kernelType);
extern "C" uint32_t AscendCFunctionGetMetaInfoCrossCoreType(const rtFuncHandle funcHandle, unsigned int* crossType);
extern "C" uint32_t AscendCFunctionGetMetaInfoCoreRation(
    const rtFuncHandle funcHandle, unsigned short* aicRation, unsigned short* aivRation);
extern "C" uint32_t AscendCGetProfkTypeImpl(const rtFuncHandle funcHandle);

#endif // __ASCENDC_RUNTIME_H__
