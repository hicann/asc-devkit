/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include <stdarg.h>
#include <stdint.h>
#include <mutex>
#include <stdio.h>
#include "securec.h"
#include "aicpu_api/aicpu_api.h"
#include <mockcpp/mockcpp.hpp>

extern "C" {
extern int32_t (*g_utAicpuGetDfxInfoHook)(uint64_t* infoAddr);
}

class TEST_AICPU_DUMP : public testing::Test {
protected:
    void SetUp() { g_utAicpuGetDfxInfoHook = nullptr; }
    void TearDown() { GlobalMockObject::verify(); }
};

struct DumpConfig {
    void* dumpBufAddr;
    size_t dumpBufSize;
    size_t dumpOffset;
};

extern "C" {
extern DumpConfig g_aicpuDumpConfig;
}

TEST_F(TEST_AICPU_DUMP, printfTest)
{
    const size_t buffer_size = 1024;
    char buffer[buffer_size] = {0};
    g_aicpuDumpConfig.dumpBufAddr = buffer;
    g_aicpuDumpConfig.dumpBufSize = buffer_size;
    g_aicpuDumpConfig.dumpOffset = 0ULL;
    *reinterpret_cast<size_t*>(buffer) = 0;
    EXPECT_NO_THROW(AscendC::printf("test\n"));

    {
        g_aicpuDumpConfig.dumpBufAddr = nullptr;
        g_aicpuDumpConfig.dumpBufSize = 1024ULL;
        EXPECT_NO_THROW(AscendC::printf("test\n"));
    }

    *reinterpret_cast<size_t*>(buffer) = buffer_size;
    g_aicpuDumpConfig.dumpBufAddr = buffer;
    g_aicpuDumpConfig.dumpBufSize = buffer_size;
    EXPECT_NO_THROW(AscendC::printf("test\n"));
}
