/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <vector>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
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
// Capture its matrix dimensions directly instead of simulating a device FIFO.
std::vector<uint16_t> copiedN;
std::vector<ptrdiff_t> copiedDstOffsets;
std::vector<ptrdiff_t> copiedSrcOffsets;
int32_t* dstBase;
int32_t* srcBase;
template <typename... Args>
void CheckMatrixCopy(
    int32_t* dst, int32_t* src, uint8_t sid, uint16_t n, uint16_t m, uint32_t dstStride, uint16_t srcStride, Args...)
{
    EXPECT_EQ(sid, 0U);
    EXPECT_EQ(m, 16U);
    EXPECT_EQ(dstStride, 256U); // 5102 uses elements.
    EXPECT_EQ(srcStride, 16U);
    copiedN.push_back(n);
    copiedDstOffsets.push_back(dst - dstBase);
    copiedSrcOffsets.push_back(src - srcBase);
}

void ResetCapture(int32_t* destination, int32_t* source)
{
    copiedN.clear();
    copiedDstOffsets.clear();
    copiedSrcOffsets.clear();
    dstBase = destination;
    srcBase = source;
}
} // namespace

#define copy_matrix_cc_to_gm CheckMatrixCopy
#include "impl/utils/debug/npu_arch_5102/asc_aicore_dump_utils.h"
#undef copy_matrix_cc_to_gm

TEST(TestDumpTensorCopy, L0CConvertsBytesToB32Fractals)
{
    struct CopyCase {
        uint32_t bytes;
        uint16_t n;
    };
    int32_t source[1] = {};
    int32_t destination[1] = {};
    for (const auto& test :
         {CopyCase{32U, 16U}, CopyCase{1024U, 16U}, CopyCase{1056U, 32U}, CopyCase{131072U, 2048U}}) {
        SCOPED_TRACE(test.bytes);
        ResetCapture(destination, source);
        EXPECT_EQ(__asc_aicore::mem_copy_cbuf_to_gm_impl(destination, source, test.bytes), 0U);
        ASSERT_EQ(copiedN.size(), 1U);
        EXPECT_EQ(copiedN[0], test.n);
        EXPECT_EQ(copiedDstOffsets[0], 0);
    }
}

// nSize lives in Xm[15:4], so one copy carries at most 255 fractals of 16x16 B32.
// A larger payload has to be split across several copies into the same record,
// because the host dump parser only reads one record per DumpTensor call.
TEST(TestDumpTensorCopy, L0CSplitsPayloadsPastTheNSizeCeiling)
{
    constexpr uint32_t ceilingBytes = 255U * 1024U;
    constexpr uint16_t ceilingN = 4080U;
    int32_t source[1] = {};
    int32_t destination[1] = {};

    ResetCapture(destination, source);
    EXPECT_EQ(__asc_aicore::mem_copy_cbuf_to_gm_impl(destination, source, ceilingBytes), 0U);
    ASSERT_EQ(copiedN.size(), 1U);
    EXPECT_EQ(copiedN[0], ceilingN);

    // One fractal past the ceiling is the smallest payload that needs two copies.
    ResetCapture(destination, source);
    EXPECT_EQ(__asc_aicore::mem_copy_cbuf_to_gm_impl(destination, source, ceilingBytes + 1024U), 0U);
    ASSERT_EQ(copiedN.size(), 2U);
    EXPECT_EQ(copiedN[0], ceilingN);
    EXPECT_EQ(copiedN[1], 16U);
    EXPECT_EQ(copiedDstOffsets[0], 0);
    EXPECT_EQ(copiedSrcOffsets[0], 0);
    EXPECT_EQ(copiedDstOffsets[1], static_cast<ptrdiff_t>(ceilingBytes / sizeof(int32_t)));
    EXPECT_EQ(copiedSrcOffsets[1], static_cast<ptrdiff_t>(ceilingBytes / sizeof(int32_t)));

    // Whatever the payload, every chunk stays inside the 12-bit nSize field.
    ResetCapture(destination, source);
    EXPECT_EQ(__asc_aicore::mem_copy_cbuf_to_gm_impl(destination, source, 4U * 1024U * 1024U), 0U);
    EXPECT_EQ(copiedN.size(), 17U);
    for (size_t i = 0; i < copiedN.size(); ++i) {
        SCOPED_TRACE(i);
        EXPECT_LE(copiedN[i], 4095U);
        EXPECT_EQ(copiedN[i] % 16U, 0U);
        EXPECT_EQ(copiedDstOffsets[i], copiedSrcOffsets[i]);
    }
}
