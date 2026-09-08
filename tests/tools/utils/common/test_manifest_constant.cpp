/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "manifest_constant.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <limits>
#include <vector>

namespace {
using Json = nlohmann::json;
using ascendc::ManifestConstant;

Json MakeConstant()
{
    return {{"name", "tiling"}, {"parameter_index", 2}, {"file", "${resource}/tiling.h"}, {"template", "@@TILING@@"}};
}

TEST(ManifestConstantTest, ResolvesDefaultAndExplicitStructSizes)
{
    for (bool explicitType : {false, true}) {
        Json input = MakeConstant();
        if (explicitType) {
            input["arg_type"] = "struct";
        }
        ManifestConstant constant;
        ASSERT_TRUE(constant.ParseDefinition(input));
        input.clear();
        EXPECT_EQ(constant.GetName(), "tiling");
        EXPECT_EQ(constant.GetParameterIndex(), 2U);
        EXPECT_EQ(constant.GetTargetFile(), "${resource}/tiling.h");
        EXPECT_EQ(constant.GetTemplateText(), "@@TILING@@");
        uint8_t byte = 0;
        const void* addresses[] = {nullptr, nullptr, &byte};
        const uint64_t sizes[] = {0U, 0U, 17U};
        ASSERT_TRUE(constant.BindArgument(3U, addresses, sizes));
        EXPECT_EQ(constant.GetBoundData(), &byte);
        EXPECT_EQ(constant.GetBoundByteSize(), 17U);
    }
}

TEST(ManifestConstantTest, PreservesSignedAndUnsignedPointerSizeBoundaries)
{
    for (const Json& size : std::vector<Json>{1, int64_t{4}, std::numeric_limits<uint64_t>::max()}) {
        Json input = MakeConstant();
        input["arg_type"] = "pointer";
        input["byte_size"] = size;
        input["parameter_index"] = std::numeric_limits<uint64_t>::max();
        ManifestConstant constant;
        ASSERT_TRUE(constant.ParseDefinition(input));
        EXPECT_EQ(constant.GetParameterIndex(), std::numeric_limits<uint64_t>::max());
        input["parameter_index"] = 0;
        ASSERT_TRUE(constant.ParseDefinition(input));
        uint8_t byte = 0;
        const void* addresses[] = {&byte};
        ASSERT_TRUE(constant.BindArgument(1U, addresses, nullptr));
        EXPECT_EQ(constant.GetBoundByteSize(), size.get<uint64_t>());
    }
}

TEST(ManifestConstantTest, RejectsInvalidFieldsWithoutChangingParsedConstant)
{
    std::vector<Json> inputs{nullptr, Json::array(), 1};
    for (const char* field : {"name", "parameter_index", "file", "template"}) {
        Json input = MakeConstant();
        input.erase(field);
        inputs.push_back(input);
    }
    for (const Json& fields : std::vector<Json>{
             {{"name", ""}},
             {{"name", 1}},
             {{"file", ""}},
             {{"file", nullptr}},
             {{"template", ""}},
             {{"template", true}},
             {{"parameter_index", -1}},
             {{"parameter_index", 1.5}},
             {{"parameter_index", "2"}},
             {{"parameter_index", true}},
             {{"arg_type", nullptr}},
             {{"arg_type", "other"}},
             {{"extra", 1}},
             {{"byte_size", 4}},
             {{"arg_type", "struct"}, {"byte_size", 4}},
             {{"arg_type", "pointer"}},
             {{"arg_type", "pointer"}, {"byte_size", 0}},
             {{"arg_type", "pointer"}, {"byte_size", -1}},
             {{"arg_type", "pointer"}, {"byte_size", 1.5}},
             {{"arg_type", "pointer"}, {"byte_size", "4"}},
             {{"arg_type", "pointer"}, {"byte_size", true}}}) {
        Json input = MakeConstant();
        input.update(fields);
        inputs.push_back(input);
    }
    ManifestConstant constant;
    Json valid = MakeConstant();
    valid.update({{"arg_type", "pointer"}, {"byte_size", 7}});
    ASSERT_TRUE(constant.ParseDefinition(valid));
    uint8_t byte = 0;
    const void* addresses[] = {nullptr, nullptr, &byte};
    ASSERT_TRUE(constant.BindArgument(3U, addresses, nullptr));
    for (const Json& input : inputs) {
        SCOPED_TRACE(input.dump());
        EXPECT_FALSE(constant.ParseDefinition(input));
        EXPECT_EQ(constant.GetName(), "tiling");
        EXPECT_EQ(constant.GetParameterIndex(), 2U);
        EXPECT_EQ(constant.GetTargetFile(), "${resource}/tiling.h");
        EXPECT_EQ(constant.GetTemplateText(), "@@TILING@@");
        EXPECT_EQ(constant.GetBoundData(), &byte);
        EXPECT_EQ(constant.GetBoundByteSize(), 7U);
    }
}
TEST(ManifestConstantTest, RejectsBindingBeforeParsingAndResetsBindingAfterSuccessfulParse)
{
    ManifestConstant constant;
    uint8_t first = 1;
    uint8_t second = 2;
    const void* addresses[] = {nullptr, nullptr, &first};
    uint64_t sizes[] = {0U, 0U, 1U};
    EXPECT_FALSE(constant.BindArgument(3U, addresses, sizes));
    EXPECT_EQ(constant.GetBoundData(), nullptr);
    EXPECT_EQ(constant.GetBoundByteSize(), 0U);
    EXPECT_FALSE(constant.ParseDefinition(Json::object()));
    ASSERT_TRUE(constant.ParseDefinition(MakeConstant()));
    ASSERT_TRUE(constant.BindArgument(3U, addresses, sizes));
    addresses[2] = &second;
    sizes[2] = 2U;
    ASSERT_TRUE(constant.BindArgument(3U, addresses, sizes));
    EXPECT_EQ(constant.GetBoundData(), &second);
    EXPECT_EQ(constant.GetBoundByteSize(), 2U);
    ASSERT_TRUE(constant.ParseDefinition(MakeConstant()));
    EXPECT_EQ(constant.GetBoundData(), nullptr);
    EXPECT_EQ(constant.GetBoundByteSize(), 0U);
    ASSERT_TRUE(constant.BindArgument(3U, addresses, sizes));
}

TEST(ManifestConstantTest, RejectsInvalidArgumentsWithoutChangingBinding)
{
    ManifestConstant constant;
    ASSERT_TRUE(constant.ParseDefinition(MakeConstant()));
    uint8_t byte = 0;
    const void* addresses[] = {nullptr, nullptr, &byte};
    const void* nullAddresses[] = {nullptr, nullptr, nullptr};
    const uint64_t sizes[] = {0U, 0U, 1U};
    const uint64_t zeroSizes[] = {0U, 0U, 0U};
    ASSERT_TRUE(constant.BindArgument(3U, addresses, sizes));
    EXPECT_FALSE(constant.BindArgument(3U, nullptr, sizes));
    EXPECT_FALSE(constant.BindArgument(2U, addresses, sizes));
    EXPECT_FALSE(constant.BindArgument(3U, nullAddresses, sizes));
    EXPECT_FALSE(constant.BindArgument(3U, addresses, nullptr));
    EXPECT_FALSE(constant.BindArgument(3U, addresses, zeroSizes));
    EXPECT_EQ(constant.GetBoundData(), &byte);
    EXPECT_EQ(constant.GetBoundByteSize(), 1U);
}
} // namespace
