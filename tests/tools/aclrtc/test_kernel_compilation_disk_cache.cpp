/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/kernel_compilation_disk_cache.h"
#include "specialization/cache_digest.h"
#include "specialization/kernel_compilation_cache_input.h"
#include "file_utils.h"
#include "securec.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <elf.h>
#include <fstream>
#include <future>
#include <iterator>
#include <nlohmann/json.hpp>
#include <sys/wait.h>
#include <unistd.h>

namespace {
namespace fs = boost::filesystem;
using namespace ascendc::aclrtc;
using ascendc::FileUtils;
using Json = nlohmann::json;

class KernelCompilationDiskCacheTest : public testing::Test {
protected:
    void SetUp() override
    {
        root_ = fs::temp_directory_path() / fs::unique_path("aclrtc_disk_cache_%%%%-%%%%");
        entry_ = root_ / "artifacts" / key_.substr(0U, 2U) / key_;
        fs::create_directories(entry_);
        Elf64_Ehdr header{};
        ASSERT_EQ(memcpy_s(header.e_ident, sizeof(header.e_ident), ELFMAG, SELFMAG), EOK);
        header.e_ident[EI_CLASS] = ELFCLASS64;
        header.e_ident[EI_DATA] = ELFDATA2LSB;
        header.e_ident[EI_VERSION] = EV_CURRENT;
        header.e_ehsize = sizeof(header);
        bytes_.resize(sizeof(header) + 16U, 0x42U);
        ASSERT_EQ(memcpy_s(bytes_.data(), bytes_.size(), &header, sizeof(header)), EOK);
        metadata_ = {
            {"schema", 1},
            {"key", key_},
            {"base", baseKey_},
            {"size", std::to_string(bytes_.size())},
            {"sha256", ComputeSha256(bytes_.data(), bytes_.size())}};
        input_.key = key_;
        input_.baseKey = baseKey_;
        WriteEntry();
    }

    void TearDown() override
    {
        boost::system::error_code ignored;
        fs::remove_all(root_, ignored);
    }

    void WriteEntry()
    {
        ASSERT_TRUE(FileUtils::WriteTextFileAtomically((entry_ / "entry.json").string(), metadata_.dump()));
        ASSERT_TRUE(FileUtils::WriteBinaryFileAtomically((entry_ / "kernel.elf").string(), bytes_));
    }

    KernelCompilationMemoryCache::KernelElf ReadKernelElf()
    {
        return KernelCompilationDiskCache(root_).ReadKernelElf(key_, baseKey_);
    }

