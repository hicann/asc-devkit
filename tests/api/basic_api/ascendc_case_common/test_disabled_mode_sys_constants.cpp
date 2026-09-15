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

#define __ASC_DISABLE_RESERVED_UBUF__
#define __ASC_DISABLE_VF_STACK_RESERVED__

#include "utils/base/sys_constants.h"

namespace {

TEST(TestSysConstants, PublicConstantsMatchArch)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001)
    EXPECT_EQ(ASC_UB_SIZE, 256 * 1024);
    EXPECT_EQ(ASC_L1_SIZE, 1024 * 1024);
    EXPECT_EQ(ASC_L0A_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0B_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0C_SIZE, 256 * 1024);
    EXPECT_EQ(ASC_BT_SIZE, 0);
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002)
    EXPECT_EQ(ASC_UB_SIZE, 256 * 1024);
    EXPECT_EQ(ASC_L1_SIZE, 1024 * 1024);
    EXPECT_EQ(ASC_L0A_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0B_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0C_SIZE, 256 * 1024);
    EXPECT_EQ(ASC_BT_SIZE, 0);
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    EXPECT_EQ(ASC_UB_SIZE, 192 * 1024 - 256);
    EXPECT_EQ(ASC_L1_SIZE, 512 * 1024 - 256);
    EXPECT_EQ(ASC_L0A_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0B_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0C_SIZE, 128 * 1024);
    EXPECT_EQ(ASC_BT_SIZE, 1 * 1024);
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3002)
    EXPECT_EQ(ASC_UB_SIZE, 248 * 1024);
    EXPECT_EQ(ASC_L1_SIZE, 1024 * 1024);
    EXPECT_EQ(ASC_L0A_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0B_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0C_SIZE, 128 * 1024);
    EXPECT_EQ(ASC_BT_SIZE, 1 * 1024);
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    EXPECT_EQ(ASC_UB_SIZE, 256 * 1024);
    EXPECT_EQ(ASC_L1_SIZE, 512 * 1024);
    EXPECT_EQ(ASC_L0A_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0B_SIZE, 64 * 1024);
    EXPECT_EQ(ASC_L0C_SIZE, 256 * 1024);
    EXPECT_EQ(ASC_BT_SIZE, 4 * 1024);
#else
    EXPECT_EQ(ASC_UB_SIZE, 0);
    EXPECT_EQ(ASC_L1_SIZE, 0);
    EXPECT_EQ(ASC_L0A_SIZE, 0);
    EXPECT_EQ(ASC_L0B_SIZE, 0);
    EXPECT_EQ(ASC_L0C_SIZE, 0);
    EXPECT_EQ(ASC_BT_SIZE, 0);
#endif
}

} // namespace
