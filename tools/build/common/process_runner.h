/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_COMMON_PROCESS_RUNNER_H
#define ASCENDC_COMMON_PROCESS_RUNNER_H

#include <chrono>
#include <string>
#include <vector>

#include <sys/types.h>

namespace ascendc {

class ProcessRunner final {
public:
    ProcessRunner() = delete;

    static bool Run(
        const std::vector<std::string>& arguments, const std::vector<std::pair<std::string, std::string>>& environment);
    static bool Run(
        const std::vector<std::string>& arguments, const std::vector<std::pair<std::string, std::string>>& environment,
        std::chrono::milliseconds timeout);

private:
    static void TerminateAndReap(pid_t child) noexcept;
};

} // namespace ascendc

#endif // ASCENDC_COMMON_PROCESS_RUNNER_H
