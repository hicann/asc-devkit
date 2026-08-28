/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

#include <cstdlib>
#include <csignal>
#include <dlfcn.h>
#include <fstream>
#include <iterator>
#include <memory>
#include <set>
#include <string>
#include <sys/resource.h>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "resource_registry.h"

namespace ascendc {
namespace specialization_compile {
namespace {

namespace fs = boost::filesystem;
using namespace mockcpp;

constexpr uint64_t TEST_MAX_PATH_SIZE = 4096U;
constexpr uint64_t TEST_MAX_RESOURCE_FILE_SIZE = 256U * 1024U * 1024U;
constexpr uint64_t TEST_MAX_MANIFEST_RESOURCE_SIZE = 512U * 1024U * 1024U;
constexpr uint64_t TEST_MAX_REGISTRY_RESOURCE_SIZE = 1024U * 1024U * 1024U;
constexpr uint64_t TEST_MAX_MANIFEST_COUNT = 4096U;
constexpr uint64_t TEST_MAX_EXTENSION_COUNT = 4096U;
constexpr uint64_t TEST_MAX_FILE_COUNT = 65536U;
constexpr uint64_t TEST_MAX_REGISTRY_FILE_COUNT = 131072U;

static_assert(
    std::is_same<std::underlying_type_t<ResourceStatus>, uint32_t>::value, "ResourceStatus must have a stable width");
static_assert(
    std::is_same<std::underlying_type_t<ResourceSourceType>, uint32_t>::value,
    "ResourceSourceType must have a stable width");

const AcCompileResourceBundleHeader* gBundleHeader = nullptr;
uint32_t gDlopenCalls = 0U;
uint32_t gDlsymCalls = 0U;
uint32_t gDlcloseCalls = 0U;
bool gDlsymReportsError = false;
uint32_t gDlerrorCalls = 0U;
std::string gDlopenFailurePath;

const AcCompileResourceBundleHeader* TestBundleGetter() { return gBundleHeader; }

void* DlopenSuccess(const char*, int)
{
    ++gDlopenCalls;
    return reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234U));
}

void* DlopenFailure(const char*, int)
{
    ++gDlopenCalls;
    return nullptr;
}

void* DlopenConfigured(const char* path, int)
{
    ++gDlopenCalls;
    if (path != nullptr && gDlopenFailurePath == path) {
        return nullptr;
    }
    return reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234U));
}

void* DlsymBundle(void*, const char*)
{
    ++gDlsymCalls;
    return reinterpret_cast<void*>(&TestBundleGetter);
}

void* DlsymMissing(void*, const char*)
{
    ++gDlsymCalls;
    return nullptr;
}

char* DlerrorControlled()
{
    ++gDlerrorCalls;
    if (gDlsymReportsError && (gDlerrorCalls % 2U) == 0U) {
        return const_cast<char*>("symbol error");
    }
    return nullptr;
}

char* DlerrorFailure() { return const_cast<char*>("loader error"); }

int DlcloseSuccess(void*)
{
    ++gDlcloseCalls;
    return 0;
}

int DlcloseFailure(void*)
{
    ++gDlcloseCalls;
    return -1;
}

AcString StringView(const std::string& value) { return {value.data(), static_cast<uint64_t>(value.size())}; }

AcCompileResourceBundle MakeBundle(const AcCompileResourceManifest* manifests, uint64_t count)
{
    AcCompileResourceBundle bundle{};
    bundle.header.magic = AC_COMPILE_RESOURCE_MAGIC;
    bundle.header.abiVersion = AC_COMPILE_RESOURCE_ABI_VERSION;
    bundle.header.structSize = sizeof(AcCompileResourceBundle);
    bundle.header.flags = 0U;
    bundle.manifests = manifests;
    bundle.manifestCount = count;
    return bundle;
}

struct SourceLayout {
    std::string soPath;
    std::string sourceRoot;
    std::string sourceDirectory;
    std::string sourcePath;
};

class ResourceRegistryTest : public testing::Test {
protected:
    struct SavedEnvironment {
        std::string name;
        bool existed;
        std::string value;
    };

    void SetUp() override
    {
        char pattern[] = "/tmp/aclrtc-resource-registry-ut-XXXXXX";
        char* created = mkdtemp(pattern);
        ASSERT_NE(created, nullptr);
        root_ = created;
        SetEnvironment("TMPDIR", root_);
        registry_.reset(new ResourceRegistry());
        ResetLoaderState();
    }

    void TearDown() override
    {
        registry_.reset();
        for (auto current = environments_.rbegin(); current != environments_.rend(); ++current) {
            if (current->existed) {
                (void)setenv(current->name.c_str(), current->value.c_str(), 1);
            } else {
                (void)unsetenv(current->name.c_str());
            }
        }
        boost::system::error_code error;
        (void)fs::remove_all(root_, error);
        GlobalMockObject::verify();
    }

    void SetEnvironment(const std::string& name, const std::string& value)
    {
        bool saved = false;
        for (const SavedEnvironment& environment : environments_) {
            if (environment.name == name) {
                saved = true;
                break;
            }
        }
        if (!saved) {
            const char* oldValue = std::getenv(name.c_str());
            environments_.push_back({name, oldValue != nullptr, oldValue == nullptr ? "" : oldValue});
        }
        ASSERT_EQ(setenv(name.c_str(), value.c_str(), 1), 0);
    }

    void UnsetEnvironment(const std::string& name)
    {
        bool saved = false;
        for (const SavedEnvironment& environment : environments_) {
            if (environment.name == name) {
                saved = true;
                break;
            }
        }
        if (!saved) {
            const char* oldValue = std::getenv(name.c_str());
            environments_.push_back({name, oldValue != nullptr, oldValue == nullptr ? "" : oldValue});
        }
        ASSERT_EQ(unsetenv(name.c_str()), 0);
    }

    fs::path Path(const std::string& relative) const { return fs::path(root_) / relative; }

    void MakeDirectory(const fs::path& path)
    {
        boost::system::error_code error;
        ASSERT_TRUE(fs::create_directories(path, error) || fs::is_directory(path));
        ASSERT_FALSE(error);
    }

