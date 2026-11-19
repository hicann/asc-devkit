/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_PROCESS_COMPILATION_CONTEXT_H
#define ASCENDC_ACLRTC_PROCESS_COMPILATION_CONTEXT_H

#include "kernel_specialization_types.h"
#include "directory_cleanup_guard.h"

#include <mutex>

namespace ascendc {
namespace aclrtc {

class ProcessCompilationContext final {
public:
    static ProcessCompilationContext& Instance();

    aclError GetSpecializationFlagObject(
        const boost::filesystem::path& ascendHome, const std::vector<std::string>& targetOptions,
        boost::filesystem::path& objectPath);

private:
    ProcessCompilationContext() = default;

    std::mutex specializationFlagMutex_;
    std::vector<std::string> specializationFlagConfiguration_;
    DirectoryCleanupGuard specializationFlagDirectory_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_PROCESS_COMPILATION_CONTEXT_H
