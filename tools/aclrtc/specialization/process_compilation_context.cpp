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

} // namespace

ProcessCompilationContext& ProcessCompilationContext::Instance()
{
    static ProcessCompilationContext context;
    return context;
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