    void WriteFile(const fs::path& path, const std::string& data = "")
    {
        MakeDirectory(path.parent_path());
        std::ofstream output(path.string(), std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(output.is_open());
        output.write(data.data(), static_cast<std::streamsize>(data.size()));
        output.close();
        ASSERT_TRUE(output);
    }

    std::string ReadFile(const fs::path& path)
    {
        std::ifstream input(path.string(), std::ios::binary);
        EXPECT_TRUE(input.is_open());
        return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    }

    SourceLayout CreateLayout(const std::string& prefix, const std::string& sourceFile = "source.cpp")
    {
        const fs::path kernel = Path(prefix) / "op_impl/ai_core/tbe/kernel";
        const fs::path so =
            kernel / "jit/Ascend910B" / ("lib" + fs::path(prefix).filename().string() + "_compile_database.so");
        const fs::path sourceDirectory = kernel / "Ascend910B/impl";
        const fs::path source = sourceDirectory / sourceFile;
        WriteFile(so);
        WriteFile(source, "source");
        return {so.string(), (kernel / "Ascend910B").string(), sourceDirectory.string(), source.string()};
    }

    std::unique_ptr<ResourceEntry> MakeEntry(ResourceSourceType type, const std::string& source = "source.cpp")
    {
        std::unique_ptr<ResourceEntry> entry(new ResourceEntry());
        entry->sourceType = type;
        entry->sourceSoPath = "test.so";
        entry->data.sourceFilePath = source;
        return entry;
    }

    void ResetLoaderState()
    {
        gBundleHeader = nullptr;
        gDlopenCalls = 0U;
        gDlsymCalls = 0U;
        gDlcloseCalls = 0U;
        gDlsymReportsError = false;
        gDlerrorCalls = 0U;
        gDlopenFailurePath.clear();
    }

    void MockSuccessfulLoader()
    {
        MOCKER(dlopen).stubs().will(invoke(DlopenSuccess));
        MOCKER(dlsym).stubs().will(invoke(DlsymBundle));
        MOCKER(dlerror).stubs().will(invoke(DlerrorControlled));
        MOCKER(dlclose).stubs().will(invoke(DlcloseSuccess));
    }

    std::string root_;
    std::vector<SavedEnvironment> environments_;
    std::unique_ptr<ResourceRegistry> registry_;
};

TEST_F(ResourceRegistryTest, AutomaticSearchRootsParsesCustomVendorAndBuiltInPaths)
{
    const fs::path opp = Path("opp");
    WriteFile(
        opp / "vendors/config.ini",
        "ignored=value\n load_priority = vendor_a, ../escape, vendor_b, vendor_a, , /absolute\n");
    SetEnvironment("ASCEND_CUSTOM_OPP_PATH", "  /custom/one:/custom/two:/custom/one::  ");
    SetEnvironment("ASCEND_OPP_PATH", (opp / ".." / opp.filename()).string());

    const AutomaticRoots roots = ResourceRegistry::AutomaticSearchRoots();
    const fs::path canonicalOpp = fs::canonical(opp);
    ASSERT_EQ(roots.custom.size(), 4U);
    EXPECT_EQ(roots.custom[0], "/custom/one/op_impl/ai_core/tbe/kernel/jit");
    EXPECT_EQ(roots.custom[1], "/custom/two/op_impl/ai_core/tbe/kernel/jit");
    EXPECT_EQ(roots.custom[2], (canonicalOpp / "vendors/vendor_a/op_impl/ai_core/tbe/kernel/jit").string());
    EXPECT_EQ(roots.custom[3], (canonicalOpp / "vendors/vendor_b/op_impl/ai_core/tbe/kernel/jit").string());
    ASSERT_EQ(roots.builtIn.size(), 1U);
    EXPECT_EQ(roots.builtIn[0], (canonicalOpp / "built-in/op_impl/ai_core/tbe/kernel/jit").string());

    SetEnvironment("ASCEND_OPP_PATH", Path("missing-opp").string());
    const AutomaticRoots missingOpp = ResourceRegistry::AutomaticSearchRoots();
    EXPECT_EQ(missingOpp.custom.size(), 2U);
    EXPECT_TRUE(missingOpp.builtIn.empty());

    const fs::path oppWithoutVendorConfig = Path("opp-without-vendor-config");
    MakeDirectory(oppWithoutVendorConfig);
    SetEnvironment("ASCEND_OPP_PATH", oppWithoutVendorConfig.string());
    const AutomaticRoots missingVendorConfig = ResourceRegistry::AutomaticSearchRoots();
    EXPECT_EQ(missingVendorConfig.custom.size(), 2U);
    ASSERT_EQ(missingVendorConfig.builtIn.size(), 1U);
    EXPECT_EQ(
        missingVendorConfig.builtIn[0],
        (fs::canonical(oppWithoutVendorConfig) / "built-in/op_impl/ai_core/tbe/kernel/jit").string());

    UnsetEnvironment("ASCEND_OPP_PATH");
    const AutomaticRoots noOpp = ResourceRegistry::AutomaticSearchRoots();
    EXPECT_EQ(noOpp.custom.size(), 2U);
    EXPECT_TRUE(noOpp.builtIn.empty());
}

TEST_F(ResourceRegistryTest, LibraryHelpersFilterFilesDirectoriesAndSymlinks)
{
    EXPECT_TRUE(ResourceRegistry::IsLibraryName("libx_compile_database.so"));
    EXPECT_FALSE(ResourceRegistry::IsLibraryName("lib_compile_database.so"));
    EXPECT_FALSE(ResourceRegistry::IsLibraryName("x_compile_database.so"));
    EXPECT_FALSE(ResourceRegistry::IsLibraryName("libx_compile_database.so.1"));

    const fs::path valid = Path("libs/libvalid_compile_database.so");
    const fs::path wrong = Path("libs/not_a_resource.so");
    const fs::path directory = Path("libs/libdirectory_compile_database.so");
    const fs::path symlink = Path("libs/liblink_compile_database.so");
    WriteFile(valid);
    WriteFile(wrong);
    MakeDirectory(directory);
    boost::system::error_code error;
    fs::create_symlink(valid, symlink, error);
    ASSERT_FALSE(error);

    std::set<std::string> found;
    ResourceRegistry::AddLibrary(valid.string(), found);
    ResourceRegistry::AddLibrary(valid.string(), found);
    ResourceRegistry::AddLibrary(wrong.string(), found);
    ResourceRegistry::AddLibrary(directory.string(), found);
    ResourceRegistry::AddLibrary(symlink.string(), found);
    ASSERT_EQ(found.size(), 1U);
    EXPECT_EQ(*found.begin(), fs::canonical(valid).string());
}

TEST_F(ResourceRegistryTest, CollectAndDiscoverLibrariesHandleExplicitAndAutomaticPaths)
{
    const fs::path explicitRoot = Path("explicit");
    const fs::path top = explicitRoot / "libtop_compile_database.so";
    const fs::path nested = explicitRoot / "nested/libnested_compile_database.so";
    WriteFile(top);
    WriteFile(nested);
    WriteFile(explicitRoot / "bad.so");

    std::set<std::string> found;
    EXPECT_TRUE(ResourceRegistry::CollectLibraries(explicitRoot.string(), ResourceSourceType::External, false, found));
    ASSERT_EQ(found.size(), 1U);
    EXPECT_EQ(*found.begin(), fs::canonical(top).string());
    found.clear();
    EXPECT_TRUE(ResourceRegistry::CollectLibraries(explicitRoot.string(), ResourceSourceType::External, true, found));
    EXPECT_EQ(found.size(), 2U);
    EXPECT_FALSE(
        ResourceRegistry::CollectLibraries(Path("missing").string(), ResourceSourceType::External, true, found));

    std::vector<LibrarySpec> libraries;
    EXPECT_EQ(ResourceRegistry::DiscoverLibraries(top.string().c_str(), libraries), ResourceStatus::Success);
    ASSERT_EQ(libraries.size(), 1U);
    EXPECT_EQ(libraries[0].sourceType, ResourceSourceType::External);

    libraries.clear();
    EXPECT_EQ(
        ResourceRegistry::DiscoverLibraries(explicitRoot.string().c_str(), libraries), ResourceStatus::InvalidResource);
    EXPECT_TRUE(libraries.empty());

    const fs::path empty = Path("empty");
    MakeDirectory(empty);
    libraries.clear();
    EXPECT_EQ(ResourceRegistry::DiscoverLibraries(empty.string().c_str(), libraries), ResourceStatus::InvalidResource);
    EXPECT_TRUE(libraries.empty());
    EXPECT_EQ(
        ResourceRegistry::DiscoverLibraries(Path("absent").string().c_str(), libraries),
        ResourceStatus::InvalidResource);

    const fs::path link = Path("explicit-link");
    boost::system::error_code error;
    fs::create_symlink(explicitRoot, link, error);
    ASSERT_FALSE(error);
    EXPECT_EQ(ResourceRegistry::DiscoverLibraries(link.string().c_str(), libraries), ResourceStatus::InvalidResource);
    const fs::path fileLink = Path("explicit-file-link.so");
    fs::create_symlink(top, fileLink, error);
    ASSERT_FALSE(error);
    EXPECT_EQ(
        ResourceRegistry::DiscoverLibraries(fileLink.string().c_str(), libraries), ResourceStatus::InvalidResource);

    const SourceLayout custom = CreateLayout("custom");
    const SourceLayout builtIn = CreateLayout("opp/built-in");
    SetEnvironment("ASCEND_CUSTOM_OPP_PATH", Path("custom").string());
    SetEnvironment("ASCEND_OPP_PATH", Path("opp").string());
    libraries.clear();
    EXPECT_EQ(ResourceRegistry::DiscoverLibraries(nullptr, libraries), ResourceStatus::Success);
    ASSERT_EQ(libraries.size(), 2U);
    EXPECT_EQ(libraries[0].path, fs::canonical(custom.soPath).string());
    EXPECT_EQ(libraries[0].sourceType, ResourceSourceType::Custom);
    EXPECT_EQ(libraries[1].path, fs::canonical(builtIn.soPath).string());
    EXPECT_EQ(libraries[1].sourceType, ResourceSourceType::BuiltIn);

    found.clear();
    EXPECT_TRUE(
        ResourceRegistry::CollectAutomaticLibraries({Path("absent-auto").string()}, ResourceSourceType::Custom, found));
}

TEST_F(ResourceRegistryTest, ResolveSourceRootAndAddSourceMatchValidateFilesystemIdentity)
{
    const fs::path sourceRoot = Path("source-root");
    const fs::path source = sourceRoot / "dir/source.cpp";
    const fs::path other = sourceRoot / "dir/other.cpp";
    WriteFile(source, "source");
    WriteFile(other, "other");
    std::string canonicalRoot;
    EXPECT_EQ(ResourceRegistry::ResolveSourceRoot(sourceRoot, "resource", canonicalRoot), ResourceStatus::Success);
    EXPECT_EQ(canonicalRoot, fs::canonical(sourceRoot).string());
    EXPECT_EQ(
        ResourceRegistry::ResolveSourceRoot(Path("missing-root"), "resource", canonicalRoot),
        ResourceStatus::InvalidResource);

    const fs::path regularRoot = Path("regular-root");
    WriteFile(regularRoot);
    EXPECT_EQ(
        ResourceRegistry::ResolveSourceRoot(regularRoot, "resource", canonicalRoot), ResourceStatus::InvalidResource);
    EXPECT_EQ(
        ResourceRegistry::ResolveSourceRoot(regularRoot / "child", "resource", canonicalRoot), ResourceStatus::IoError);
    const fs::path linkedRoot = Path("linked-root");
    boost::system::error_code error;
    fs::create_symlink(sourceRoot, linkedRoot, error);
    ASSERT_FALSE(error);
    EXPECT_EQ(
        ResourceRegistry::ResolveSourceRoot(linkedRoot, "resource", canonicalRoot), ResourceStatus::InvalidResource);

    std::set<std::string> matches;
    EXPECT_EQ(
        ResourceRegistry::AddSourceMatch(other, "source.cpp", canonicalRoot, "resource", matches),
        ResourceStatus::Success);
    EXPECT_TRUE(matches.empty());
    EXPECT_EQ(
        ResourceRegistry::AddSourceMatch(source, "source.cpp", canonicalRoot, "resource", matches),
        ResourceStatus::Success);
    ASSERT_EQ(matches.size(), 1U);
    EXPECT_EQ(*matches.begin(), fs::canonical(source).parent_path().string());

    const fs::path matchingDirectory = sourceRoot / "source.cpp";
    MakeDirectory(matchingDirectory);
    EXPECT_EQ(
        ResourceRegistry::AddSourceMatch(matchingDirectory, "source.cpp", canonicalRoot, "resource", matches),
        ResourceStatus::Success);
    const fs::path matchingLink = sourceRoot / "link/source.cpp";
    MakeDirectory(matchingLink.parent_path());
    fs::create_symlink(source, matchingLink, error);
    ASSERT_FALSE(error);
    EXPECT_EQ(
        ResourceRegistry::AddSourceMatch(matchingLink, "source.cpp", canonicalRoot, "resource", matches),
        ResourceStatus::Success);

    const fs::path outside = Path("outside/source.cpp");
    WriteFile(outside);
    EXPECT_EQ(
        ResourceRegistry::AddSourceMatch(outside, "source.cpp", canonicalRoot, "resource", matches),
        ResourceStatus::InvalidResource);
    EXPECT_EQ(
        ResourceRegistry::AddSourceMatch(Path("missing/source.cpp"), "source.cpp", canonicalRoot, "resource", matches),
        ResourceStatus::IoError);
}

TEST_F(ResourceRegistryTest, LocateSourceFileRequiresValidLayoutAndUniqueMatch)
{
    const SourceLayout valid = CreateLayout("locate");
    LibrarySpec spec{valid.soPath, ResourceSourceType::External};
    std::string located;
    EXPECT_EQ(ResourceRegistry::LocateSourceFile(spec, "resource", "source.cpp", located), ResourceStatus::Success);
    EXPECT_EQ(located, fs::canonical(valid.sourcePath).parent_path().string());

    WriteFile(fs::path(valid.sourceRoot) / "duplicate/source.cpp");
    EXPECT_EQ(
        ResourceRegistry::LocateSourceFile(spec, "resource", "source.cpp", located), ResourceStatus::InvalidResource);
    EXPECT_EQ(
        ResourceRegistry::LocateSourceFile(spec, "resource", "missing.cpp", located), ResourceStatus::InvalidResource);

    spec.path = Path("bad-layout/libx_compile_database.so").string();
    EXPECT_EQ(
        ResourceRegistry::LocateSourceFile(spec, "resource", "source.cpp", located), ResourceStatus::InvalidResource);

    const SourceLayout missing = CreateLayout("missing-source-root");
    boost::system::error_code error;
    (void)fs::remove_all(missing.sourceRoot, error);
    ASSERT_FALSE(error);
    spec.path = missing.soPath;
    EXPECT_EQ(
        ResourceRegistry::LocateSourceFile(spec, "resource", "source.cpp", located), ResourceStatus::InvalidResource);

    const SourceLayout linked = CreateLayout("linked-source-root");
    const fs::path actual = Path("actual-source-root");
    fs::rename(linked.sourceRoot, actual, error);
    ASSERT_FALSE(error);
    fs::create_symlink(actual, linked.sourceRoot, error);
    ASSERT_FALSE(error);
    spec.path = linked.soPath;
    EXPECT_EQ(
        ResourceRegistry::LocateSourceFile(spec, "resource", "source.cpp", located), ResourceStatus::InvalidResource);
}

TEST_F(ResourceRegistryTest, GetBundleRejectsMissingSymbolAndInvalidAbi)
{
    LibraryHandle library(nullptr);
    const LibrarySpec spec{"bundle.so", ResourceSourceType::External};
    const AcCompileResourceBundle* output = nullptr;
    MOCKER(dlsym).stubs().will(invoke(DlsymMissing));
    MOCKER(dlerror).stubs().will(invoke(DlerrorControlled));
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::LoadError);
    gDlsymReportsError = true;
    gDlerrorCalls = 0U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::LoadError);
}

