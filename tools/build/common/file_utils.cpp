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
#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ascendc_tool_log.h"

namespace ascendc {
namespace {

namespace fs = boost::filesystem;

// Owns a same-directory temporary file until replacement succeeds. Destruction
// removes any unpublished file, including during exception unwinding.
class AtomicFileWriter final {
public:
    AtomicFileWriter() = default;
    ~AtomicFileWriter() noexcept;
    AtomicFileWriter(const AtomicFileWriter&) = delete;
    AtomicFileWriter& operator=(const AtomicFileWriter&) = delete;

    // Each object writes one temporary file. Success means all I/O is complete and
    // all descriptors are closed; the destination remains untouched until ReplaceDestinationAtomically succeeds.
    bool WriteTextToTemporaryFile(const std::string& destinationPath, const std::string& text);
    bool CopyFileToTemporaryFile(const std::string& sourcePath, const std::string& destinationPath);
    bool ReplaceDestinationAtomically() noexcept;

private:
    int32_t CreateUniqueTemporaryFile(const std::string& destinationPath, mode_t permissions);

    std::string canonicalDestinationPath_;
    std::string ownedTemporaryFilePath_;
    bool ownsTemporaryFile_{false};
    bool temporaryFileWriteSucceeded_{false};
};

constexpr size_t FILE_COPY_BUFFER_BYTES = 64U * 1024U;

class OwnedFileDescriptor final {
public:
    explicit OwnedFileDescriptor(int32_t descriptor) noexcept : descriptor_(descriptor) {}
    ~OwnedFileDescriptor() noexcept { (void)Close(); }
    OwnedFileDescriptor(const OwnedFileDescriptor&) = delete;
    OwnedFileDescriptor& operator=(const OwnedFileDescriptor&) = delete;

