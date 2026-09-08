/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_COMPILATION_MANIFEST_PARSER_H
#define ASCENDC_ACLRTC_COMPILATION_MANIFEST_PARSER_H

#include "compilation_manifest.h"
#include <nlohmann/json_fwd.hpp>

namespace ascendc {
namespace aclrtc {

class CompilationManifestParser final {
public:
    explicit CompilationManifestParser(const nlohmann::json& manifest) : borrowedManifest_(manifest) {}
    aclError ParseSelected(const std::string& kernelName, bool enableSuperKernel, CompilationManifest& result) const;

private:
    aclError ParseConstants(const nlohmann::json& kernel, std::vector<ManifestConstant>& constants) const;
    aclError ExpandOptionsReference(
        const std::string& argument, const std::string& location, std::vector<std::string>& arguments) const;
    aclError ParseCommand(
        const nlohmann::json& command, const std::string& objectName, ManifestObjectKind objectKind, size_t index,
        ManifestCommand& result) const;
    aclError ParseSelectedObjects(
        const nlohmann::json& kernel, bool enableSuperKernel, CompilationManifest& result) const;

    const nlohmann::json& borrowedManifest_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_COMPILATION_MANIFEST_PARSER_H