TEST_F(ResourceRegistryTest, GetBundleValidatesEveryHeaderAndManifestField)
{
    const std::string json = "{}";
    AcCompileResourceManifest manifest{StringView(json), nullptr, 0U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    LibraryHandle library(nullptr);
    const LibrarySpec spec{"bundle.so", ResourceSourceType::BuiltIn};
    const AcCompileResourceBundle* output = nullptr;
    MOCKER(dlsym).stubs().will(invoke(DlsymBundle));
    MOCKER(dlerror).stubs().will(invoke(DlerrorControlled));

    gBundleHeader = nullptr;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    gBundleHeader = &bundle.header;
    bundle.header.magic = 0U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    bundle.header.magic = AC_COMPILE_RESOURCE_MAGIC;
    bundle.header.abiVersion = 0U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    bundle.header.abiVersion = AC_COMPILE_RESOURCE_ABI_VERSION;
    bundle.header.structSize = sizeof(AcCompileResourceBundleHeader);
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    bundle.header.structSize = sizeof(AcCompileResourceBundle);
    bundle.header.flags = 1U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    bundle.header.flags = 0U;
    bundle.manifestCount = 0U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    bundle.manifestCount = TEST_MAX_MANIFEST_COUNT + 1U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    bundle.manifestCount = 1U;
    bundle.manifests = nullptr;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    bundle.manifests = &manifest;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::Success);
    EXPECT_EQ(output, &bundle);
}

TEST_F(ResourceRegistryTest, GetBundleValidatesExtensionTable)
{
    const std::string json = "{}";
    AcCompileResourceManifest manifest{StringView(json), nullptr, 0U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    LibraryHandle library(nullptr);
    const LibrarySpec spec{"bundle.so", ResourceSourceType::External};
    const AcCompileResourceBundle* output = nullptr;
    MOCKER(dlsym).stubs().will(invoke(DlsymBundle));
    MOCKER(dlerror).stubs().will(invoke(DlerrorControlled));
    gBundleHeader = &bundle.header;

    const uint8_t payload = 0U;
    AcCompileResourceExtension extension{1000U, 1U, 0U, &payload, sizeof(payload)};
    bundle.extensions = &extension;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);

    bundle.extensions = nullptr;
    bundle.extensionCount = 1U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);

    bundle.extensionCount = TEST_MAX_EXTENSION_COUNT + 1U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);

    bundle.extensions = &extension;
    bundle.extensionCount = 1U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::Success);

    extension.data = nullptr;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    extension.data = &payload;
    extension.flags = 2U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);
    extension.flags = AC_COMPILE_RESOURCE_EXTENSION_REQUIRED;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::InvalidResource);

    extension.flags = 0U;
    extension.data = nullptr;
    extension.dataSize = 0U;
    EXPECT_EQ(ResourceRegistry::GetBundle(library, spec, output), ResourceStatus::Success);
}

