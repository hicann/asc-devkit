/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "build_workspace.h"

#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ascendc_tool_log.h"
#include "file_utils.h"

namespace ascendc {
namespace manifest_generator {

bool BuildWorkspace::Create(const std::string& requestedParent, bool keepTemp)
{
    if (!session_.empty()) {
        ASCENDLOGE("Build workspace is already initialized: %s", session_.c_str());
        return false;
    }
    const char* temporary = std::getenv("TMPDIR");
    const std::string requested =
        requestedParent.empty() ? (temporary == nullptr || temporary[0] == '\0' ? "/tmp" : temporary) : requestedParent;
    std::string absolute;
    if (!FileUtils::MakeAbsolutePath(requested, absolute)) {
        ASCENDLOGE("Failed to resolve bundle work directory: %s", requested.c_str());
        return false;
    }
    if (!FileUtils::CreateDirectories(absolute)) {
        ASCENDLOGE("Failed to create bundle work directory: %s", absolute.c_str());
        return false;
    }
    std::string parent;
    if (!FileUtils::ResolveDirectory(absolute, parent)) {
        ASCENDLOGE("Bundle work directory is unavailable: %s", absolute.c_str());
        return false;
    }
    boost::system::error_code error;
    const boost::filesystem::path candidate =
        boost::filesystem::path(parent) / boost::filesystem::unique_path(".manifest-generator-%%%%%%%%", error);
    if (error || !boost::filesystem::create_directory(candidate, error)) {
        ASCENDLOGE(
            "Failed to create private bundle work directory: parent=%s error=%s", parent.c_str(),
            error ? error.message().c_str() : "path already exists");
        return false;
    }
    ASCENDLOGD("Created private bundle work directory %s", candidate.c_str());
    session_ = candidate.string();
    keepTemp_ = keepTemp;
    return true;
}

BuildWorkspace::~BuildWorkspace()
{
    if (!session_.empty() && !keepTemp_) {
        if (FileUtils::RemoveAll(session_)) {
            ASCENDLOGD("Removed private bundle work directory %s", session_.c_str());
        } else {
            ASCENDLOGW("Failed to completely remove private bundle work directory %s", session_.c_str());
        }
    }
}

const std::string& BuildWorkspace::Session() const noexcept { return session_; }

} // namespace manifest_generator
} // namespace ascendc
