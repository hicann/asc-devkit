/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef RTC_SPECIALIZATION_RUNTIME_RESOURCES_H
#define RTC_SPECIALIZATION_RUNTIME_RESOURCES_H
#include "acl/acl.h"
#include <array>
#include <cstdio>
#include <cstdlib>

// Own resources on early returns; creation and the normal cleanup call remain explicit in main.
struct RuntimeResources {
    bool initialized = false;
    aclrtContext context = nullptr;
    aclrtStream stream = nullptr;
    aclrtBinHandle binary = nullptr;
    void* xHost = nullptr;
    void* yHost = nullptr;
    void* zHost = nullptr;
    void* xDevice = nullptr;
    void* yDevice = nullptr;
    void* zDevice = nullptr;
    void* workspaceDevice = nullptr;
    void* tilingDevice = nullptr;

    bool Close()
    {
        if (stream != nullptr && aclrtSynchronizeStream(stream) != ACL_SUCCESS) {
            return false;
        }
        if (binary != nullptr) {
            if (aclrtBinaryUnLoad(binary) != ACL_SUCCESS) {
                return false;
            }
            binary = nullptr;
        }
        if (xDevice != nullptr) {
            if (aclrtFree(xDevice) != ACL_SUCCESS) {
                return false;
            }
            xDevice = nullptr;
        }
        if (yDevice != nullptr) {
            if (aclrtFree(yDevice) != ACL_SUCCESS) {
                return false;
            }
            yDevice = nullptr;
        }
        if (zDevice != nullptr) {
            if (aclrtFree(zDevice) != ACL_SUCCESS) {
                return false;
            }
            zDevice = nullptr;
        }
        if (workspaceDevice != nullptr) {
            if (aclrtFree(workspaceDevice) != ACL_SUCCESS) {
                return false;
            }
            workspaceDevice = nullptr;
        }
        if (tilingDevice != nullptr) {
            if (aclrtFree(tilingDevice) != ACL_SUCCESS) {
                return false;
            }
            tilingDevice = nullptr;
        }
        for (void** buffer : std::array{&xHost, &yHost, &zHost}) {
            if (*buffer != nullptr) {
                std::free(*buffer);
                *buffer = nullptr;
            }
        }
        if (stream != nullptr) {
            if (aclrtDestroyStream(stream) != ACL_SUCCESS) {
                return false;
            }
            stream = nullptr;
        }
        if (context != nullptr) {
            if (aclrtDestroyContext(context) != ACL_SUCCESS) {
                return false;
            }
            context = nullptr;
        }
        if (initialized) {
            if (aclFinalize() != ACL_SUCCESS) {
                return false;
            }
            initialized = false;
        }
        return true;
    }

    ~RuntimeResources()
    {
        if (!Close()) {
            std::fprintf(stderr, "Resource cleanup failed\n");
        }
    }
};
#endif
