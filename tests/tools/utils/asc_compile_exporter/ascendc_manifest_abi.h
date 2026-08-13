/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef ASCENDC_MANIFEST_ABI_H
#define ASCENDC_MANIFEST_ABI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AC_COMPILE_RESOURCE_MAGIC 0x41435253u
#define AC_COMPILE_RESOURCE_ABI_VERSION 1u
#define AC_COMPILE_RESOURCE_EXTENSION_REQUIRED 0x1ULL

typedef struct AcString {
    const char* data;
    uint64_t size;
} AcString;

typedef struct AcCompileResourceExtension {
    uint32_t type;
    uint32_t version;
    uint64_t flags;
    const void* data;
    uint64_t dataSize;
} AcCompileResourceExtension;

typedef struct AcCompileResourceFile {
    AcString fileName;
    AcString filePath;
    const uint8_t* data;
    uint64_t size;
} AcCompileResourceFile;

typedef struct AcCompileResourceManifest {
    AcString json;
    const AcCompileResourceFile* files;
    uint64_t fileCount;
} AcCompileResourceManifest;

typedef struct AcCompileResourceBundleHeader {
    uint32_t magic;
    uint32_t abiVersion;
    uint32_t structSize;
    uint32_t flags;
} AcCompileResourceBundleHeader;

typedef struct AcCompileResourceBundle {
    AcCompileResourceBundleHeader header;
    const AcCompileResourceManifest* manifests;
    uint64_t manifestCount;
    const AcCompileResourceExtension* extensions;
    uint64_t extensionCount;
} AcCompileResourceBundle;

typedef const AcCompileResourceBundleHeader* (*AscendcGetCompileResourceBundleFn)(void);

const AcCompileResourceBundleHeader* AscendcGetCompileResourceBundle(void);

#ifdef __cplusplus
}
#endif

#endif // ASCENDC_MANIFEST_ABI_H
