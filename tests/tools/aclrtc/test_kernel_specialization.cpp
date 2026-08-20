/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "acl/acl_rt_compile.h"
#include "specialization/kernel_specialization_types.h"

#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#define ACLRTC_STRINGIFY_IMPL(value) #value
#define ACLRTC_STRINGIFY(value) ACLRTC_STRINGIFY_IMPL(value)

namespace {
constexpr aclError ACL_ERROR_RTC_INVALID_INPUT = 176001;
constexpr aclError ACL_ERROR_RTC_INVALID_OPTION = 176002;
constexpr char RESOURCE_ID[] = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

aclrtcKernelSpecRequest CreateValidRequest()
{
    aclrtcKernelSpecRequest request{};
    request.resourceId = RESOURCE_ID;
    request.kernelEntry = "add_custom_100000";
    return request;
}

TEST(KernelSpecializationApiTest, RequestLayoutIsStableAndExtensible)
{
    static_assert(std::is_standard_layout_v<aclrtcKernelSpecRequest>);
    static_assert(std::extent_v<decltype(aclrtcKernelSpecRequest::reserved)> == 256);
    static_assert(std::is_same_v<decltype(aclrtcKernelSpecRequest::argsCount), uint64_t>);
    static_assert(std::is_same_v<decltype(aclrtcKernelSpecRequest::optionCount), uint64_t>);
    static_assert(std::is_same_v<decltype(aclrtcKernelSpecRequest::skOptionCount), uint64_t>);
    static_assert(offsetof(aclrtcKernelSpecRequest, resourceId) == 0U);
    static_assert(offsetof(aclrtcKernelSpecRequest, kernelEntry) == 8U);
    static_assert(offsetof(aclrtcKernelSpecRequest, argsCount) == 16U);
    static_assert(offsetof(aclrtcKernelSpecRequest, options) == 40U);
    static_assert(offsetof(aclrtcKernelSpecRequest, skOptions) == 56U);
    static_assert(offsetof(aclrtcKernelSpecRequest, reserved) == 72U);
    static_assert(sizeof(aclrtcKernelSpecRequest) == 328U);
}

TEST(KernelSpecializationApiTest, PublicApiVisibilityMacroIsEmptyForConsumers)
{
    EXPECT_STREQ(ACLRTC_STRINGIFY(ACLRTC_API), "");
}

TEST(KernelSpecializationRequestTest, NormalizesBasicRequestAndCopiesOptions)
{
    const char* options[] = {"-g", "-DVALUE=1"};
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.options = options;
    request.optionCount = 2;

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest), 0);
    EXPECT_EQ(normalizedRequest.resourceId, RESOURCE_ID);
    EXPECT_EQ(normalizedRequest.kernelName, "add_custom_100000");
    EXPECT_EQ(normalizedRequest.outputElfPath, "/tmp/kernel.elf");
    EXPECT_EQ(normalizedRequest.compilationVariant, ascendc::aclrtc::CompilationVariant::Basic);
    EXPECT_EQ(normalizedRequest.compilerOptions.basicOptions, (std::vector<std::string>{"-g", "-DVALUE=1"}));
    EXPECT_TRUE(normalizedRequest.compilerOptions.superKernelOptions.empty());
}

TEST(KernelSpecializationRequestTest, AcceptsOpaqueResourceIdAndNormalizesRelativeOutputPath)
{
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.resourceId = "vendor/operator/resource-v1";

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    ASSERT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "relative/kernel.elf", normalizedRequest), 0);
    EXPECT_EQ(normalizedRequest.resourceId, request.resourceId);
    EXPECT_EQ(normalizedRequest.outputElfPath, boost::filesystem::absolute("relative/kernel.elf").lexically_normal());
}

TEST(KernelSpecializationRequestTest, EnablesSuperKernelAndRemovesControlOption)
{
    const char* skOptions[] = {"-g", "--enable-super-kernel", "-DDCCI_OFF"};
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.skOptions = skOptions;
    request.skOptionCount = 3;

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest), 0);
    EXPECT_EQ(normalizedRequest.compilationVariant, ascendc::aclrtc::CompilationVariant::BasicWithSuperKernel);
    EXPECT_EQ(normalizedRequest.compilerOptions.superKernelOptions, (std::vector<std::string>{"-g", "-DDCCI_OFF"}));
}

TEST(KernelSpecializationRequestTest, IgnoresSuperKernelOptionsWithoutEnable)
{
    const char* skOptions[] = {"-g", "-oignored"};
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.skOptions = skOptions;
    request.skOptionCount = 2;

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest), 0);
    EXPECT_EQ(normalizedRequest.compilationVariant, ascendc::aclrtc::CompilationVariant::Basic);
    EXPECT_TRUE(normalizedRequest.compilerOptions.superKernelOptions.empty());
}

