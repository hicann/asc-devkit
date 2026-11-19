/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "process_compilation_context.h"

#include "ascendc_tool_log.h"
#include "compilation_plan_executor.h"
#include "file_utils.h"
#include "kernel_specialization_diagnostics.h"
#include "resource_registry.h"

#include <boost/filesystem.hpp>
#include <charconv>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <utility>

namespace ascendc {
namespace aclrtc {
namespace {
namespace fs = boost::filesystem;
constexpr char SPECIALIZATION_FLAG_SOURCE_FILE_NAME[] = "_ascendc_specialization_flag.cpp";
constexpr char SPECIALIZATION_FLAG_OBJECT_FILE_NAME[] = "_ascendc_specialization_flag.o";
constexpr char SPECIALIZATION_FLAG_SOURCE[] =
    "#include \"basic_api/kernel_tensor.h\"\n"
    "__attribute__((used, section(\".ascend.meta\")))\n"
    "static const BinaryMetaSpecializationFlag g_ascend_specialization_flag = "
    "{{B_TYPE_SPECIALIZATION_FLAG, sizeof(uint32_t)}, 0};\n";

constexpr uint64_t BYTES_PER_MEBIBYTE = 1024U * 1024U;

uint64_t ReadCacheUnsignedInteger(
    ProcessCompilationContext& context, const char* name, uint64_t defaultValue, uint64_t minimum, uint64_t maximum)
{
    const auto value = context.GetEnvironmentVariable(name);
    if (!value.has_value()) {
        return defaultValue;
    }
    uint64_t parsedValue = 0U;
    const auto result = std::from_chars(value->data(), value->data() + value->size(), parsedValue);
    if (result.ec != std::errc() || result.ptr != value->data() + value->size() || parsedValue < minimum ||
        parsedValue > maximum) {
        ASCENDLOGW(
            "Invalid %s: expected an integer in [%llu, %llu]; use default %llu", name,
            static_cast<unsigned long long>(minimum), static_cast<unsigned long long>(maximum),
            static_cast<unsigned long long>(defaultValue));
        return defaultValue;
    }
    return parsedValue;
}

fs::path ReadCacheDirectory(ProcessCompilationContext& context)
{
    const auto configuredPath = context.GetEnvironmentVariable("ASCEND_RTC_SPECIALIZE_CACHE_PATH");
    fs::path directory;
    if (configuredPath.has_value() && !configuredPath->empty()) {
        directory = *configuredPath;
    } else {
        const auto home = context.GetEnvironmentVariable("HOME");
        if (!home.has_value() || home->empty()) {
            ASCENDLOGW("Compilation disk cache directory is unavailable: HOME is unset or empty");
            return {};
        }
        directory = fs::path(*home) / ".cache/aclrtc/kernel_specialization";
    }
    boost::system::error_code error;
    // Resolve relative cache paths once against the working directory at first use.
    directory = fs::absolute(directory, error);
    if (error) {
        ASCENDLOGW("Failed to resolve compilation disk cache directory: %s", error.message().c_str());
        return {};
    }
    return directory.lexically_normal();
}

bool ReadPackageTimestamp(const fs::path& versionFile, std::string& timestamp)
{
    constexpr uintmax_t maximumVersionFileBytes = 64U * 1024U;
    std::vector<uint8_t> bytes;
    if (!FileUtils::ReadRegularFile(versionFile.string(), maximumVersionFileBytes, bytes)) {
        return false;
    }
    std::istringstream input(std::string(bytes.begin(), bytes.end()));
    std::string line;
    bool found = false;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        constexpr char prefix[] = "timestamp=";
        if (line.compare(0U, sizeof(prefix) - 1U, prefix) != 0) {
            continue;
        }
        if (found || line.size() == sizeof(prefix) - 1U) {
            return false;
        }
        timestamp = line.substr(sizeof(prefix) - 1U);
        found = true;
    }
    return found;
}

} // namespace

ProcessCompilationContext& ProcessCompilationContext::Instance()
{
    static ProcessCompilationContext context;
    return context;
}

bool ProcessCompilationContext::GetEnvironmentVariable(
    const std::string& name, const char* location, std::string& value)
{
    const auto savedValue = GetEnvironmentVariable(name);
    if (!savedValue.has_value()) {
        ASCENDLOGE(
            "Environment variable %s referenced at %s was not set on first use; "
            "set it before kernel specialization with: export %s=<value>",
            name.c_str(), location, name.c_str());
        return false;
    }
    value = *savedValue;
    return true;
}

std::optional<std::string> ProcessCompilationContext::GetEnvironmentVariable(const std::string& name)
{
    const std::lock_guard<std::mutex> lock(environmentMutex_);
    auto variable = environmentValues_.find(name);
    if (variable == environmentValues_.end()) {
        // Cache the first observed value, including an unset variable, for the process lifetime.
        const char* environmentValue = std::getenv(name.c_str());
        std::optional<std::string> savedValue;
        if (environmentValue != nullptr) {
            savedValue = environmentValue;
        }
        variable = environmentValues_.emplace(name, std::move(savedValue)).first;
    }
    return variable->second;
}

std::map<std::string, std::optional<std::string>> ProcessCompilationContext::GetCompilerEnvironment()
{
    std::map<std::string, std::optional<std::string>> environment;
    for (const char* name :
         {"PATH", "LD_LIBRARY_PATH", "CPATH", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH", "LIBRARY_PATH", "LANG", "LC_ALL",
          "TZ", "SOURCE_DATE_EPOCH", "HOME", "TMPDIR"}) {
        environment.emplace(name, GetEnvironmentVariable(name));
    }
    return environment;
}

KernelCompilationMemoryCache& ProcessCompilationContext::GetCompilationMemoryCache() { return compilationMemoryCache_; }

bool ProcessCompilationContext::IsCacheEnabled()
{
    const std::lock_guard<std::mutex> lock(cacheConfigMutex_);
    if (!cacheEnabled_.has_value()) {
        const auto disabled = GetEnvironmentVariable("ASCEND_RTC_SPECIALIZE_CACHE_DISABLE");
        if (disabled.has_value() && *disabled != "0" && *disabled != "1") {
            ASCENDLOGW("Invalid ASCEND_RTC_SPECIALIZE_CACHE_DISABLE: expected 0 or 1; use default 0");
        }
        cacheEnabled_ = !disabled.has_value() || *disabled != "1";
        ASCENDLOGI("Kernel compilation cache configuration: enabled=%d", static_cast<int>(*cacheEnabled_));
    }
    return *cacheEnabled_;
}

const KernelCompilationDiskCacheConfig& ProcessCompilationContext::GetDiskCacheConfig()
{
    const std::lock_guard<std::mutex> lock(cacheConfigMutex_);
    if (!diskCacheConfig_.has_value()) {
        KernelCompilationDiskCacheConfig config;
        config.directory = ReadCacheDirectory(*this);
        config.maximumSizeBytes =
            ReadCacheUnsignedInteger(
                *this, "ASCEND_RTC_SPECIALIZE_CACHE_SIZE", config.maximumSizeBytes / BYTES_PER_MEBIBYTE, 1U,
                std::numeric_limits<uint64_t>::max() / BYTES_PER_MEBIBYTE) *
            BYTES_PER_MEBIBYTE;
        config.cleanupRatio = static_cast<uint32_t>(
            ReadCacheUnsignedInteger(*this, "ASCEND_RTC_SPECIALIZE_CACHE_CLEAN_RATIO", config.cleanupRatio, 1U, 100U));
        diskCacheConfig_ = std::move(config);
        ASCENDLOGI(
            "Kernel compilation disk cache configuration: directory=%s maximum_bytes=%llu cleanup_ratio=%u",
            diskCacheConfig_->directory.c_str(), static_cast<unsigned long long>(diskCacheConfig_->maximumSizeBytes),
            diskCacheConfig_->cleanupRatio);
    }
    return *diskCacheConfig_;
}

const std::optional<std::map<std::string, std::string>>& ProcessCompilationContext::GetToolchainTimestamps(
    const fs::path& ascendHome)
{
    const std::lock_guard<std::mutex> lock(toolchainMutex_);
    if (!toolchainTimestampsRead_) {
        // Treat the toolchain as fixed for the process lifetime, including failed timestamp reads.
        toolchainTimestampsRead_ = true;
        std::map<std::string, std::string> timestamps;
        for (const char* package : {"bisheng-compiler", "asc-devkit"}) {
            const fs::path versionFile = ascendHome / "share/info" / package / "version.info";
            std::string timestamp;
            if (!ReadPackageTimestamp(versionFile, timestamp)) {
                ASCENDLOGW(
                    "Compilation cache identity is unavailable: missing or invalid package timestamp: path=%s; "
                    "continue normal compilation",
                    versionFile.c_str());
                return toolchainTimestamps_;
            }
            timestamps.emplace(package, std::move(timestamp));
        }
        toolchainTimestamps_ = std::move(timestamps);
    }
    return toolchainTimestamps_;
}

aclError ProcessCompilationContext::GetSpecializationFlagObject(
    const fs::path& ascendHome, const std::vector<std::string>& targetOptions, fs::path& objectPath)
{
    std::vector<std::string> configuration{ascendHome.string()};
    configuration.insert(configuration.end(), targetOptions.begin(), targetOptions.end());
    const std::lock_guard<std::mutex> lock(specializationFlagMutex_);
    if (!specializationFlagDirectory_.GetDirectoryPath().empty()) {
        if (configuration != specializationFlagConfiguration_) {
            ASCENDLOGE("Specialization metadata compiler configuration differs from the first successful compilation");
            return ACLRTC_ERROR_COMPILATION;
        }
    } else {
        DirectoryCleanupGuard directory;
        const fs::path temporaryDirectory = fs::temp_directory_path();
        if (!directory.CreateUniqueSubdirectory(temporaryDirectory.string(), "aclrtc_specialization_metadata")) {
            ASCENDLOGE("Failed to create specialization metadata directory: parent=%s", temporaryDirectory.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        const fs::path sourcePath = fs::path(directory.GetDirectoryPath()) / SPECIALIZATION_FLAG_SOURCE_FILE_NAME;
        const fs::path compiledObject = fs::path(directory.GetDirectoryPath()) / SPECIALIZATION_FLAG_OBJECT_FILE_NAME;
        if (!FileUtils::WriteTextFileAtomically(sourcePath.string(), SPECIALIZATION_FLAG_SOURCE)) {
            ASCENDLOGE("Failed to write specialization metadata source: path=%s", sourcePath.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        CompilationCommand command{};
        command.commandKind = CompilationCommandKind::Compile;
        command.diagnosticLabel = "specialization_flag/compile";
        command.executablePath = ascendHome / "bin/bisheng";
        const fs::path ascPath = ascendHome / "asc";
        command.arguments = {
            "-c", "-O3", "-xcce", sourcePath.string(), "-I" + ascPath.string(), "-I" + (ascPath / "include").string()};
        command.arguments.insert(command.arguments.end(), targetOptions.begin(), targetOptions.end());
        command.arguments.insert(
            command.arguments.end(),
            {"--cce-aicore-only", "-std=c++17", "-DTILING_KEY_VAR=0", "-o", compiledObject.string()});
        KernelCompilationPlan plan;
        plan.compilerEnvironment = GetCompilerEnvironment();
        plan.compilerEnvironment["ASCEND_HOME_PATH"] = ascendHome.string();
        plan.compilationCommands.emplace_back(std::move(command));
        KernelSpecializationDiagnostics diagnostics(directory.GetDirectoryPath(), "specialization_metadata");
        CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
        const aclError result = executor.ExecuteCompilationPlan(plan);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
        if (!FileUtils::IsRegularFile(compiledObject.string()) || fs::file_size(compiledObject) == 0U) {
            ASCENDLOGE("Specialization metadata compiler did not produce an object: %s", compiledObject.c_str());
            return ACLRTC_ERROR_COMPILATION;
        }
        specializationFlagConfiguration_ = std::move(configuration);
        specializationFlagDirectory_ = std::move(directory);
    }
    if (specialization_compile::IsKernelMetaSavingEnabled()) {
        specializationFlagDirectory_.PreserveDirectory();
    }
    objectPath = fs::path(specializationFlagDirectory_.GetDirectoryPath()) / SPECIALIZATION_FLAG_OBJECT_FILE_NAME;
    return ACLRTC_SUCCESS;
}

} // namespace aclrtc
} // namespace ascendc
