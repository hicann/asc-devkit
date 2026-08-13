/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the LICENSE.
 */

#include <gtest/gtest.h>

#include "bundle_source_generator.h"
#include "file_utils.h"
#include "test_support.h"

namespace ascendc {
namespace manifest_generator {
namespace {

using asc_compile_exporter_test::ModuleTest;
using asc_compile_exporter_test::ReadTestFile;

TEST_F(ModuleTest, SourceGeneratorEmitsBundleManifestAbiAndMakefile)
{
    const std::string work = FileUtils::JoinPath(root_, "work");
    ASSERT_TRUE(FileUtils::CreateDirectories(work));
    const std::vector<ManifestUnit> units = {
        {"{\"base_dir\":\"resources\"}", {{"kernel.bin", "resources/kernel.bin", {0U, 127U, 255U}}}}};

    ASSERT_TRUE(BundleSourceGenerator(work, units, BundleOutputKind::kSharedObject).Generate());

    const std::string abiHeader = ReadTestFile(FileUtils::JoinPath(work, "ascendc_manifest_abi.h"));
    const std::string bundleSource = ReadTestFile(FileUtils::JoinPath(work, "compile_resource_bundle.cpp"));
    const std::string manifestSource = ReadTestFile(FileUtils::JoinPath(work, "manifest_0.cpp"));
    const std::string makefile = ReadTestFile(FileUtils::JoinPath(work, "Makefile"));
    EXPECT_NE(abiHeader.find("AC_COMPILE_RESOURCE_MAGIC"), std::string::npos);
    EXPECT_NE(abiHeader.find("typedef struct AcCompileResourceBundle"), std::string::npos);
    EXPECT_NE(abiHeader.find("AscendcGetCompileResourceBundle(void)"), std::string::npos);
    EXPECT_NE(bundleSource.find("#include \"ascendc_manifest_abi.h\""), std::string::npos);
    EXPECT_NE(bundleSource.find("GetManifest0()"), std::string::npos);
    EXPECT_NE(bundleSource.find("1ULL, nullptr, 0ULL"), std::string::npos);
    EXPECT_NE(manifestSource.find("#include \"ascendc_manifest_abi.h\""), std::string::npos);
    EXPECT_NE(manifestSource.find("0, 127, 255"), std::string::npos);
    EXPECT_NE(manifestSource.find("g_manifest_0_file_0_path_bytes"), std::string::npos);
    EXPECT_NE(manifestSource.find("g_manifest_0_file_0_path ="), std::string::npos);
    EXPECT_NE(manifestSource.find("20ULL"), std::string::npos);
    EXPECT_NE(makefile.find("manifest_0.cpp"), std::string::npos);
    EXPECT_EQ(makefile.find("ASCENDC_BUNDLE_INCLUDE_DIR"), std::string::npos);
    EXPECT_NE(makefile.find("-std=c++11"), std::string::npos);
    EXPECT_NE(makefile.find("-shared $(OBJECTS)"), std::string::npos);
    EXPECT_NE(makefile.find("-Wl,-z,relro,-z,now,-z,noexecstack"), std::string::npos);
    EXPECT_TRUE(BundleSourceGenerator(work, units, BundleOutputKind::kSharedObject).Generate());
}

TEST_F(ModuleTest, SourceGeneratorEmitsRelocatableObjectMakefile)
{
    const std::string work = FileUtils::JoinPath(root_, "relocatable-object-work");
    ASSERT_TRUE(FileUtils::CreateDirectories(work));
    const std::vector<ManifestUnit> units = {{"{\"base_dir\":\"resources\"}", {}}};

    ASSERT_TRUE(BundleSourceGenerator(work, units, BundleOutputKind::kRelocatableObject).Generate());

    const std::string makefile = ReadTestFile(FileUtils::JoinPath(work, "Makefile"));
    EXPECT_NE(makefile.find("-r $(OBJECTS)"), std::string::npos);
    EXPECT_EQ(makefile.find("-shared"), std::string::npos);
    EXPECT_EQ(makefile.find("-Wl,-z,relro,-z,now,-z,noexecstack"), std::string::npos);
    EXPECT_NE(makefile.find("-std=c++11 -fPIC -fvisibility=hidden"), std::string::npos);
}

TEST_F(ModuleTest, SourceGeneratorEmitsManifestWithoutResourceFiles)
{
    const std::string work = FileUtils::JoinPath(root_, "empty-resource-work");
    ASSERT_TRUE(FileUtils::CreateDirectories(work));
    const std::vector<ManifestUnit> units = {{"{\"base_dir\":\"resources\"}", {}}};

    ASSERT_TRUE(BundleSourceGenerator(work, units, BundleOutputKind::kSharedObject).Generate());

    const std::string manifestSource = ReadTestFile(FileUtils::JoinPath(work, "manifest_0.cpp"));
    EXPECT_NE(manifestSource.find("nullptr, 0ULL"), std::string::npos);
    EXPECT_EQ(manifestSource.find("AcCompileResourceFile g_manifest_0_files"), std::string::npos);
}

TEST_F(ModuleTest, SourceGeneratorReportsFailureAtEachGenerationStage)
{
    const std::vector<ManifestUnit> units = {{"{\"base_dir\":\"resources\"}", {}}};

    const std::string bundleWork = FileUtils::JoinPath(root_, "bundle-failure-work");
    ASSERT_TRUE(FileUtils::CreateDirectories(bundleWork));
    ASSERT_TRUE(FileUtils::CreateDirectories(FileUtils::JoinPath(bundleWork, "compile_resource_bundle.cpp")));
    EXPECT_FALSE(BundleSourceGenerator(bundleWork, units, BundleOutputKind::kSharedObject).Generate());

    const std::string manifestWork = FileUtils::JoinPath(root_, "manifest-failure-work");
    ASSERT_TRUE(FileUtils::CreateDirectories(manifestWork));
    ASSERT_TRUE(FileUtils::CreateDirectories(FileUtils::JoinPath(manifestWork, "manifest_0.cpp")));
    EXPECT_FALSE(BundleSourceGenerator(manifestWork, units, BundleOutputKind::kSharedObject).Generate());

    const std::string makefileWork = FileUtils::JoinPath(root_, "makefile-failure-work");
    ASSERT_TRUE(FileUtils::CreateDirectories(makefileWork));
    ASSERT_TRUE(FileUtils::CreateDirectories(FileUtils::JoinPath(makefileWork, "Makefile")));
    EXPECT_FALSE(BundleSourceGenerator(makefileWork, units, BundleOutputKind::kSharedObject).Generate());
}

} // namespace
} // namespace manifest_generator
} // namespace ascendc
