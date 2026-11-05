/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include "stub_fun.h"
#include "impl/utils/sys_macros_impl.h"
#include "impl/utils/common_types.h"

// Use the device debug utilities with the existing CPU instruction stubs.
struct float4_e2m1x2_t;
namespace __cce_scalar {
using ::copy_ubuf_to_ubuf;
}
#pragma push_macro("ASCENDC_CPU_DEBUG")
#undef ASCENDC_CPU_DEBUG
#include "impl/utils/debug/asc_debug_utils.h"
#pragma pop_macro("ASCENDC_CPU_DEBUG")

namespace {
// The instruction has more arguments than the default mockcpp supports.
// Capture its burst length directly instead of simulating a device FIFO.
uint32_t capturedBlocks;
uint32_t copyCalls;
template <typename... Args>
void CheckUbCopy(void*, void*, uint8_t sid, uint16_t nBurst, uint32_t lenBurst, Args...)
{
    ++copyCalls;
    EXPECT_EQ(sid, 0U);
    EXPECT_EQ(nBurst, 1U);
    capturedBlocks = lenBurst;
}
} // namespace

#define copy_ubuf_to_gm CheckUbCopy
#include "impl/utils/debug/npu_arch_2201/asc_aicore_dump_utils.h"
#undef copy_ubuf_to_gm

TEST(TestDumpTensorCopy, UbConvertsBytesTo32ByteBlocks)
{
    uint8_t source[8192] = {};
    uint8_t destination[8192] = {};
    struct CopyCase {
        uint32_t bytes;
        uint32_t blocks;
    };
    for (const auto& test : {CopyCase{32U, 1U}, CopyCase{1056U, 33U}, CopyCase{8192U, 256U}}) {
        SCOPED_TRACE(test.bytes);
        copyCalls = 0U;
        capturedBlocks = 0U;
        EXPECT_EQ(__asc_aicore::mem_copy_ub_to_gm_impl(destination, source, test.bytes), 0U);
        EXPECT_EQ(copyCalls, 1U);
        EXPECT_EQ(capturedBlocks, test.blocks);
    }
}