TEST_F(ResourceRegistryTest, AppendFileCopiesPayloadAndRejectsInvalidFieldsAndLimits)
{
    const std::string name = "resource.bin";
    const std::string path = "nested/resource.bin";
    const std::vector<uint8_t> payload = {1U, 2U, 3U};
    AcCompileResourceFile file{StringView(name), StringView(path), payload.data(), payload.size()};
    ResourceEntry entry;
    std::set<std::string> paths;
    uint64_t manifestBytes = 0U;
    StagedResources staged;
    EXPECT_EQ(
        ResourceRegistry::AppendFile(file, "resource", entry, paths, manifestBytes, staged), ResourceStatus::Success);
    ASSERT_EQ(entry.files.size(), 1U);
    EXPECT_EQ(entry.files[0].bytes, payload);
    EXPECT_EQ(manifestBytes, payload.size());
    EXPECT_EQ(staged.bytes, payload.size());
    EXPECT_EQ(
        ResourceRegistry::AppendFile(file, "resource", entry, paths, manifestBytes, staged),
        ResourceStatus::InvalidResource);

    auto expectInvalid = [&](const AcCompileResourceFile& invalid, uint64_t initialManifest = 0U,
                             uint64_t initialRegistry = 0U) {
        ResourceEntry localEntry;
        std::set<std::string> localPaths;
        StagedResources localStaged;
        localStaged.bytes = initialRegistry;
        EXPECT_EQ(
            ResourceRegistry::AppendFile(invalid, "resource", localEntry, localPaths, initialManifest, localStaged),
            ResourceStatus::InvalidResource);
    };

    AcCompileResourceFile invalid = file;
    invalid.fileName = {nullptr, 1U};
    expectInvalid(invalid);
    invalid = file;
    invalid.fileName = {name.data(), TEST_MAX_PATH_SIZE + 1U};
    expectInvalid(invalid);
    const std::string unsafeName = "../resource.bin";
    invalid = file;
    invalid.fileName = StringView(unsafeName);
    expectInvalid(invalid);
    const std::string mismatch = "other.bin";
    invalid = file;
    invalid.fileName = StringView(mismatch);
    expectInvalid(invalid);
    const std::string absolute = "/resource.bin";
    invalid = file;
    invalid.filePath = StringView(absolute);
    expectInvalid(invalid);
    const std::string traversal = "nested/../resource.bin";
    invalid = file;
    invalid.filePath = StringView(traversal);
    expectInvalid(invalid);
    invalid = file;
    invalid.size = TEST_MAX_RESOURCE_FILE_SIZE + 1U;
    expectInvalid(invalid);
    invalid = file;
    invalid.data = nullptr;
    expectInvalid(invalid);
    invalid = file;
    invalid.size = 2U;
    expectInvalid(invalid, TEST_MAX_MANIFEST_RESOURCE_SIZE - 1U);
    expectInvalid(invalid, 0U, TEST_MAX_REGISTRY_RESOURCE_SIZE - 1U);

    AcCompileResourceFile empty{StringView(name), StringView(name), nullptr, 0U};
    ResourceEntry emptyEntry;
    std::set<std::string> emptyPaths;
    manifestBytes = 0U;
    staged = {};
    EXPECT_EQ(
        ResourceRegistry::AppendFile(empty, "resource", emptyEntry, emptyPaths, manifestBytes, staged),
        ResourceStatus::Success);
    EXPECT_TRUE(emptyEntry.files[0].bytes.empty());
}

TEST_F(ResourceRegistryTest, CopyManifestFilesValidatesTablesConflictsAndSorts)
{
    const std::string nameA = "a.bin";
    const std::string pathA = "z/a.bin";
    const std::string nameB = "b.bin";
    const std::string pathB = "a/b.bin";
    const std::vector<uint8_t> bytes = {7U};
    AcCompileResourceFile files[] = {
        {StringView(nameA), StringView(pathA), bytes.data(), bytes.size()},
        {StringView(nameB), StringView(pathB), bytes.data(), bytes.size()},
    };
    const std::string json = "{}";
    AcCompileResourceManifest unit{StringView(json), files, 2U};
    ResourceEntry entry;
    StagedResources staged;
    EXPECT_EQ(ResourceRegistry::CopyManifestFiles(unit, "resource", entry, staged), ResourceStatus::Success);
    ASSERT_EQ(entry.files.size(), 2U);
    EXPECT_EQ(entry.files[0].relativePath, pathB);
    EXPECT_EQ(entry.files[1].relativePath, pathA);
    EXPECT_EQ(staged.files, 2U);

    ResourceEntry local;
    StagedResources localStage;
    AcCompileResourceManifest invalid{StringView(json), files, 0U};
    EXPECT_EQ(
        ResourceRegistry::CopyManifestFiles(invalid, "resource", local, localStage), ResourceStatus::InvalidResource);
    invalid = {StringView(json), nullptr, 1U};
    EXPECT_EQ(
        ResourceRegistry::CopyManifestFiles(invalid, "resource", local, localStage), ResourceStatus::InvalidResource);
    invalid = {StringView(json), files, TEST_MAX_FILE_COUNT + 1U};
    EXPECT_EQ(
        ResourceRegistry::CopyManifestFiles(invalid, "resource", local, localStage), ResourceStatus::InvalidResource);
    invalid = {StringView(json), nullptr, 0U};
    localStage.files = TEST_MAX_REGISTRY_FILE_COUNT + 1U;
    EXPECT_EQ(
        ResourceRegistry::CopyManifestFiles(invalid, "resource", local, localStage), ResourceStatus::InvalidResource);
    localStage.files = TEST_MAX_REGISTRY_FILE_COUNT;
    invalid = {StringView(json), files, 1U};
    EXPECT_EQ(
        ResourceRegistry::CopyManifestFiles(invalid, "resource", local, localStage), ResourceStatus::InvalidResource);

    const std::string invalidPath = "../a.bin";
    AcCompileResourceFile invalidFile{StringView(nameA), StringView(invalidPath), bytes.data(), bytes.size()};
    ResourceEntry invalidEntry;
    StagedResources invalidStage;
    invalid = {StringView(json), &invalidFile, 1U};
    EXPECT_EQ(
        ResourceRegistry::CopyManifestFiles(invalid, "resource", invalidEntry, invalidStage),
        ResourceStatus::InvalidResource);

    const std::string parentName = "dir";
    const std::string childName = "child.bin";
    const std::string childPath = "dir/child.bin";
    AcCompileResourceFile conflicting[] = {
        {StringView(parentName), StringView(parentName), bytes.data(), bytes.size()},
        {StringView(childName), StringView(childPath), bytes.data(), bytes.size()},
    };
    ResourceEntry conflictEntry;
    StagedResources conflictStage;
    invalid = {StringView(json), conflicting, 2U};
    EXPECT_EQ(
        ResourceRegistry::CopyManifestFiles(invalid, "resource", conflictEntry, conflictStage),
        ResourceStatus::InvalidResource);
}

