/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/process_compilation_context.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdlib>
#include <optional>
#include <future>
#include <sys/stat.h>

namespace {
namespace fs = boost::filesystem;
using namespace ascendc::aclrtc;

std::string ReadFile(const fs::path& path)
{
    std::ifstream stream(path.string(), std::ios::binary);
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}

class ProcessCompilationContextTest : public testing::Test {
protected:
    void SetUp() override
    {
        for (const char* name :
             {"ASCEND_OP_COMPILE_SAVE_KERNEL_META", "ASCEND_RTC_SPECIALIZE_CACHE_DISABLE",
              "ASCEND_RTC_SPECIALIZE_CACHE_PATH", "ASCEND_RTC_SPECIALIZE_CACHE_SIZE",
              "ASCEND_RTC_SPECIALIZE_CACHE_CLEAN_RATIO", "HOME"}) {
            const char* previous = std::getenv(name);
            previousEnvironment_[name] = previous == nullptr ? std::nullopt : std::optional<std::string>(previous);
            unsetenv(name);
        }
        root_ = fs::temp_directory_path() / fs::unique_path("aclrtc_context_%%%%-%%%%");
        fs::create_directories(root_ / "bin");
        std::ofstream script((root_ / "bin/bisheng").string());
        script << "#!/bin/sh\n"
                  "printf 'compile\\n' >> \"$0.calls\"\n"
                  "test ! -e \"$0.fail\" || exit 2\n"
                  "test ! -e \"$0.no-output\" || exit 0\n"
                  "while [ \"$#\" -gt 0 ]; do\n"
                  "  if [ \"$1\" = '-o' ]; then\n"
                  "    shift\n"
                  "    printf 'object\\000bytes' > \"$1\"\n"
                  "    exit $?\n"
                  "  fi\n"
                  "  shift\n"
                  "done\nexit 3\n";
        script.close();
        ASSERT_EQ(chmod((root_ / "bin/bisheng").c_str(), 0700), 0);
    }

    void TearDown() override
    {
        for (const auto& variable : previousEnvironment_) {
            if (variable.second.has_value()) {
                setenv(variable.first.c_str(), variable.second->c_str(), 1);
            } else {
                unsetenv(variable.first.c_str());
            }
        }
        (void)context_.specializationFlagDirectory_.RemoveDirectory();
        boost::system::error_code ignored;
        fs::remove_all(root_, ignored);
    }

    aclError GetFlagObject(fs::path& path)
    {
        return context_.GetSpecializationFlagObject(root_, {"--npu-arch=dav-2201"}, path);
    }

