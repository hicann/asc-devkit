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

#include <functional>
#include <string>

#include <unistd.h>

#include "file_utils.h"
#include "nlohmann/json.hpp"
#include "resource_manifest_validator.h"
#include "test_support.h"

namespace ascendc {
namespace manifest_generator {
namespace {

using Json = nlohmann::json;
using ManifestMutation = std::function<void(Json&)>;
using asc_compile_exporter_test::ModuleTest;
using asc_compile_exporter_test::WriteTestFile;

Json MakeValidManifest()
{
    Json constant = {
        {"name", "weight"},
        {"parameter_index", Json::number_unsigned_t(0U)},
        {"byte_size", Json::number_unsigned_t(4U)},
        {"file", "${resource}/resources/weight.bin"},
        {"template", "weight_template"},
    };
    Json command = {
        {"type", "compile"},
        {"stage", Json::number_unsigned_t(0U)},
        {"cmd", Json::array(
                    {"${options:common_compile}", "${source_file_path}", "${resource}/resources/weight.bin",
                     "${env:ASCEND_HOME_PATH}/include", "${source_file_path}", "${output}/kernel.o"})},
    };
    Json object = {
        {"object_name", "kernel.o"},
        {"object_type", "basic"},
        {"commands", Json::array({command})},
        {"outputs", Json::array({"${output}/kernel.o"})},
    };
    Json kernel = {
        {"kernel_name", "test_kernel"},
        {"constant_infos", Json::array({constant})},
        {"objects", Json::array({object})},
        {"link_options", Json::array()},
    };
    return {
        {"resource_id", "test-resource"},
        {"soc_version", "Ascend910B"},
        {"source_file", "kernel.cpp"},
        {"resource_path", "resources"},
        {"options", {{"common_compile", Json::array({"-O2", "-I${resource}/resources/include"})}}},
        {"kernels", Json::array({kernel})},
    };
}

Json& Kernel(Json& manifest) { return manifest["kernels"][0U]; }

Json& Object(Json& manifest) { return Kernel(manifest)["objects"][0U]; }

Json& Command(Json& manifest) { return Object(manifest)["commands"][0U]; }

Json& Constant(Json& manifest) { return Kernel(manifest)["constant_infos"][0U]; }

class ResourceManifestValidatorTest : public ModuleTest {
protected:
    void SetUp() override
    {
        ModuleTest::SetUp();
        manifestPath_ = FileUtils::JoinPath(root_, "test_manifest.json");
        WriteTestFile(FileUtils::JoinPath(root_, "resources/weight.bin"), "weight");
        ASSERT_TRUE(FileUtils::CreateDirectories(FileUtils::JoinPath(root_, "resources/include")));
    }

    bool Validate(const Json& manifest) const { return ValidateResourceManifest(manifest, manifestPath_); }

    void ExpectInvalid(const char* scenario, const ManifestMutation& mutate) const
    {
        SCOPED_TRACE(scenario);
        Json manifest = MakeValidManifest();
        mutate(manifest);
        EXPECT_FALSE(Validate(manifest));
    }