    int32_t Get() const noexcept { return descriptor_; }
    bool Close() noexcept
    {
        if (descriptor_ < 0) {
            return true;
        }
        const int32_t descriptor = descriptor_;
        descriptor_ = -1;
        if (close(descriptor) != 0) {
            ASCENDLOGE("Failed to close file descriptor: fd=%d errno=%d", descriptor, errno);
            return false;
        }
        return true;
    }

private:
    int32_t descriptor_;
};

bool WriteAllBytes(int32_t borrowedFileDescriptor, const char* data, size_t byteCount) noexcept
{
    size_t writtenByteCount = 0U;
    while (writtenByteCount < byteCount) {
        const size_t chunkSize = std::min(byteCount - writtenByteCount, FILE_COPY_BUFFER_BYTES);
        const ssize_t writeResult = write(borrowedFileDescriptor, data + writtenByteCount, chunkSize);
        if (writeResult < 0 && errno == EINTR) {
            continue;
        }
        if (writeResult <= 0) {
            ASCENDLOGE(
                "Failed to write temporary file: fd=%d written=%zu requested=%zu errno=%d", borrowedFileDescriptor,
                writtenByteCount, byteCount, writeResult < 0 ? errno : 0);
            return false;
        }
        writtenByteCount += static_cast<size_t>(writeResult);
    }
    return true;
}

bool CopyAllBytes(int32_t borrowedSourceDescriptor, int32_t borrowedDestinationDescriptor) noexcept
{
    char buffer[FILE_COPY_BUFFER_BYTES];
    while (true) {
        const ssize_t readByteCount = read(borrowedSourceDescriptor, buffer, sizeof(buffer));
        if (readByteCount == 0) {
            return true;
        }
        if (readByteCount < 0) {
            if (errno == EINTR) {
                continue;
            }
            ASCENDLOGE("Failed to read copy source: fd=%d errno=%d", borrowedSourceDescriptor, errno);
            return false;
        }
        if (!WriteAllBytes(borrowedDestinationDescriptor, buffer, static_cast<size_t>(readByteCount))) {
            return false;
        }
    }
}

AtomicFileWriter::~AtomicFileWriter() noexcept
{
    if (!ownsTemporaryFile_) {
        return;
    }
    const int32_t unlinkResult = unlink(ownedTemporaryFilePath_.c_str());
    if (unlinkResult != 0 && errno != ENOENT) {
        ASCENDLOGW(
            "Failed to remove unpublished temporary file: path=%s errno=%d", ownedTemporaryFilePath_.c_str(), errno);
    }
}

int32_t AtomicFileWriter::CreateUniqueTemporaryFile(const std::string& destinationPath, mode_t permissions)
{
    if (!canonicalDestinationPath_.empty()) {
        ASCENDLOGE("File replacement has already been prepared");
        return -1;
    }
    if (!FileUtils::ResolveRegularFilePathForWrite(destinationPath, canonicalDestinationPath_)) {
        return -1;
    }
    constexpr uint32_t maxCreationAttempts = 32U;
    static std::atomic<uint64_t> temporaryFileSequence{0};
    for (uint32_t attempt = 0U; attempt < maxCreationAttempts; ++attempt) {
        // Allocate the cleanup path before acquiring the descriptor. O_EXCL
        // rejects collisions without touching existing files or symlinks.
        ownedTemporaryFilePath_ = canonicalDestinationPath_ + ".asc_tmp_" + std::to_string(getpid()) + "_" +
                                  std::to_string(temporaryFileSequence.fetch_add(1U, std::memory_order_relaxed));
        const int32_t descriptor =
            open(ownedTemporaryFilePath_.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, permissions);
        if (descriptor >= 0) {
            ownsTemporaryFile_ = true;
            return descriptor;
        }
        if (errno != EEXIST) {
            break;
        }
    }
    ASCENDLOGE(
        "Failed to create temporary output file: destination=%s temporary=%s errno=%d", destinationPath.c_str(),
        ownedTemporaryFilePath_.c_str(), errno);
    return -1;
}

bool AtomicFileWriter::WriteTextToTemporaryFile(const std::string& destinationPath, const std::string& text)
{
    constexpr mode_t privateTextPermissions = 0600;
    OwnedFileDescriptor output(CreateUniqueTemporaryFile(destinationPath, privateTextPermissions));
    if (output.Get() < 0) {
        return false;
    }
    const bool writeSucceeded = WriteAllBytes(output.Get(), text.data(), text.size());
    const bool closeSucceeded = output.Close();
    temporaryFileWriteSucceeded_ = writeSucceeded && closeSucceeded;
    if (!temporaryFileWriteSucceeded_) {
        ASCENDLOGE(
            "Failed to prepare temporary output file: destination=%s temporary=%s", destinationPath.c_str(),
            ownedTemporaryFilePath_.c_str());
    }
    return temporaryFileWriteSucceeded_;
}

bool AtomicFileWriter::CopyFileToTemporaryFile(const std::string& sourcePath, const std::string& destinationPath)
{
    std::string canonicalSourcePath;
    if (!FileUtils::ResolveRegularFilePathForRead(sourcePath, canonicalSourcePath)) {
        return false;
    }
    OwnedFileDescriptor input(open(canonicalSourcePath.c_str(), O_RDONLY | O_CLOEXEC));
    if (input.Get() < 0) {
        ASCENDLOGE("Failed to open copy source: path=%s errno=%d", sourcePath.c_str(), errno);
        return false;
    }
    // open applies the caller's umask, preserving ordinary output-file permissions.
    constexpr mode_t copiedFilePermissions = 0644;
    OwnedFileDescriptor output(CreateUniqueTemporaryFile(destinationPath, copiedFilePermissions));
    if (output.Get() < 0) {
        return false;
    }
    const bool copySucceeded = CopyAllBytes(input.Get(), output.Get());
    const bool sourceCloseSucceeded = input.Close();
    const bool outputCloseSucceeded = output.Close();
    temporaryFileWriteSucceeded_ = copySucceeded && sourceCloseSucceeded && outputCloseSucceeded;
    if (!temporaryFileWriteSucceeded_) {
        ASCENDLOGE(
            "Failed to prepare file copy: source=%s destination=%s temporary=%s", sourcePath.c_str(),
            destinationPath.c_str(), ownedTemporaryFilePath_.c_str());
    }
    return temporaryFileWriteSucceeded_;
}

bool AtomicFileWriter::ReplaceDestinationAtomically() noexcept
{
    if (!temporaryFileWriteSucceeded_) {
        ASCENDLOGE(
            "Cannot replace destination before temporary file write succeeds: destination=%s",
            canonicalDestinationPath_.c_str());
        return false;
    }
    if (rename(ownedTemporaryFilePath_.c_str(), canonicalDestinationPath_.c_str()) != 0) {
        ASCENDLOGE("Failed to replace destination file: path=%s errno=%d", canonicalDestinationPath_.c_str(), errno);
        return false;
    }
    ownsTemporaryFile_ = false;
    temporaryFileWriteSucceeded_ = false;
    return true;
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
        ASCENDLOGE("Failed to make path absolute: path=%s error=%s", path.c_str(), error.message().c_str());
        absolute.clear();
        return false;
    }
    absolute = result.string();
    ASCENDLOGD("Made path absolute: input=%s absolute=%s", path.c_str(), absolute.c_str());
    return true;
}

bool FileUtils::IsSafeRelativePath(const std::string& path)
{
    if (path.empty() || path.find('\\') != std::string::npos || path.find('\0') != std::string::npos) {
        return false;
    }
    const fs::path filesystemPath(path);
    if (!filesystemPath.is_relative()) {
        return false;
    }
    if (filesystemPath.lexically_normal().generic_string() != path) {
        return false;
    }
    for (const fs::path& component : filesystemPath) {
        if (component == fs::path(".") || component == fs::path("..")) {
            return false;
        }
    }
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
        ASCENDLOGE("Failed to resolve canonical path: path=%s error=%s", path.c_str(), error.message().c_str());
        resolved.clear();
        return false;
    }
    resolved = canonical.string();
    ASCENDLOGD("Resolved canonical path: path=%s resolved=%s", path.c_str(), resolved.c_str());
    return true;
}