TEST_F(ResourceRegistryTest, LoadManifestValidatesJsonOwnershipSourceAndDuplicates)
{
    const SourceLayout layout = CreateLayout("manifest-invalid");
    const LibrarySpec spec{layout.soPath, ResourceSourceType::External};
    StageState stage;
    auto loadText = [&](const std::string& text) {
        AcCompileResourceManifest unit{StringView(text), nullptr, 0U};
        return ResourceRegistry::LoadManifest(unit, spec, 0U, stage);
    };
    EXPECT_EQ(loadText("{"), ResourceStatus::InvalidResource);
    EXPECT_EQ(loadText("[]"), ResourceStatus::InvalidResource);
    EXPECT_EQ(loadText(R"({"source_file":"source.cpp"})"), ResourceStatus::InvalidResource);
    EXPECT_EQ(loadText(R"({"resource_id":1,"source_file":"source.cpp"})"), ResourceStatus::InvalidResource);
    EXPECT_EQ(loadText(R"({"resource_id":""})"), ResourceStatus::InvalidResource);
    EXPECT_EQ(stage.external.resources.find(""), stage.external.resources.end());
    EXPECT_EQ(loadText(R"({"resource_id":"metadata-only"})"), ResourceStatus::Success);
    EXPECT_TRUE(stage.external.resources.at("metadata-only")->data.sourceFilePath.empty());
    EXPECT_EQ(loadText(R"({"resource_id":"resource","source_file":1})"), ResourceStatus::InvalidResource);
    EXPECT_EQ(loadText(R"({"resource_id":"../bad","source_file":"source.cpp"})"), ResourceStatus::Success);
    EXPECT_NE(stage.external.resources.find("../bad"), stage.external.resources.end());
    EXPECT_EQ(loadText(R"({"resource_id":"resource","source_file":"../source.cpp"})"), ResourceStatus::InvalidResource);
    EXPECT_EQ(loadText(R"({"resource_id":"missing","source_file":"missing.cpp"})"), ResourceStatus::InvalidResource);

    const std::string valid = R"({"resource_id":"resource","source_file":"source.cpp","value":3})";
    EXPECT_EQ(loadText(valid), ResourceStatus::Success);
    EXPECT_EQ(loadText(valid), ResourceStatus::Conflict);

    AcCompileResourceManifest invalidString{{nullptr, 1U}, nullptr, 0U};
    EXPECT_EQ(ResourceRegistry::LoadManifest(invalidString, spec, 1U, stage), ResourceStatus::InvalidResource);
}

TEST_F(ResourceRegistryTest, LoadManifestStagesExternalCustomAndBuiltInResources)
{
    StageState stage;
    const std::vector<std::pair<ResourceSourceType, std::string>> cases = {
        {ResourceSourceType::External, "external"},
        {ResourceSourceType::Custom, "custom-stage"},
        {ResourceSourceType::BuiltIn, "builtin-stage"},
    };
    for (const auto& item : cases) {
        const SourceLayout layout = CreateLayout(item.second);
        const std::string json = "{\"resource_id\":\"" + item.second + "\",\"source_file\":\"source.cpp\"}";
        AcCompileResourceManifest unit{StringView(json), nullptr, 0U};
        EXPECT_EQ(
            ResourceRegistry::LoadManifest(unit, {layout.soPath, item.first}, 0U, stage), ResourceStatus::Success);
    }
    ASSERT_EQ(stage.external.resources.size(), 1U);
    ASSERT_EQ(stage.custom.resources.size(), 1U);
    ASSERT_EQ(stage.builtIn.resources.size(), 1U);
    const fs::path externalSource = Path("external/op_impl/ai_core/tbe/kernel/Ascend910B/impl/source.cpp");
    EXPECT_EQ(
        stage.external.resources.at("external")->data.sourceFilePath,
        fs::canonical(externalSource).parent_path().string());
}

TEST_F(ResourceRegistryTest, LibraryDeleterHandlesNullAndSuccessfulClose)
{
    MOCKER(dlclose).expects(once()).will(invoke(DlcloseSuccess));
    LibraryDeleter deleter;
    deleter(nullptr);
    deleter(reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234U)));
    EXPECT_EQ(gDlcloseCalls, 1U);
}

TEST_F(ResourceRegistryTest, LibraryDeleterHandlesCloseFailure)
{
    MOCKER(dlclose).expects(once()).will(invoke(DlcloseFailure));
    LibraryDeleter{}(reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234U)));
    EXPECT_EQ(gDlcloseCalls, 1U);
}

TEST_F(ResourceRegistryTest, LoadLibraryReportsOpenFailure)
{
    const fs::path so = Path("open-failure.so");
    WriteFile(so);
    MOCKER(dlopen).expects(once()).will(invoke(DlopenFailure));
    MOCKER(dlerror).stubs().will(invoke(DlerrorFailure));
    StageState stage;
    EXPECT_EQ(
        ResourceRegistry::LoadLibrary({so.string(), ResourceSourceType::External}, stage), ResourceStatus::LoadError);
    EXPECT_EQ(gDlopenCalls, 1U);
}

TEST_F(ResourceRegistryTest, LoadLibraryRejectsUnresolvablePathBeforeDlopen)
{
    MOCKER(dlopen).stubs().will(invoke(DlopenFailure));
    StageState stage;
    EXPECT_EQ(
        ResourceRegistry::LoadLibrary({Path("missing.so").string(), ResourceSourceType::External}, stage),
        ResourceStatus::InvalidResource);
    EXPECT_EQ(gDlopenCalls, 0U);
}

TEST_F(ResourceRegistryTest, LoadLibraryReportsBundleFailureAndClosesHandle)
{
    const fs::path so = Path("bundle-failure.so");
    WriteFile(so);
    MOCKER(dlopen).expects(once()).will(invoke(DlopenSuccess));
    MOCKER(dlsym).stubs().will(invoke(DlsymMissing));
    MOCKER(dlerror).stubs().will(invoke(DlerrorControlled));
    MOCKER(dlclose).expects(once()).will(invoke(DlcloseSuccess));
    StageState stage;
    EXPECT_EQ(
        ResourceRegistry::LoadLibrary({so.string(), ResourceSourceType::Custom}, stage), ResourceStatus::LoadError);
    EXPECT_EQ(gDlcloseCalls, 1U);
}