TEST(KernelSpecializationRequestTest, RejectsInvalidAbiFields)
{
    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;

    aclrtcKernelSpecRequest request = CreateValidRequest();
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);

    request = CreateValidRequest();
    request.resourceId = "";
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);

    request = CreateValidRequest();
    request.reserved[1] = 1;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);

    request = CreateValidRequest();
    request.optionCount = 1;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);
}

TEST(KernelSpecializationRequestTest, RejectsSpecificRequestValidationFailures)
{
    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;

    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.reserved[7] = 1U;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);

    request = CreateValidRequest();
    request.optionCount = 1U;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);

    const char* options[] = {"-o"};
    request = CreateValidRequest();
    request.options = options;
    request.optionCount = 1U;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_OPTION);
}

TEST(KernelSpecializationRequestTest, RejectsInvalidRequestInputs)
{
    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;

    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.resourceId = nullptr;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);

    request = CreateValidRequest();
    request.kernelEntry = "";
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);

    const char* nullOption[] = {nullptr};
    request = CreateValidRequest();
    request.options = nullOption;
    request.optionCount = 1U;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_OPTION);

    const char* optionWithControlByte[] = {"-DVALUE=line\nbreak"};
    request = CreateValidRequest();
    request.options = optionWithControlByte;
    request.optionCount = 1U;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_OPTION);

    request = CreateValidRequest();
    request.skOptions = nullOption;
    request.skOptionCount = 1U;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_OPTION);
}

TEST(KernelSpecializationRequestTest, RejectsCompilerOptionThatExceedsIndividualTextLimit)
{
    const std::string oversizedOption(1024U * 1024U + 1U, 'x');
    const char* options[] = {oversizedOption.c_str()};
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.options = options;
    request.optionCount = 1U;

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_OPTION);
}

TEST(KernelSpecializationRequestTest, RejectsCompilerOptionsThatExceedCombinedTextLimit)
{
    const std::string maximumLengthOption(1024U * 1024U, 'x');
    const char* options[] = {
        maximumLengthOption.c_str(), maximumLengthOption.c_str(), maximumLengthOption.c_str(),
        maximumLengthOption.c_str(), maximumLengthOption.c_str()};
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.options = options;
    request.optionCount = 5U;

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_OPTION);
}

TEST(KernelSpecializationRequestTest, RejectsTooManyCompilerOptions)
{
    const std::vector<const char*> options(4097U, "-g");
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.options = options.data();
    request.optionCount = options.size();

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_INPUT);
}

TEST(KernelSpecializationRequestTest, RejectsOptionsThatCanRedirectCompilerInputsOrOutputs)
{
    const std::array<const char*, 6> invalidOptions = {"@response.rsp",  "-o",         "-onew.o", "--output",
                                                       "--output=new.o", "line\nbreak"};

    for (const char* invalidOption : invalidOptions) {
        SCOPED_TRACE(invalidOption);
        const char* options[] = {invalidOption};
        aclrtcKernelSpecRequest request = CreateValidRequest();
        request.options = options;
        request.optionCount = 1;
        ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
        EXPECT_EQ(
            ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
            ACL_ERROR_RTC_INVALID_OPTION);
    }
}

TEST(KernelSpecializationRequestTest, RejectsSuperKernelOptionsThatRedirectCompilerOutputWhenForwarded)
{
    const char* skOptions[] = {"--enable-super-kernel", "-o", "unexpected.elf"};
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.skOptions = skOptions;
    request.skOptionCount = 3U;

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_OPTION);
}

TEST(KernelSpecializationRequestTest, TreatsShellMetacharactersAsLiteralArgvText)
{
    const char* options[] = {"-DVALUE=$(touch /tmp/not-executed);still-literal"};
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.options = options;
    request.optionCount = 1;

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest), 0);
    ASSERT_EQ(normalizedRequest.compilerOptions.basicOptions.size(), 1U);
    EXPECT_EQ(normalizedRequest.compilerOptions.basicOptions[0], options[0]);
}

TEST(KernelSpecializationRequestTest, RejectsDuplicateSuperKernelEnableOption)
{
    const char* skOptions[] = {"--enable-super-kernel", "--enable-super-kernel"};
    aclrtcKernelSpecRequest request = CreateValidRequest();
    request.skOptions = skOptions;
    request.skOptionCount = 2;

    ascendc::aclrtc::NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, "/tmp/kernel.elf", normalizedRequest),
        ACL_ERROR_RTC_INVALID_OPTION);
}
} // namespace