    // Local instance gives each test the same initial state as a fresh process.
    ProcessCompilationContext context_;
    fs::path root_;
    std::map<std::string, std::optional<std::string>> previousEnvironment_;
};

TEST_F(ProcessCompilationContextTest, CompilesLazilyAndReturnsTheSameObjectPath)
{
    EXPECT_EQ(&ProcessCompilationContext::Instance(), &ProcessCompilationContext::Instance());
    EXPECT_TRUE(context_.specializationFlagDirectory_.GetDirectoryPath().empty());
    EXPECT_FALSE(fs::exists(root_ / "bin/bisheng.calls"));
    fs::path first;
    ASSERT_EQ(GetFlagObject(first), ACLRTC_SUCCESS);
    fs::path second;
    ASSERT_EQ(GetFlagObject(second), ACLRTC_SUCCESS);
    EXPECT_EQ(second, first);
    EXPECT_EQ(first.filename(), "_ascendc_specialization_flag.o");
    EXPECT_EQ(ReadFile(first), std::string("object\0bytes", 12));
    EXPECT_NE(first.parent_path(), root_);
    EXPECT_EQ(ReadFile(root_ / "bin/bisheng.calls"), "compile\n");
    const auto replay = ReadFile(first.parent_path() / "aclrtc_replay.sh");
    EXPECT_NE(replay.find("-DTILING_KEY_VAR=0"), std::string::npos);
}

TEST_F(ProcessCompilationContextTest, KeepsFirstEnvironmentValueIncludingUnsetAndEmpty)
{
    for (const char* initial : std::vector<const char*>{nullptr, "", "first"}) {
        SCOPED_TRACE(initial == nullptr ? "unset" : initial);
        ProcessCompilationContext context;
        constexpr char name[] = "ACLRTC_TEST_PROCESS_CONTEXT_VALUE";
        initial == nullptr ? unsetenv(name) : setenv(name, initial, 1);
        const auto expected = initial == nullptr ? std::nullopt : std::optional<std::string>(initial);
        EXPECT_EQ(context.GetEnvironmentVariable(name), expected);
        for (const char* changed : {"second", ""}) {
            ASSERT_EQ(setenv(name, changed, 1), 0);
            EXPECT_EQ(context.GetEnvironmentVariable(name), expected);
        }
        ASSERT_EQ(unsetenv(name), 0);
        EXPECT_EQ(context.GetEnvironmentVariable(name), expected);
    }
}

TEST_F(ProcessCompilationContextTest, ParsesCacheSwitchOnceAndDefaultsInvalidValuesToEnabled)
{
    for (const char* value : std::vector<const char*>{nullptr, "0", "1", "", "-1", "+1", " 1", "1 ", "2", "abc"}) {
        SCOPED_TRACE(value == nullptr ? "unset" : value);
        value == nullptr ? unsetenv("ASCEND_RTC_SPECIALIZE_CACHE_DISABLE") :
                           setenv("ASCEND_RTC_SPECIALIZE_CACHE_DISABLE", value, 1);
        ProcessCompilationContext context;
        const bool expected = value == nullptr || std::string(value) != "1";
        EXPECT_EQ(context.IsCacheEnabled(), expected);
        ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_DISABLE", expected ? "1" : "0", 1), 0);
        EXPECT_EQ(context.IsCacheEnabled(), expected);
    }
}

TEST_F(ProcessCompilationContextTest, ResolvesAndSavesDiskCacheConfigurationWithoutCreatingDirectories)
{
    ASSERT_EQ(setenv("HOME", root_.c_str(), 1), 0);
    for (const bool explicitPath : {false, true}) {
        SCOPED_TRACE(explicitPath);
        ProcessCompilationContext context;
        const fs::path relative = root_.filename() / "nested/../cache";
        if (explicitPath) {
            ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_PATH", relative.c_str(), 1), 0);
            ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_SIZE", "2", 1), 0);
            ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_CLEAN_RATIO", "25", 1), 0);
        }
        const auto config = context.GetDiskCacheConfig();
        EXPECT_EQ(
            config.directory, explicitPath ? fs::current_path() / root_.filename() / "cache" :
                                             root_ / ".cache/aclrtc/kernel_specialization");
        EXPECT_EQ(config.maximumSizeBytes, explicitPath ? 2097152U : 524288000U);
        EXPECT_EQ(config.cleanupRatio, explicitPath ? 25U : 50U);
        EXPECT_FALSE(fs::exists(config.directory));
        ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_PATH", "/changed", 1), 0);
        ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_SIZE", "10", 1), 0);
        ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_CLEAN_RATIO", "75", 1), 0);
        EXPECT_EQ(context.GetDiskCacheConfig().directory, config.directory);
        EXPECT_EQ(context.GetDiskCacheConfig().maximumSizeBytes, config.maximumSizeBytes);
        EXPECT_EQ(context.GetDiskCacheConfig().cleanupRatio, config.cleanupRatio);
    }
}

TEST_F(ProcessCompilationContextTest, UsesSavedHomeAndAllowsExplicitDirectoryWithoutHome)
{
    EXPECT_TRUE(context_.GetDiskCacheConfig().directory.empty());
    EXPECT_TRUE(context_.IsCacheEnabled());
    ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_PATH", root_.c_str(), 1), 0);
    ProcessCompilationContext explicitContext;
    EXPECT_EQ(explicitContext.GetDiskCacheConfig().directory, root_);
    ASSERT_EQ(setenv("HOME", "", 1), 0);
    ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_PATH", "", 1), 0);
    ProcessCompilationContext emptyContext;
    EXPECT_TRUE(emptyContext.GetDiskCacheConfig().directory.empty());
    ASSERT_EQ(setenv("HOME", root_.c_str(), 1), 0);
    ProcessCompilationContext savedContext;
    EXPECT_EQ(savedContext.GetEnvironmentVariable("HOME"), std::optional<std::string>(root_.string()));
    ASSERT_EQ(setenv("HOME", "/changed-home", 1), 0);
    EXPECT_EQ(savedContext.GetDiskCacheConfig().directory, root_ / ".cache/aclrtc/kernel_specialization");
}

TEST_F(ProcessCompilationContextTest, ValidatesDiskCacheNumbersAndConversionBoundaries)
{
    for (const char* value : {"", "0", "-1", "+1", " 1", "1 ", "abc", "1.5", "500MB", "50%", "18446744073709551616"}) {
        SCOPED_TRACE(value);
        ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_SIZE", value, 1), 0);
        ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_CLEAN_RATIO", value, 1), 0);
        ProcessCompilationContext context;
        EXPECT_EQ(context.GetDiskCacheConfig().maximumSizeBytes, 524288000U);
        EXPECT_EQ(context.GetDiskCacheConfig().cleanupRatio, 50U);
    }
    struct Boundary {
        const char* size;
        const char* ratio;
        uint64_t expectedBytes;
        uint32_t expectedRatio;
    };
    for (const auto& boundary :
         {Boundary{"1", "1", 1048576U, 1U}, Boundary{"17592186044415", "100", 18446744073708503040ULL, 100U},
          Boundary{"17592186044416", "101", 524288000U, 50U}}) {
        SCOPED_TRACE(boundary.size);
        ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_SIZE", boundary.size, 1), 0);
        ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_CLEAN_RATIO", boundary.ratio, 1), 0);
        ProcessCompilationContext context;
        EXPECT_EQ(context.GetDiskCacheConfig().maximumSizeBytes, boundary.expectedBytes);
        EXPECT_EQ(context.GetDiskCacheConfig().cleanupRatio, boundary.expectedRatio);
    }
}

TEST_F(ProcessCompilationContextTest, ConcurrentRequestsCompileOnce)
{
    std::promise<void> start;
    const auto ready = start.get_future().share();
    std::vector<fs::path> paths(8);
    std::vector<std::future<aclError>> results;
    for (size_t index = 0; index < paths.size(); ++index) {
        results.push_back(std::async(std::launch::async, [this, ready, &paths, index]() {
            ready.wait();
            return GetFlagObject(paths[index]);
        }));
    }
    start.set_value();
    for (auto& result : results) {
        EXPECT_EQ(result.get(), ACLRTC_SUCCESS);
    }
    for (const auto& path : paths) {
        EXPECT_EQ(path, paths.front());
    }
    EXPECT_EQ(ReadFile(paths.front()), std::string("object\0bytes", 12));
    EXPECT_EQ(ReadFile(root_ / "bin/bisheng.calls"), "compile\n");
}

TEST_F(ProcessCompilationContextTest, RejectsChangedArchitectureOrToolchainWithoutRecompiling)
{
    fs::path object;
    ASSERT_EQ(GetFlagObject(object), ACLRTC_SUCCESS);
    fs::path rejected = "unchanged";
    EXPECT_EQ(context_.GetSpecializationFlagObject(root_, {"--npu-arch=dav-3510"}, rejected), ACLRTC_ERROR_COMPILATION);
    EXPECT_EQ(
        context_.GetSpecializationFlagObject(root_ / "other", {"--npu-arch=dav-2201"}, rejected),
        ACLRTC_ERROR_COMPILATION);
    EXPECT_EQ(rejected, "unchanged");
    EXPECT_EQ(GetFlagObject(object), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(root_ / "bin/bisheng.calls"), "compile\n");
}

TEST_F(ProcessCompilationContextTest, RetriesAfterCompilerFailureOrMissingOutput)
{
    for (const char* marker : {"bisheng.fail", "bisheng.no-output"}) {
        SCOPED_TRACE(marker);
        ProcessCompilationContext context;
        const fs::path failureMarker = root_ / "bin" / marker;
        std::ofstream(failureMarker.string()).close();
        fs::path object;
        EXPECT_EQ(
            context.GetSpecializationFlagObject(root_, {"--npu-arch=dav-2201"}, object), ACLRTC_ERROR_COMPILATION);
        EXPECT_TRUE(context.specializationFlagDirectory_.GetDirectoryPath().empty());
        EXPECT_TRUE(object.empty());
        fs::remove(failureMarker);
        ASSERT_EQ(context.GetSpecializationFlagObject(root_, {"--npu-arch=dav-2201"}, object), ACLRTC_SUCCESS);
        ASSERT_EQ(context.GetSpecializationFlagObject(root_, {"--npu-arch=dav-2201"}, object), ACLRTC_SUCCESS);
        EXPECT_EQ(ReadFile(root_ / "bin/bisheng.calls"), "compile\ncompile\n");
        fs::remove(root_ / "bin/bisheng.calls");
    }
}

TEST_F(ProcessCompilationContextTest, ObjectLivesUntilOwnerIsDestroyed)
{
    fs::path object;
    {
        ProcessCompilationContext metadata;
        ASSERT_EQ(metadata.GetSpecializationFlagObject(root_, {"--npu-arch=dav-2201"}, object), ACLRTC_SUCCESS);
        EXPECT_TRUE(fs::is_regular_file(object));
    }
    EXPECT_FALSE(fs::exists(object.parent_path()));
}
TEST_F(ProcessCompilationContextTest, RetainsSharedObjectWhenALaterRequestEnablesReplay)
{
    fs::path first;
    fs::path second;
    {
        ProcessCompilationContext metadata;
        ASSERT_EQ(metadata.GetSpecializationFlagObject(root_, {"--npu-arch=dav-2201"}, first), ACLRTC_SUCCESS);
        setenv("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "1", 1);
        ASSERT_EQ(metadata.GetSpecializationFlagObject(root_, {"--npu-arch=dav-2201"}, second), ACLRTC_SUCCESS);
    }
    EXPECT_EQ(first, second);
    EXPECT_TRUE(fs::is_regular_file(first));
    EXPECT_TRUE(fs::is_regular_file(first.parent_path() / "aclrtc_replay.sh"));
    fs::remove_all(first.parent_path());
}
} // namespace
