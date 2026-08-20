/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_COMPILATION_PLAN_EXECUTOR_H
#define ASCENDC_ACLRTC_COMPILATION_PLAN_EXECUTOR_H

#include "kernel_specialization_types.h"

#include <chrono>
#include <cstddef>

namespace ascendc {
namespace aclrtc {

class KernelSpecializationDiagnostics;

struct CompilationCommandExecutionLimits {
    std::chrono::milliseconds compileCommandTimeout;
    std::chrono::milliseconds objectCopyAndLinkCommandTimeout;
    std::chrono::milliseconds terminationGracePeriod;
    uint64_t capturedOutputByteLimit;

    static CompilationCommandExecutionLimits ProductionDefaults() noexcept;
};

class CompilationPlanExecutor final {
public:
    CompilationPlanExecutor(
        KernelSpecializationDiagnostics& specializationDiagnostics, CompilationCommandExecutionLimits executionLimits);

    aclError ExecuteCompilationPlan(const KernelCompilationPlan& compilationPlan) const;

private:
    aclError ExecuteCompilationCommand(
        const CompilationCommand& compilationCommand, CompilationProcessResult& processResult) const;
    std::chrono::milliseconds GetCommandExecutionTimeout(CompilationCommandKind commandKind) const noexcept;

    KernelSpecializationDiagnostics& specializationDiagnostics_;
    CompilationCommandExecutionLimits executionLimits_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_COMPILATION_PLAN_EXECUTOR_H
