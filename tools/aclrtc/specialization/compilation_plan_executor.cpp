/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the LICENSE.
 */

#include "compilation_plan_executor.h"

#include "ascendc_tool_log.h"
#include "kernel_specialization_diagnostics.h"
#include "process_executor.h"

#include <exception>
#include <new>

namespace ascendc {
namespace aclrtc {

CompilationCommandExecutionLimits CompilationCommandExecutionLimits::ProductionDefaults() noexcept
{
    return {std::chrono::minutes(5), std::chrono::minutes(1), std::chrono::seconds(5)};
}

CompilationPlanExecutor::CompilationPlanExecutor(
    KernelSpecializationDiagnostics& specializationDiagnostics, CompilationCommandExecutionLimits executionLimits)
    : specializationDiagnostics_(specializationDiagnostics), commandExecutionLimits_(executionLimits)
{}

aclError CompilationPlanExecutor::ExecuteCompilationPlan(const KernelCompilationPlan& compilationPlan) const
{
    try {
        for (const CompilationCommand& compilationCommand : compilationPlan.compilationCommands) {
            ASCENDLOGI(
                "Executing command %s with %s", compilationCommand.diagnosticLabel.c_str(),
                compilationCommand.executablePath.c_str());
            specializationDiagnostics_.AppendCommandToReplayScript(compilationCommand);
            specializationDiagnostics_.AppendCommandExecutionHeaderToLog(compilationCommand);
            ProcessExecutorResult executorResult;
            const aclError commandExecutionStatus = ExecuteCompilationCommand(compilationCommand, executorResult);
            specializationDiagnostics_.AppendCommandExecutionResultToLog(compilationCommand, executorResult);
            if (commandExecutionStatus != ACLRTC_SUCCESS) {
                ASCENDLOGE(
                    "Command %s failed: outcome=%s code=%d elapsed_ms=%lld", compilationCommand.diagnosticLabel.c_str(),
                    executorResult.GetOutcomeName(), static_cast<int>(executorResult.terminationCode),
                    static_cast<long long>(executorResult.elapsedTime.count()));
                specializationDiagnostics_.LogCommandFailureRecoveryHint();
                return commandExecutionStatus;
            }
        }
        return ACLRTC_SUCCESS;
    } catch (const std::bad_alloc&) {
        ASCENDLOGE("Out of memory while executing kernel compilation commands");
        return ACLRTC_ERROR_OUT_OF_MEMORY;
    } catch (const std::exception& exception) {
        ASCENDLOGE("Kernel compilation command execution failed with exception: %s", exception.what());
        return ACLRTC_ERROR_FAILURE;
    } catch (...) {
        ASCENDLOGE("Kernel compilation command execution failed with an unknown exception");
        return ACLRTC_ERROR_FAILURE;
    }
}

aclError CompilationPlanExecutor::ExecuteCompilationCommand(
    const CompilationCommand& compilationCommand, ProcessExecutorResult& executorResult) const
{
    ProcessExecutorRequest executorRequest;
    executorRequest.arguments.reserve(compilationCommand.commandArguments.size() + 1U);
    executorRequest.arguments.emplace_back(compilationCommand.executablePath.string());
    executorRequest.arguments.insert(
        executorRequest.arguments.end(), compilationCommand.commandArguments.begin(),
        compilationCommand.commandArguments.end());
    executorRequest.mirroredOutputLogFilePath = specializationDiagnostics_.GetCompilationLogFilePath();
    executorRequest.executionTimeout = compilationCommand.commandKind == CompilationCommandKind::Compile ?
                                           commandExecutionLimits_.compileCommandTimeout :
                                           commandExecutionLimits_.objectCopyAndLinkCommandTimeout;
    executorRequest.terminationGracePeriod = commandExecutionLimits_.terminationGracePeriod;
    executorResult = ProcessExecutor::Execute(executorRequest);
    if (executorResult.HasSuccessfulExit()) {
        return ACLRTC_SUCCESS;
    }
    if (executorResult.outcome == ProcessOutcome::ExecutorFailure) {
        return ACLRTC_ERROR_FAILURE;
    }
    return compilationCommand.commandKind == CompilationCommandKind::Link ? ACLRTC_ERROR_LINKING :
                                                                            ACLRTC_ERROR_COMPILATION;
}

} // namespace aclrtc
} // namespace ascendc
