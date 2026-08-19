/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "collected_manifest_repository.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ascendc_tool_log.h"
#include "file_utils.h"
#include "nlohmann/json.hpp"
#include "resource_manifest_validator.h"

using Json = nlohmann::json;

namespace ascendc {
namespace manifest_generator {
namespace {

namespace fs = boost::filesystem;

constexpr uint32_t MAX_MANIFEST_BYTES = 8U * 1024U * 1024U;
constexpr uint32_t MAX_RESOURCE_FILE_BYTES = 1U * 1024U * 1024U;
constexpr uint32_t MAX_BUNDLE_BYTES = 256U * 1024U * 1024U;
constexpr size_t MAX_MANIFEST_COUNT = 4096U;
constexpr size_t MAX_FILES_PER_MANIFEST = 4096U;
constexpr size_t MAX_FILES_PER_BUNDLE = 65536U;
constexpr size_t MAX_DIRECTORY_DEPTH = 64U;

class ResourceBudget final {
public:
    bool AddResource(uint32_t size, const std::string& field)
    {
        if (fileCount_ >= MAX_FILES_PER_BUNDLE) {
            ASCENDLOGE("Bundle exceeds resource file count limit of %zu", MAX_FILES_PER_BUNDLE);
            return false;
        }
        if (!AddPayload(size, field)) {
            return false;
        }
        ++fileCount_;
        return true;
    }

