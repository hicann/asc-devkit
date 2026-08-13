/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_BUILD_WORKSPACE_H
#define ASCENDC_BUILD_WORKSPACE_H

#include <string>

namespace ascendc {
namespace manifest_generator {

class BuildWorkspace final {
public:
    BuildWorkspace() = default;

    bool Create(const std::string& requestedParent, bool keepTemp);

    BuildWorkspace(const BuildWorkspace&) = delete;
    BuildWorkspace& operator=(const BuildWorkspace&) = delete;
    ~BuildWorkspace();

    const std::string& Session() const noexcept;

private:
    std::string session_;
    bool keepTemp_ = false;
};

} // namespace manifest_generator
} // namespace ascendc

#endif // ASCENDC_BUILD_WORKSPACE_H
