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

#include "build_workspace.h"
#include "file_utils.h"
#include "test_support.h"

namespace ascendc {
namespace manifest_generator {
namespace {

using asc_compile_exporter_test::LstatPathExists;
using asc_compile_exporter_test::ModuleTest;
using asc_compile_exporter_test::ScopedEnvironment;
using asc_compile_exporter_test::WriteTestFile;

TEST_F(ModuleTest, BuildWorkspaceHonorsCleanupPolicy)
{
    const std::string parent = FileUtils::JoinPath(root_, "sessions");
    std::string removedSession;
    {
        BuildWorkspace workspace;
        ASSERT_TRUE(workspace.Create(parent, false));
        removedSession = workspace.Session();
        EXPECT_TRUE(FileUtils::IsDirectory(removedSession));
        EXPECT_FALSE(workspace.Create(parent, false));
        EXPECT_EQ(workspace.Session(), removedSession);
    }
    EXPECT_FALSE(LstatPathExists(removedSession));

    std::string keptSession;
    {
        BuildWorkspace workspace;
        ASSERT_TRUE(workspace.Create(parent, true));
        keptSession = workspace.Session();
    }
    EXPECT_TRUE(FileUtils::IsDirectory(keptSession));

    const std::string regularFile = FileUtils::JoinPath(root_, "not-a-directory");
    WriteTestFile(regularFile, "file");
    BuildWorkspace failedWorkspace;
    EXPECT_FALSE(failedWorkspace.Create(regularFile, false));
    EXPECT_FALSE(failedWorkspace.Create("/proc", false));
}

TEST_F(ModuleTest, BuildWorkspaceUsesTmpDir)
{
    ScopedEnvironment temporary("TMPDIR", root_.c_str());
    std::string session;
    {
        BuildWorkspace workspace;
        ASSERT_TRUE(workspace.Create("", false));
        session = workspace.Session();
        EXPECT_TRUE(FileUtils::IsPathWithin(session, root_));
    }
    EXPECT_FALSE(LstatPathExists(session));
}

} // namespace
} // namespace manifest_generator
} // namespace ascendc