bool FileUtils::ResolveRegularFilePathForRead(const std::string& path, std::string& resolved)
{
    resolved.clear();
    if (path.empty() || path.find('\0') != std::string::npos) {
        ASCENDLOGE("Input file path must be nonempty and contain no embedded NUL");
        return false;
    }
    if (!ResolveCanonicalPath(path, resolved)) {
        return false;
    }
    if (!IsRegularFile(resolved)) {
        ASCENDLOGE("Rejected non-regular input file: path=%s", path.c_str());
        resolved.clear();
        return false;
    }
    return true;
}

bool FileUtils::OpenRegularFileForRead(const std::string& path, std::ifstream& borrowedInput)
{
    std::string canonicalPath;
    if (borrowedInput.is_open()) {
        ASCENDLOGE("Cannot open file using an already open stream: path=%s", path.c_str());
        return false;
    }
    if (!ResolveRegularFilePathForRead(path, canonicalPath)) {
        return false;
    }
    borrowedInput.open(canonicalPath, std::ios::in | std::ios::binary);
    if (!borrowedInput) {
        ASCENDLOGE("Failed to open regular input file: path=%s", path.c_str());
        return false;
    }
    return true;
}

bool FileUtils::OpenRegularFileForWrite(
    const std::string& path, std::ofstream& borrowedOutput, std::ios::openmode writeMode)
{
    std::string canonicalPath;
    if (borrowedOutput.is_open()) {
        ASCENDLOGE("Cannot open file using an already open stream: path=%s", path.c_str());
        return false;
    }
    if (!ResolveRegularFilePathForWrite(path, canonicalPath)) {
        return false;
    }
    borrowedOutput.open(canonicalPath, std::ios::out | std::ios::binary | writeMode);
    if (!borrowedOutput) {
        ASCENDLOGE("Failed to open regular output file: path=%s", path.c_str());
        return false;
    }
    return true;
}

