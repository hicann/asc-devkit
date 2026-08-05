/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <array>
#include <gtest/gtest.h>
#include "utils/debug/asc_dump.h"
#include "utils/debug/asc_printf.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1 && __NPU_ARCH__ == 3510
TEST(SimdVfDebugDumpDisabledTest, PrintfIsSilentAndDumpFallbacksRemainAvailable)
{
    testing::internal::CaptureStdout();
    __asc_simd_vf::printf("disabled %llu", 1ULL);
    EXPECT_TRUE(testing::internal::GetCapturedStdout().empty());

    int16_t ubInput[1] = {0};
    std::array<int16_t, 128> regInput = {};

#ifndef NDEBUG
    EXPECT_DEATH(asc_dump_ubuf<int16_t>(ubInput, 0, 1), "asc_dump_ubuf is not supported in cpu mode");
    EXPECT_DEATH(asc_dump_reg<int16_t>(regInput, 0, 1), "asc_dump_reg is not supported in cpu mode");
    EXPECT_DEATH(asc_dump<int16_t>(ubInput, 0, 1), "asc_dump is not supported in cpu mode");
    EXPECT_DEATH(asc_dump<int16_t>(regInput, 0, 1), "asc_dump is not supported in cpu mode");
    EXPECT_DEATH(__asc_simd_vf::asc_dump_ubuf<int16_t>(ubInput, 0, 1), "asc_dump_ubuf is not supported in cpu mode");
    EXPECT_DEATH(__asc_simd_vf::asc_dump_reg<int16_t>(regInput, 0, 1), "asc_dump_reg is not supported in cpu mode");
    EXPECT_DEATH(__asc_simd_vf::asc_dump<int16_t>(ubInput, 0, 1), "asc_dump is not supported in cpu mode");
    EXPECT_DEATH(__asc_simd_vf::asc_dump<int16_t>(regInput, 0, 1), "asc_dump is not supported in cpu mode");
#else
    asc_dump_ubuf<int16_t>(ubInput, 0, 1);
    asc_dump_reg<int16_t>(regInput, 0, 1);
    asc_dump<int16_t>(ubInput, 0, 1);
    asc_dump<int16_t>(regInput, 0, 1);
    __asc_simd_vf::asc_dump_ubuf<int16_t>(ubInput, 0, 1);
    __asc_simd_vf::asc_dump_reg<int16_t>(regInput, 0, 1);
    __asc_simd_vf::asc_dump<int16_t>(ubInput, 0, 1);
    __asc_simd_vf::asc_dump<int16_t>(regInput, 0, 1);
#endif
}
#endif
