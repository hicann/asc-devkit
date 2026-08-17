/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "resource_registry.h"

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <fstream>
#include <set>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ascendc_manifest_abi.h"
#include "ascendc_tool_log.h"
#include "file_utils.h"
#include "nlohmann/json.hpp"

namespace ascendc {
namespace specialization_compile {

void LibraryDeleter::operator()(void* handle) const noexcept
{
    if (handle != nullptr && dlclose(handle) != 0) {
        ASCENDLOGW("Failed to close compile resource shared object: handle=%p reason=dlclose returned nonzero", handle);
    }
}

namespace {

namespace fs = boost::filesystem;
using Json = nlohmann::json;

constexpr char RELATIVE_JIT_ROOT[] = "op_impl/ai_core/tbe/kernel/jit";
constexpr char LIBRARY_NAME_SUFFIX[] = "_compile_database.so";
constexpr uint64_t MAX_MANIFEST_SIZE = 8U * 1024U * 1024U;
constexpr uint64_t MAX_PATH_SIZE = 4096U;
constexpr uint64_t MAX_RESOURCE_FILE_SIZE = 256U * 1024U * 1024U;
constexpr uint64_t MAX_MANIFEST_RESOURCE_SIZE = 512U * 1024U * 1024U;
constexpr uint64_t MAX_REGISTRY_RESOURCE_SIZE = 1024U * 1024U * 1024U;
constexpr uint64_t MAX_MANIFEST_COUNT = 4096U;
constexpr uint64_t MAX_EXTENSION_COUNT = 4096U;
constexpr uint64_t MAX_FILE_COUNT = 65536U;
constexpr uint64_t MAX_REGISTRY_FILE_COUNT = 131072U;

const char* SourceTypeName(ResourceSourceType sourceType) noexcept
{
    switch (sourceType) {
        case ResourceSourceType::External:
            return "external";
        case ResourceSourceType::Custom:
            return "custom";
        case ResourceSourceType::BuiltIn:
            return "built-in";
    }
    return "unknown";
}

const char* ResourceStatusName(ResourceStatus status) noexcept
{
    switch (status) {
        case ResourceStatus::Success:
            return "success";
        case ResourceStatus::NotFound:
            return "not_found";
        case ResourceStatus::Conflict:
            return "conflict";
        case ResourceStatus::InvalidResource:
            return "invalid_resource";
        case ResourceStatus::IoError:
            return "io_error";
        case ResourceStatus::LoadError:
            return "load_error";
        case ResourceStatus::InternalError:
            return "internal_error";
    }
    return "unknown";
}

void CleanupPath(const std::string& path) noexcept
{
    if (path.empty()) {
        return;
    }
    if (!FileUtils::RemoveAll(path)) {
        ASCENDLOGW("Failed to remove compile resource path: path=%s reason=recursive removal failed", path.c_str());
    }
}

bool ShouldKeepTemporaryRoot() noexcept
{
    const char* environment = std::getenv("ASCEND_OP_COMPILE_SAVE_KERNEL_META");
    if (environment == nullptr) {
        return false;
    }
    auto isWhitespace = [](char value) {
        return value == ' ' || value == '\t' || value == '\n' || value == '\r' || value == '\f' || value == '\v';
    };
    const char* begin = environment;
    while (*begin != '\0' && isWhitespace(*begin)) {
        ++begin;
    }
    const char* end = begin;
    while (*end != '\0') {
        ++end;
    }
    while (end != begin && isWhitespace(*(end - 1))) {
        --end;
    }
    return end == begin + 1 && *begin == '1';
}

ResourceStatus CreateTemporaryRoot(std::string& temporaryRoot)
{
    const char* environment = std::getenv("TMPDIR");
    const fs::path configured = environment == nullptr || *environment == '\0' ? "/tmp" : environment;
    boost::system::error_code error;
    const fs::path parent = fs::canonical(configured, error);
    if (error) {
        ASCENDLOGE(
            "Failed to resolve compile resource temporary parent: configured=%s error=%s", configured.c_str(),
            error.message().c_str());
        return ResourceStatus::IoError;
    }
    if (!fs::is_directory(parent, error)) {
        ASCENDLOGE(
            "Compile resource temporary parent is not a directory: configured=%s resolved=%s error=%s",
            configured.c_str(), parent.c_str(), error ? error.message().c_str() : "none");
        return ResourceStatus::IoError;
    }
    const std::string pattern = (parent / "aclrtc-resource-XXXXXX").string();
    std::vector<char> writable(pattern.begin(), pattern.end());
    writable.push_back('\0');
    char* created = mkdtemp(writable.data());
    if (created == nullptr) {
        ASCENDLOGE(
            "Failed to create compile resource temporary root: parent=%s error=%s", parent.c_str(),
            std::strerror(errno));
        return ResourceStatus::IoError;
    }
    const fs::path canonical = fs::canonical(created, error);
    if (error) {
        ASCENDLOGE(
            "Failed to resolve created compile resource temporary root: path=%s error=%s", created,
            error.message().c_str());
        CleanupPath(created);
        return ResourceStatus::IoError;
    }
    if (canonical != fs::path(created).lexically_normal()) {
        ASCENDLOGE(
            "Compile resource temporary root identity mismatch: expected=%s actual=%s", created, canonical.c_str());
        CleanupPath(created);
        return ResourceStatus::IoError;
    }
    temporaryRoot = canonical.string();
    ASCENDLOGI("Created compile resource temporary root: path=%s", temporaryRoot.c_str());
    return ResourceStatus::Success;
}

bool TryAddWithinLimit(uint64_t value, uint64_t limit, uint64_t& total) noexcept
{
    if (total > limit || value > limit - total) {
        return false;
    }
    total += value;
    return true;
}

ResourceStatus CopyString(const AcString& value, const char* field, uint64_t limit, std::string& output)
{
    if (value.size != 0U && value.data == nullptr) {
        ASCENDLOGE(
            "Compile resource string has null data: field=%s size=%" PRIu64 " expected=non-null data", field,
            value.size);
        return ResourceStatus::InvalidResource;
    }
    if (value.size > limit) {
        ASCENDLOGE(
            "Compile resource string exceeds size limit: field=%s actual=%" PRIu64 " limit=%" PRIu64, field, value.size,
            limit);
        return ResourceStatus::InvalidResource;
    }
    output.assign(value.size == 0U ? "" : value.data, value.size);
    return ResourceStatus::Success;
}

bool IsPlainName(const std::string& value)
{
    return !value.empty() && FileUtils::IsSafeRelativePath(value) && FileUtils::FileName(value) == value;
}

std::string Trim(const std::string& value)
{
    const size_t begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return {};
    }
    return value.substr(begin, value.find_last_not_of(" \t\r\n") - begin + 1U);
}

std::vector<std::string> SplitList(const char* value, char delimiter)
{
    std::vector<std::string> result;
    const std::string text(value == nullptr ? "" : value);
    size_t begin = 0U;
    while (begin <= text.size()) {
        const size_t end = text.find(delimiter, begin);
        const std::string item = Trim(text.substr(begin, end - begin));
        if (!item.empty() && std::find(result.begin(), result.end(), item) == result.end()) {
            result.push_back(item);
        }
        if (end == std::string::npos) {
            break;
        }
        begin = end + 1U;
    }
    return result;
}

ResourceStatus ValidateBundleHeader(const AcCompileResourceBundleHeader* header, const LibrarySpec& spec)
{
    if (header == nullptr) {
        ASCENDLOGE(
            "Compile resource bundle entry point returned null: source_type=%s so=%s", SourceTypeName(spec.sourceType),
            spec.path.c_str());
        return ResourceStatus::InvalidResource;
    }
    if (header->magic != AC_COMPILE_RESOURCE_MAGIC) {
        ASCENDLOGE(
            "Compile resource bundle magic mismatch: source_type=%s so=%s expected=0x%08x actual=0x%08x",
            SourceTypeName(spec.sourceType), spec.path.c_str(), AC_COMPILE_RESOURCE_MAGIC, header->magic);
        return ResourceStatus::InvalidResource;
    }
    if (header->abiVersion != AC_COMPILE_RESOURCE_ABI_VERSION) {
        ASCENDLOGE(
            "Compile resource bundle ABI version mismatch: source_type=%s so=%s expected=%u actual=%u",
            SourceTypeName(spec.sourceType), spec.path.c_str(), AC_COMPILE_RESOURCE_ABI_VERSION, header->abiVersion);
        return ResourceStatus::InvalidResource;
    }
    if (header->structSize != sizeof(AcCompileResourceBundle)) {
        ASCENDLOGE(
            "Compile resource bundle size mismatch: source_type=%s so=%s expected=%zu actual=%u",
            SourceTypeName(spec.sourceType), spec.path.c_str(), sizeof(AcCompileResourceBundle), header->structSize);
        return ResourceStatus::InvalidResource;
    }
    if (header->flags != 0U) {
        ASCENDLOGE(
            "Compile resource bundle has unsupported flags: source_type=%s so=%s expected=0 actual=0x%08x",
            SourceTypeName(spec.sourceType), spec.path.c_str(), header->flags);
        return ResourceStatus::InvalidResource;
    }
    return ResourceStatus::Success;
}

ResourceStatus ValidateManifestTable(const AcCompileResourceBundle& bundle, const LibrarySpec& spec)
{
    if (bundle.manifestCount == 0U) {
        ASCENDLOGE(
            "Compile resource bundle has no manifests: source_type=%s so=%s expected_count=1..%" PRIu64,
            SourceTypeName(spec.sourceType), spec.path.c_str(), MAX_MANIFEST_COUNT);
        return ResourceStatus::InvalidResource;
    }
    if (bundle.manifestCount > MAX_MANIFEST_COUNT) {
        ASCENDLOGE(
            "Compile resource manifest count exceeds limit: source_type=%s so=%s actual=%" PRIu64 " limit=%" PRIu64,
            SourceTypeName(spec.sourceType), spec.path.c_str(), bundle.manifestCount, MAX_MANIFEST_COUNT);
        return ResourceStatus::InvalidResource;
    }
    if (bundle.manifests == nullptr) {
        ASCENDLOGE(
            "Compile resource manifest table is null: source_type=%s so=%s count=%" PRIu64,
            SourceTypeName(spec.sourceType), spec.path.c_str(), bundle.manifestCount);
        return ResourceStatus::InvalidResource;
    }
    return ResourceStatus::Success;
}

ResourceStatus ValidateExtensionTable(const AcCompileResourceBundle& bundle, const LibrarySpec& spec)
{
    if (bundle.extensionCount == 0U) {
        if (bundle.extensions != nullptr) {
            ASCENDLOGE(
                "Compile resource extension table is inconsistent: source_type=%s so=%s count=0 pointer=%p "
                "expected=null",
                SourceTypeName(spec.sourceType), spec.path.c_str(), static_cast<const void*>(bundle.extensions));
            return ResourceStatus::InvalidResource;
        }
        return ResourceStatus::Success;
    }
    if (bundle.extensionCount > MAX_EXTENSION_COUNT) {
        ASCENDLOGE(
            "Compile resource extension count exceeds limit: source_type=%s so=%s actual=%" PRIu64 " limit=%" PRIu64,
            SourceTypeName(spec.sourceType), spec.path.c_str(), bundle.extensionCount, MAX_EXTENSION_COUNT);
        return ResourceStatus::InvalidResource;
    }
    if (bundle.extensions == nullptr) {
        ASCENDLOGE(
            "Compile resource extension table is null: source_type=%s so=%s count=%" PRIu64,
            SourceTypeName(spec.sourceType), spec.path.c_str(), bundle.extensionCount);
        return ResourceStatus::InvalidResource;
    }
    for (uint64_t index = 0U; index < bundle.extensionCount; ++index) {
        const AcCompileResourceExtension& extension = bundle.extensions[index];
        const uint64_t unsupportedFlags = extension.flags & ~AC_COMPILE_RESOURCE_EXTENSION_REQUIRED;
        if (unsupportedFlags != 0U) {
            ASCENDLOGE(
                "Compile resource extension has unsupported flags: source_type=%s so=%s extension=%" PRIu64
                " type=%u version=%u flags=0x%" PRIx64 " supported_mask=0x%" PRIx64,
                SourceTypeName(spec.sourceType), spec.path.c_str(), index, extension.type, extension.version,
                extension.flags, static_cast<uint64_t>(AC_COMPILE_RESOURCE_EXTENSION_REQUIRED));
            return ResourceStatus::InvalidResource;
        }
        if (extension.dataSize != 0U && extension.data == nullptr) {
            ASCENDLOGE(
                "Compile resource extension has null data: source_type=%s so=%s extension=%" PRIu64
                " type=%u version=%u size=%" PRIu64 " expected=non-null data",
                SourceTypeName(spec.sourceType), spec.path.c_str(), index, extension.type, extension.version,
                extension.dataSize);
            return ResourceStatus::InvalidResource;
        }
        if ((extension.flags & AC_COMPILE_RESOURCE_EXTENSION_REQUIRED) != 0U) {
            ASCENDLOGE(
                "Compile resource bundle requires an unsupported extension: source_type=%s so=%s extension=%" PRIu64
                " type=%u version=%u",
                SourceTypeName(spec.sourceType), spec.path.c_str(), index, extension.type, extension.version);
            return ResourceStatus::InvalidResource;
        }
    }
    return ResourceStatus::Success;
}

struct ManifestOwnership {
    Json document;
    std::string resourceId;
    std::string sourceFile;
    bool hasSourceFile = false;
};

ResourceStatus ParseResourceId(
    const Json& document, const LibrarySpec& spec, uint64_t manifestIndex, std::string& resourceId)
{
    if (!document.contains("resource_id")) {
        ASCENDLOGE(
            "Compile resource manifest is missing required field: source_type=%s so=%s manifest=%" PRIu64
            " field=resource_id",
            SourceTypeName(spec.sourceType), spec.path.c_str(), manifestIndex);
        return ResourceStatus::InvalidResource;
    }
    if (!document.at("resource_id").is_string()) {
        ASCENDLOGE(
            "Compile resource manifest field has invalid type: source_type=%s so=%s manifest=%" PRIu64
            " field=resource_id expected=string actual=%s",
            SourceTypeName(spec.sourceType), spec.path.c_str(), manifestIndex, document.at("resource_id").type_name());
        return ResourceStatus::InvalidResource;
    }
    resourceId = document.at("resource_id").get<std::string>();
    return ResourceStatus::Success;
}

ResourceStatus ParseSourceFile(
    const Json& document, const LibrarySpec& spec, uint64_t manifestIndex, ManifestOwnership& ownership)
{
    ownership.hasSourceFile = document.contains("source_file");
    if (!ownership.hasSourceFile) {
        return ResourceStatus::Success;
    }
    if (!document.at("source_file").is_string()) {
        ASCENDLOGE(
            "Compile resource manifest field has invalid type: resource_id=%s source_type=%s so=%s manifest=%" PRIu64
            " field=source_file expected=string actual=%s",
            ownership.resourceId.c_str(), SourceTypeName(spec.sourceType), spec.path.c_str(), manifestIndex,
            document.at("source_file").type_name());
        return ResourceStatus::InvalidResource;
    }
    ownership.sourceFile = document.at("source_file").get<std::string>();
    if (!IsPlainName(ownership.sourceFile)) {
        ASCENDLOGE(
            "Compile resource manifest has unsafe source file name: resource_id=%s source_type=%s so=%s "
            "manifest=%" PRIu64 " source_file=%s expected=plain file name",
            ownership.resourceId.c_str(), SourceTypeName(spec.sourceType), spec.path.c_str(), manifestIndex,
            ownership.sourceFile.c_str());
        return ResourceStatus::InvalidResource;
    }
    return ResourceStatus::Success;
}

ResourceStatus ParseManifestOwnership(
    const AcCompileResourceManifest& unit, const LibrarySpec& spec, uint64_t manifestIndex,
    ManifestOwnership& ownership)
{
    std::string manifestText;
    ResourceStatus status = CopyString(unit.json, "manifest.json", MAX_MANIFEST_SIZE, manifestText);
    if (status != ResourceStatus::Success) {
        return status;
    }
    ownership.document = Json::parse(manifestText, nullptr, false);
    if (ownership.document.is_discarded()) {
        ASCENDLOGE(
            "Failed to parse compile resource manifest JSON: source_type=%s so=%s manifest=%" PRIu64
            " reason=malformed JSON",
            SourceTypeName(spec.sourceType), spec.path.c_str(), manifestIndex);
        return ResourceStatus::InvalidResource;
    }
    if (!ownership.document.is_object()) {
        ASCENDLOGE(
            "Compile resource manifest root has invalid type: source_type=%s so=%s manifest=%" PRIu64
            " expected=object actual=%s",
            SourceTypeName(spec.sourceType), spec.path.c_str(), manifestIndex, ownership.document.type_name());
        return ResourceStatus::InvalidResource;
    }
    status = ParseResourceId(ownership.document, spec, manifestIndex, ownership.resourceId);
    if (status != ResourceStatus::Success) {
        return status;
    }
    return ParseSourceFile(ownership.document, spec, manifestIndex, ownership);
}

ResourceStatus PrepareMaterializationRoot(
    const std::string& temporaryRoot, const std::string& resourceId, ResourceSourceType sourceType,
    std::string& categoryRoot, std::string& canonicalRoot)
{
    boost::system::error_code error;
    const fs::path randomComponent = fs::unique_path("materialize-%%%%%%%%%%%%%%%%", error);
    if (error || randomComponent.empty() || randomComponent.has_parent_path()) {
        ASCENDLOGE(
            "Failed to generate compile resource materialization path: resource_id=%s source_type=%s error=%s",
            resourceId.c_str(), SourceTypeName(sourceType), error ? error.message().c_str() : "invalid random path");
        return ResourceStatus::IoError;
    }
    const std::string categoryParent = FileUtils::JoinPath(temporaryRoot, SourceTypeName(sourceType));
    categoryRoot = FileUtils::JoinPath(categoryParent, randomComponent.string());
    if (!FileUtils::CreateDirectories(categoryRoot)) {
        ASCENDLOGE(
            "Failed to create compile resource materialization directory: resource_id=%s source_type=%s path=%s",
            resourceId.c_str(), SourceTypeName(sourceType), categoryRoot.c_str());
        return ResourceStatus::IoError;
    }
    const fs::path canonical = fs::canonical(categoryRoot, error);
    if (error) {
        ASCENDLOGE(
            "Failed to resolve compile resource materialization directory: resource_id=%s source_type=%s path=%s "
            "error=%s",
            resourceId.c_str(), SourceTypeName(sourceType), categoryRoot.c_str(), error.message().c_str());
        return ResourceStatus::IoError;
    }
    if (canonical != fs::path(categoryRoot).lexically_normal()) {
        ASCENDLOGE(
            "Compile resource materialization directory identity mismatch: resource_id=%s source_type=%s expected=%s "
            "actual=%s",
            resourceId.c_str(), SourceTypeName(sourceType), categoryRoot.c_str(), canonical.c_str());
        return ResourceStatus::IoError;
    }
    canonicalRoot = canonical.string();
    return ResourceStatus::Success;
}

ResourceStatus ResolveExplicitDiscoveryPath(const char* path, bool& isFile, std::string& canonical)
{
    isFile = FileUtils::IsRegularFile(path);
    if (isFile) {
        if (FileUtils::IsSymlink(path)) {
            ASCENDLOGE("Explicit compile resource SO must not be a symbolic link: path=%s expected=regular file", path);
            return ResourceStatus::InvalidResource;
        }
        if (!FileUtils::ResolveCanonicalPath(path, canonical)) {
            ASCENDLOGE("Failed to resolve explicit compile resource SO: path=%s", path);
            return ResourceStatus::InvalidResource;
        }
        return ResourceStatus::Success;
    }
    if (FileUtils::IsSymlink(path)) {
        ASCENDLOGE("Explicit compile resource directory must not be a symbolic link: path=%s expected=directory", path);
        return ResourceStatus::InvalidResource;
    }
    if (!FileUtils::IsDirectory(path)) {
        ASCENDLOGE("Explicit compile resource path is neither a regular file nor a directory: path=%s", path);
        return ResourceStatus::InvalidResource;
    }
    return ResourceStatus::Success;
}

ResourceStatus ValidateResourceFilePath(
    const std::string& resourceId, const std::string& fileName, const std::string& relativePath,
    std::set<std::string>& paths)
{
    if (!IsPlainName(fileName)) {
        ASCENDLOGE(
            "Compile resource file name is unsafe: resource_id=%s file_name=%s expected=plain file name",
            resourceId.c_str(), fileName.c_str());
        return ResourceStatus::InvalidResource;
    }
    if (!FileUtils::IsSafeRelativePath(relativePath)) {
        ASCENDLOGE(
            "Compile resource file path is unsafe: resource_id=%s path=%s expected=relative path without traversal",
            resourceId.c_str(), relativePath.c_str());
        return ResourceStatus::InvalidResource;
    }
    const std::string pathFileName = FileUtils::FileName(relativePath);
    if (pathFileName != fileName) {
        ASCENDLOGE(
            "Compile resource file name does not match its path: resource_id=%s file_name=%s path=%s "
            "path_file_name=%s",
            resourceId.c_str(), fileName.c_str(), relativePath.c_str(), pathFileName.c_str());
        return ResourceStatus::InvalidResource;
    }
    if (!paths.insert(relativePath).second) {
        ASCENDLOGE(
            "Compile resource file path is duplicated in manifest: resource_id=%s path=%s", resourceId.c_str(),
            relativePath.c_str());
        return ResourceStatus::InvalidResource;
    }
    return ResourceStatus::Success;
}

ResourceStatus ValidateResourceFilePayload(
    const AcCompileResourceFile& file, const std::string& resourceId, const std::string& relativePath,
    uint64_t& manifestBytes, StagedResources& staged)
{
    if (file.size > MAX_RESOURCE_FILE_SIZE) {
        ASCENDLOGE(
            "Compile resource file exceeds size limit: resource_id=%s path=%s actual=%" PRIu64 " limit=%" PRIu64,
            resourceId.c_str(), relativePath.c_str(), file.size, MAX_RESOURCE_FILE_SIZE);
        return ResourceStatus::InvalidResource;
    }
    if (file.size != 0U && file.data == nullptr) {
        ASCENDLOGE(
            "Compile resource file has null payload: resource_id=%s path=%s size=%" PRIu64 " expected=non-null data",
            resourceId.c_str(), relativePath.c_str(), file.size);
        return ResourceStatus::InvalidResource;
    }
    if (!TryAddWithinLimit(file.size, MAX_MANIFEST_RESOURCE_SIZE, manifestBytes)) {
        ASCENDLOGE(
            "Compile resource manifest payload exceeds size limit: resource_id=%s path=%s current=%" PRIu64
            " incoming=%" PRIu64 " limit=%" PRIu64,
            resourceId.c_str(), relativePath.c_str(), manifestBytes, file.size, MAX_MANIFEST_RESOURCE_SIZE);
        return ResourceStatus::InvalidResource;
    }
    if (!TryAddWithinLimit(file.size, MAX_REGISTRY_RESOURCE_SIZE, staged.bytes)) {
        ASCENDLOGE(
            "Staged compile resource payload exceeds registry size limit: resource_id=%s path=%s current=%" PRIu64
            " incoming=%" PRIu64 " limit=%" PRIu64,
            resourceId.c_str(), relativePath.c_str(), staged.bytes, file.size, MAX_REGISTRY_RESOURCE_SIZE);
        return ResourceStatus::InvalidResource;
    }
    return ResourceStatus::Success;
}

ResourceStatus ValidateManifestFileTable(
    const AcCompileResourceManifest& unit, const std::string& resourceId, const StagedResources& staged)
{
    if (unit.fileCount > MAX_FILE_COUNT) {
        ASCENDLOGE(
            "Compile resource manifest file count exceeds limit: resource_id=%s actual=%" PRIu64 " limit=%" PRIu64,
            resourceId.c_str(), unit.fileCount, MAX_FILE_COUNT);
        return ResourceStatus::InvalidResource;
    }
    if (unit.fileCount == 0U && unit.files != nullptr) {
        ASCENDLOGE(
            "Compile resource manifest has inconsistent file table: resource_id=%s count=0 pointer=%p "
            "expected_pointer=null",
            resourceId.c_str(), static_cast<const void*>(unit.files));
        return ResourceStatus::InvalidResource;
    }
    if (unit.fileCount != 0U && unit.files == nullptr) {
        ASCENDLOGE(
            "Compile resource manifest has inconsistent file table: resource_id=%s count=%" PRIu64
            " pointer=null expected_pointer=non-null",
            resourceId.c_str(), unit.fileCount);
        return ResourceStatus::InvalidResource;
    }
    if (staged.files > MAX_REGISTRY_FILE_COUNT || unit.fileCount > MAX_REGISTRY_FILE_COUNT - staged.files) {
        ASCENDLOGE(
            "Staged compile resource file count exceeds registry limit: resource_id=%s staged=%" PRIu64
            " incoming=%" PRIu64 " limit=%" PRIu64,
            resourceId.c_str(), staged.files, unit.fileCount, MAX_REGISTRY_FILE_COUNT);
        return ResourceStatus::InvalidResource;
    }
    return ResourceStatus::Success;
}

bool CheckRegistryLimits(const StagedResources& staged, ResourceSourceType sourceType, uint64_t& bytes, uint64_t& files)
{
    if (!staged.discovered || staged.conflict) {
        return true;
    }
    const uint64_t currentBytes = bytes;
    if (!TryAddWithinLimit(staged.bytes, MAX_REGISTRY_RESOURCE_SIZE, bytes)) {
        ASCENDLOGE(
            "Compile resource registry payload limit exceeded: source_type=%s current=%" PRIu64 " incoming=%" PRIu64
            " limit=%" PRIu64,
            SourceTypeName(sourceType), currentBytes, staged.bytes, MAX_REGISTRY_RESOURCE_SIZE);
        return false;
    }
    const uint64_t currentFiles = files;
    if (!TryAddWithinLimit(staged.files, MAX_REGISTRY_FILE_COUNT, files)) {
        ASCENDLOGE(
            "Compile resource registry file count limit exceeded: source_type=%s current=%" PRIu64 " incoming=%" PRIu64
            " limit=%" PRIu64,
            SourceTypeName(sourceType), currentFiles, staged.files, MAX_REGISTRY_FILE_COUNT);
        return false;
    }
    return true;
}

} // namespace

AutomaticRoots ResourceRegistry::AutomaticSearchRoots()
{
    AutomaticRoots roots;
    for (const std::string& path : SplitList(std::getenv("ASCEND_CUSTOM_OPP_PATH"), ':')) {
        roots.custom.push_back(FileUtils::JoinPath(path, RELATIVE_JIT_ROOT));
    }
    const char* environment = std::getenv("ASCEND_OPP_PATH");
    if (environment == nullptr || *environment == '\0') {
        ASCENDLOGD(
            "Automatic compile resource roots collected without built-in OPP path: custom_roots=%zu "
            "reason=ASCEND_OPP_PATH is unset",
            roots.custom.size());
        return roots;
    }
    const std::string oppRoot(environment);
    const std::string vendorConfig = FileUtils::JoinPath(oppRoot, "vendors/config.ini");
    std::ifstream input(vendorConfig);
    if (!input.is_open()) {
        ASCENDLOGW(
            "Unable to read optional compile resource vendor configuration: path=%s reason=open failed; "
            "built-in discovery will continue",
            vendorConfig.c_str());
    }
    std::string line;
    while (std::getline(input, line)) {
        const size_t separator = line.find('=');
        if (separator == std::string::npos || Trim(line.substr(0U, separator)) != "load_priority") {
            continue;
        }
        for (const std::string& vendor : SplitList(line.substr(separator + 1U).c_str(), ',')) {
            if (IsPlainName(vendor)) {
                roots.custom.push_back(
                    FileUtils::JoinPath(FileUtils::JoinPath(oppRoot, "vendors/" + vendor), RELATIVE_JIT_ROOT));
            } else {
                ASCENDLOGW(
                    "Ignoring unsafe compile resource vendor name: path=%s vendor=%s expected=plain directory name",
                    vendorConfig.c_str(), vendor.c_str());
            }
        }
        break;
    }
    roots.builtIn.push_back(FileUtils::JoinPath(FileUtils::JoinPath(oppRoot, "built-in"), RELATIVE_JIT_ROOT));
    ASCENDLOGD(
        "Automatic compile resource roots collected: custom_roots=%zu built_in_roots=%zu opp_root=%s",
        roots.custom.size(), roots.builtIn.size(), oppRoot.c_str());
    return roots;
}

bool ResourceRegistry::IsLibraryName(const std::string& path)
{
    const std::string name = FileUtils::FileName(path);
    return name.size() > std::strlen("lib") + std::strlen(LIBRARY_NAME_SUFFIX) && name.rfind("lib", 0U) == 0U &&
           name.compare(
               name.size() - std::strlen(LIBRARY_NAME_SUFFIX), std::strlen(LIBRARY_NAME_SUFFIX), LIBRARY_NAME_SUFFIX) ==
               0;
}

void ResourceRegistry::AddLibrary(const std::string& path, std::set<std::string>& found)
{
    if (!IsLibraryName(path) || !FileUtils::IsRegularFile(path) || FileUtils::IsSymlink(path)) {
        return;
    }
    std::string canonical;
    if (!FileUtils::ResolveCanonicalPath(path, canonical)) {
        ASCENDLOGW("Ignoring compile resource SO with unresolved path: path=%s", path.c_str());
        return;
    }
    found.insert(canonical);
}

bool ResourceRegistry::CollectLibraries(
    const std::string& root, ResourceSourceType sourceType, bool recursive, std::set<std::string>& found)
{
    const size_t initialCount = found.size();
    ASCENDLOGD(
        "Scanning compile resource directory: source_type=%s root=%s recursive=%s", SourceTypeName(sourceType),
        root.c_str(), recursive ? "true" : "false");
    std::vector<std::string> pending = {root};
    boost::system::error_code error;
    while (!pending.empty()) {
        const std::string currentRoot = std::move(pending.back());
        pending.pop_back();
        fs::directory_iterator current(currentRoot, fs::directory_options::none, error);
        const fs::directory_iterator end;
        if (error) {
            ASCENDLOGE(
                "Failed to scan compile resource directory: source_type=%s path=%s error=%s",
                SourceTypeName(sourceType), currentRoot.c_str(), error.message().c_str());
            return false;
        }
        while (current != end) {
            const std::string path = current->path().string();
            AddLibrary(path, found);
            const fs::file_status status = recursive ? current->symlink_status(error) : fs::file_status();
            if (error) {
                ASCENDLOGE(
                    "Failed to inspect compile resource path: source_type=%s path=%s error=%s",
                    SourceTypeName(sourceType), path.c_str(), error.message().c_str());
                return false;
            }
            if (recursive && fs::is_directory(status)) {
                pending.push_back(path);
            }
            current.increment(error);
            if (error) {
                ASCENDLOGE(
                    "Failed to scan compile resource directory: source_type=%s path=%s error=%s",
                    SourceTypeName(sourceType), currentRoot.c_str(), error.message().c_str());
                return false;
            }
        }
    }
    ASCENDLOGD(
        "Finished scanning compile resource directory: source_type=%s root=%s libraries_added=%zu",
        SourceTypeName(sourceType), root.c_str(), found.size() - initialCount);
    return true;
}

bool ResourceRegistry::CollectAutomaticLibraries(
    const std::vector<std::string>& roots, ResourceSourceType sourceType, std::set<std::string>& found)
{
    for (const std::string& root : roots) {
        if (!FileUtils::IsDirectory(root)) {
            ASCENDLOGD(
                "Skipping missing automatic compile resource root: source_type=%s path=%s", SourceTypeName(sourceType),
                root.c_str());
            continue;
        }
        if (!CollectLibraries(root, sourceType, true, found)) {
            return false;
        }
    }
    return true;
}

ResourceStatus ResourceRegistry::DiscoverLibraries(const char* directory, std::vector<LibrarySpec>& libraries)
{
    const bool automatic = directory == nullptr || *directory == '\0';
    ASCENDLOGI(
        "Discovering compile resource libraries: mode=%s path=%s", automatic ? "automatic" : "explicit",
        automatic ? "<environment>" : directory);
    if (!automatic) {
        bool isFile = false;
        std::string canonical;
        const ResourceStatus status = ResolveExplicitDiscoveryPath(directory, isFile, canonical);
        if (status != ResourceStatus::Success) {
            return status;
        }
        if (isFile) {
            libraries.push_back({canonical, ResourceSourceType::External});
            ASCENDLOGI("Discovered explicit compile resource SO: path=%s", canonical.c_str());
            return ResourceStatus::Success;
        }
        std::set<std::string> found;
        if (!CollectLibraries(directory, ResourceSourceType::External, false, found)) {
            return ResourceStatus::IoError;
        }
        for (const std::string& path : found) {
            libraries.push_back({path, ResourceSourceType::External});
        }
        if (libraries.empty()) {
            ASCENDLOGW(
                "No compile resource libraries found in explicit directory: path=%s pattern=lib*%s recursive=false",
                directory, LIBRARY_NAME_SUFFIX);
            return ResourceStatus::NotFound;
        }
        ASCENDLOGI(
            "Discovered compile resource libraries: mode=explicit path=%s count=%zu", directory, libraries.size());
        return ResourceStatus::Success;
    }
    const AutomaticRoots roots = AutomaticSearchRoots();
    std::set<std::string> custom;
    std::set<std::string> builtIn;
    if (!CollectAutomaticLibraries(roots.custom, ResourceSourceType::Custom, custom) ||
        !CollectAutomaticLibraries(roots.builtIn, ResourceSourceType::BuiltIn, builtIn)) {
        return ResourceStatus::IoError;
    }
    for (const std::string& path : custom) {
        libraries.push_back({path, ResourceSourceType::Custom});
    }
    for (const std::string& path : builtIn) {
        libraries.push_back({path, ResourceSourceType::BuiltIn});
    }
    if (libraries.empty()) {
        ASCENDLOGW(
            "No compile resource libraries found by automatic discovery: custom_roots=%zu built_in_roots=%zu "
            "pattern=lib*%s",
            roots.custom.size(), roots.builtIn.size(), LIBRARY_NAME_SUFFIX);
        return ResourceStatus::NotFound;
    }
    ASCENDLOGI(
        "Discovered compile resource libraries: mode=automatic custom=%zu built_in=%zu total=%zu", custom.size(),
        builtIn.size(), libraries.size());
    return ResourceStatus::Success;
}

ResourceStatus ResourceRegistry::GetBundle(
    const LibraryHandle& library, const LibrarySpec& spec, const AcCompileResourceBundle*& bundle)
{
    dlerror();
    const auto getter =
        reinterpret_cast<AscendcGetCompileResourceBundleFn>(dlsym(library.get(), "AscendcGetCompileResourceBundle"));
    const char* symbolError = dlerror();
    if (symbolError != nullptr || getter == nullptr) {
        ASCENDLOGE(
            "Failed to resolve compile resource bundle entry point: source_type=%s so=%s "
            "symbol=AscendcGetCompileResourceBundle error=%s",
            SourceTypeName(spec.sourceType), spec.path.c_str(),
            symbolError == nullptr ? "symbol resolved to null" : symbolError);
        return ResourceStatus::LoadError;
    }
    const AcCompileResourceBundleHeader* header = getter();
    ResourceStatus status = ValidateBundleHeader(header, spec);
    if (status != ResourceStatus::Success) {
        return status;
    }
    bundle = reinterpret_cast<const AcCompileResourceBundle*>(header);
    status = ValidateManifestTable(*bundle, spec);
    if (status == ResourceStatus::Success) {
        status = ValidateExtensionTable(*bundle, spec);
    }
    if (status == ResourceStatus::Success) {
        ASCENDLOGD(
            "Validated compile resource bundle: source_type=%s so=%s manifests=%" PRIu64 " extensions=%" PRIu64,
            SourceTypeName(spec.sourceType), spec.path.c_str(), bundle->manifestCount, bundle->extensionCount);
    }
    return status;
}

ResourceStatus ResourceRegistry::ResolveSourceRoot(
    const fs::path& searchRoot, const std::string& resourceId, std::string& canonicalRoot)
{
    boost::system::error_code error;
    const fs::file_status status = fs::symlink_status(searchRoot, error);
    if (error) {
        ASCENDLOGE(
            "Failed to inspect compile source root: resource_id=%s root=%s error=%s", resourceId.c_str(),
            searchRoot.c_str(), error.message().c_str());
        return error == boost::system::errc::no_such_file_or_directory ? ResourceStatus::InvalidResource :
                                                                         ResourceStatus::IoError;
    }
    if (fs::is_symlink(status)) {
        ASCENDLOGE(
            "Compile source root must not be a symbolic link: resource_id=%s root=%s", resourceId.c_str(),
            searchRoot.c_str());
        return ResourceStatus::InvalidResource;
    }
    if (!fs::is_directory(status)) {
        ASCENDLOGE(
            "Compile source root is not a directory: resource_id=%s root=%s", resourceId.c_str(), searchRoot.c_str());
        return ResourceStatus::InvalidResource;
    }
    const fs::path canonical = fs::canonical(searchRoot, error);
    if (error) {
        ASCENDLOGE(
            "Failed to resolve compile source root: resource_id=%s root=%s error=%s", resourceId.c_str(),
            searchRoot.c_str(), error.message().c_str());
        return ResourceStatus::IoError;
    }
    canonicalRoot = canonical.string();
    return ResourceStatus::Success;
}

ResourceStatus ResourceRegistry::AddSourceMatch(
    const fs::path& candidate, const std::string& sourceFile, const std::string& canonicalRoot,
    const std::string& resourceId, std::set<std::string>& matches)
{
    if (candidate.filename() != sourceFile) {
        return ResourceStatus::Success;
    }
    boost::system::error_code error;
    const fs::file_status status = fs::symlink_status(candidate, error);
    if (error) {
        ASCENDLOGE(
            "Failed to inspect compile source candidate: resource_id=%s path=%s error=%s", resourceId.c_str(),
            candidate.c_str(), error.message().c_str());
        return ResourceStatus::IoError;
    }
    if (!fs::is_regular_file(status)) {
        return ResourceStatus::Success;
    }
    const fs::path canonical = fs::canonical(candidate, error);
    if (error) {
        ASCENDLOGE(
            "Failed to resolve compile source candidate: resource_id=%s path=%s error=%s", resourceId.c_str(),
            candidate.c_str(), error.message().c_str());
        return ResourceStatus::IoError;
    }
    if (!FileUtils::IsPathWithin(canonical.string(), canonicalRoot)) {
        ASCENDLOGE(
            "Compile source candidate escaped root: resource_id=%s candidate=%s canonical=%s root=%s",
            resourceId.c_str(), candidate.c_str(), canonical.c_str(), canonicalRoot.c_str());
        return ResourceStatus::InvalidResource;
    }
    matches.insert(canonical.parent_path().string());
    return ResourceStatus::Success;
}

ResourceStatus ResourceRegistry::LocateSourceFile(
    const LibrarySpec& spec, const std::string& resourceId, const std::string& sourceFile, std::string& located)
{
    // Relative path example:
    //   kernel/jit/ascend910b/libfoo_compile_database.so  <- resource SO
    //   kernel/ascend910b/impl/foo.cpp                    <- source file
    // Derive the SoC and kernel root from the SO path, then recursively search the sibling kernel/<soc> tree.
    // Exactly one regular-file match is required; located is set to that file's parent directory.
    const fs::path soPath(spec.path);
    const fs::path soc = soPath.parent_path().filename();
    const fs::path jitRoot = soPath.parent_path().parent_path();
    const fs::path kernelRoot = jitRoot.parent_path();
    if (soc.empty() || jitRoot.filename() != "jit" || kernelRoot.filename() != "kernel") {
        ASCENDLOGE(
            "Compile resource SO is outside kernel/jit/<soc>: resource_id=%s source_type=%s so=%s", resourceId.c_str(),
            SourceTypeName(spec.sourceType), spec.path.c_str());
        return ResourceStatus::InvalidResource;
    }
    const fs::path searchRoot = kernelRoot / soc;
    std::string canonicalRoot;
    ResourceStatus status = ResolveSourceRoot(searchRoot, resourceId, canonicalRoot);
    if (status != ResourceStatus::Success) {
        return status;
    }
    std::set<std::string> matches;
    boost::system::error_code error;
    fs::recursive_directory_iterator current(canonicalRoot, fs::directory_options::none, error);
    const fs::recursive_directory_iterator end;
    while (!error && current != end) {
        status = AddSourceMatch(current->path(), sourceFile, canonicalRoot, resourceId, matches);
        if (status != ResourceStatus::Success) {
            return status;
        }
        current.increment(error);
    }
    if (error) {
        ASCENDLOGE(
            "Failed to search compile source: resource_id=%s root=%s error=%s", resourceId.c_str(), searchRoot.c_str(),
            error.message().c_str());
        return ResourceStatus::IoError;
    }
    if (matches.size() != 1U) {
        ASCENDLOGE(
            "Compile source file must have exactly one match: resource_id=%s root=%s source_file=%s actual_matches=%zu "
            "expected_matches=1",
            resourceId.c_str(), searchRoot.c_str(), sourceFile.c_str(), matches.size());
        return ResourceStatus::InvalidResource;
    }
    located = *matches.begin();
    ASCENDLOGD(
        "Located compile source file: resource_id=%s source_file=%s directory=%s", resourceId.c_str(),
        sourceFile.c_str(), located.c_str());
    return ResourceStatus::Success;
}

bool ResourceRegistry::FindPathConflict(const std::set<std::string>& paths)
{
    for (const std::string& path : paths) {
        size_t separator = path.find('/');
        while (separator != std::string::npos) {
            if (paths.count(path.substr(0U, separator)) != 0U) {
                const std::string parent = path.substr(0U, separator);
                ASCENDLOGE(
                    "Compile resource file path conflicts with another file: parent=%s child=%s "
                    "reason=a file cannot also be a directory",
                    parent.c_str(), path.c_str());
                return true;
            }
            separator = path.find('/', separator + 1U);
        }
    }
    return false;
}

ResourceStatus ResourceRegistry::AppendFile(
    const AcCompileResourceFile& file, const std::string& resourceId, ResourceEntry& entry,
    std::set<std::string>& paths, uint64_t& manifestBytes, StagedResources& staged)
{
    std::string fileName;
    std::string relativePath;
    ResourceStatus status = CopyString(file.fileName, "resource.fileName", MAX_PATH_SIZE, fileName);
    if (status == ResourceStatus::Success) {
        status = CopyString(file.filePath, "resource.filePath", MAX_PATH_SIZE, relativePath);
    }
    if (status != ResourceStatus::Success) {
        return status;
    }
    status = ValidateResourceFilePath(resourceId, fileName, relativePath, paths);
    if (status == ResourceStatus::Success) {
        status = ValidateResourceFilePayload(file, resourceId, relativePath, manifestBytes, staged);
    }
    if (status != ResourceStatus::Success) {
        return status;
    }
    std::vector<uint8_t> bytes;
    if (file.size != 0U) {
        bytes.assign(file.data, file.data + file.size);
    }
    entry.files.push_back({std::move(fileName), std::move(relativePath), std::move(bytes)});
    return ResourceStatus::Success;
}

ResourceStatus ResourceRegistry::CopyManifestFiles(
    const AcCompileResourceManifest& unit, const std::string& resourceId, ResourceEntry& entry, StagedResources& staged)
{
    const ResourceStatus tableStatus = ValidateManifestFileTable(unit, resourceId, staged);
    if (tableStatus != ResourceStatus::Success) {
        return tableStatus;
    }
    entry.files.reserve(static_cast<size_t>(unit.fileCount));
    std::set<std::string> paths;
    uint64_t manifestBytes = 0U;
    for (uint64_t index = 0U; index < unit.fileCount; ++index) {
        const ResourceStatus status = AppendFile(unit.files[index], resourceId, entry, paths, manifestBytes, staged);
        if (status != ResourceStatus::Success) {
            return status;
        }
    }
    if (FindPathConflict(paths)) {
        return ResourceStatus::InvalidResource;
    }
    staged.files += unit.fileCount;
    std::sort(entry.files.begin(), entry.files.end(), [](const ResourceFileData& left, const ResourceFileData& right) {
        return left.relativePath < right.relativePath;
    });
    ASCENDLOGD(
        "Validated compile resource manifest files: resource_id=%s files=%" PRIu64 " bytes=%" PRIu64,
        resourceId.c_str(), unit.fileCount, manifestBytes);
    return ResourceStatus::Success;
}

StagedResources& ResourceRegistry::SelectStagedResources(StageState& stage, ResourceSourceType sourceType)
{
    if (sourceType == ResourceSourceType::Custom) {
        return stage.custom;
    }
    return sourceType == ResourceSourceType::BuiltIn ? stage.builtIn : stage.external;
}

ResourceStatus ResourceRegistry::LoadManifest(
    const AcCompileResourceManifest& unit, const LibrarySpec& spec, uint64_t manifestIndex, StageState& stage)
{
    ASCENDLOGI(
        "Loading compile resource manifest: source_type=%s so=%s manifest=%" PRIu64, SourceTypeName(spec.sourceType),
        spec.path.c_str(), manifestIndex);
    ManifestOwnership ownership;
    ResourceStatus status = ParseManifestOwnership(unit, spec, manifestIndex, ownership);
    if (status != ResourceStatus::Success) {
        return status;
    }
    std::unique_ptr<ResourceEntry> entry(new ResourceEntry());
    if (ownership.hasSourceFile) {
        status = LocateSourceFile(spec, ownership.resourceId, ownership.sourceFile, entry->data.sourceFilePath);
    }
    StagedResources& staged = SelectStagedResources(stage, spec.sourceType);
    if (status == ResourceStatus::Success) {
        status = CopyManifestFiles(unit, ownership.resourceId, *entry, staged);
    }
    if (status != ResourceStatus::Success) {
        return status;
    }
    entry->data.json = std::move(ownership.document);
    entry->sourceSoPath = spec.path;
    entry->sourceType = spec.sourceType;
    const auto existing = staged.resources.find(ownership.resourceId);
    if (existing != staged.resources.end()) {
        ASCENDLOGE(
            "Duplicate resource_id in load transaction: resource_id=%s incoming_source_type=%s incoming_so=%s "
            "existing_source_type=%s existing_so=%s",
            ownership.resourceId.c_str(), SourceTypeName(entry->sourceType), entry->sourceSoPath.c_str(),
            SourceTypeName(existing->second->sourceType), existing->second->sourceSoPath.c_str());
        return ResourceStatus::Conflict;
    }
    const size_t fileCount = entry->files.size();
    staged.resources.emplace(ownership.resourceId, std::move(entry));
    ASCENDLOGI(
        "Loaded compile resource manifest: resource_id=%s source_type=%s so=%s manifest=%" PRIu64 " files=%zu",
        ownership.resourceId.c_str(), SourceTypeName(spec.sourceType), spec.path.c_str(), manifestIndex, fileCount);
    return ResourceStatus::Success;
}

ResourceStatus ResourceRegistry::LoadLibrary(const LibrarySpec& spec, StageState& stage)
{
    ASCENDLOGI("Loading compile resource SO: source_type=%s so=%s", SourceTypeName(spec.sourceType), spec.path.c_str());
    LibraryHandle library(dlopen(spec.path.c_str(), RTLD_NOW | RTLD_LOCAL));
    if (!library) {
        const char* error = dlerror();
        ASCENDLOGE(
            "Failed to open compile resource SO: source_type=%s so=%s error=%s", SourceTypeName(spec.sourceType),
            spec.path.c_str(), error == nullptr ? "unknown" : error);
        return ResourceStatus::LoadError;
    }
    const AcCompileResourceBundle* bundle = nullptr;
    ResourceStatus status = GetBundle(library, spec, bundle);
    if (status != ResourceStatus::Success) {
        return status;
    }
    for (uint64_t manifestIndex = 0U; manifestIndex < bundle->manifestCount; ++manifestIndex) {
        status = LoadManifest(bundle->manifests[manifestIndex], spec, manifestIndex, stage);
        if (status != ResourceStatus::Success) {
            ASCENDLOGE(
                "Stopped loading compile resource SO at manifest: source_type=%s so=%s manifest=%" PRIu64
                " status=%s(%d)",
                SourceTypeName(spec.sourceType), spec.path.c_str(), manifestIndex, ResourceStatusName(status),
                static_cast<int>(status));
            return status;
        }
    }
    ASCENDLOGI(
        "Loaded compile resource SO: source_type=%s so=%s manifests=%" PRIu64, SourceTypeName(spec.sourceType),
        spec.path.c_str(), bundle->manifestCount);
    return ResourceStatus::Success;
}

ResourceStatus ResourceRegistry::LoadLibraries(const std::vector<LibrarySpec>& libraries, StageState& stage)
{
    ASCENDLOGI("Loading compile resource SO list: count=%zu", libraries.size());
    for (const LibrarySpec& library : libraries) {
        SelectStagedResources(stage, library.sourceType).discovered = true;
        const ResourceStatus status = LoadLibrary(library, stage);
        if (status != ResourceStatus::Success) {
            ASCENDLOGE(
                "Stopped loading compile resource SO list: source_type=%s so=%s status=%s(%d)",
                SourceTypeName(library.sourceType), library.path.c_str(), ResourceStatusName(status),
                static_cast<int>(status));
            return status;
        }
    }
    ASCENDLOGI("Loaded compile resource SO list: count=%zu", libraries.size());
    return ResourceStatus::Success;
}

ResourceStatus ResourceRegistry::WriteMaterializedFiles(
    const std::vector<ResourceFileData>& files, const std::string& root)
{
    ASCENDLOGD("Writing materialized compile resource files: root=%s files=%zu", root.c_str(), files.size());
    for (const ResourceFileData& file : files) {
        const std::string path = FileUtils::JoinPath(root, file.relativePath);
        const std::string parent = FileUtils::ParentPath(path);
        if (!FileUtils::CreateDirectories(parent)) {
            ASCENDLOGE(
                "Failed to create materialized compile resource parent directory: root=%s relative_path=%s "
                "parent=%s",
                root.c_str(), file.relativePath.c_str(), parent.c_str());
            return ResourceStatus::IoError;
        }
        errno = 0;
        std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
        if (!output.is_open()) {
            const int openError = errno;
            ASCENDLOGE(
                "Failed to open materialized compile resource file: root=%s relative_path=%s path=%s error=%s",
                root.c_str(), file.relativePath.c_str(), path.c_str(),
                openError == 0 ? "stream open failed" : std::strerror(openError));
            return ResourceStatus::IoError;
        }
        if (!file.bytes.empty()) {
            output.write(
                reinterpret_cast<const char*>(file.bytes.data()), static_cast<std::streamsize>(file.bytes.size()));
        }
        if (!FileUtils::FinalizeOutput(output)) {
            ASCENDLOGE(
                "Failed to finalize materialized compile resource file: root=%s relative_path=%s path=%s "
                "reason=write, flush, or close failed",
                root.c_str(), file.relativePath.c_str(), path.c_str());
            return ResourceStatus::IoError;
        }
    }
    ASCENDLOGD("Wrote materialized compile resource files: root=%s files=%zu", root.c_str(), files.size());
    return ResourceStatus::Success;
}

ResourceRegistry& ResourceRegistry::Instance()
{
    static ResourceRegistry registry;
    return registry;
}

ResourceRegistry::ResourceRegistry() = default;

ResourceRegistry::~ResourceRegistry()
{
    if (!keepTemporaryRoot_) {
        CleanupPath(temporaryRoot_);
    }
}

ResourceEntry* ResourceRegistry::FindResource(const std::string& resourceId) noexcept
{
    const auto external = externalResources_.find(resourceId);
    if (external != externalResources_.end()) {
        return external->second.get();
    }
    const auto custom = customResources_.find(resourceId);
    if (custom != customResources_.end()) {
        return custom->second.get();
    }
    const auto builtIn = builtInResources_.find(resourceId);
    return builtIn == builtInResources_.end() ? nullptr : builtIn->second.get();
}

bool ResourceRegistry::HasCommitConflict(const ResourceStore& incoming, const ResourceStore& committed)
{
    for (const auto& item : incoming) {
        const auto existing = committed.find(item.first);
        if (existing != committed.end()) {
            ASCENDLOGW(
                "Compile resource conflicts with an already registered resource and will not be added: "
                "resource_id=%s incoming_source_type=%s incoming_so=%s "
                "existing_source_type=%s existing_so=%s",
                item.first.c_str(), SourceTypeName(item.second->sourceType), item.second->sourceSoPath.c_str(),
                SourceTypeName(existing->second->sourceType), existing->second->sourceSoPath.c_str());
            return true;
        }
    }
    return false;
}

ResourceStatus ResourceRegistry::Commit(StageState& stage)
{
    std::lock_guard<std::mutex> lock(registryMutex_);
    ASCENDLOGI(
        "Committing compile resources: external=%zu custom=%zu built_in=%zu", stage.external.resources.size(),
        stage.custom.resources.size(), stage.builtIn.resources.size());
    ResourceStatus status = ResourceStatus::Success;
    auto checkCategory = [&](StagedResources& staged, ResourceStore& committed, ResourceSourceType sourceType) {
        if (!staged.discovered && !staged.conflict) {
            return;
        }
        if (!staged.conflict && HasCommitConflict(staged.resources, committed)) {
            staged.conflict = true;
        }
        if (staged.conflict) {
            ASCENDLOGW(
                "Compile resource category will not be committed because conflicts were detected: source_type=%s "
                "resource_count=%zu",
                SourceTypeName(sourceType), staged.resources.size());
            status = ResourceStatus::Conflict;
            return;
        }
        committed.reserve(committed.size() + staged.resources.size());
    };
    checkCategory(stage.external, externalResources_, ResourceSourceType::External);
    checkCategory(stage.custom, customResources_, ResourceSourceType::Custom);
    checkCategory(stage.builtIn, builtInResources_, ResourceSourceType::BuiltIn);
    uint64_t bytes = resourceBytes_;
    uint64_t files = resourceFileCount_;
    if (!CheckRegistryLimits(stage.external, ResourceSourceType::External, bytes, files) ||
        !CheckRegistryLimits(stage.custom, ResourceSourceType::Custom, bytes, files) ||
        !CheckRegistryLimits(stage.builtIn, ResourceSourceType::BuiltIn, bytes, files)) {
        return ResourceStatus::InvalidResource;
    }
    auto commitCategory = [&](StagedResources& staged, ResourceStore& committed) {
        if (!staged.discovered || staged.conflict) {
            return;
        }
        committed.merge(staged.resources);
    };
    commitCategory(stage.external, externalResources_);
    commitCategory(stage.custom, customResources_);
    commitCategory(stage.builtIn, builtInResources_);
    resourceBytes_ = bytes;
    resourceFileCount_ = files;
    ASCENDLOGI(
        "Finished committing compile resources: status=%s(%d) registered_external=%zu registered_custom=%zu "
        "registered_built_in=%zu total_files=%" PRIu64 " total_bytes=%" PRIu64,
        ResourceStatusName(status), static_cast<int>(status), externalResources_.size(), customResources_.size(),
        builtInResources_.size(), resourceFileCount_, resourceBytes_);
    return status;
}

ResourceStatus ResourceRegistry::Load(const char* directory)
{
    const bool automatic = directory == nullptr || *directory == '\0';
    std::lock_guard<std::mutex> loadLock(loadMutex_);
    if (automatic && automaticLoadAttempted_) {
        ASCENDLOGI(
            "Returning cached automatic compile resource load result: status=%s(%d)",
            ResourceStatusName(automaticLoadStatus_), static_cast<int>(automaticLoadStatus_));
        return automaticLoadStatus_;
    }
    ASCENDLOGI(
        "Loading compile resources: mode=%s directory=%s", automatic ? "automatic" : "explicit",
        automatic ? "<environment>" : directory);
    std::vector<LibrarySpec> libraries;
    ResourceStatus status = DiscoverLibraries(directory, libraries);
    if (status != ResourceStatus::Success) {
        ASCENDLOGE(
            "Compile resource load stopped during discovery: mode=%s path=%s status=%s(%d)",
            automatic ? "automatic" : "explicit", automatic ? "<environment>" : directory, ResourceStatusName(status),
            static_cast<int>(status));
    } else {
        ASCENDLOGI(
            "Compile resource discovery completed: mode=%s libraries=%zu", automatic ? "automatic" : "explicit",
            libraries.size());
        StageState stage;
        status = LoadLibraries(libraries, stage);
        if (status == ResourceStatus::Success) {
            status = Commit(stage);
        }
    }
    if (automatic) {
        automaticLoadAttempted_ = true;
        automaticLoadStatus_ = status;
    }
    if (status == ResourceStatus::Success) {
        ASCENDLOGI(
            "Finished loading compile resources: mode=%s libraries=%zu status=%s(%d)",
            automatic ? "automatic" : "explicit", libraries.size(), ResourceStatusName(status),
            static_cast<int>(status));
    }
    return status;
}

ResourceStatus ResourceRegistry::Materialize(
    const std::string& resourceId, const ResourceEntry& entry, ResourceData& resource)
{
    ASCENDLOGI(
        "Materializing compile resource: resource_id=%s source_type=%s so=%s files=%zu", resourceId.c_str(),
        SourceTypeName(entry.sourceType), entry.sourceSoPath.c_str(), entry.files.size());
    std::string temporaryRoot;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        if (temporaryRoot_.empty()) {
            keepTemporaryRoot_ = ShouldKeepTemporaryRoot();
            const ResourceStatus status = CreateTemporaryRoot(temporaryRoot_);
            if (status != ResourceStatus::Success) {
                return status;
            }
        }
        temporaryRoot = temporaryRoot_;
    }
    ASCENDLOGD(
        "Compile resource temporary root is ready: resource_id=%s path=%s", resourceId.c_str(), temporaryRoot.c_str());
    std::string categoryRoot;
    std::string canonicalRoot;
    ResourceStatus status =
        PrepareMaterializationRoot(temporaryRoot, resourceId, entry.sourceType, categoryRoot, canonicalRoot);
    if (status == ResourceStatus::Success) {
        ASCENDLOGD(
            "Compile resource materialization directory is ready: resource_id=%s path=%s", resourceId.c_str(),
            canonicalRoot.c_str());
        status = WriteMaterializedFiles(entry.files, canonicalRoot);
    }
    if (status != ResourceStatus::Success) {
        if (!keepTemporaryRoot_) {
            CleanupPath(categoryRoot);
        }
        resource.resourceDir.clear();
        return status;
    }
    resource.resourceDir = canonicalRoot;
    ASCENDLOGI(
        "Compile resource materialized: resource_id=%s source_type=%s so=%s files=%zu directory=%s", resourceId.c_str(),
        SourceTypeName(entry.sourceType), entry.sourceSoPath.c_str(), entry.files.size(), resource.resourceDir.c_str());
    return ResourceStatus::Success;
}

