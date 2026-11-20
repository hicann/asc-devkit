/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_compilation_disk_cache.h"

#include "ascendc_tool_log.h"
#include "cache_digest.h"
#include "directory_cleanup_guard.h"
#include "file_utils.h"
#include "kernel_compilation_cache_input.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <elf.h>
#include <nlohmann/json.hpp>
#include <cctype>
#include <ctime>
#include <limits>
#include <utility>

namespace ascendc {
namespace aclrtc {
namespace {
namespace fs = boost::filesystem;
using Json = nlohmann::json;
constexpr size_t CACHE_SHARD_PREFIX_LENGTH = 2U;
constexpr int32_t CACHE_ENTRY_SCHEMA_VERSION = 1;

const char* GetArtifactIdentityMismatch(const Json& metadata, const std::string& key, const std::string& baseKey)
{
    if (!metadata.is_object()) {
        return "invalid_json";
    }
    if (!metadata.contains("schema") || !metadata["schema"].is_number_integer() ||
        metadata["schema"] != CACHE_ENTRY_SCHEMA_VERSION) {
        return "schema_mismatch";
    }
    if (!metadata.contains("key") || metadata["key"] != key) {
        return "key_mismatch";
    }
    if (!metadata.contains("base") || metadata["base"] != baseKey) {
        return "base_mismatch";
    }
    if (!metadata.contains("size") || !metadata["size"].is_string()) {
        return "invalid_size";
    }
    return !metadata.contains("sha256") || !metadata["sha256"].is_string() ? "invalid_digest" : nullptr;
}

bool IsElfArtifact(const std::vector<uint8_t>& bytes)
{
    return bytes.size() >= sizeof(Elf64_Ehdr) && std::equal(ELFMAG, ELFMAG + SELFMAG, bytes.begin()) &&
           bytes[EI_CLASS] == ELFCLASS64 && bytes[EI_DATA] == ELFDATA2LSB;
}

const char* GetArtifactContentMismatch(const Json& metadata, const std::vector<uint8_t>& bytes)
{
    if (!IsElfArtifact(bytes)) {
        return "invalid_elf_header";
    }
    if (metadata["size"] != std::to_string(bytes.size())) {
        return "size_mismatch";
    }
    return metadata["sha256"] != ComputeSha256(bytes.data(), bytes.size()) ? "digest_mismatch" : nullptr;
}

bool IsHexadecimalName(const std::string& name, size_t length)
{
    return name.size() == length && std::all_of(name.begin(), name.end(), [](const char character) {
               return std::isxdigit(static_cast<unsigned char>(character)) != 0;
           });
}

bool GetArtifactEntrySize(const fs::path& entryDirectory, uint64_t& size)
{
    size = 0U;
    // Incomplete entries still consume cache space.
    for (const char* name : {"kernel.elf", "entry.json", "aclrtc_cache_inputs.json"}) {
        boost::system::error_code error;
        const fs::path path = entryDirectory / name;
        const fs::file_status status = fs::status(path, error);
        if (status.type() == fs::file_not_found) {
            continue;
        }
        if (error) {
            ASCENDLOGW(
                "Failed to stat cache file during cleanup: path=%s error=%d message=%s", path.c_str(), error.value(),
                error.message().c_str());
            return false;
        }
        if (!fs::is_regular_file(status)) {
            continue;
        }
        const uintmax_t fileSize = fs::file_size(path, error);
        if (error || fileSize > std::numeric_limits<uint64_t>::max() - size) {
            ASCENDLOGW(
                "Failed to account for cache file during cleanup: path=%s error=%d message=%s overflow=%d",
                path.c_str(), error.value(), error.message().c_str(),
                static_cast<int>(fileSize > std::numeric_limits<uint64_t>::max() - size));
            return false;
        }
        size += static_cast<uint64_t>(fileSize);
    }
    return true;
}

uint64_t GetRemainingCacheSize(uint64_t maximumSizeBytes, uint32_t cleanupRatio)
{
    const uint64_t remainingRatio = PERCENTAGE_BASE - cleanupRatio;
    return (maximumSizeBytes / PERCENTAGE_BASE) * remainingRatio +
           (maximumSizeBytes % PERCENTAGE_BASE) * remainingRatio / PERCENTAGE_BASE;
}

struct CacheEntryInfo {
    fs::path path;
    uint64_t size;
    std::time_t lastWriteTime;
};

bool IsCacheDirectory(const fs::path& path, const char* kind, boost::system::error_code& error)
{
    // Do not follow directory symlinks during cleanup.
    const fs::file_status status = fs::symlink_status(path, error);
    if (error) {
        ASCENDLOGW(
            "Failed to inspect cache %s during cleanup: path=%s error=%d message=%s", kind, path.c_str(), error.value(),
            error.message().c_str());
    }
    return !error && fs::is_directory(status);
}

bool CollectCacheShardEntries(const fs::path& shardDirectory, std::vector<CacheEntryInfo>& entries, uint64_t& totalSize)
{
    const std::string prefixName = shardDirectory.filename().string();
    boost::system::error_code error;
    for (fs::directory_iterator entry(shardDirectory, error); !error && entry != fs::directory_iterator();
         entry.increment(error)) {
        // Staging and invalid-entry directories may still be in use.
        const std::string key = entry->path().filename().string();
        if (!IsHexadecimalName(key, SHA256_HEX_DIGEST_LENGTH) ||
            key.compare(0U, CACHE_SHARD_PREFIX_LENGTH, prefixName) != 0) {
            continue;
        }
        if (!IsCacheDirectory(entry->path(), "entry", error)) {
            continue;
        }
        uint64_t entrySize = 0U;
        if (!GetArtifactEntrySize(entry->path(), entrySize)) {
            continue;
        }
        const std::time_t lastWriteTime = fs::last_write_time(entry->path(), error);
        if (error) {
            ASCENDLOGW(
                "Failed to inspect cache entry timestamp: path=%s error=%d message=%s", entry->path().c_str(),
                error.value(), error.message().c_str());
            continue;
        }
        if (entrySize > std::numeric_limits<uint64_t>::max() - totalSize) {
            ASCENDLOGW("Cache size accounting overflow: path=%s; skip entry", entry->path().c_str());
            continue;
        }
        totalSize += entrySize;
        entries.push_back({entry->path(), entrySize, lastWriteTime});
    }
    if (error) {
        ASCENDLOGW(
            "Failed to scan cache directory for cleanup: path=%s error=%d message=%s", shardDirectory.c_str(),
            error.value(), error.message().c_str());
        return false;
    }
    return true;
}

bool CollectCacheEntries(const fs::path& artifactsDirectory, std::vector<CacheEntryInfo>& entries, uint64_t& totalSize)
{
    boost::system::error_code error;
    const fs::file_status status = fs::symlink_status(artifactsDirectory, error);
    if (!fs::is_directory(status) || error) {
        if (error && error != boost::system::errc::no_such_file_or_directory) {
            ASCENDLOGW(
                "Failed to inspect cache directory for cleanup: path=%s error=%d message=%s",
                artifactsDirectory.c_str(), error.value(), error.message().c_str());
        }
        return false;
    }
    for (fs::directory_iterator prefix(artifactsDirectory, error); !error && prefix != fs::directory_iterator();
         prefix.increment(error)) {
        const std::string prefixName = prefix->path().filename().string();
        if (!IsHexadecimalName(prefixName, CACHE_SHARD_PREFIX_LENGTH)) {
            continue;
        }
        if (!IsCacheDirectory(prefix->path(), "shard", error)) {
            continue;
        }
        if (!CollectCacheShardEntries(prefix->path(), entries, totalSize)) {
            return false;
        }
    }
    if (error) {
        ASCENDLOGW(
            "Failed to scan cache directory for cleanup: path=%s error=%d message=%s", artifactsDirectory.c_str(),
            error.value(), error.message().c_str());
        return false;
    }
    return true;
}

KernelCompilationMemoryCache::KernelElf ReadValidatedCacheEntry(
    const fs::path& entryDirectory, const std::string& key, const std::string& baseKey)
{
    constexpr uint64_t maximumMetadataBytes = 64U * 1024U;
    std::vector<uint8_t> metadataBytes;
    if (!FileUtils::ReadRegularFile((entryDirectory / "entry.json").string(), maximumMetadataBytes, metadataBytes)) {
        ASCENDLOGW(
            "Failed to read cache metadata: key=%s path=%s/entry.json; treat as cache miss", key.c_str(),
            entryDirectory.c_str());
        return {};
    }
    const Json metadata = Json::parse(metadataBytes.begin(), metadataBytes.end(), nullptr, false);
    const char* mismatch = GetArtifactIdentityMismatch(metadata, key, baseKey);
    if (mismatch != nullptr) {
        ASCENDLOGW(
            "Invalid cache metadata: key=%s path=%s/entry.json reason=%s; treat as cache miss", key.c_str(),
            entryDirectory.c_str(), mismatch);
        return {};
    }
    auto kernelElf = std::make_shared<std::vector<uint8_t>>();
    if (!FileUtils::ReadRegularFile(
            (entryDirectory / "kernel.elf").string(), MAXIMUM_CACHED_KERNEL_ELF_BYTES, *kernelElf)) {
        ASCENDLOGW(
            "Failed to read cache ELF: key=%s path=%s/kernel.elf; treat as cache miss", key.c_str(),
            entryDirectory.c_str());
        return {};
    }
    mismatch = GetArtifactContentMismatch(metadata, *kernelElf);
    if (mismatch != nullptr) {
        ASCENDLOGW(
            "Invalid cache ELF: key=%s path=%s/kernel.elf reason=%s; treat as cache miss", key.c_str(),
            entryDirectory.c_str(), mismatch);
        return {};
    }
    return kernelElf;
}

bool WriteStagedCacheEntry(
    const fs::path& stagedDirectory, const KernelCompilationCacheInput& cacheInput,
    const std::vector<uint8_t>& kernelElf)
{
    const Json metadata{
        {"schema", CACHE_ENTRY_SCHEMA_VERSION},
        {"key", cacheInput.key},
        {"base", cacheInput.baseKey},
        {"size", std::to_string(kernelElf.size())},
        {"sha256", ComputeSha256(kernelElf.data(), kernelElf.size())}};
    if (!FileUtils::WriteBinaryFileAtomically((stagedDirectory / "kernel.elf").string(), kernelElf)) {
        ASCENDLOGW(
            "Failed to write cache ELF: key=%s path=%s/kernel.elf; skip cache publication", cacheInput.key.c_str(),
            stagedDirectory.c_str());
        return false;
    }
    if (!FileUtils::WriteTextFileAtomically((stagedDirectory / "entry.json").string(), metadata.dump())) {
        ASCENDLOGW(
            "Failed to write cache metadata: key=%s path=%s/entry.json; skip cache publication", cacheInput.key.c_str(),
            stagedDirectory.c_str());
        return false;
    }
    return true;
}

} // namespace

KernelCompilationDiskCache::KernelCompilationDiskCache(fs::path directory) : directory_(std::move(directory)) {}

KernelCompilationMemoryCache::KernelElf KernelCompilationDiskCache::ReadKernelElf(
    const std::string& key, const std::string& baseKey) const
{
    if (directory_.empty()) {
        ASCENDLOGD("Kernel compilation disk cache bypass: reason=directory_unavailable key=%s", key.c_str());
        return {};
    }
    try {
        const fs::path entryDirectory = directory_ / "artifacts" / key.substr(0U, CACHE_SHARD_PREFIX_LENGTH) / key;
        boost::system::error_code error;
        if (!fs::exists(entryDirectory, error)) {
            if (error) {
                ASCENDLOGW(
                    "Failed to inspect cache entry: key=%s path=%s error=%d message=%s; treat as cache miss",
                    key.c_str(), entryDirectory.c_str(), error.value(), error.message().c_str());
            } else {
                ASCENDLOGD("Kernel compilation disk cache miss: key=%s path=%s", key.c_str(), entryDirectory.c_str());
            }
            return {};
        }
        return ReadValidatedCacheEntry(entryDirectory, key, baseKey);
    } catch (const std::exception& error) {
        ASCENDLOGW(
            "Failed to read kernel compilation disk cache: key=%s directory=%s error=%s; treat as cache miss",
            key.c_str(), directory_.c_str(), error.what());
        return {};
    }
}

bool KernelCompilationDiskCache::PublishKernelElf(
    const KernelCompilationCacheInput& cacheInput, const std::vector<uint8_t>& kernelElf) const
{
    if (directory_.empty()) {
        return false;
    }
    if (kernelElf.size() > MAXIMUM_CACHED_KERNEL_ELF_BYTES || !IsElfArtifact(kernelElf)) {
        ASCENDLOGW("Kernel compilation artifact is not eligible for disk caching: key=%s", cacheInput.key.c_str());
        return false;
    }
    try {
        const fs::path entryDirectory =
            directory_ / "artifacts" / cacheInput.key.substr(0U, CACHE_SHARD_PREFIX_LENGTH) / cacheInput.key;
        if (!FileUtils::CreateDirectories(entryDirectory.parent_path().string())) {
            ASCENDLOGW(
                "Failed to create cache shard directory: key=%s path=%s; skip cache publication",
                cacheInput.key.c_str(), entryDirectory.parent_path().c_str());
            return false;
        }
        // Publish ELF and metadata together by renaming a private sibling directory.
        DirectoryCleanupGuard staging;
        if (!staging.CreateUniqueSubdirectory(entryDirectory.parent_path().string(), "staging")) {
            ASCENDLOGW(
                "Failed to create cache staging directory: key=%s parent=%s; skip cache publication",
                cacheInput.key.c_str(), entryDirectory.parent_path().c_str());
            return false;
        }
        const fs::path stagedDirectory = staging.GetDirectoryPath();
        if (!WriteStagedCacheEntry(stagedDirectory, cacheInput, kernelElf)) {
            return false;
        }
        if (!PublishDirectory(stagedDirectory, entryDirectory, cacheInput)) {
            return false;
        }
        ASCENDLOGI("Kernel compilation disk cache entry is ready: key=%s", cacheInput.key.c_str());
        return true;
    } catch (const std::exception& error) {
        ASCENDLOGW(
            "Failed to publish kernel compilation disk cache: key=%s directory=%s error=%s; use compiled result",
            cacheInput.key.c_str(), directory_.c_str(), error.what());
        return false;
    }
}

void KernelCompilationDiskCache::CleanCacheIfOverLimit(uint64_t maximumSizeBytes, uint32_t cleanupRatio) const
{
    if (directory_.empty() || maximumSizeBytes == 0U || cleanupRatio == 0U) {
        return;
    }
    try {
        const fs::path artifactsDirectory = directory_ / "artifacts";
        std::vector<CacheEntryInfo> entries;
        uint64_t totalSize = 0U;
        if (!CollectCacheEntries(artifactsDirectory, entries, totalSize)) {
            return;
        }
        if (totalSize <= maximumSizeBytes) {
            return;
        }
        // Evict by publication time; cache hits do not refresh the timestamp.
        std::sort(entries.begin(), entries.end(), [](const CacheEntryInfo& left, const CacheEntryInfo& right) {
            return left.lastWriteTime < right.lastWriteTime;
        });
        // The cleanup target is based on the configured limit.
        const uint64_t targetSize = GetRemainingCacheSize(maximumSizeBytes, cleanupRatio);
        const uint64_t initialSize = totalSize;
        uint64_t removedEntries = 0U;
        uint64_t failedEntries = 0U;
        for (const CacheEntryInfo& entry : entries) {
            if (totalSize <= targetSize) {
                break;
            }
            if (!FileUtils::RemoveAll(entry.path.string())) {
                ASCENDLOGW("Failed to remove expired kernel compilation cache entry: path=%s", entry.path.c_str());
                ++failedEntries;
                continue;
            }
            ++removedEntries;
            totalSize -= std::min(totalSize, entry.size);
        }
        ASCENDLOGI(
            "Cleaned kernel compilation disk cache: directory=%s estimated_size=%llu target=%llu "
            "estimated_freed=%llu removed_entries=%llu failed_entries=%llu target_reached=%d",
            directory_.c_str(), static_cast<unsigned long long>(totalSize), static_cast<unsigned long long>(targetSize),
            static_cast<unsigned long long>(initialSize - totalSize), static_cast<unsigned long long>(removedEntries),
            static_cast<unsigned long long>(failedEntries), static_cast<int>(totalSize <= targetSize));
    } catch (const std::exception& error) {
        ASCENDLOGW(
            "Failed to clean kernel compilation disk cache: directory=%s error=%s", directory_.c_str(), error.what());
    }
}

bool KernelCompilationDiskCache::PublishDirectory(
    const fs::path& stagedDirectory, const fs::path& entryDirectory,
    const KernelCompilationCacheInput& cacheInput) const
{
    boost::system::error_code error;
    fs::rename(stagedDirectory, entryDirectory, error);
    if (!error) {
        return true;
    }
    if (error != boost::system::errc::file_exists && error != boost::system::errc::directory_not_empty) {
        ASCENDLOGW(
            "Failed to publish cache directory: key=%s staging=%s target=%s error=%d message=%s; use compiled result",
            cacheInput.key.c_str(), stagedDirectory.c_str(), entryDirectory.c_str(), error.value(),
            error.message().c_str());
        return false;
    }
    if (ReadKernelElf(cacheInput.key, cacheInput.baseKey)) {
        return true;
    }
    DirectoryCleanupGuard obsolete;
    if (!obsolete.CreateUniqueSubdirectory(entryDirectory.parent_path().string(), "invalid")) {
        ASCENDLOGW(
            "Failed to create cache replacement directory: key=%s parent=%s; use compiled result",
            cacheInput.key.c_str(), entryDirectory.parent_path().c_str());
        return false;
    }
    // Move the whole entry aside so concurrent readers never observe file-by-file replacement.
    fs::rename(entryDirectory, obsolete.GetDirectoryPath(), error);
    if (error && error != boost::system::errc::no_such_file_or_directory) {
        ASCENDLOGW(
            "Failed to move invalid cache entry: key=%s source=%s target=%s error=%d message=%s; use compiled result",
            cacheInput.key.c_str(), entryDirectory.c_str(), obsolete.GetDirectoryPath().c_str(), error.value(),
            error.message().c_str());
        return false;
    }
    fs::rename(stagedDirectory, entryDirectory, error);
    if (!error) {
        return true;
    }
    if (ReadKernelElf(cacheInput.key, cacheInput.baseKey)) {
        return true;
    }
    ASCENDLOGW(
        "Failed to replace cache directory: key=%s staging=%s target=%s error=%d message=%s; use compiled result",
        cacheInput.key.c_str(), stagedDirectory.c_str(), entryDirectory.c_str(), error.value(),
        error.message().c_str());
    return false;
}

} // namespace aclrtc
} // namespace ascendc
