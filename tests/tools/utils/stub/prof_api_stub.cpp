/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <stdio.h>
#include "aprof_pub.h"

extern "C" {
uint32_t g_msprofReportCompactInfoCallCount = 0;
uint32_t g_msprofReportAdditionalInfoCallCount = 0;
uint32_t g_msprofReportApiCallCount = 0;
uint32_t g_msprofRegisterCallbackCallCount = 0;
uint32_t g_msprofLastRegisterModuleId = 0;
ProfCommandHandle g_msprofLastRegisterCallback = nullptr;
uint32_t g_msprofUnRegisterCallbackCallCount = 0;
uint32_t g_msprofLastUnRegisterModuleId = 0;
ProfCommandHandle g_msprofLastUnRegisterCallback = nullptr;
MsprofApi g_msprofLastApi = {};
MsprofCompactInfo g_msprofLastCompactInfo = {};
MsprofAdditionalInfo g_msprofLastAdditionalInfo = {};
}

int32_t MsprofReportCompactInfo(uint32_t agingFlag, const VOID_PTR data, uint32_t length)
{
    ++g_msprofReportCompactInfoCallCount;
    if (data != nullptr && length == sizeof(MsprofCompactInfo)) {
        g_msprofLastCompactInfo = *reinterpret_cast<const MsprofCompactInfo*>(data);
    }
    return 0;
}

int32_t MsprofReportAdditionalInfo(uint32_t agingFlag, const VOID_PTR data, uint32_t length)
{
    ++g_msprofReportAdditionalInfoCallCount;
    if (data != nullptr && length == sizeof(MsprofAdditionalInfo)) {
        g_msprofLastAdditionalInfo = *reinterpret_cast<const MsprofAdditionalInfo*>(data);
    }
    return 0;
}

int32_t MsprofReportApi(uint32_t agingFlag, const MsprofApi* api)
{
    ++g_msprofReportApiCallCount;
    if (api != nullptr) {
        g_msprofLastApi = *api;
    }
    return 0;
}

uint64_t MsprofGetHashId(const char* hashInfo, size_t length) { return 0; }

uint64_t MsprofStr2Id(const char* hashInfo, size_t length) { return 0; }

uint64_t MsprofSysCycleTime() { return 0; }

int32_t MsprofRegisterCallback(uint32_t moduleId, ProfCommandHandle handle)
{
    ++g_msprofRegisterCallbackCallCount;
    g_msprofLastRegisterModuleId = moduleId;
    g_msprofLastRegisterCallback = handle;
    return 1;
}

int32_t MsprofUnRegisterCallback(uint32_t moduleId, ProfCommandHandle handle)
{
    ++g_msprofUnRegisterCallbackCallCount;
    g_msprofLastUnRegisterModuleId = moduleId;
    g_msprofLastUnRegisterCallback = handle;
    return 0;
}