TEST_F(ResourceRegistryTest, LoadLibraryReportsManifestFailure)
{
    const SourceLayout layout = CreateLayout("load-library");
    const std::string invalidJson = "{";
    AcCompileResourceManifest manifest{StringView(invalidJson), nullptr, 0U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    gBundleHeader = &bundle.header;
    MockSuccessfulLoader();
    StageState stage;
    EXPECT_EQ(
        ResourceRegistry::LoadLibrary({layout.soPath, ResourceSourceType::BuiltIn}, stage),
        ResourceStatus::InvalidResource);
}

TEST_F(ResourceRegistryTest, LoadLibraryLoadsValidManifest)
{
    const SourceLayout layout = CreateLayout("load-library-success");
    const fs::path soPath(layout.soPath);
    const std::string nonCanonicalPath =
        (soPath.parent_path() / ".." / soPath.parent_path().filename() / soPath.filename()).string();
    const std::string json = R"({"resource_id":"loaded","source_file":"source.cpp"})";
    AcCompileResourceManifest manifest{StringView(json), nullptr, 0U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    gBundleHeader = &bundle.header;
    MockSuccessfulLoader();
    StageState stage;
    EXPECT_EQ(
        ResourceRegistry::LoadLibrary({nonCanonicalPath, ResourceSourceType::Custom}, stage), ResourceStatus::Success);
    ASSERT_EQ(stage.custom.resources.size(), 1U);
    EXPECT_EQ(stage.custom.resources.at("loaded")->sourceSoPath, fs::canonical(soPath).string());
    EXPECT_EQ(gDlcloseCalls, 1U);
}

TEST_F(ResourceRegistryTest, LoadLibrariesContinuesAfterAllFailuresAndReturnsFirstFailure)
{
    const fs::path first = Path("first.so");
    const fs::path second = Path("second.so");
    WriteFile(first);
    WriteFile(second);
    MOCKER(dlopen).stubs().will(invoke(DlopenFailure));
    MOCKER(dlerror).stubs().will(invoke(DlerrorFailure));
    StageState stage;
    const std::vector<LibrarySpec> libraries = {
        {first.string(), ResourceSourceType::External},
        {second.string(), ResourceSourceType::BuiltIn},
    };
    EXPECT_EQ(ResourceRegistry::LoadLibraries(libraries, stage), ResourceStatus::LoadError);
    EXPECT_EQ(gDlopenCalls, 2U);
    EXPECT_TRUE(stage.external.discovered);
    EXPECT_TRUE(stage.builtIn.discovered);
    EXPECT_TRUE(stage.external.resources.empty());
    EXPECT_TRUE(stage.builtIn.resources.empty());
}

TEST_F(ResourceRegistryTest, LoadLibrariesSkipsFailedSoAndKeepsSuccessfulSo)
{
    const std::string json = R"({"resource_id":"continued"})";
    AcCompileResourceManifest manifest{StringView(json), nullptr, 0U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    gBundleHeader = &bundle.header;
    const fs::path first = Path("first.so");
    const fs::path second = Path("second.so");
    WriteFile(first);
    WriteFile(second);
    gDlopenFailurePath = first.string();
    MOCKER(dlopen).stubs().will(invoke(DlopenConfigured));
    MOCKER(dlsym).stubs().will(invoke(DlsymBundle));
    MOCKER(dlerror).stubs().will(invoke(DlerrorControlled));
    MOCKER(dlclose).stubs().will(invoke(DlcloseSuccess));

    StageState stage;
    const std::vector<LibrarySpec> libraries = {
        {gDlopenFailurePath, ResourceSourceType::External},
        {second.string(), ResourceSourceType::BuiltIn},
    };
    EXPECT_EQ(ResourceRegistry::LoadLibraries(libraries, stage), ResourceStatus::Success);
    EXPECT_EQ(gDlopenCalls, 2U);
    EXPECT_EQ(gDlcloseCalls, 1U);
    EXPECT_TRUE(stage.external.discovered);
    EXPECT_TRUE(stage.external.resources.empty());
    EXPECT_TRUE(stage.builtIn.discovered);
    EXPECT_EQ(stage.builtIn.resources.count("continued"), 1U);
}

TEST_F(ResourceRegistryTest, LoadLibrariesDiscardsPartialManifestsFromFailedSo)
{
    const std::string validJson = R"({"resource_id":"partial"})";
    const std::string invalidJson = "{";
    AcCompileResourceManifest manifests[] = {
        {StringView(validJson), nullptr, 0U},
        {StringView(invalidJson), nullptr, 0U},
    };
    AcCompileResourceBundle bundle = MakeBundle(manifests, 2U);
    gBundleHeader = &bundle.header;
    MockSuccessfulLoader();
    const fs::path so = Path("partial.so");
    WriteFile(so);

    StageState stage;
    EXPECT_EQ(
        ResourceRegistry::LoadLibraries({{so.string(), ResourceSourceType::External}}, stage),
        ResourceStatus::InvalidResource);
    EXPECT_TRUE(stage.external.discovered);
    EXPECT_TRUE(stage.external.resources.empty());
    EXPECT_EQ(stage.external.bytes, 0U);
    EXPECT_EQ(stage.external.files, 0U);
}

TEST_F(ResourceRegistryTest, LoadLibrariesSkipsResourcesConflictingWithAnEarlierSo)
{
    const std::string json = R"({"resource_id":"conflict"})";
    AcCompileResourceManifest manifest{StringView(json), nullptr, 0U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    gBundleHeader = &bundle.header;
    MockSuccessfulLoader();
    const fs::path first = Path("conflict-first.so");
    const fs::path second = Path("conflict-second.so");
    WriteFile(first);
    WriteFile(second);

    StageState stage;
    EXPECT_EQ(
        ResourceRegistry::LoadLibraries(
            {{first.string(), ResourceSourceType::External}, {second.string(), ResourceSourceType::External}}, stage),
        ResourceStatus::Success);
    EXPECT_EQ(gDlopenCalls, 2U);
    ASSERT_EQ(stage.external.resources.size(), 1U);
    EXPECT_EQ(stage.external.resources.at("conflict")->sourceSoPath, fs::canonical(first).string());
}

TEST_F(ResourceRegistryTest, LoadLibrariesSkipsResourcesThatExceedCumulativeLimits)
{
    const std::string json = R"({"resource_id":"limited"})";
    const std::string name = "payload.bin";
    const std::string path = "payload.bin";
    const std::vector<uint8_t> payload = {1U};
    AcCompileResourceFile file{StringView(name), StringView(path), payload.data(), payload.size()};
    AcCompileResourceManifest manifest{StringView(json), &file, 1U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    gBundleHeader = &bundle.header;
    MockSuccessfulLoader();
    const fs::path so = Path("limit.so");
    WriteFile(so);

    StageState stage;
    stage.external.bytes = TEST_MAX_REGISTRY_RESOURCE_SIZE;
    EXPECT_EQ(
        ResourceRegistry::LoadLibraries({{so.string(), ResourceSourceType::External}}, stage),
        ResourceStatus::InvalidResource);
    EXPECT_EQ(gDlopenCalls, 1U);
    EXPECT_TRUE(stage.external.discovered);
    EXPECT_TRUE(stage.external.resources.empty());
    EXPECT_EQ(stage.external.bytes, TEST_MAX_REGISTRY_RESOURCE_SIZE);
    EXPECT_EQ(stage.external.files, 0U);
}

TEST_F(ResourceRegistryTest, LoadLibrariesLoadsValidList)
{
    const SourceLayout layout = CreateLayout("load-list");
    const std::string json = R"({"resource_id":"list","source_file":"source.cpp"})";
    AcCompileResourceManifest manifest{StringView(json), nullptr, 0U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    gBundleHeader = &bundle.header;
    MockSuccessfulLoader();
    StageState stage;
    EXPECT_EQ(
        ResourceRegistry::LoadLibraries({{layout.soPath, ResourceSourceType::External}}, stage),
        ResourceStatus::Success);
    EXPECT_TRUE(stage.external.discovered);
    EXPECT_EQ(stage.external.resources.size(), 1U);
}

TEST_F(ResourceRegistryTest, CommitMergesCategoriesAndFindResourceUsesExpectedPrecedence)
{
    StageState stage;
    stage.external.discovered = true;
    stage.custom.discovered = true;
    stage.builtIn.discovered = true;
    stage.external.bytes = 3U;
    stage.custom.bytes = 4U;
    stage.builtIn.bytes = 5U;
    stage.external.files = 1U;
    stage.custom.files = 2U;
    stage.builtIn.files = 3U;
    stage.external.resources.emplace("shared", MakeEntry(ResourceSourceType::External, "external"));
    stage.custom.resources.emplace("shared", MakeEntry(ResourceSourceType::Custom, "custom"));
    stage.builtIn.resources.emplace("shared", MakeEntry(ResourceSourceType::BuiltIn, "builtin"));
    stage.custom.resources.emplace("custom-only", MakeEntry(ResourceSourceType::Custom));
    EXPECT_EQ(registry_->Commit(stage), ResourceStatus::Success);
    EXPECT_EQ(registry_->resourceBytes_, 12U);
    EXPECT_EQ(registry_->resourceFileCount_, 6U);
    ASSERT_NE(registry_->FindResource("shared"), nullptr);
    EXPECT_EQ(registry_->FindResource("shared")->sourceType, ResourceSourceType::External);
    registry_->externalResources_.erase("shared");
    EXPECT_EQ(registry_->FindResource("shared")->sourceType, ResourceSourceType::Custom);
    registry_->customResources_.erase("shared");
    EXPECT_EQ(registry_->FindResource("shared")->sourceType, ResourceSourceType::BuiltIn);
    EXPECT_EQ(registry_->FindResource("missing"), nullptr);

    StageState ignored;
    ignored.external.resources.emplace("ignored", MakeEntry(ResourceSourceType::External));
    EXPECT_EQ(registry_->Commit(ignored), ResourceStatus::Success);
    EXPECT_EQ(registry_->FindResource("ignored"), nullptr);
}

TEST_F(ResourceRegistryTest, CommitRejectsConflictsAndLimitsWithoutPartialMerge)
{
    StageState initial;
    initial.external.discovered = true;
    initial.external.resources.emplace("existing", MakeEntry(ResourceSourceType::External));
    EXPECT_EQ(registry_->Commit(initial), ResourceStatus::Success);

    StageState conflict;
    conflict.external.discovered = true;
    conflict.external.resources.emplace("existing", MakeEntry(ResourceSourceType::External));
    conflict.external.resources.emplace("must-not-merge", MakeEntry(ResourceSourceType::External));
    EXPECT_EQ(registry_->Commit(conflict), ResourceStatus::Conflict);
    EXPECT_TRUE(conflict.external.conflict);
    EXPECT_EQ(registry_->FindResource("must-not-merge"), nullptr);

    StageState preconflicted;
    preconflicted.custom.discovered = true;
    preconflicted.custom.conflict = true;
    preconflicted.custom.resources.emplace("preconflict", MakeEntry(ResourceSourceType::Custom));
    EXPECT_EQ(registry_->Commit(preconflicted), ResourceStatus::Conflict);
    EXPECT_EQ(registry_->FindResource("preconflict"), nullptr);

    StageState bytes;
    bytes.external.discovered = true;
    bytes.external.bytes = TEST_MAX_REGISTRY_RESOURCE_SIZE + 1U;
    bytes.external.resources.emplace("too-large", MakeEntry(ResourceSourceType::External));
    EXPECT_EQ(registry_->Commit(bytes), ResourceStatus::InvalidResource);
    EXPECT_EQ(registry_->FindResource("too-large"), nullptr);

    StageState files;
    files.external.discovered = true;
    files.external.files = TEST_MAX_REGISTRY_FILE_COUNT + 1U;
    files.external.resources.emplace("too-many", MakeEntry(ResourceSourceType::External));
    EXPECT_EQ(registry_->Commit(files), ResourceStatus::InvalidResource);
    EXPECT_EQ(registry_->FindResource("too-many"), nullptr);
}

TEST_F(ResourceRegistryTest, WriteMaterializedFilesWritesNestedAndEmptyPayloadsAndReportsIoErrors)
{
    const std::vector<ResourceFileData> files = {
        {"data.bin", "nested/data.bin", {1U, 2U, 3U}},
        {"empty.bin", "empty.bin", {}},
    };
    const fs::path output = Path("materialized");
    const std::string nonCanonicalOutput = (output / ".." / output.filename()).string();
    EXPECT_EQ(ResourceRegistry::WriteMaterializedFiles(files, nonCanonicalOutput), ResourceStatus::Success);
    EXPECT_EQ(ReadFile(output / "nested/data.bin"), std::string("\1\2\3", 3U));
    EXPECT_TRUE(ReadFile(output / "empty.bin").empty());

    const fs::path redirectedRoot = Path("redirected-root");
    const fs::path redirectedTarget = Path("redirected-target");
    MakeDirectory(redirectedRoot);
    MakeDirectory(redirectedTarget);
    boost::system::error_code error;
    fs::create_directory_symlink(redirectedTarget, redirectedRoot / "redirect", error);
    ASSERT_FALSE(error);
    const std::vector<ResourceFileData> redirected = {{"data.bin", "redirect/data.bin", {1U}}};
    EXPECT_EQ(ResourceRegistry::WriteMaterializedFiles(redirected, redirectedRoot.string()), ResourceStatus::IoError);
    EXPECT_FALSE(fs::exists(redirectedTarget / "data.bin"));

    const fs::path blockedRoot = Path("blocked-root");
    WriteFile(blockedRoot, "file");
    EXPECT_EQ(ResourceRegistry::WriteMaterializedFiles(files, blockedRoot.string()), ResourceStatus::IoError);

    const fs::path directoryTarget = Path("directory-target");
    MakeDirectory(directoryTarget / "data.bin");
    const std::vector<ResourceFileData> direct = {{"data.bin", "data.bin", {1U}}};
    EXPECT_EQ(ResourceRegistry::WriteMaterializedFiles(direct, directoryTarget.string()), ResourceStatus::IoError);
}

TEST_F(ResourceRegistryTest, ReportsMaterializedFileFinalizeFailure)
{
    struct rlimit originalFileSizeLimit {};
    ASSERT_EQ(getrlimit(RLIMIT_FSIZE, &originalFileSizeLimit), 0);
    struct sigaction ignoreFileSizeSignal {};
    struct sigaction originalFileSizeSignal {};
    ignoreFileSizeSignal.sa_handler = SIG_IGN;
    ASSERT_EQ(sigemptyset(&ignoreFileSizeSignal.sa_mask), 0);
    ASSERT_EQ(sigaction(SIGXFSZ, &ignoreFileSizeSignal, &originalFileSizeSignal), 0);
    struct rlimit zeroFileSizeLimit = originalFileSizeLimit;
    zeroFileSizeLimit.rlim_cur = 0U;
    ASSERT_EQ(setrlimit(RLIMIT_FSIZE, &zeroFileSizeLimit), 0);

    const fs::path output = Path("materialized-finalize-failure");
    const std::vector<ResourceFileData> files = {{"data.bin", "data.bin", {1U}}};
    const ResourceStatus writeStatus = ResourceRegistry::WriteMaterializedFiles(files, output.string());

    const int restoreLimitResult = setrlimit(RLIMIT_FSIZE, &originalFileSizeLimit);
    const int restoreSignalResult = sigaction(SIGXFSZ, &originalFileSizeSignal, nullptr);
    ASSERT_EQ(restoreLimitResult, 0);
    ASSERT_EQ(restoreSignalResult, 0);
    EXPECT_EQ(writeStatus, ResourceStatus::IoError);
}

TEST_F(ResourceRegistryTest, LookupValidatesInputsAndReturnsIndependentMaterializations)
{
    ResourceData output;
    output.resourceDir = "stale";
    EXPECT_EQ(registry_->Lookup(nullptr, output), ResourceStatus::InvalidResource);
    EXPECT_TRUE(output.json.is_null());
    EXPECT_TRUE(output.resourceDir.empty());
    EXPECT_TRUE(output.sourceFilePath.empty());
    output.resourceDir = "stale";
    EXPECT_EQ(registry_->Lookup("", output), ResourceStatus::InvalidResource);
    EXPECT_TRUE(output.resourceDir.empty());
    output.resourceDir = "stale";
    EXPECT_EQ(registry_->Lookup("missing", output), ResourceStatus::NotFound);
    EXPECT_TRUE(output.resourceDir.empty());

    std::unique_ptr<ResourceEntry> entry = MakeEntry(ResourceSourceType::External);
    entry->files.push_back({"payload.bin", "nested/payload.bin", {4U, 5U, 6U}});
    const std::string resourceId = "resource/with/slash";
    registry_->externalResources_.emplace(resourceId, std::move(entry));
    ResourceData firstOutput;
    EXPECT_EQ(registry_->Lookup(resourceId.c_str(), firstOutput), ResourceStatus::Success);
    const std::string firstDirectory = firstOutput.resourceDir;
    const fs::path firstPath(firstDirectory);
    EXPECT_EQ(firstPath.filename().string().find("materialize-"), 0U);
    EXPECT_EQ(firstPath.parent_path().filename(), "external");
    EXPECT_EQ(ReadFile(fs::path(firstDirectory) / "nested/payload.bin"), std::string("\4\5\6", 3U));
    ResourceData secondOutput;
    EXPECT_EQ(registry_->Lookup(resourceId.c_str(), secondOutput), ResourceStatus::Success);
    EXPECT_EQ(firstOutput.resourceDir, firstDirectory);
    EXPECT_NE(secondOutput.resourceDir, firstDirectory);
    EXPECT_EQ(ReadFile(fs::path(firstOutput.resourceDir) / "nested/payload.bin"), std::string("\4\5\6", 3U));
    EXPECT_EQ(ReadFile(fs::path(secondOutput.resourceDir) / "nested/payload.bin"), std::string("\4\5\6", 3U));
    EXPECT_TRUE(registry_->externalResources_.at(resourceId)->data.resourceDir.empty());

    std::unique_ptr<ResourceEntry> unknown = MakeEntry(static_cast<ResourceSourceType>(99));
    registry_->builtInResources_.emplace("unknown", std::move(unknown));
    ResourceData unknownOutput;
    EXPECT_EQ(registry_->Lookup("unknown", unknownOutput), ResourceStatus::Success);
    const fs::path unknownPath(unknownOutput.resourceDir);
    EXPECT_EQ(unknownPath.filename().string().find("materialize-"), 0U);
    EXPECT_EQ(unknownPath.parent_path().filename(), "unknown");
}

TEST_F(ResourceRegistryTest, LookupReportsTemporaryRootAndMaterializationFailures)
{
    ResourceData output;
    std::unique_ptr<ResourceEntry> entry = MakeEntry(ResourceSourceType::External);
    registry_->externalResources_.emplace("bad-temp", std::move(entry));
    output.resourceDir = "stale";
    SetEnvironment("TMPDIR", Path("missing-temp").string());
    EXPECT_EQ(registry_->Lookup("bad-temp", output), ResourceStatus::IoError);
    EXPECT_TRUE(output.resourceDir.empty());

    output.resourceDir = "stale";
    SetEnvironment("TMPDIR", "/proc");
    EXPECT_EQ(registry_->Lookup("bad-temp", output), ResourceStatus::IoError);
    EXPECT_TRUE(output.resourceDir.empty());

    ASSERT_TRUE(registry_->ownedMaterializationRoot_.RegisterDirectoryForCleanup(Path("forced-materialize").string()));
    WriteFile(fs::path(registry_->ownedMaterializationRoot_.GetDirectoryPath()) / "external", "blocking file");
    output.resourceDir = "stale";
    EXPECT_EQ(registry_->Lookup("bad-temp", output), ResourceStatus::IoError);
    EXPECT_TRUE(output.resourceDir.empty());
    EXPECT_TRUE(registry_->externalResources_.at("bad-temp")->data.resourceDir.empty());
}

TEST_F(ResourceRegistryTest, DestructorHonorsSaveKernelMetaAtTemporaryRootCreation)
{
    SetEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "1");
    registry_->externalResources_.emplace("keep-temp", MakeEntry(ResourceSourceType::External));
    ResourceData output;
    EXPECT_EQ(registry_->Lookup("keep-temp", output), ResourceStatus::Success);
    const std::string retainedRoot = registry_->ownedMaterializationRoot_.GetDirectoryPath();
    ASSERT_TRUE(fs::is_directory(retainedRoot));
    registry_.reset();
    EXPECT_TRUE(fs::is_directory(retainedRoot));

    registry_.reset(new ResourceRegistry());
    SetEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "true");
    registry_->externalResources_.emplace("remove-temp", MakeEntry(ResourceSourceType::External));
    EXPECT_EQ(registry_->Lookup("remove-temp", output), ResourceStatus::Success);
    const std::string removedRoot = registry_->ownedMaterializationRoot_.GetDirectoryPath();
    ASSERT_TRUE(fs::is_directory(removedRoot));
    registry_.reset();
    EXPECT_FALSE(fs::exists(removedRoot));
}

TEST_F(ResourceRegistryTest, SaveKernelMetaTrimsWhitespaceAndKeepsFailedMaterialization)
{
    SetEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", " \t1\r\n");
    std::unique_ptr<ResourceEntry> entry = MakeEntry(ResourceSourceType::External);
    entry->files.push_back({"invalid.bin", ".", {1U}});
    registry_->externalResources_.emplace("failed-materialization", std::move(entry));

    ResourceData output;
    EXPECT_EQ(registry_->Lookup("failed-materialization", output), ResourceStatus::IoError);
    EXPECT_TRUE(output.resourceDir.empty());
    EXPECT_TRUE(registry_->retainMaterializedDirectories_);
    const fs::path retainedRoot(registry_->ownedMaterializationRoot_.GetDirectoryPath());
    const fs::path categoryParent = retainedRoot / "external";
    std::vector<fs::path> retainedCategories;
    for (fs::directory_iterator current(categoryParent), end; current != end; ++current) {
        retainedCategories.push_back(current->path());
    }
    ASSERT_EQ(retainedCategories.size(), 1U);
    EXPECT_TRUE(fs::is_directory(retainedCategories.front()));

    registry_.reset();
    EXPECT_TRUE(fs::is_directory(retainedRoot));
}

TEST_F(ResourceRegistryTest, LoadExplicitResourceEndToEnd)
{
    const SourceLayout layout = CreateLayout("public-load");
    const std::string json = R"({"resource_id":"public","source_file":"source.cpp","kind":"test"})";
    const std::string name = "payload.bin";
    const std::string relative = "assets/payload.bin";
    const std::vector<uint8_t> payload = {8U, 9U};
    AcCompileResourceFile file{StringView(name), StringView(relative), payload.data(), payload.size()};
    AcCompileResourceManifest manifest{StringView(json), &file, 1U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    gBundleHeader = &bundle.header;
    MockSuccessfulLoader();

    EXPECT_EQ(registry_->Load(layout.soPath.c_str()), ResourceStatus::Success);
    ResourceData output;
    EXPECT_EQ(registry_->Lookup("public", output), ResourceStatus::Success);
    EXPECT_EQ(output.json.at("kind"), "test");
    EXPECT_EQ(output.sourceFilePath, fs::canonical(layout.sourcePath).parent_path().string());
    EXPECT_EQ(ReadFile(fs::path(output.resourceDir) / relative), std::string("\10\11", 2U));
}

TEST_F(ResourceRegistryTest, LoadReportsDiscoveryAndDynamicLoaderFailures)
{
    const fs::path empty = Path("load-empty");
    MakeDirectory(empty);
    EXPECT_EQ(registry_->Load(Path("load-missing").string().c_str()), ResourceStatus::InvalidResource);
    EXPECT_EQ(registry_->Load(empty.string().c_str()), ResourceStatus::InvalidResource);

    const fs::path so = Path("load/libresource_compile_database.so");
    WriteFile(so);
    MOCKER(dlopen).expects(once()).will(invoke(DlopenFailure));
    MOCKER(dlerror).stubs().will(invoke(DlerrorFailure));
    EXPECT_EQ(registry_->Load(so.string().c_str()), ResourceStatus::LoadError);
}

TEST_F(ResourceRegistryTest, LoadReportsManifestFailure)
{
    const SourceLayout layout = CreateLayout("public-invalid-manifest");
    const std::string invalidJson = "{";
    AcCompileResourceManifest manifest{StringView(invalidJson), nullptr, 0U};
    AcCompileResourceBundle bundle = MakeBundle(&manifest, 1U);
    gBundleHeader = &bundle.header;
    MockSuccessfulLoader();

    EXPECT_EQ(registry_->Load(layout.soPath.c_str()), ResourceStatus::InvalidResource);
    EXPECT_EQ(registry_->FindResource("public-invalid-manifest"), nullptr);
}

TEST_F(ResourceRegistryTest, LoadRejectsEmptyResourceIdAndDoesNotCommitPartialBundle)
{
    const SourceLayout layout = CreateLayout("public-empty-resource-id");
    const std::string validJson = R"({"resource_id":"valid-resource"})";
    const std::string emptyJson = R"({"resource_id":""})";
    AcCompileResourceManifest invalidManifests[] = {
        {StringView(validJson), nullptr, 0U},
        {StringView(emptyJson), nullptr, 0U},
    };
    AcCompileResourceBundle invalidBundle = MakeBundle(invalidManifests, 2U);
    gBundleHeader = &invalidBundle.header;
    MockSuccessfulLoader();

    EXPECT_EQ(registry_->Load(layout.soPath.c_str()), ResourceStatus::InvalidResource);
    EXPECT_EQ(registry_->FindResource(""), nullptr);
    EXPECT_EQ(registry_->FindResource("valid-resource"), nullptr);

    AcCompileResourceManifest validManifest{StringView(validJson), nullptr, 0U};
    AcCompileResourceBundle validBundle = MakeBundle(&validManifest, 1U);
    gBundleHeader = &validBundle.header;
    EXPECT_EQ(registry_->Load(layout.soPath.c_str()), ResourceStatus::Success);
    ResourceData output;
    EXPECT_EQ(registry_->Lookup("valid-resource", output), ResourceStatus::Success);
}

TEST_F(ResourceRegistryTest, LoadAutomaticCachesFirstStatus)
{
    UnsetEnvironment("ASCEND_CUSTOM_OPP_PATH");
    UnsetEnvironment("ASCEND_OPP_PATH");
    EXPECT_EQ(registry_->Load(nullptr), ResourceStatus::NotFound);

    const SourceLayout layout = CreateLayout("late-custom");
    SetEnvironment("ASCEND_CUSTOM_OPP_PATH", Path("late-custom").string());
    EXPECT_EQ(registry_->Load(""), ResourceStatus::NotFound);
    EXPECT_TRUE(registry_->automaticLoadAttempted_);
    EXPECT_EQ(registry_->automaticLoadStatus_, ResourceStatus::NotFound);
    EXPECT_TRUE(fs::exists(layout.soPath));
}

TEST_F(ResourceRegistryTest, InstanceReturnsStableSingleton)
{
    EXPECT_EQ(&ResourceRegistry::Instance(), &ResourceRegistry::Instance());
}

} // namespace
} // namespace specialization_compile
} // namespace ascendc
