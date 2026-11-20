/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_DIAGNOSTICS_H
#define ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_DIAGNOSTICS_H

#include "kernel_specialization_types.h"

#include <boost/filesystem/path.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace ascendc {

struct ProcessExecutorResult;

namespace aclrtc {

class KernelSpecializationDiagnostics final {
public:
    KernelSpecializationDiagnostics(boost::filesystem::path resourceWorktreePath, std::string specializationSessionId);

    void WriteManifestSnapshot(const nlohmann::json& resourceManifest) noexcept;
    static void WriteCacheInputSnapshot(
        const boost::filesystem::path& resourceWorktreePath, const nlohmann::json& cacheInput) noexcept;
    void AppendCommandToReplayScript(
        const CompilationCommand& compilationCommand,
        const std::map<std::string, std::optional<std::string>>& compilerEnvironment = {}) noexcept;
    void AppendCommandExecutionHeaderToLog(const CompilationCommand& compilationCommand) noexcept;
    std::string GetCompilationLogFilePath() const;
    const std::string& GetSpecializationSessionId() const noexcept { return specializationSessionId_; }
    void AppendCommandExecutionResultToLog(
        const CompilationCommand& compilationCommand, const ProcessExecutorResult& executorResult) noexcept;
    void LogCommandFailureRecoveryHint() const noexcept;
    void WriteSpecializationResult(
        const boost::filesystem::path& outputElfPath, aclError specializationStatus, const char* stage) noexcept;

private:
    std::string specializationSessionId_;
    bool diagnosticsEnabled_{false};
    boost::filesystem::path resourceWorktreePath_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_DIAGNOSTICS_H