bool FileUtils::WriteTextFile(const std::string& path, const std::string& text, std::ios::openmode writeMode)
{
    std::ofstream output;
    if (!OpenRegularFileForWrite(path, output, writeMode)) {
        return false;
    }
    output << text;
    if (!FinalizeOutput(output)) {
        ASCENDLOGE("Failed to finish writing text file: path=%s", path.c_str());
        return false;
    }
    return true;
}

bool FileUtils::ResolveRegularFilePathForWrite(const std::string& path, std::string& resolved)
{
    resolved.clear();
    if (path.empty() || path.find('\0') != std::string::npos) {
        ASCENDLOGE("Output file path must be nonempty and contain no embedded NUL");
        return false;
    }
    const fs::path inputPath(path);
    const fs::path filename = inputPath.filename();
    if (filename.empty() || filename == "." || filename == "..") {
        ASCENDLOGE("Output path must name a file: path=%s", path.c_str());
        return false;
    }
    const std::string parentPath = inputPath.has_parent_path() ? inputPath.parent_path().string() : ".";
    std::string canonicalParentPath;
    if (!ResolveCanonicalPath(parentPath, canonicalParentPath)) {
        return false;
    }
    boost::system::error_code error;
    if (!fs::is_directory(fs::path(canonicalParentPath), error) || error) {
        ASCENDLOGE(
            "Failed to resolve output parent directory: path=%s error=%d message=%s", path.c_str(), error.value(),
            error ? error.message().c_str() : "parent is not a directory");
        return false;
    }
    const fs::path candidate = fs::path(canonicalParentPath) / filename;
    const fs::file_status status = fs::symlink_status(candidate, error);
    if (status.type() != fs::file_not_found && (error || !fs::is_regular_file(status))) {
        ASCENDLOGE(
            "Rejected output file: path=%s error=%d message=%s", path.c_str(), error.value(),
            error ? error.message().c_str() : "output is not a regular file");
        return false;
    }
    resolved = candidate.string();
    return true;
}

bool FileUtils::ResolveDirectory(const std::string& path, std::string& resolved)
{
    ASCENDLOGD("Resolving directory: path=%s", path.c_str());
    boost::system::error_code error;
    const fs::file_status inputStatus = fs::symlink_status(fs::path(path), error);
    if (error) {
        ASCENDLOGE("Failed to inspect directory: path=%s error=%s", path.c_str(), error.message().c_str());
        resolved.clear();
        return false;
    }
    if (fs::is_symlink(inputStatus)) {
        ASCENDLOGE("Rejected symlink directory: path=%s", path.c_str());
        resolved.clear();
        return false;
    }
    std::string canonicalPath;
    if (!ResolveCanonicalPath(path, canonicalPath)) {
        resolved.clear();
        return false;
    }
    if (!fs::is_directory(fs::path(canonicalPath), error) || error) {
        if (error) {
            ASCENDLOGE(
                "Failed to inspect resolved directory: path=%s error=%s", canonicalPath.c_str(),
                error.message().c_str());
        } else {
            ASCENDLOGE("Resolved path is not a directory: path=%s resolved=%s", path.c_str(), canonicalPath.c_str());
        }
        resolved.clear();
        return false;
    }
    resolved = canonicalPath;
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
        ASCENDLOGE(
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
        ASCENDLOGE("Failed to create directories: path=%s error=%s", path.c_str(), error.message().c_str());
        return false;
    }
    if (!fs::is_directory(fs::path(path), error) || error) {
        if (error) {
            ASCENDLOGE("Failed to inspect created directory: path=%s error=%s", path.c_str(), error.message().c_str());
        } else {
            ASCENDLOGE("Created path is not a directory: path=%s", path.c_str());
        }
        return false;
    }
    ASCENDLOGD("Created or verified directories: path=%s", path.c_str());
    return true;
}

bool FileUtils::RemoveAll(const std::string& path) noexcept
{
    ASCENDLOGD("Removing path recursively: path=%s", path.c_str());
    boost::system::error_code error;
    (void)fs::remove_all(fs::path(path), error);
    if (error) {
        ASCENDLOGE("Failed to remove path recursively: path=%s error=%d", path.c_str(), error.value());
        return false;
    }
    ASCENDLOGI("Removed path recursively: path=%s", path.c_str());
    return true;
}

