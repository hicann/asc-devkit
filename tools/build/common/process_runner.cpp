/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "process_runner.h"

#include <cerrno>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <thread>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ascendc_tool_log.h"

namespace ascendc {
namespace {

constexpr uint32_t SUBPROCESS_TIMEOUT_SECONDS = 30U * 60U;
constexpr uint32_t SUBPROCESS_TERMINATION_GRACE_SECONDS = 5U;
constexpr uint32_t SUBPROCESS_POLL_MILLISECONDS = 10U;

// Use shell-compatible exit codes: 126 for child setup failure and 127 when execv cannot start the command.
constexpr int SUBPROCESS_SETUP_FAILURE_EXIT_CODE = 126;
constexpr int SUBPROCESS_EXEC_FAILURE_EXIT_CODE = 127;

} // namespace

bool ProcessRunner::Run(
    const std::vector<std::string>& arguments, const std::vector<std::pair<std::string, std::string>>& environment)
{
    return Run(arguments, environment, std::chrono::seconds(SUBPROCESS_TIMEOUT_SECONDS));
}

bool ProcessRunner::Run(
    const std::vector<std::string>& arguments, const std::vector<std::pair<std::string, std::string>>& environment,
    std::chrono::milliseconds timeout)
{
    if (arguments.empty()) {
        ASCENDLOGE("Subprocess arguments must not be empty");
        return false;
    }
    if (timeout.count() <= 0) {
        ASCENDLOGE("Subprocess timeout must be positive");
        return false;
    }
    std::vector<char*> argv;
    argv.reserve(arguments.size() + 1U);
    for (const std::string& argument : arguments) {
        argv.push_back(const_cast<char*>(argument.c_str()));
    }
    argv.push_back(nullptr);

    const pid_t child = fork();
    if (child < 0) {
        const int forkError = errno;
        ASCENDLOGE("Failed to fork subprocess: %s", std::strerror(forkError));
        return false;
    }
    if (child == 0) {
        if (setpgid(0, 0) != 0) {
            _exit(SUBPROCESS_SETUP_FAILURE_EXIT_CODE);
        }
        for (const char* name : {"MAKEFLAGS", "MFLAGS", "MAKEFILES", "GNUMAKEFLAGS"}) {
            (void)unsetenv(name);
        }
        for (const auto& item : environment) {
            if (setenv(item.first.c_str(), item.second.c_str(), 1) != 0) {
                _exit(SUBPROCESS_SETUP_FAILURE_EXIT_CODE);
            }
        }
        execv(argv[0], argv.data());
        _exit(SUBPROCESS_EXEC_FAILURE_EXIT_CODE);
    }

    ASCENDLOGD("Started subprocess pid=%d executable=%s", static_cast<int>(child), arguments.front().c_str());

    int status = 0;
    const std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::now() + timeout;
    while (true) {
        const pid_t waited = waitpid(child, &status, WNOHANG);
        if (waited == child) {
            break;
        }
        if (waited < 0 && errno != EINTR) {
            const int waitError = errno;
            TerminateAndReap(child);
            ASCENDLOGE("Failed to wait for subprocess: %s", std::strerror(waitError));
            return false;
        }
        if (std::chrono::steady_clock::now() >= deadline) {
            TerminateAndReap(child);
            ASCENDLOGE("Subprocess timed out: %s", arguments.front().c_str());
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SUBPROCESS_POLL_MILLISECONDS));
    }
    if (WIFSIGNALED(status)) {
        ASCENDLOGE("Subprocess terminated by signal %d: %s", WTERMSIG(status), arguments.front().c_str());
        return false;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        const int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        ASCENDLOGE("Subprocess exited with code %d: %s", exitCode, arguments.front().c_str());
        return false;
    }
    ASCENDLOGD("Subprocess pid=%d completed successfully", static_cast<int>(child));
    return true;
}

void ProcessRunner::TerminateAndReap(pid_t child) noexcept
{
    (void)kill(-child, SIGTERM);
    const std::chrono::steady_clock::time_point deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(SUBPROCESS_TERMINATION_GRACE_SECONDS);
    int status = 0;
    bool childReaped = false;
    while (std::chrono::steady_clock::now() < deadline) {
        if (!childReaped) {
            const pid_t waited = waitpid(child, &status, WNOHANG);
            childReaped = waited == child || (waited < 0 && errno == ECHILD);
        }
        errno = 0;
        if (kill(-child, 0) != 0 && errno == ESRCH) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SUBPROCESS_POLL_MILLISECONDS));
    }
    errno = 0;
    if (kill(-child, 0) == 0 || errno != ESRCH) {
        (void)kill(-child, SIGKILL);
    }
    if (!childReaped) {
        pid_t waited = -1;
        do {
            waited = waitpid(child, &status, 0);
        } while (waited < 0 && errno == EINTR);
    }
}

} // namespace ascendc
