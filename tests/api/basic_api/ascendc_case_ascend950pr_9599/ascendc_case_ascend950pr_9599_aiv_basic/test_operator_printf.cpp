/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#define ASCENDC_DUMP 1
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <vector>
#include "kernel_operator.h"
#include "utils/debug/asc_printf.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
__aicore__ inline uint32_t asc_debug_get_core_idx()
{
    if ASCEND_IS_AIV {
        return AscendC::GetBlockIdxImpl();
    } else {
        return AscendC::GetBlockIdxImpl() + AscendC::AIV_CORE_NUM;
    }
}
#endif

using namespace AscendC;

enum class PrintfCaseEnum : uint32_t { VALUE = 9 };

class TestPrintfSuite : public testing::Test {
protected:
    void TearDown() { GlobalMockObject::verify(); }
};

int32_t RaiseStubForPrintf(int32_t input) { return 0; }

void DataPrintfCase(__gm__ uint8_t* srcGm, __gm__ uint8_t* workGm, __gm__ uint32_t dataSize, __gm__ uint64_t dumpSize)
{
    GlobalTensor<uint8_t> srcGlobal;
    srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ uint8_t*>(srcGm), dataSize);

    InitDump(false, workGm, dumpSize);
    PrintTimeStamp(0);

    const int32_t signedValue = -3;
    const uint32_t unsignedValue = 6U;
    const float floatValue = 1.25F;
    const double doubleValue = 2.5;
    const PrintfCaseEnum enumValue = PrintfCaseEnum::VALUE;
    __gm__ uint8_t* ptrValue = srcGm;

    PRINTF("PRINTF_CASE %s %d %u", "hello_printf", signedValue, unsignedValue);
    PRINTF(
        "PRINTF_CASE2 %f %lf %p %d", floatValue, doubleValue, static_cast<void*>(ptrValue),
        static_cast<int32_t>(enumValue));
}

TEST_F(TestPrintfSuite, PrintfCase)
{
    int32_t coreTypeTmp = g_coreType;
    auto blockNumTmp = block_num;
    auto blockIdxTmp = block_idx;
    auto subBlockIdxTmp = sub_block_idx;
    auto taskRationTmp = g_taskRation;
    SetGCoreType(2);
    block_num = 1;
    block_idx = 0;
    sub_block_idx = 0;
    g_taskRation = 1;

    constexpr uint32_t dataSize = 64;
    constexpr uint64_t dumpSize = DUMP_UINTSIZE;
    uint8_t srcGm[dataSize] = {0};
    std::vector<uint8_t> workGm(dumpSize, 0);
    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStubForPrintf));

    DataPrintfCase(srcGm, workGm.data(), dataSize, dumpSize);

    SetGCoreType(coreTypeTmp);
    block_num = blockNumTmp;
    block_idx = blockIdxTmp;
    sub_block_idx = subBlockIdxTmp;
    g_taskRation = taskRationTmp;
}

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
TEST_F(TestPrintfSuite, SimdVfPrintfMatchesAicoreCpuFallback)
{
    testing::internal::CaptureStdout();
    __asc_aicore::printf("SIMD_VF_PRINTF %s %d %u %.2f 0x%x", "value", -3, 6U, 1.25, 0x2AU);
    const std::string aicoreOutput = testing::internal::GetCapturedStdout();

    testing::internal::CaptureStdout();
    __asc_simd_vf::printf("SIMD_VF_PRINTF %s %d %u %.2f 0x%x", "value", -3, 6U, 1.25, 0x2AU);
    const std::string simdVfOutput = testing::internal::GetCapturedStdout();

    EXPECT_EQ(simdVfOutput, aicoreOutput);
    EXPECT_EQ(simdVfOutput, "SIMD_VF_PRINTF value -3 6 1.25 0x2a");
}
#endif

TEST_F(TestPrintfSuite, DumpBlockIdxAndSysVarCase)
{
    int32_t coreTypeTmp = g_coreType;
    auto blockNumTmp = block_num;
    auto blockIdxTmp = block_idx;
    auto subBlockIdxTmp = sub_block_idx;
    auto taskRationTmp = g_taskRation;

    block_num = 1;
    SetGCoreType(2);
    block_idx = 3;
    sub_block_idx = 1;
    g_taskRation = 2;
    EXPECT_EQ(GetBlockIdx(), 7);
    EXPECT_EQ(asc_debug_get_core_idx(), 7);

    SetGCoreType(1);
    block_idx = 4;
    sub_block_idx = 0;
    g_taskRation = 2;
    EXPECT_EQ(GetBlockIdx(), 4);
    EXPECT_EQ(asc_debug_get_core_idx(), 4 + AIV_CORE_NUM);

    SetGCoreType(coreTypeTmp);
    block_num = blockNumTmp;
    block_idx = blockIdxTmp;
    sub_block_idx = subBlockIdxTmp;
    g_taskRation = taskRationTmp;
}
