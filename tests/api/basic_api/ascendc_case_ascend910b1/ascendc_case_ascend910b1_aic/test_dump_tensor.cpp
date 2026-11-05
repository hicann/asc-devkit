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

#include "impl/utils/debug/npu_arch_2201/asc_aicore_dump_utils.h"

namespace {
using namespace __asc_aicore;

uint16_t copiedBlocks;
void RecordL1Copy(void*, void*, uint8_t sid, uint16_t bursts, uint16_t blocks, uint16_t srcStride, uint16_t dstStride)
{
    EXPECT_EQ(sid, 0U);
    EXPECT_EQ(bursts, 1U);
    EXPECT_EQ(srcStride, 0U);
    EXPECT_EQ(dstStride, 0U);
    copiedBlocks = blocks;
}

class TestDumpTensorCopy : public testing::Test {
protected:
    void TearDown() override { GlobalMockObject::verify(); }
};

TEST_F(TestDumpTensorCopy, L1ConvertsBytesTo32ByteBlocks)
{
    uint8_t source[8192] = {};
    uint8_t destination[8192] = {};
    using CopyL1 = void (*)(void*, void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t);
    MOCKER(copy_cbuf_to_gm, CopyL1).stubs().will(invoke(RecordL1Copy));
    struct CopyCase {
        uint32_t bytes;
        uint16_t blocks;
    };
    for (const auto& test : {CopyCase{32U, 1U}, CopyCase{1056U, 33U}, CopyCase{8192U, 256U}}) {
        SCOPED_TRACE(test.bytes);
        copiedBlocks = 0U;
        EXPECT_EQ(mem_copy_l1buf_to_gm_impl(destination, source, test.bytes), 0U);
        EXPECT_EQ(copiedBlocks, test.blocks);
    }
}

TEST_F(TestDumpTensorCopy, L0CConvertsBytesToB32Fractals)
{
    struct CopyCase {
        uint32_t bytes;
        uint16_t expectedN;
    };
    for (const auto& test :
         {CopyCase{32U, 16U}, CopyCase{1024U, 16U}, CopyCase{1056U, 32U}, CopyCase{131072U, 2048U}}) {
        SCOPED_TRACE(test.bytes);
        uint16_t n, m, dstStride, srcStride;
        get_matrix_copy_param(test.bytes, n, m, dstStride, srcStride);
        EXPECT_EQ(n, test.expectedN);
        EXPECT_EQ(m, 16U);
        EXPECT_EQ(dstStride, 32U); // 2201 uses 32B blocks.
        EXPECT_EQ(srcStride, 16U);
    }
}
} // namespace