    bool AddPayload(uint32_t size, const std::string& field)
    {
        if (size > MAX_BUNDLE_BYTES - totalBytes_) {
            ASCENDLOGE("Bundle exceeds raw payload limit while adding %s", field.c_str());
            return false;
        }
        totalBytes_ += size;
        return true;
    }

private:
    uint32_t totalBytes_{0U};
    size_t fileCount_{0U};
};

bool ValidateAndExtractResourcePath(std::string& text, const std::string& manifestPath, std::string& resourcePath)
{
    Json manifest = Json::parse(text, nullptr, false);
    if (manifest.is_discarded()) {
        ASCENDLOGE("Manifest is not valid JSON: %s", manifestPath.c_str());
        return false;
    }
    if (manifest.contains("schema_version")) {
        ASCENDLOGE("Manifest must not contain schema_version: %s", manifestPath.c_str());
        return false;
    }

    if (!ValidateResourceManifest(manifest, manifestPath)) {
        return false;
    }

    const Json::const_iterator resourcePathValue = manifest.find("resource_path");
    if (resourcePathValue == manifest.end() || !resourcePathValue->is_string()) {
        ASCENDLOGE("Manifest resource_path is unavailable: %s", manifestPath.c_str());
        return false;
    }
    resourcePath = resourcePathValue->get<std::string>();
    manifest["schema_version"] = "1.0";
    text = manifest.dump();
    return true;
}

bool IsManifestFile(const fs::path& path)
{
    constexpr const char* suffix = "_manifest.json";
    const std::string name = path.filename().string();
    return name.size() > std::strlen(suffix) &&
           name.compare(name.size() - std::strlen(suffix), std::strlen(suffix), suffix) == 0;
}

bool AdvanceIterator(fs::recursive_directory_iterator& iterator, const std::string& root)
{
    boost::system::error_code error;
    iterator.increment(error);
    if (!error) {
        return true;
    }
    ASCENDLOGE("Failed to enumerate %s: %s", root.c_str(), error.message().c_str());
    return false;
}

bool DiscoverManifestFiles(const std::string& manifestSearchRoot, std::vector<std::string>& manifests)
{
    boost::system::error_code error;
    fs::recursive_directory_iterator iterator(fs::path(manifestSearchRoot), fs::directory_options::none, error);
    const fs::recursive_directory_iterator end;
    if (error) {
        ASCENDLOGE("Failed to enumerate %s: %s", manifestSearchRoot.c_str(), error.message().c_str());
        return false;
    }
    while (iterator != end) {
        const fs::path path = iterator->path();
        const fs::file_status status = iterator->symlink_status(error);
        if (error) {
            ASCENDLOGE("Failed to inspect collection entry %s: %s", path.c_str(), error.message().c_str());
            return false;
        }
        if (fs::is_symlink(status)) {
            iterator.disable_recursion_pending();
        } else if (fs::is_directory(status)) {
            if (static_cast<size_t>(iterator.depth()) >= MAX_DIRECTORY_DEPTH) {
                ASCENDLOGE("Collection exceeds directory depth limit of %zu: %s", MAX_DIRECTORY_DEPTH, path.c_str());
                return false;
            }
        } else if (fs::is_regular_file(status) && IsManifestFile(path)) {
            if (manifests.size() >= MAX_MANIFEST_COUNT) {
                ASCENDLOGE("Collection exceeds manifest count limit of %zu", MAX_MANIFEST_COUNT);
                return false;
            }
            manifests.push_back(path.string());
        }
        if (!AdvanceIterator(iterator, manifestSearchRoot)) {
            return false;
        }
    }
    std::sort(manifests.begin(), manifests.end());
    if (manifests.empty()) {
        ASCENDLOGE("Collected resources contain no manifest units");
        return false;
    }
    return true;
}

bool LoadResourceFile(
    const fs::path& source, const std::string& manifestRoot, const std::string& manifestPath, ResourceBudget& budget,
    ManifestUnit& unit)
{
    std::vector<uint8_t> data;
    if (!FileUtils::ReadRegularFile(source.string(), MAX_RESOURCE_FILE_BYTES, data)) {
        ASCENDLOGE("Failed to read permitted manifest resource: %s", source.c_str());
        return false;
    }
    if (data.empty()) {
        ASCENDLOGD("Skipping empty manifest resource %s", source.c_str());
        return true;
    }
    if (unit.files.size() >= MAX_FILES_PER_MANIFEST) {
        ASCENDLOGE("Manifest exceeds resource file count limit: %s", manifestPath.c_str());
        return false;
    }
    if (!budget.AddResource(static_cast<uint32_t>(data.size()), "manifest resource " + source.string())) {
        return false;
    }
    boost::system::error_code error;
    const fs::path relative = fs::relative(source, fs::path(manifestRoot), error);
    if (error || relative.empty()) {
        ASCENDLOGE(
            "Failed to make manifest resource path relative: path=%s root=%s error=%s", source.c_str(),
            manifestRoot.c_str(), error ? error.message().c_str() : "empty relative path");
        return false;
    }
    ResourceFile file;
    file.fileName = source.filename().string();
    file.filePath = relative.generic_string();
    file.data = std::move(data);
    unit.files.push_back(std::move(file));
    return true;
}

bool LoadResourceFiles(
    const std::string& directory, const std::string& manifestRoot, const std::string& manifestPath,
    ResourceBudget& budget, ManifestUnit& unit)
{
    boost::system::error_code error;
    fs::recursive_directory_iterator iterator(fs::path(directory), fs::directory_options::none, error);
    const fs::recursive_directory_iterator end;
    if (error) {
        ASCENDLOGE("Failed to enumerate manifest resource_path %s: %s", directory.c_str(), error.message().c_str());
        return false;
    }
    while (iterator != end) {
        const fs::path source = iterator->path();
        const fs::file_status status = iterator->symlink_status(error);
        if (error) {
            ASCENDLOGE("Failed to inspect manifest resource %s: %s", source.c_str(), error.message().c_str());
            return false;
        }
        if (fs::is_symlink(status)) {
            ASCENDLOGE("Manifest resource_path contains a symlink: %s", source.c_str());
            return false;
        }
        if (fs::is_directory(status)) {
            if (static_cast<size_t>(iterator.depth()) >= MAX_DIRECTORY_DEPTH) {
                ASCENDLOGE(
                    "Manifest resources exceed directory depth limit of %zu: %s", MAX_DIRECTORY_DEPTH, source.c_str());
                return false;
            }
        } else if (!fs::is_regular_file(status)) {
            ASCENDLOGE("Manifest resource_path contains a non-regular file: %s", source.c_str());
            return false;
        } else if (!LoadResourceFile(source, manifestRoot, manifestPath, budget, unit)) {
            return false;
        }
        if (!AdvanceIterator(iterator, directory)) {
            return false;
        }
    }
    return true;
}

bool LoadUnit(const std::string& manifestPath, ResourceBudget& budget, ManifestUnit& unit)
{
    ASCENDLOGD("Loading manifest %s", manifestPath.c_str());
    std::vector<uint8_t> manifestBytes;
    if (!FileUtils::ReadRegularFile(manifestPath, MAX_MANIFEST_BYTES, manifestBytes)) {
        ASCENDLOGE("Failed to read permitted manifest: %s", manifestPath.c_str());
        return false;
    }
    if (!budget.AddPayload(static_cast<uint32_t>(manifestBytes.size()), "manifest " + manifestPath)) {
        return false;
    }
    unit.json.assign(manifestBytes.begin(), manifestBytes.end());
    std::string resourcePath;
    if (!ValidateAndExtractResourcePath(unit.json, manifestPath, resourcePath)) {
        return false;
    }
    if (resourcePath.empty()) {
        ASCENDLOGD("Loaded manifest %s with no resource files", manifestPath.c_str());
        return true;
    }

    const std::string manifestRoot = FileUtils::ParentPath(manifestPath);
    const fs::path resourcePathValue(resourcePath);
    const std::string resourceInput =
        resourcePathValue.is_absolute() ? resourcePath : FileUtils::JoinPath(manifestRoot, resourcePath);
    std::string resourceRoot;
    if (!FileUtils::ResolveSubdirectory(resourceInput, manifestRoot, resourceRoot)) {
        ASCENDLOGE(
            "Manifest resource_path must be a directory below the manifest root and must not be a symlink: %s",
            resourceInput.c_str());
        return false;
    }
    if (!LoadResourceFiles(resourceRoot, manifestRoot, manifestPath, budget, unit)) {
        return false;
    }
    std::sort(unit.files.begin(), unit.files.end(), [](const ResourceFile& left, const ResourceFile& right) {
        return left.filePath < right.filePath;
    });
    ASCENDLOGD("Loaded manifest %s with %zu resource file(s)", manifestPath.c_str(), unit.files.size());
    return true;
}

} // namespace

CollectedManifestRepository::CollectedManifestRepository(std::string manifestSearchRoot) noexcept
    : manifestSearchRoot_(std::move(manifestSearchRoot))
{}

bool CollectedManifestRepository::Load(std::vector<ManifestUnit>& units) const
{
    ASCENDLOGD("Discovering manifests under %s", manifestSearchRoot_.c_str());
    ResourceBudget budget;
    std::vector<std::string> manifests;
    if (!DiscoverManifestFiles(manifestSearchRoot_, manifests)) {
        return false;
    }
    ASCENDLOGI("Discovered %zu manifest(s)", manifests.size());
    units.clear();
    units.reserve(manifests.size());
    for (const std::string& manifest : manifests) {
        ManifestUnit unit;
        if (!LoadUnit(manifest, budget, unit)) {
            return false;
        }
        units.push_back(std::move(unit));
    }
    return true;
}

} // namespace manifest_generator
} // namespace ascendc
