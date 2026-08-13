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

#include "manifest_bundle_types.h"

namespace ascendc {
namespace manifest_generator {
namespace {

TEST(ManifestBundleTypesTest, StoresManifestAndResourceFields)
{
    const ResourceFile resource = {"kernel.cpp", "resources/kernel.cpp", {1U, 2U, 3U}};
    const ManifestUnit manifest = {"{\"base_dir\":\"resources\"}", {resource}};

    EXPECT_EQ(resource.fileName, "kernel.cpp");
    EXPECT_EQ(resource.filePath, "resources/kernel.cpp");
    EXPECT_EQ(resource.data.size(), 3U);
    EXPECT_EQ(manifest.files.size(), 1U);
    EXPECT_EQ(manifest.files[0].fileName, resource.fileName);
}

} // namespace
} // namespace manifest_generator
} // namespace ascendc
