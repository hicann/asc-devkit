/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/**
 * npuops_exception_dump - 初始化入口
 */
#include "npuops_exception_dump.h"

#include "exception_callback.h"
#include "utils.h"

extern "C" int npuopsExceptionDumpInit(const char* kernels[], size_t count)
{
    using namespace npuops;

    // 1. 注册 Runtime exception callback（幂等）
    if (RegisterExceptionCallback() != 0) {
        return -1;
    }

    // 2. 设置 kernel name 前缀启用列表（count==0 对所有 kernel 生效）
    SetEnabledKernels(kernels, count);

    if (kernels == nullptr || count == 0) {
        LogInfo("exception dump enabled for ALL kernels");
    } else {
        for (size_t i = 0; i < count; i++) {
            LogInfo("exception dump enabled for kernel prefix: %s", kernels[i] == nullptr ? "(null)" : kernels[i]);
        }
    }
    LogInfo("dump dir: %s (NPUOPS_DUMP_DIR), level: %d (NPUOPS_DUMP_LEVEL)", GetDumpDir().c_str(), GetDumpLevel());
    return 0;
}