    const std::string key_ = std::string(64U, 'a');
    const std::string baseKey_ = std::string(64U, 'b');
    fs::path root_;
    fs::path entry_;
    std::vector<uint8_t> bytes_;
    Json metadata_;
    KernelCompilationCacheInput input_;
};

TEST_F(KernelCompilationDiskCacheTest, ReadsCompletedArtifactFromIndependentCacheInstances)
{
    const auto first = ReadKernelElf();
    const auto second = ReadKernelElf();
    ASSERT_TRUE(first);
    ASSERT_TRUE(second);
    EXPECT_EQ(*first, bytes_);
    EXPECT_EQ(*second, bytes_);
    EXPECT_NE(first, second);
    std::vector<uint8_t> metadataBytes;
    ASSERT_TRUE(FileUtils::ReadRegularFile((entry_ / "entry.json").string(), 65536U, metadataBytes));
    EXPECT_EQ(std::string(metadataBytes.begin(), metadataBytes.end()), metadata_.dump());
}

TEST_F(KernelCompilationDiskCacheTest, MissingEntriesAndUnavailableDirectoriesAreMisses)
{
    EXPECT_FALSE(KernelCompilationDiskCache({}).ReadKernelElf(key_, baseKey_));
    EXPECT_FALSE(KernelCompilationDiskCache(root_ / "absent").ReadKernelElf(key_, baseKey_));
    EXPECT_FALSE(fs::exists(root_ / "absent"));
    EXPECT_FALSE(KernelCompilationDiskCache(root_).ReadKernelElf(std::string(64U, 'c'), baseKey_));
    EXPECT_FALSE(KernelCompilationDiskCache(entry_ / "entry.json").ReadKernelElf(key_, baseKey_));
    EXPECT_FALSE(fs::exists(root_ / "artifacts/cc"));
}

TEST_F(KernelCompilationDiskCacheTest, RejectsMissingOversizeAndNonRegularEntryFiles)
{
    for (const char* file : {"entry.json", "kernel.elf"}) {
        SCOPED_TRACE(file);
        const fs::path path = entry_ / file;
        ASSERT_TRUE(fs::remove(path));
        EXPECT_FALSE(ReadKernelElf());
        fs::create_directory(path);
        EXPECT_FALSE(ReadKernelElf());
        ASSERT_TRUE(fs::remove(path));
        WriteEntry();
        fs::resize_file(path, std::string(file) == "entry.json" ? 65537U : 512ULL * 1024ULL * 1024ULL + 1U);
        EXPECT_FALSE(ReadKernelElf());
        WriteEntry();
    }
}

TEST_F(KernelCompilationDiskCacheTest, RejectsMalformedMetadataAndMismatchedIdentity)
{
    for (const Json& invalid : {Json(nullptr), Json::array(), Json("text"), Json::object()}) {
        ASSERT_TRUE(FileUtils::WriteTextFileAtomically((entry_ / "entry.json").string(), invalid.dump()));
        EXPECT_FALSE(ReadKernelElf());
    }
    ASSERT_TRUE(FileUtils::WriteTextFileAtomically((entry_ / "entry.json").string(), "{invalid"));
    EXPECT_FALSE(ReadKernelElf());
    const Json changes = {
        {"schema", 2}, {"key", "wrong-key"}, {"base", "wrong-base"}, {"size", 80}, {"sha256", nullptr}};
    for (const auto& change : changes.items()) {
        SCOPED_TRACE(change.key());
        Json invalid = metadata_;
        invalid[change.key()] = change.value();
        ASSERT_TRUE(FileUtils::WriteTextFileAtomically((entry_ / "entry.json").string(), invalid.dump()));
        EXPECT_FALSE(ReadKernelElf());
        invalid.erase(change.key());
        ASSERT_TRUE(FileUtils::WriteTextFileAtomically((entry_ / "entry.json").string(), invalid.dump()));
        EXPECT_FALSE(ReadKernelElf());
    }
    WriteEntry();
    EXPECT_FALSE(KernelCompilationDiskCache(root_).ReadKernelElf(key_, "wrong-base"));
}

TEST_F(KernelCompilationDiskCacheTest, RejectsChangedTruncatedAndAppendedArtifactBytes)
{
    const auto original = bytes_;
    bytes_.back() ^= 1U;
    WriteEntry();
    EXPECT_FALSE(ReadKernelElf());
    bytes_ = original;
    bytes_.pop_back();
    WriteEntry();
    EXPECT_FALSE(ReadKernelElf());
    bytes_ = original;
    bytes_.push_back(1U);
    WriteEntry();
    EXPECT_FALSE(ReadKernelElf());
    bytes_.clear();
    WriteEntry();
    EXPECT_FALSE(ReadKernelElf());
    EXPECT_TRUE(fs::exists(entry_ / "kernel.elf"));
}

TEST_F(KernelCompilationDiskCacheTest, RejectsInvalidElfFormatEvenWithAMatchingDigest)
{
    const auto original = bytes_;
    for (const size_t position : {size_t{0U}, static_cast<size_t>(EI_CLASS), static_cast<size_t>(EI_DATA)}) {
        bytes_ = original;
        bytes_[position] = 0U;
        metadata_["sha256"] = ComputeSha256(bytes_.data(), bytes_.size());
        WriteEntry();
        EXPECT_FALSE(ReadKernelElf());
    }
    bytes_.assign({'\177', 'E', 'L', 'F'});
    metadata_["size"] = std::to_string(bytes_.size());
    metadata_["sha256"] = ComputeSha256(bytes_.data(), bytes_.size());
    WriteEntry();
    EXPECT_FALSE(ReadKernelElf());
}

TEST_F(KernelCompilationDiskCacheTest, PublishesCompleteEntriesAndKeepsExistingValidArtifacts)
{
    for (const char* state : {"missing", "valid", "corrupt", "missing-elf"}) {
        SCOPED_TRACE(state);
        WriteEntry();
        if (std::string(state) == "missing") {
            fs::remove_all(entry_);
        } else if (std::string(state) == "corrupt") {
            ASSERT_TRUE(FileUtils::WriteTextFileAtomically((entry_ / "entry.json").string(), "invalid"));
        } else if (std::string(state) == "missing-elf") {
            ASSERT_TRUE(fs::remove(entry_ / "kernel.elf"));
        }
        auto published = bytes_;
        published.back() ^= 1U;
        ASSERT_TRUE(KernelCompilationDiskCache(root_).PublishKernelElf(input_, published));
        const auto loaded = ReadKernelElf();
        ASSERT_TRUE(loaded);
        EXPECT_EQ(*loaded, std::string(state) == "valid" ? bytes_ : published);
        EXPECT_EQ(std::distance(fs::directory_iterator(entry_), fs::directory_iterator()), 2);
        EXPECT_EQ(std::distance(fs::directory_iterator(entry_.parent_path()), fs::directory_iterator()), 1);
    }
}

TEST_F(KernelCompilationDiskCacheTest, PublicationFailuresLeaveNoPartialEntry)
{
    fs::remove_all(entry_);
    EXPECT_FALSE(KernelCompilationDiskCache({}).PublishKernelElf(input_, bytes_));
    const fs::path unavailable = root_ / "file";
    ASSERT_TRUE(FileUtils::WriteTextFileAtomically(unavailable.string(), "existing"));
    EXPECT_FALSE(KernelCompilationDiskCache(unavailable).PublishKernelElf(input_, bytes_));
    input_.baseKey = std::string(1U, static_cast<char>(0xff));
    EXPECT_FALSE(KernelCompilationDiskCache(root_).PublishKernelElf(input_, bytes_));
    EXPECT_FALSE(fs::exists(entry_));
    EXPECT_TRUE(fs::is_empty(entry_.parent_path()));
}

TEST_F(KernelCompilationDiskCacheTest, DoesNotPublishInvalidElfBytes)
{
    fs::remove_all(entry_);
    EXPECT_FALSE(KernelCompilationDiskCache(root_).PublishKernelElf(input_, {}));
    bytes_[0] = 0U;
    EXPECT_FALSE(KernelCompilationDiskCache(root_).PublishKernelElf(input_, bytes_));
    EXPECT_FALSE(fs::exists(entry_));
    EXPECT_TRUE(fs::is_empty(entry_.parent_path()));
}

TEST_F(KernelCompilationDiskCacheTest, ConcurrentProcessesPublishOneCompleteEntry)
{
    fs::remove_all(entry_);
    std::vector<pid_t> children;
    for (size_t index = 0U; index < 2U; ++index) {
        const pid_t child = fork();
        ASSERT_GE(child, 0);
        if (child == 0) {
            _exit(KernelCompilationDiskCache(root_).PublishKernelElf(input_, bytes_) ? 0 : 1);
        }
        children.push_back(child);
    }
    for (const pid_t child : children) {
        int status = 0;
        ASSERT_EQ(waitpid(child, &status, 0), child);
        ASSERT_TRUE(WIFEXITED(status));
        EXPECT_EQ(WEXITSTATUS(status), 0);
    }
    const auto kernelElf = ReadKernelElf();
    ASSERT_TRUE(kernelElf);
    EXPECT_EQ(*kernelElf, bytes_);
    EXPECT_EQ(std::distance(fs::directory_iterator(entry_.parent_path()), fs::directory_iterator()), 1);
}

TEST_F(KernelCompilationDiskCacheTest, CleansOldEntriesTowardTheConfiguredRemainingCapacity)
{
    const KernelCompilationDiskCache cache(root_);
    ASSERT_TRUE(cache.PublishKernelElf(input_, bytes_));
    const uint64_t firstSize = fs::file_size(entry_ / "kernel.elf") + fs::file_size(entry_ / "entry.json");
    input_.key = std::string(64U, 'c');
    input_.baseKey = std::string(64U, 'd');
    entry_ = root_ / "artifacts" / input_.key.substr(0U, 2U) / input_.key;
    ASSERT_TRUE(cache.PublishKernelElf(input_, bytes_));
    const uint64_t secondSize = fs::file_size(entry_ / "kernel.elf") + fs::file_size(entry_ / "entry.json");

    cache.CleanCacheIfOverLimit(firstSize + secondSize - 1U, 1U);
    size_t remainingEntries = 0U;
    for (const auto& prefix : fs::directory_iterator(root_ / "artifacts")) {
        for (const auto& entry : fs::directory_iterator(prefix)) {
            if (fs::is_directory(entry)) {
                ++remainingEntries;
            }
        }
    }
    EXPECT_EQ(remainingEntries, 1U);
}

TEST_F(KernelCompilationDiskCacheTest, ReadsLegacyEntriesAndCountsTheirInputDescriptionsDuringCleanup)
{
    const uint64_t artifactSize = fs::file_size(entry_ / "kernel.elf") + fs::file_size(entry_ / "entry.json");
    ASSERT_TRUE(FileUtils::WriteTextFileAtomically((entry_ / "aclrtc_cache_inputs.json").string(), "{}"));
    const auto kernelElf = ReadKernelElf();
    ASSERT_TRUE(kernelElf);
    EXPECT_EQ(*kernelElf, bytes_);
    KernelCompilationDiskCache(root_).CleanCacheIfOverLimit(artifactSize, 100U);
    EXPECT_FALSE(fs::exists(entry_));
}

TEST_F(KernelCompilationDiskCacheTest, CountsRemainingBytesOfIncompleteEntries)
{
    const KernelCompilationDiskCache cache(root_);
    const std::vector<std::string> files{"kernel.elf", "entry.json", "aclrtc_cache_inputs.json"};
    for (const auto& selected : files) {
        for (const bool keepOnlySelected : {false, true}) {
            SCOPED_TRACE(selected + (keepOnlySelected ? " only" : " missing"));
            fs::create_directories(entry_);
            WriteEntry();
            ASSERT_TRUE(FileUtils::WriteTextFileAtomically((entry_ / "aclrtc_cache_inputs.json").string(), "{}"));
            uint64_t remainingSize = 0U;
            for (const auto& file : files) {
                if ((file == selected) == keepOnlySelected) {
                    remainingSize += fs::file_size(entry_ / file);
                } else {
                    ASSERT_TRUE(fs::remove(entry_ / file));
                }
            }
            cache.CleanCacheIfOverLimit(remainingSize, 100U);
            ASSERT_TRUE(fs::exists(entry_));
            cache.CleanCacheIfOverLimit(remainingSize - 1U, 100U);
            ASSERT_FALSE(fs::exists(entry_));
        }
    }
}

TEST_F(KernelCompilationDiskCacheTest, CleansOnlyFormalEntriesWithMatchingPrefixes)
{
    const fs::path prefix = entry_.parent_path();
    const std::vector<fs::path> ignored{prefix / "staging-test/nested", prefix / "invalid-test/nested",
                                        prefix / std::string(64U, 'b'), root_ / "artifacts/a" / key_,
                                        root_ / "artifacts/aaa" / key_, root_ / "artifacts/gg" / key_};
    for (const auto& directory : ignored) {
        fs::create_directories(directory);
        std::ofstream(directory / "kernel.elf") << std::string(1024U, 'x');
    }
    KernelCompilationDiskCache(root_).CleanCacheIfOverLimit(1U, 100U);
    EXPECT_FALSE(fs::exists(entry_));
    for (const auto& directory : ignored) {
        EXPECT_TRUE(fs::is_regular_file(directory / "kernel.elf")) << directory;
    }
}

TEST_F(KernelCompilationDiskCacheTest, DoesNotFollowDirectorySymlinksDuringCleanup)
{
    const fs::path outside = root_ / "outside";
    const fs::path marker = entry_ / "unrelated-important-file";
    ASSERT_TRUE(FileUtils::WriteTextFileAtomically(marker.string(), "keep"));
    for (const fs::path& link : {root_ / "artifacts", entry_.parent_path(), entry_}) {
        SCOPED_TRACE(link.string());
        fs::rename(link, outside);
        fs::create_directory_symlink(outside, link);
        KernelCompilationDiskCache(root_).CleanCacheIfOverLimit(1U, 100U);
        ASSERT_TRUE(fs::is_symlink(fs::symlink_status(link)));
        EXPECT_TRUE(fs::exists(entry_ / "kernel.elf"));
        EXPECT_TRUE(fs::exists(marker));
        fs::remove(link);
        fs::rename(outside, link);
    }
}

TEST_F(KernelCompilationDiskCacheTest, DoesNotCreateOrFailWhenCleaningAnUnavailableCache)
{
    const KernelCompilationDiskCache emptyCache(root_ / "absent");
    emptyCache.CleanCacheIfOverLimit(1U, 50U);
    EXPECT_FALSE(fs::exists(root_ / "absent"));
    KernelCompilationDiskCache(root_).CleanCacheIfOverLimit(0U, 50U);
    KernelCompilationDiskCache(root_).CleanCacheIfOverLimit(1U, 0U);
    EXPECT_TRUE(fs::exists(entry_));
}

using KernelElf = KernelCompilationMemoryCache::KernelElf;

KernelElf CreateKernelElf(std::initializer_list<uint8_t> bytes)
{
    return std::make_shared<const std::vector<uint8_t>>(bytes);
}

TEST(KernelCompilationMemoryCacheTest, ReusesImmutableArtifactsAndSeparatesKeys)
{
    KernelCompilationMemoryCache cache;
    const auto first = CreateKernelElf({1U});
    const auto second = CreateKernelElf({2U});
    cache.CacheKernelElf("first", first);
    cache.CacheKernelElf("second", second);
    cache.CacheKernelElf("first", second);
    EXPECT_EQ(cache.GetKernelElf("first"), first);
    EXPECT_EQ(cache.GetKernelElf("second"), second);
    EXPECT_FALSE(cache.GetKernelElf("missing"));
}

TEST(KernelCompilationMemoryCacheTest, ClearsResidencyAtTheBudgetAndKeepsBorrowedArtifactsAlive)
{
    KernelCompilationMemoryCache cache(4U);
    cache.CacheKernelElf("first", CreateKernelElf({1U, 2U, 3U}));
    const auto borrowed = cache.GetKernelElf("first");
    ASSERT_TRUE(borrowed);
    const auto second = CreateKernelElf({4U, 5U});
    cache.CacheKernelElf("second", second);
    EXPECT_FALSE(cache.GetKernelElf("first"));
    EXPECT_EQ(cache.GetKernelElf("second"), second);
    EXPECT_EQ(*borrowed, (std::vector<uint8_t>{1U, 2U, 3U}));
}

TEST(KernelCompilationMemoryCacheTest, SkipsNullAndOversizeArtifactsWithoutEvictingExistingEntries)
{
    KernelCompilationMemoryCache cache(2U);
    const auto small = CreateKernelElf({1U, 2U});
    cache.CacheKernelElf("small", small);
    cache.CacheKernelElf("empty", {});
    cache.CacheKernelElf("large", CreateKernelElf({3U, 4U, 5U}));
    EXPECT_FALSE(cache.GetKernelElf("empty"));
    EXPECT_FALSE(cache.GetKernelElf("large"));
    EXPECT_EQ(cache.GetKernelElf("small"), small);
}

TEST(KernelCompilationMemoryCacheTest, ConcurrentInsertsCountTheSameKeyOnce)
{
    KernelCompilationMemoryCache cache(6U);
    const auto kernelElf = CreateKernelElf({1U, 2U, 3U});
    std::promise<void> start;
    const auto ready = start.get_future().share();
    std::vector<std::future<void>> writers;
    for (size_t index = 0U; index < 8U; ++index) {
        writers.emplace_back(std::async(std::launch::async, [&]() {
            ready.wait();
            cache.CacheKernelElf("same", kernelElf);
        }));
    }
    start.set_value();
    for (auto& writer : writers) {
        writer.get();
    }
    cache.CacheKernelElf("other", kernelElf);
    EXPECT_EQ(cache.GetKernelElf("same"), kernelElf);
    EXPECT_EQ(cache.GetKernelElf("other"), kernelElf);
}

} // namespace
