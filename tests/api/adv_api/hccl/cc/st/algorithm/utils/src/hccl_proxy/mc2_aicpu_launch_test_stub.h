/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MC2_AICPU_LAUNCH_TEST_STUB_H
#define MC2_AICPU_LAUNCH_TEST_STUB_H

#include <cstdint>
#include <string>

// Records the AICPU launch arguments intercepted by aclrt_stub.cc so that tests can assert on the
// MC2 simple-ctx protocol without a real runtime.
struct Mc2AicpuLaunchStubState {
    // aclrtGetOpExecuteTimeout
    uint32_t opExecuteTimeoutMs = 0U;
    bool opExecuteTimeoutFail = false;
    uint32_t opExecuteTimeoutCalls = 0U;
    // rtAicpuKernelLaunchExWithArgs
    uint32_t launchCalls = 0U;
    int32_t launchRet = 0;
    uint32_t kernelType = 0U;
    uint32_t numBlocks = 0U;
    uint32_t flags = 0U;
    uint32_t argsSize = 0U;
    uint32_t soNameAddrOffset = 0U;
    uint32_t kernelNameAddrOffset = 0U;
    uint16_t timeout = 0U;
    bool isNoNeedH2DCopy = false;
    uint64_t ctxArgs[2] = {0U, 0U};
    std::string opName;
    std::string argsBlob;
    void* stream = nullptr;
};

Mc2AicpuLaunchStubState& GetMc2AicpuLaunchStubState();
void ResetMc2AicpuLaunchStubState();

#endif // MC2_AICPU_LAUNCH_TEST_STUB_H
