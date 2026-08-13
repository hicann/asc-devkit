/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the LICENSE.
 */

#include <gtest/gtest.h>

#include <cerrno>
#include <chrono>
#include <cstdlib>

#include <signal.h>
#include <sys/wait.h>

#include "file_utils.h"
#include "process_runner.h"
#include "test_support.h"

namespace {

bool g_failFork = false;
bool g_failNextWaitpid = false;

} // namespace

extern "C" pid_t __real_fork(void);
extern "C" pid_t __real_waitpid(pid_t pid, int* status, int options);

extern "C" pid_t __wrap_fork(void)
{
    if (g_failFork) {
        errno = EAGAIN;
        return -1;
    }
    return __real_fork();
}

extern "C" pid_t __wrap_waitpid(pid_t pid, int* status, int options)
{
    if (g_failNextWaitpid) {
        g_failNextWaitpid = false;
        errno = EIO;
        return -1;
    }
    return __real_waitpid(pid, status, options);
}

namespace ascendc {
namespace {

using asc_compile_exporter_test::ModuleTest;
using asc_compile_exporter_test::ReadTestFile;
using asc_compile_exporter_test::ScopedEnvironment;

TEST_F(ModuleTest, ProcessRunnerValidatesArgumentsAndPropagatesEnvironment)
{
    EXPECT_FALSE(ProcessRunner::Run({}, {}));
    EXPECT_FALSE(ProcessRunner::Run({"/bin/true"}, {}, std::chrono::milliseconds(0U)));
    EXPECT_FALSE(ProcessRunner::Run({"/bin/sh", "-c", "exit 7"}, {}));

    const std::string output = FileUtils::JoinPath(root_, "environment.txt");
    ScopedEnvironment makeFlags("MAKEFLAGS", "must-not-leak");
    ASSERT_TRUE(ProcessRunner::Run(
        {"/bin/sh", "-c",
         "printf '%s|%s' \"$ASC_COMPILE_EXPORTER_UNIT_VALUE\" \"${MAKEFLAGS-unset}\" > "
         "\"$ASC_COMPILE_EXPORTER_UNIT_OUTPUT\""},
        {{"ASC_COMPILE_EXPORTER_UNIT_VALUE", "visible"}, {"ASC_COMPILE_EXPORTER_UNIT_OUTPUT", output}}));
    EXPECT_EQ(ReadTestFile(output), "visible|unset");
}

TEST_F(ModuleTest, ProcessRunnerReportsExecAndSignalFailures)
{
    g_failFork = true;
    EXPECT_FALSE(ProcessRunner::Run({"/bin/true"}, {}));
    g_failFork = false;
    EXPECT_FALSE(ProcessRunner::Run({FileUtils::JoinPath(root_, "missing-command")}, {}));
    EXPECT_FALSE(ProcessRunner::Run({"/bin/sh", "-c", "kill -TERM $$"}, {}));
}

TEST_F(ModuleTest, ProcessRunnerTimesOutAndReapsProcessGroup)
{
    const std::string pidFile = FileUtils::JoinPath(root_, "timed-out.pid");
    const std::string terminationFile = FileUtils::JoinPath(root_, "termination.txt");
    EXPECT_FALSE(ProcessRunner::Run(
        {"/bin/sh", "-c",
         "trap 'printf terminated > \"$ASC_COMPILE_EXPORTER_TERMINATION_FILE\"; exit 0' TERM; "
         "printf '%s' \"$$\" > \"$ASC_COMPILE_EXPORTER_PID_FILE\"; sleep 10"},
        {{"ASC_COMPILE_EXPORTER_PID_FILE", pidFile}, {"ASC_COMPILE_EXPORTER_TERMINATION_FILE", terminationFile}},
        std::chrono::milliseconds(100U)));

    EXPECT_EQ(ReadTestFile(terminationFile), "terminated");
    const pid_t child = static_cast<pid_t>(std::strtol(ReadTestFile(pidFile).c_str(), nullptr, 10));
    ASSERT_GT(child, 0);
    errno = 0;
    EXPECT_EQ(kill(child, 0), -1);
    EXPECT_EQ(errno, ESRCH);
}

TEST_F(ModuleTest, ProcessRunnerReportsWaitFailureAndReapsChild)
{
    g_failNextWaitpid = true;
    EXPECT_FALSE(ProcessRunner::Run({"/bin/true"}, {}, std::chrono::seconds(1U)));
    EXPECT_FALSE(g_failNextWaitpid);
}

} // namespace
} // namespace ascendc
