/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_MANIFEST_BUNDLE_COMPILER_H
#define ASCENDC_MANIFEST_BUNDLE_COMPILER_H

#include <cstdint>
#include <string>

#include "manifest_bundle_types.h"

namespace ascendc {
namespace manifest_generator {

struct BuildCollectedBundleRequest {
    std::string manifestSearchRoot;
    std::string outputPath;
    std::string makeExecutable;
    std::string cxxCompiler;
    BundleOutputKind outputKind = BundleOutputKind::kSharedObject;
    bool keepTemp = false;
    uint32_t jobs = 0U;
};

class ManifestBundleCompiler final {
public:
    // The request must contain paths, an output kind, and a job count already validated by the caller.
    explicit ManifestBundleCompiler(BuildCollectedBundleRequest request) noexcept;

    bool Compile() const;

private:
    bool BuildBundle(const std::string& workDir, const std::string& output) const;
    bool PublishBundle(const std::string& staged) const;
    bool PublishBundleAcrossFileSystems(const std::string& staged) const;

    BuildCollectedBundleRequest request_;
};

} // namespace manifest_generator
} // namespace ascendc

#endif // ASCENDC_MANIFEST_BUNDLE_COMPILER_H
