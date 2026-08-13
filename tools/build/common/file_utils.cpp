/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "file_utils.h"

#include <limits>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ascendc_tool_log.h"

namespace ascendc {
namespace {

namespace fs = boost::filesystem;

void LogFilesystemError(const char* operation, const std::string& path, const boost::system::error_code& error)
{
    ASCENDLOGW("Failed to %s: path=%s error=%s", operation, path.c_str(), error.message().c_str());
}

} // namespace

std::string FileUtils::JoinPath(const std::string& left, const std::string& right)
{
    if (left.empty()) {
        return right;
    }
    if (right.empty()) {
        return left;
    }
    return (fs::path(left) / fs::path(right)).string();
}

std::string FileUtils::ParentPath(const std::string& path) { return fs::path(path).parent_path().string(); }

std::string FileUtils::FileName(const std::string& path) { return fs::path(path).filename().string(); }

bool FileUtils::MakeAbsolutePath(const std::string& path, std::string& absolute)
{
    ASCENDLOGD("Making path absolute: path=%s", path.c_str());
    boost::system::error_code error;
    const fs::path result = fs::absolute(fs::path(path), error);
    if (error) {
        LogFilesystemError("make path absolute", path, error);
        absolute.clear();
        return false;
    }
    absolute = result.string();
    ASCENDLOGD("Made path absolute: input=%s absolute=%s", path.c_str(), absolute.c_str());
    return true;
}

bool FileUtils::IsPathWithin(const std::string& path, const std::string& root)
{
    if (path.empty() || root.empty()) {
        return false;
    }
    const fs::path normalizedPath = fs::path(path).lexically_normal();
    const fs::path normalizedRoot = fs::path(root).lexically_normal();
    if (normalizedPath.is_absolute() != normalizedRoot.is_absolute()) {
        return false;
    }
    fs::path::const_iterator pathPart = normalizedPath.begin();
    for (fs::path::const_iterator rootPart = normalizedRoot.begin(); rootPart != normalizedRoot.end(); ++rootPart) {
        if (pathPart == normalizedPath.end() || *pathPart != *rootPart) {
            return false;
        }
        ++pathPart;
    }
    return true;
}

bool FileUtils::PathExists(const std::string& path) noexcept
{
    boost::system::error_code error;
    const bool result = fs::exists(fs::path(path), error);
    return !error && result;
}

bool FileUtils::IsDirectory(const std::string& path) noexcept
{
    boost::system::error_code error;
    const bool result = fs::is_directory(fs::path(path), error);
    return !error && result;
}

bool FileUtils::IsRegularFile(const std::string& path) noexcept
{
    boost::system::error_code error;
    const bool result = fs::is_regular_file(fs::path(path), error);
    return !error && result;
}

bool FileUtils::IsSymlink(const std::string& path) noexcept
{
    boost::system::error_code error;
    const fs::file_status status = fs::symlink_status(fs::path(path), error);
    return !error && fs::is_symlink(status);
}

bool FileUtils::ResolveCanonicalPath(const std::string& path, std::string& resolved)
{
    ASCENDLOGD("Resolving canonical path: path=%s", path.c_str());
    boost::system::error_code error;
    const fs::path canonical = fs::canonical(fs::path(path), error);
    if (error) {
        LogFilesystemError("resolve canonical path", path, error);
        resolved.clear();
        return false;
    }
    resolved = canonical.string();
    ASCENDLOGD("Resolved canonical path: path=%s resolved=%s", path.c_str(), resolved.c_str());
    return true;
}

bool FileUtils::ResolveDirectory(const std::string& path, std::string& resolved)
{
    ASCENDLOGD("Resolving directory: path=%s", path.c_str());
    boost::system::error_code error;
    const fs::file_status inputStatus = fs::symlink_status(fs::path(path), error);
    if (error) {
        LogFilesystemError("inspect directory", path, error);
        resolved.clear();
        return false;
    }
    if (fs::is_symlink(inputStatus)) {
        ASCENDLOGW("Rejected symlink directory: path=%s", path.c_str());
        resolved.clear();
        return false;
    }
    const fs::path canonical = fs::canonical(fs::path(path), error);
    if (error) {
        LogFilesystemError("resolve directory", path, error);
        resolved.clear();
        return false;
    }
    if (!fs::is_directory(canonical, error) || error) {
        if (error) {
            LogFilesystemError("inspect resolved directory", canonical.string(), error);
        } else {
            ASCENDLOGW("Resolved path is not a directory: path=%s resolved=%s", path.c_str(), canonical.c_str());
        }
        resolved.clear();
        return false;
    }
    resolved = canonical.string();
    ASCENDLOGD("Resolved directory: path=%s resolved=%s", path.c_str(), resolved.c_str());
    return true;
}

bool FileUtils::ResolveSubdirectory(const std::string& path, const std::string& root, std::string& resolved)
{
    ASCENDLOGD("Resolving subdirectory: path=%s root=%s", path.c_str(), root.c_str());
    std::string canonical;
    if (!ResolveDirectory(path, canonical)) {
        resolved.clear();
        return false;
    }
    if (!IsPathWithin(canonical, root)) {
        ASCENDLOGW(
            "Resolved directory is outside root: path=%s resolved=%s root=%s", path.c_str(), canonical.c_str(),
            root.c_str());
        resolved.clear();
        return false;
    }
    resolved.swap(canonical);
    ASCENDLOGD("Resolved subdirectory: path=%s resolved=%s", path.c_str(), resolved.c_str());
    return true;
}

bool FileUtils::CreateDirectories(const std::string& path)
{
    ASCENDLOGD("Creating directories: path=%s", path.c_str());
    if (path.empty()) {
        return true;
    }
    boost::system::error_code error;
    fs::create_directories(fs::path(path), error);
    if (error) {
        LogFilesystemError("create directories", path, error);
        return false;
    }
    if (!fs::is_directory(fs::path(path), error) || error) {
        if (error) {
            LogFilesystemError("inspect created directory", path, error);
        } else {
            ASCENDLOGW("Created path is not a directory: path=%s", path.c_str());
        }
        return false;
    }
    ASCENDLOGD("Created or verified directories: path=%s", path.c_str());
    return true;
}

bool FileUtils::RemoveAll(const std::string& path)
{
    ASCENDLOGD("Removing path recursively: path=%s", path.c_str());
    boost::system::error_code error;
    (void)fs::remove_all(fs::path(path), error);
    if (error) {
        LogFilesystemError("remove path recursively", path, error);
        return false;
    }
    ASCENDLOGI("Removed path recursively: path=%s", path.c_str());
    return true;
}

bool FileUtils::ReadRegularFile(const std::string& path, uintmax_t maximum, std::vector<uint8_t>& data)
{
    ASCENDLOGD("Reading regular file: path=%s maximum=%ju", path.c_str(), maximum);
    data.clear();
    boost::system::error_code error;
    const fs::file_status inputStatus = fs::symlink_status(fs::path(path), error);
    if (error || fs::is_symlink(inputStatus) || !fs::is_regular_file(inputStatus)) {
        if (error) {
            LogFilesystemError("inspect regular file", path, error);
        } else {
            ASCENDLOGW("Rejected non-regular file: path=%s", path.c_str());
        }
        return false;
    }
    const uintmax_t fileSize = fs::file_size(fs::path(path), error);
    if (error) {
        LogFilesystemError("read regular file size", path, error);
        return false;
    }
    if (fileSize > maximum || fileSize > static_cast<uintmax_t>(std::numeric_limits<size_t>::max()) ||
        fileSize > static_cast<uintmax_t>(std::numeric_limits<std::streamsize>::max())) {
        ASCENDLOGW("Regular file exceeds size limit: path=%s size=%ju maximum=%ju", path.c_str(), fileSize, maximum);
        return false;
    }
    std::ifstream input(path.c_str(), std::ios::binary);
    if (!input.is_open()) {
        ASCENDLOGW("Failed to open regular file: path=%s", path.c_str());
        return false;
    }
    data.resize(static_cast<size_t>(fileSize));
    if (!data.empty()) {
        input.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    }
    input.close();
    if (!input) {
        ASCENDLOGW("Failed to read or close regular file: path=%s", path.c_str());
        data.clear();
        return false;
    }
    ASCENDLOGD("Read regular file: path=%s bytes=%zu", path.c_str(), data.size());
    return true;
}

bool FileUtils::FinalizeOutput(std::ofstream& output)
{
    ASCENDLOGD("Finalizing output stream");
    output.flush();
    const bool writeSucceeded = static_cast<bool>(output);
    output.close();
    const bool success = writeSucceeded && static_cast<bool>(output);
    if (!success) {
        ASCENDLOGW("Failed to flush or close output stream");
        return false;
    }
    ASCENDLOGD("Finalized output stream");
    return true;
}

bool FileUtils::CopyFile(const std::string& source, const std::string& destination) noexcept
{
    ASCENDLOGD("Copying file: source=%s destination=%s", source.c_str(), destination.c_str());
    boost::system::error_code error;
    if (!fs::copy_file(fs::path(source), fs::path(destination), fs::copy_options::none, error)) {
        if (error) {
            ASCENDLOGW(
                "Failed to copy file: source=%s destination=%s error=%s", source.c_str(), destination.c_str(),
                error.message().c_str());
        } else {
            ASCENDLOGW("Failed to copy file: source=%s destination=%s", source.c_str(), destination.c_str());
        }
        return false;
    }
    ASCENDLOGI("Copied file: source=%s destination=%s", source.c_str(), destination.c_str());
    return true;
}

} // namespace ascendc