bool FileUtils::ReadRegularFile(const std::string& path, uintmax_t maximum, std::vector<uint8_t>& data)
{
    ASCENDLOGD("Reading regular file: path=%s maximum=%ju", path.c_str(), maximum);
    data.clear();
    std::string absolutePath;
    if (!MakeAbsolutePath(path, absolutePath)) {
        return false;
    }
    const fs::path normalizedPath = fs::path(absolutePath).lexically_normal();
    boost::system::error_code error;
    const fs::file_status inputStatus = fs::symlink_status(normalizedPath, error);
    if (error || fs::is_symlink(inputStatus) || !fs::is_regular_file(inputStatus)) {
        if (error) {
            ASCENDLOGE("Failed to inspect regular file: path=%s error=%s", path.c_str(), error.message().c_str());
        } else {
            ASCENDLOGE("Rejected non-regular file: path=%s", path.c_str());
        }
        return false;
    }
    std::string canonicalPath;
    if (!ResolveRegularFilePathForRead(normalizedPath.string(), canonicalPath)) {
        return false;
    }
    const uintmax_t fileSize = fs::file_size(fs::path(canonicalPath), error);
    if (error) {
        ASCENDLOGE("Failed to read regular file size: path=%s error=%s", path.c_str(), error.message().c_str());
        return false;
    }
    if (fileSize > maximum || fileSize > static_cast<uintmax_t>(std::numeric_limits<size_t>::max()) ||
        fileSize > static_cast<uintmax_t>(std::numeric_limits<std::streamsize>::max())) {
        ASCENDLOGE("Regular file exceeds size limit: path=%s size=%ju maximum=%ju", path.c_str(), fileSize, maximum);
        return false;
    }
    std::ifstream input(canonicalPath.c_str(), std::ios::binary);
    if (!input.is_open()) {
        ASCENDLOGE("Failed to open regular file: path=%s", path.c_str());
        return false;
    }
    data.resize(static_cast<size_t>(fileSize));
    if (!data.empty()) {
        input.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    }
    input.close();
    if (!input) {
        ASCENDLOGE("Failed to read or close regular file: path=%s", path.c_str());
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
        ASCENDLOGE("Failed to flush or close output stream");
        return false;
    }
    ASCENDLOGD("Finalized output stream");
    return true;
}

bool FileUtils::WriteTextFileAtomically(const std::string& destinationPath, const std::string& text)
{
    AtomicFileWriter writer;
    return writer.WriteTextToTemporaryFile(destinationPath, text) && writer.ReplaceDestinationAtomically();
}

bool FileUtils::CopyFileAtomically(
    const std::string& sourcePath, const std::string& destinationPath, const std::function<bool()>& beforeReplacement)
{
    AtomicFileWriter writer;
    if (!writer.CopyFileToTemporaryFile(sourcePath, destinationPath)) {
        return false;
    }
    if (beforeReplacement && !beforeReplacement()) {
        ASCENDLOGE(
            "File replacement cancelled by prerequisite failure: source=%s destination=%s", sourcePath.c_str(),
            destinationPath.c_str());
        return false;
    }
    return writer.ReplaceDestinationAtomically();
}

bool FileUtils::CopyFile(const std::string& source, const std::string& destination) noexcept
{
    ASCENDLOGD("Copying file: source=%s destination=%s", source.c_str(), destination.c_str());
    boost::system::error_code error;
    if (!fs::copy_file(fs::path(source), fs::path(destination), fs::copy_options::none, error)) {
        if (error) {
            ASCENDLOGE(
                "Failed to copy file: source=%s destination=%s error=%s", source.c_str(), destination.c_str(),
                error.message().c_str());
        } else {
            ASCENDLOGE("Failed to copy file: source=%s destination=%s", source.c_str(), destination.c_str());
        }
        return false;
    }
    ASCENDLOGI("Copied file: source=%s destination=%s", source.c_str(), destination.c_str());
    return true;
}

} // namespace ascendc
