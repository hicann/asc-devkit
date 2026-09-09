/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_COMMON_FILE_UTILS_H
#define ASCENDC_COMMON_FILE_UTILS_H

#include <cstdint>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace ascendc {

class FileUtils final {
public:
    FileUtils() = delete;

    static std::string JoinPath(const std::string& left, const std::string& right);
    static std::string ParentPath(const std::string& path);
    static std::string FileName(const std::string& path);
    static bool MakeAbsolutePath(const std::string& path, std::string& absolute);
    static bool IsSafeRelativePath(const std::string& path);
    static bool IsPathWithin(const std::string& path, const std::string& root);

    static bool PathExists(const std::string& path) noexcept;
    static bool IsDirectory(const std::string& path) noexcept;
    static bool IsRegularFile(const std::string& path) noexcept;
    static bool IsSymlink(const std::string& path) noexcept;
    static bool ResolveCanonicalPath(const std::string& path, std::string& resolved);
    // Read paths may resolve symlinks; callers requiring symlink rejection must
    // check that policy separately (as ReadRegularFile does).
    static bool ResolveRegularFilePathForRead(const std::string& path, std::string& resolved);
    // Streams are borrowed and must be closed on entry; callers own their lifetime.
    static bool OpenRegularFileForRead(const std::string& path, std::ifstream& borrowedInput);
    static bool OpenRegularFileForWrite(
        const std::string& path, std::ofstream& borrowedOutput, std::ios::openmode writeMode);
    static bool WriteTextFile(const std::string& path, const std::string& text, std::ios::openmode writeMode);

    // Resolves an existing regular file or a new filename under an existing canonical parent.
    // Rejects symlinks and special files at the final component; does not create the file.
    static bool ResolveRegularFilePathForWrite(const std::string& path, std::string& resolved);
    static bool ResolveDirectory(const std::string& path, std::string& resolved);
    static bool ResolveSubdirectory(const std::string& path, const std::string& root, std::string& resolved);

    static bool CreateDirectories(const std::string& path);
    static bool RemoveAll(const std::string& path) noexcept;

    static bool ReadRegularFile(const std::string& path, uintmax_t maximum, std::vector<uint8_t>& data);
    static bool FinalizeOutput(std::ofstream& output);
    static bool WriteTextFileAtomically(const std::string& destinationPath, const std::string& text);
    // Runs beforeReplacement after copying and closing both files, before replacing
    // the destination. A false result or exception leaves the destination unchanged;
    // exceptions propagate after temporary-file cleanup. An empty callback is a no-op.
    static bool CopyFileAtomically(
        const std::string& sourcePath, const std::string& destinationPath,
        const std::function<bool()>& beforeReplacement = {});
    static bool CopyFile(const std::string& source, const std::string& destination) noexcept;
};

} // namespace ascendc

#endif // ASCENDC_COMMON_FILE_UTILS_H
