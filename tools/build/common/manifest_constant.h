/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_COMMON_MANIFEST_CONSTANT_H
#define ASCENDC_COMMON_MANIFEST_CONSTANT_H

#include <cstdint>
#include <string>
#include <nlohmann/json_fwd.hpp>

namespace ascendc {

class ManifestConstant final {
public:
    bool ParseDefinition(const nlohmann::json& constantJson);
    bool BindArgument(uint64_t argsCount, const void* const* argsAddr, const uint64_t* argsBytes);

    const std::string& GetName() const { return name_; }
    uint64_t GetParameterIndex() const { return parameterIndex_; }
    const std::string& GetTargetFile() const { return targetFile_; }
    const std::string& GetTemplateText() const { return templateText_; }
    const void* GetBoundData() const { return borrowedData_; }
    uint64_t GetBoundByteSize() const { return boundByteSize_; }

private:
    bool definitionParsed_{false};
    std::string name_;
    uint64_t parameterIndex_{0U};
    // Zero means the byte size must be obtained from the bound argument.
    uint64_t manifestByteSize_{0U};
    std::string targetFile_;
    std::string templateText_;
    const void* borrowedData_{nullptr};
    uint64_t boundByteSize_{0U};
};

} // namespace ascendc

#endif // ASCENDC_COMMON_MANIFEST_CONSTANT_H
