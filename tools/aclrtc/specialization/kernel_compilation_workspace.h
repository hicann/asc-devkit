/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_KERNEL_COMPILATION_WORKSPACE_H
#define ASCENDC_ACLRTC_KERNEL_COMPILATION_WORKSPACE_H

#include "kernel_specialization_types.h"
#include "directory_cleanup_guard.h"

#include <boost/filesystem/path.hpp>
#include <cstdint>

namespace ascendc {
namespace aclrtc {

enum class WorktreeRetentionPolicy : uint32_t {
    RemoveAfterCompilation,
    RetainAfterCompilation,
};

class KernelCompilationWorkspace final {
public:
    KernelCompilationWorkspace(boost::filesystem::path worktreePath, WorktreeRetentionPolicy retentionPolicy);
    ~KernelCompilationWorkspace() = default;

    KernelCompilationWorkspace(const KernelCompilationWorkspace&) = delete;
    KernelCompilationWorkspace& operator=(const KernelCompilationWorkspace&) = delete;

    boost::filesystem::path GetWorktreePath() const;
    aclError CreateOutputDirectoriesAndApplySourcePatches(const KernelCompilationPlan& compilationPlan);
    aclError CleanupWorktreeBeforeElfPublication();

private:
    DirectoryCleanupGuard worktreeCleanupGuard_;
    WorktreeRetentionPolicy retentionPolicy_{WorktreeRetentionPolicy::RemoveAfterCompilation};
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_COMPILATION_WORKSPACE_H