ResourceStatus ResourceRegistry::Lookup(const char* resourceId, ResourceData& resource)
{
    resource = ResourceData{};
    if (resourceId == nullptr) {
        ASCENDLOGE("Compile resource lookup rejected: resource_id=<null> reason=identifier pointer is null");
        return ResourceStatus::InvalidResource;
    }
    if (*resourceId == '\0') {
        ASCENDLOGE("Compile resource lookup rejected: resource_id=<empty> reason=identifier is empty");
        return ResourceStatus::InvalidResource;
    }
    ASCENDLOGD("Looking up compile resource: resource_id=%s", resourceId);
    ResourceEntry* entry = nullptr;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        entry = FindResource(resourceId);
    }
    if (entry == nullptr) {
        ASCENDLOGD("Compile resource lookup missed: resource_id=%s reason=not registered", resourceId);
        return ResourceStatus::NotFound;
    }
    ASCENDLOGD(
        "Compile resource lookup matched: resource_id=%s source_type=%s so=%s", resourceId,
        SourceTypeName(entry->sourceType), entry->sourceSoPath.c_str());
    std::lock_guard<std::mutex> materializeLock(entry->materializeMutex);
    ResourceData materialized = entry->data;
    materialized.resourceDir.clear();
    const ResourceStatus status = Materialize(resourceId, *entry, materialized);
    if (status == ResourceStatus::Success) {
        resource = std::move(materialized);
        ASCENDLOGD(
            "Compile resource lookup completed: resource_id=%s directory=%s", resourceId, resource.resourceDir.c_str());
    }
    return status;
}

} // namespace specialization_compile
} // namespace ascendc
