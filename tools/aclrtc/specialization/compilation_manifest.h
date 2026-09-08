/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_COMPILATION_MANIFEST_H
#define ASCENDC_ACLRTC_COMPILATION_MANIFEST_H

#include "kernel_specialization_types.h"
#include "manifest_constant.h"

namespace ascendc {
namespace aclrtc {

enum class ManifestObjectKind { Basic, SuperKernel };

struct ManifestCommand {
    CompilationCommandKind commandKind{CompilationCommandKind::Compile};
    ManifestObjectKind objectKind{ManifestObjectKind::Basic};
    std::optional<uint32_t> parallelStage;
    std::string diagnosticLabel;
    std::string executable;
    std::vector<std::string> arguments;
};

// Owns the selected fields independently of the source JSON.
struct CompilationManifest {
    std::vector<ManifestConstant> constants;
    std::vector<ManifestCommand> commands;
    std::vector<std::string> linkOptions;
    std::vector<std::string> linkInputs;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_COMPILATION_MANIFEST_H