    std::string manifestPath_;
};

TEST_F(ResourceManifestValidatorTest, AcceptsCompleteAndMinimalValidManifests)
{
    EXPECT_TRUE(Validate(MakeValidManifest()));

    Json skManifest = MakeValidManifest();
    Object(skManifest)["object_type"] = "sk";
    EXPECT_TRUE(Validate(skManifest));

    Json embeddedOutput = MakeValidManifest();
    Command(embeddedOutput)["cmd"].back() = "-o${output}/kernel.o";
    EXPECT_TRUE(Validate(embeddedOutput));

    Json minimal = MakeValidManifest();
    minimal.erase("source_file");
    minimal["resource_path"] = "";
    minimal.erase("options");
    Kernel(minimal).erase("constant_infos");
    Kernel(minimal).erase("link_options");
    Command(minimal)["cmd"] = Json::array({"kernel.cpp", "${env:ASCEND_HOME_PATH}/include", "${output}/kernel.o"});
    EXPECT_TRUE(Validate(minimal));

    Json noCommonCompile = MakeValidManifest();
    noCommonCompile["options"] = Json::object();
    Command(noCommonCompile)["cmd"].erase(Command(noCommonCompile)["cmd"].begin());
    EXPECT_TRUE(Validate(noCommonCompile));

    Json emptyCommonCompile = noCommonCompile;
    emptyCommonCompile["options"]["common_compile"] = Json::array();
    EXPECT_TRUE(Validate(emptyCommonCompile));

    Json namedOptions = MakeValidManifest();
    namedOptions["options"]["custom_compile"] =
        Json::array({"${env:CUSTOM_INCLUDE}/include", "${source_file_path}", "-I${resource}/resources/include"});
    Command(namedOptions)["cmd"][0U] = "${options:custom_compile}";
    Command(namedOptions)["cmd"].insert(Command(namedOptions)["cmd"].begin() + 1U, "${options:custom_compile}");
    EXPECT_TRUE(Validate(namedOptions));
}

TEST_F(ResourceManifestValidatorTest, RejectsInvalidTopLevelFields)
{
    ExpectInvalid("manifest is not an object", [](Json& manifest) { manifest = Json::array(); });
    ExpectInvalid("missing resource_id", [](Json& manifest) { manifest.erase("resource_id"); });
    ExpectInvalid("resource_id has wrong type", [](Json& manifest) { manifest["resource_id"] = 1U; });
    ExpectInvalid("resource_id is empty", [](Json& manifest) { manifest["resource_id"] = ""; });
    ExpectInvalid("missing soc_version", [](Json& manifest) { manifest.erase("soc_version"); });
    ExpectInvalid("soc_version has wrong type", [](Json& manifest) { manifest["soc_version"] = 1U; });
    ExpectInvalid("soc_version is empty", [](Json& manifest) { manifest["soc_version"] = ""; });
    ExpectInvalid("missing resource_path", [](Json& manifest) { manifest.erase("resource_path"); });
    ExpectInvalid("missing kernels", [](Json& manifest) { manifest.erase("kernels"); });
    ExpectInvalid("kernels has wrong type", [](Json& manifest) { manifest["kernels"] = Json::object(); });
    ExpectInvalid("kernels is empty", [](Json& manifest) { manifest["kernels"] = Json::array(); });
}

TEST_F(ResourceManifestValidatorTest, ValidatesManifestPathsAndCommonOptions)
{
    ExpectInvalid("source_file has wrong type", [](Json& manifest) { manifest["source_file"] = 1U; });
    ExpectInvalid(
        "source_file contains a directory", [](Json& manifest) { manifest["source_file"] = "src/kernel.cpp"; });
    ExpectInvalid("source_file traverses", [](Json& manifest) { manifest["source_file"] = "../kernel.cpp"; });
    ExpectInvalid("resource_path has wrong type", [](Json& manifest) { manifest["resource_path"] = 1U; });
    ExpectInvalid("resource_path is absolute", [](Json& manifest) { manifest["resource_path"] = "/resources"; });
    ExpectInvalid("resource_path traverses", [](Json& manifest) { manifest["resource_path"] = "../resources"; });
    ExpectInvalid("options has wrong type", [](Json& manifest) { manifest["options"] = Json::array(); });
    ExpectInvalid(
        "common_compile has wrong type", [](Json& manifest) { manifest["options"]["common_compile"] = "-O2"; });
    ExpectInvalid("common_compile contains non-string", [](Json& manifest) {
        manifest["options"]["common_compile"] = Json::array({1U});
    });
    ExpectInvalid("common_compile contains common marker", [](Json& manifest) {
        manifest["options"]["common_compile"] = Json::array({"${options:common_compile}"});
    });
}

TEST_F(ResourceManifestValidatorTest, ValidatesMarkersAndReferencedPaths)
{
    ExpectInvalid("command contains embedded NUL", [](Json& manifest) {
        Command(manifest)["cmd"][1U] = std::string("prefix\0suffix", 13U);
    });
    ExpectInvalid(
        "command marker is unterminated", [](Json& manifest) { Command(manifest)["cmd"][1U] = "${source_dir"; });
    ExpectInvalid(
        "command marker is unsupported", [](Json& manifest) { Command(manifest)["cmd"][1U] = "${unknown}/x"; });
    ExpectInvalid("resource marker occurs twice", [](Json& manifest) {
        Command(manifest)["cmd"][1U] = "${resource}/resources/${resource}/x";
    });
    ExpectInvalid(
        "resource marker has no relative path", [](Json& manifest) { Command(manifest)["cmd"][1U] = "${resource}"; });
    ExpectInvalid("resource path traverses", [](Json& manifest) {
        Command(manifest)["cmd"][1U] = "${resource}/resources/../outside";
    });
    ExpectInvalid("resource path is outside resource_path", [](Json& manifest) {
        Command(manifest)["cmd"][1U] = "${resource}/outside/file.cpp";
    });
    ExpectInvalid("resource marker with empty resource_path", [](Json& manifest) { manifest["resource_path"] = ""; });
    ExpectInvalid("resource target is missing", [](Json& manifest) {
        Command(manifest)["cmd"][1U] = "${resource}/resources/missing.bin";
    });
    const std::string symlinkTarget = FileUtils::JoinPath(root_, "resource-target.bin");
    const std::string symlinkPath = FileUtils::JoinPath(root_, "resources/resource-link.bin");
    WriteTestFile(symlinkTarget, "target");
    ASSERT_EQ(symlink(symlinkTarget.c_str(), symlinkPath.c_str()), 0);
    ExpectInvalid("resource target is a symlink", [&symlinkPath](Json& manifest) {
        Command(manifest)["cmd"][1U] = "${resource}/resources/" + FileUtils::FileName(symlinkPath);
    });
    ExpectInvalid("source_file_path requires source_file", [](Json& manifest) { manifest.erase("source_file"); });
    ExpectInvalid("environment marker name is invalid", [](Json& manifest) {
        Command(manifest)["cmd"][3U] = "${env:INVALID-NAME}/include";
    });
    ExpectInvalid(
        "environment marker name is empty", [](Json& manifest) { Command(manifest)["cmd"][3U] = "${env:}/include"; });
    ExpectInvalid("output has no relative path", [](Json& manifest) { Command(manifest)["cmd"].back() = "${output}"; });
    ExpectInvalid("output contains another marker", [](Json& manifest) {
        Command(manifest)["cmd"].back() = "${output}/${source_dir}/kernel.o";
    });
    ExpectInvalid(
        "output path traverses", [](Json& manifest) { Command(manifest)["cmd"].back() = "${output}/../kernel.o"; });
    ExpectInvalid("output contains a directory", [](Json& manifest) {
        Command(manifest)["cmd"].back() = "${output}/objects/kernel.o";
    });
}

TEST_F(ResourceManifestValidatorTest, ValidatesConstants)
{
    ExpectInvalid(
        "constant is not an object", [](Json& manifest) { Kernel(manifest)["constant_infos"][0U] = "weight"; });
    ExpectInvalid("constant has extra field", [](Json& manifest) { Constant(manifest)["extra"] = true; });
    ExpectInvalid("constant name is missing", [](Json& manifest) {
        Constant(manifest).erase("name");
        Constant(manifest)["extra"] = true;
    });
    ExpectInvalid("constant name has wrong type", [](Json& manifest) { Constant(manifest)["name"] = 1U; });
    ExpectInvalid("constant name is empty", [](Json& manifest) { Constant(manifest)["name"] = ""; });
    ExpectInvalid("parameter_index has wrong type", [](Json& manifest) { Constant(manifest)["parameter_index"] = -1; });
    ExpectInvalid("byte_size has wrong type", [](Json& manifest) { Constant(manifest)["byte_size"] = "4"; });
    ExpectInvalid(
        "byte_size is zero", [](Json& manifest) { Constant(manifest)["byte_size"] = Json::number_unsigned_t(0U); });
    ExpectInvalid("constant file has wrong type", [](Json& manifest) { Constant(manifest)["file"] = 1U; });
    ExpectInvalid("constant template has wrong type", [](Json& manifest) { Constant(manifest)["template"] = 1U; });
    ExpectInvalid("constant template is empty", [](Json& manifest) { Constant(manifest)["template"] = ""; });
    ExpectInvalid(
        "constant file lacks resource prefix", [](Json& manifest) { Constant(manifest)["file"] = "resources/x"; });
    ExpectInvalid("constant file is outside resource_path", [](Json& manifest) {
        Constant(manifest)["file"] = "${resource}/outside/weight.bin";
    });
    ExpectInvalid("constant name is duplicated", [](Json& manifest) {
        Kernel(manifest)["constant_infos"].push_back(Constant(manifest));
    });
}

TEST_F(ResourceManifestValidatorTest, ValidatesKernelStructure)
{
    ExpectInvalid("kernel is not an object", [](Json& manifest) { manifest["kernels"][0U] = "kernel"; });
    ExpectInvalid("kernel name is missing", [](Json& manifest) { Kernel(manifest).erase("kernel_name"); });
    ExpectInvalid("kernel name has wrong type", [](Json& manifest) { Kernel(manifest)["kernel_name"] = 1U; });
    ExpectInvalid("kernel name is empty", [](Json& manifest) { Kernel(manifest)["kernel_name"] = ""; });
    ExpectInvalid(
        "constant_infos has wrong type", [](Json& manifest) { Kernel(manifest)["constant_infos"] = Json::object(); });
    ExpectInvalid("objects is missing", [](Json& manifest) { Kernel(manifest).erase("objects"); });
    ExpectInvalid("objects has wrong type", [](Json& manifest) { Kernel(manifest)["objects"] = Json::object(); });
    ExpectInvalid("objects is empty", [](Json& manifest) { Kernel(manifest)["objects"] = Json::array(); });
    ExpectInvalid(
        "link_options has wrong type", [](Json& manifest) { Kernel(manifest)["link_options"] = Json::object(); });
    ExpectInvalid("kernel name is duplicated", [](Json& manifest) { manifest["kernels"].push_back(Kernel(manifest)); });
}

TEST_F(ResourceManifestValidatorTest, ValidatesObjectStructureAndOutputs)
{
    ExpectInvalid("object is not an object", [](Json& manifest) { Kernel(manifest)["objects"][0U] = "object"; });
    ExpectInvalid("object name is missing", [](Json& manifest) { Object(manifest).erase("object_name"); });
    ExpectInvalid("object name has wrong type", [](Json& manifest) { Object(manifest)["object_name"] = 1U; });
    ExpectInvalid("object name is empty", [](Json& manifest) { Object(manifest)["object_name"] = ""; });
    ExpectInvalid("object type is missing", [](Json& manifest) { Object(manifest).erase("object_type"); });
    ExpectInvalid("object type has wrong type", [](Json& manifest) { Object(manifest)["object_type"] = 1U; });
    ExpectInvalid("commands is missing", [](Json& manifest) { Object(manifest).erase("commands"); });
    ExpectInvalid("commands has wrong type", [](Json& manifest) { Object(manifest)["commands"] = Json::object(); });
    ExpectInvalid("commands is empty", [](Json& manifest) { Object(manifest)["commands"] = Json::array(); });
    ExpectInvalid("outputs is missing", [](Json& manifest) { Object(manifest).erase("outputs"); });
    ExpectInvalid("outputs has wrong type", [](Json& manifest) { Object(manifest)["outputs"] = Json::object(); });
    ExpectInvalid("outputs is empty", [](Json& manifest) { Object(manifest)["outputs"] = Json::array(); });
    ExpectInvalid(
        "object name contains a path", [](Json& manifest) { Object(manifest)["object_name"] = "obj/kernel.o"; });
    ExpectInvalid(
        "object name is duplicated", [](Json& manifest) { Kernel(manifest)["objects"].push_back(Object(manifest)); });
    ExpectInvalid("object output is not a string", [](Json& manifest) { Object(manifest)["outputs"][0U] = 1U; });
    ExpectInvalid(
        "object output has invalid syntax", [](Json& manifest) { Object(manifest)["outputs"][0U] = "kernel.o"; });
    ExpectInvalid("object output is not referenced", [](Json& manifest) {
        Object(manifest)["outputs"][0U] = "${output}/other.o";
    });
}

TEST_F(ResourceManifestValidatorTest, ValidatesCommands)
{
    ExpectInvalid("command is not an object", [](Json& manifest) { Object(manifest)["commands"][0U] = "compile"; });
    ExpectInvalid("command type is missing", [](Json& manifest) { Command(manifest).erase("type"); });
    ExpectInvalid("command type has wrong type", [](Json& manifest) { Command(manifest)["type"] = 1U; });
    ExpectInvalid("command stage is missing", [](Json& manifest) { Command(manifest).erase("stage"); });
    ExpectInvalid("command stage has wrong type", [](Json& manifest) { Command(manifest)["stage"] = -1; });
    ExpectInvalid("command cmd is missing", [](Json& manifest) { Command(manifest).erase("cmd"); });
    ExpectInvalid("command cmd has wrong type", [](Json& manifest) { Command(manifest)["cmd"] = Json::object(); });
    ExpectInvalid("command option is not a string", [](Json& manifest) { Command(manifest)["cmd"][0U] = 1U; });
    ExpectInvalid("common marker references empty options", [](Json& manifest) {
        manifest["options"]["common_compile"] = Json::array();
    });
    ExpectInvalid("options marker references missing field", [](Json& manifest) {
        Command(manifest)["cmd"][0U] = "${options:missing_compile}";
    });
    ExpectInvalid("options marker has invalid name", [](Json& manifest) {
        Command(manifest)["cmd"][0U] = "${options:invalid-name}";
    });
    ExpectInvalid("options marker is embedded", [](Json& manifest) {
        Command(manifest)["cmd"][0U] = "prefix${options:common_compile}";
    });
}

} // namespace
} // namespace manifest_generator
} // namespace ascendc
