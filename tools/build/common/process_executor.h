/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_COMMON_PROCESS_EXECUTOR_H
#define ASCENDC_COMMON_PROCESS_EXECUTOR_H

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ascendc {

enum class ProcessOutcome : uint32_t {
    ExecutorFailure,
    Exited,
    Signaled,
    TimedOut,
};

struct ProcessExecutorRequest {
    std::vector<std::string> arguments;
    std::vector<std::pair<std::string, std::string>> environmentOverrides;
    std::vector<std::string> removedEnvironmentVariables;
    std::string mirroredOutputLogFilePath;
    std::chrono::milliseconds executionTimeout{0};
    std::chrono::milliseconds terminationGracePeriod{0};
};

struct ProcessExecutorResult {
    ProcessOutcome outcome{ProcessOutcome::ExecutorFailure};
    int32_t terminationCode{0};
    std::chrono::milliseconds elapsedTime{0};

    bool HasSuccessfulExit() const noexcept;
    const char* GetOutcomeName() const noexcept;
};

class ProcessExecutor final {
public:
    ProcessExecutor() = delete;

    static ProcessExecutorResult Execute(const ProcessExecutorRequest& request);
};

} // namespace ascendc

#endif // ASCENDC_COMMON_PROCESS_EXECUTOR_H
