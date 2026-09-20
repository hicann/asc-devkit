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
#include "mockcpp/mockcpp.hpp"
#define ASCENDC_OOM 1
#include "kernel_utils.h"
#include "kernel_common.h"
#include "kernel_operator.h"
#undef ASCENDC_OOM

using namespace std;
using namespace AscendC;
class TestDMAOom : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestDMAOom, TestCheckGmMemOverflow)
{
    const uint64_t len = 1024;
    uint8_t* workspaceGm = (uint8_t*)AscendC::GmAlloc(len * sizeof(uint8_t));

    bool isSrc = true;
    uint64_t gmLen = 256;
    for (int i = 0; i < 8; i++) {
        g_oomAddrArange.addr[i] = reinterpret_cast<uintptr_t>(workspaceGm + 128);
        g_oomAddrArange.len[i] = 128;
    }
    g_oomAddrArange.count = 8;
    g_oomAddrArange.len[0] = 0;
    AscendCUtils::CheckGmMemOverflow(workspaceGm, isSrc, 0);
    AscendCUtils::CheckGmMemOverflow(workspaceGm, isSrc, gmLen);
    AscendCUtils::CheckGmMemOverflow(workspaceGm + 128, isSrc, 1024);
    AscendCUtils::CheckGmMemOverflow(workspaceGm + 128, isSrc, 128);
    AscendCUtils::CheckGmMemOverflow(workspaceGm + 1024, isSrc, gmLen);
    EXPECT_EQ(g_oomAddrArange.len[1], 128);
    g_oomAddrArange.count = 0;
    AscendCUtils::CheckGmMemOverflow(workspaceGm, isSrc, gmLen);
    AscendC::GmFree((void*)workspaceGm);
}

TEST_F(TestDMAOom, TestOOMCheckAddrRange)
{
    const uint64_t len = 1024;
    uint8_t* workspaceGm = (uint8_t*)AscendC::GmAlloc(len * sizeof(uint8_t));
    OOMInit();
    OOMCheckAddrRange(workspaceGm, len);
    EXPECT_EQ(g_oomAddrArange.len[0], len);
    AscendC::GmFree((void*)workspaceGm);
}

TEST_F(TestDMAOom, TestOOMCheckTensorListRange)
{
    const uint64_t len = 1024;
    uint8_t* argsGm = (uint8_t*)AscendC::GmAlloc(len * sizeof(uint8_t));
    uint64_t* dynamicPtr = (uint64_t*)argsGm;
    *(dynamicPtr) = 0x28;
    *(dynamicPtr + 1) = 0x0000000100000001;
    *(dynamicPtr + 2) = 2048;
    *(dynamicPtr + 3) = 0x0000000100000001;
    *(dynamicPtr + 4) = 2048;
    uint8_t* argsGm1 = (uint8_t*)AscendC::GmAlloc(len * sizeof(uint8_t));
    uint8_t* argsGm2 = (uint8_t*)AscendC::GmAlloc(len * sizeof(uint8_t));
    *(dynamicPtr + 5) = reinterpret_cast<uint64_t>(argsGm1);
    *(dynamicPtr + 6) = reinterpret_cast<uint64_t>(argsGm2);
    uint64_t tmpAddr = *(dynamicPtr + 5);

    OOMInit();
    OOMCheckTensorListRange(dynamicPtr, 2);
    EXPECT_EQ(g_oomAddrArange.len[0], 2);

    uintptr_t inputOutputAddr = 0;
    uint64_t inputOutputLen = 0;
    bool ret = OOMCheckAddrInTensorList(0, tmpAddr, inputOutputAddr, inputOutputLen);
    EXPECT_EQ(ret, true);
    OOMInit();
    AscendC::GmFree((void*)argsGm);
    AscendC::GmFree((void*)argsGm1);
    AscendC::GmFree((void*)argsGm2);
}
