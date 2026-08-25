/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "directory_cleanup_guard.h"

#include "ascendc_tool_log.h"
#include "file_utils.h"

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <utility>

namespace ascendc {
namespace {
namespace fs = boost::filesystem;

bool IsValidDirectoryNamePrefix(const std::string& directoryNamePrefix)
{
    if (directoryNamePrefix.empty() || directoryNamePrefix.find('%') != std::string::npos ||
        directoryNamePrefix.find('\0') != std::string::npos || directoryNamePrefix.find('\\') != std::string::npos) {
        return false;
    }
    const fs::path prefixPath(directoryNamePrefix);
    return prefixPath.is_relative() && !prefixPath.has_parent_path();
}

} // namespace

DirectoryCleanupGuard::~DirectoryCleanupGuard() noexcept
{
    if (removeDirectoryOnDestruction_) {
        (void)RemoveDirectory();
    }
}

DirectoryCleanupGuard::DirectoryCleanupGuard(DirectoryCleanupGuard&& other) noexcept { MoveStateFrom(other); }

DirectoryCleanupGuard& DirectoryCleanupGuard::operator=(DirectoryCleanupGuard&& other) noexcept
{
    if (this == &other) {
        return *this;
    }
    DirectoryCleanupGuard previousDirectory(std::move(*this));
    MoveStateFrom(other);
    return *this;
}

bool DirectoryCleanupGuard::CreateUniqueSubdirectory(
    const std::string& parentDirectoryPath, const std::string& directoryNamePrefix)
{
    if (!directoryPath_.empty()) {
        ASCENDLOGE("Directory cleanup guard already manages a path: path=%s", directoryPath_.c_str());
        return false;
    }
    if (!IsValidDirectoryNamePrefix(directoryNamePrefix)) {
        ASCENDLOGE(
            "Temporary directory name prefix must be one path component without %% characters: prefix=%s",
            directoryNamePrefix.c_str());
        return false;
    }

    std::string resolvedParentDirectoryPath;
    if (!FileUtils::ResolveDirectory(parentDirectoryPath, resolvedParentDirectoryPath)) {
        ASCENDLOGE("Temporary directory parent is unavailable: parent=%s", parentDirectoryPath.c_str());
        return false;
    }
    boost::system::error_code filesystemError;
    const fs::path uniqueName = fs::unique_path(directoryNamePrefix + "-%%%%%%%%%%%%%%%%", filesystemError);
    if (filesystemError || uniqueName.empty() || uniqueName.has_parent_path()) {
        ASCENDLOGE(
            "Failed to generate temporary directory name: parent=%s prefix=%s error=%s",
            resolvedParentDirectoryPath.c_str(), directoryNamePrefix.c_str(),
            filesystemError ? filesystemError.message().c_str() : "invalid generated name");
        return false;
    }
    const fs::path createdDirectoryPath = fs::path(resolvedParentDirectoryPath) / uniqueName;
    constexpr mode_t privateDirectoryPermissions = S_IRWXU;
    if (mkdir(createdDirectoryPath.c_str(), privateDirectoryPermissions) != 0) {
        const int createError = errno;
        ASCENDLOGE(
            "Failed to create temporary directory: path=%s errno=%d message=%s", createdDirectoryPath.c_str(),
            createError, std::strerror(createError));
        return false;
    }

    if (chmod(createdDirectoryPath.c_str(), privateDirectoryPermissions) != 0) {
        const int permissionError = errno;
        ASCENDLOGE(
            "Failed to make temporary directory private: path=%s errno=%d message=%s", createdDirectoryPath.c_str(),
            permissionError, std::strerror(permissionError));
        (void)FileUtils::RemoveAll(createdDirectoryPath.string());
        return false;
    }

    std::string resolvedCreatedDirectoryPath;
    if (!FileUtils::ResolveDirectory(createdDirectoryPath.string(), resolvedCreatedDirectoryPath) ||
        fs::path(resolvedCreatedDirectoryPath) != createdDirectoryPath.lexically_normal()) {
        ASCENDLOGE("Created temporary directory has an unexpected identity: path=%s", createdDirectoryPath.c_str());
        (void)FileUtils::RemoveAll(createdDirectoryPath.string());
        return false;
    }
    directoryPath_ = std::move(resolvedCreatedDirectoryPath);
    removeDirectoryOnDestruction_ = true;
    ASCENDLOGD("Created temporary directory: path=%s", directoryPath_.c_str());
    return true;
}

bool DirectoryCleanupGuard::RegisterDirectoryForCleanup(std::string directoryPath)
{
    if (directoryPath.empty()) {
        ASCENDLOGE("Cannot register an empty directory path for cleanup");
        return false;
    }
    if (!directoryPath_.empty()) {
        ASCENDLOGE("Directory cleanup guard already manages a path: path=%s", directoryPath_.c_str());
        return false;
    }
    directoryPath_ = std::move(directoryPath);
    removeDirectoryOnDestruction_ = true;
    return true;
}

bool DirectoryCleanupGuard::RemoveDirectory() noexcept
{
    if (directoryPath_.empty()) {
        return true;
    }
    if (!FileUtils::RemoveAll(directoryPath_)) {
        ASCENDLOGW("Failed to remove directory registered for cleanup: path=%s", directoryPath_.c_str());
        return false;
    }
    directoryPath_.clear();
    return true;
}

void DirectoryCleanupGuard::PreserveDirectory() noexcept { removeDirectoryOnDestruction_ = false; }

const std::string& DirectoryCleanupGuard::GetDirectoryPath() const noexcept { return directoryPath_; }

void DirectoryCleanupGuard::MoveStateFrom(DirectoryCleanupGuard& other) noexcept
{
    directoryPath_ = std::move(other.directoryPath_);
    removeDirectoryOnDestruction_ = other.removeDirectoryOnDestruction_;
    other.directoryPath_.clear();
    other.removeDirectoryOnDestruction_ = false;
}

} // namespace ascendc
