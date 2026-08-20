/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_specialization_types.h"

#include "ascendc_tool_log.h"

#include <algorithm>
#include <atomic>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <cstring>
#include <unistd.h>

namespace ascendc {
namespace aclrtc {
namespace {
constexpr size_t MAX_REQUEST_STRING_BYTES = 1024U * 1024U;
constexpr size_t MAX_USER_OPTION_COUNT = 4096U;
constexpr size_t MAX_REQUEST_TEXT_BYTES = 4U * 1024U * 1024U;
constexpr char SUPER_KERNEL_ENABLE_OPTION[] = "--enable-super-kernel";

bool CopyRequiredBoundedCString(
    const char* input, const char* inputLocation, std::string& output, size_t& copiedRequestTextByteCount)
{
    if (input == nullptr) {
        ASCENDLOGE("%s must not be null", inputLocation);
        return false;
    }
    const size_t inputLength = strnlen(input, MAX_REQUEST_STRING_BYTES + 1U);
    if (inputLength == 0U) {
        ASCENDLOGE("%s must not be empty", inputLocation);
        return false;
    }
    if (inputLength > MAX_REQUEST_STRING_BYTES) {
        ASCENDLOGE("%s must be NUL-terminated within %zu bytes", inputLocation, MAX_REQUEST_STRING_BYTES);
        return false;
    }
    if (inputLength > MAX_REQUEST_TEXT_BYTES - copiedRequestTextByteCount) {
        ASCENDLOGE(
            "%s would exceed the %zu-byte combined request text limit: already_copied=%zu field_bytes=%zu; reduce "
            "the number or length of compiler options",
            inputLocation, MAX_REQUEST_TEXT_BYTES, copiedRequestTextByteCount, inputLength);
        return false;
    }
    output.assign(input, inputLength);
    copiedRequestTextByteCount += inputLength;
    return true;
}

bool FindAsciiControlCharacter(
    const std::string& compilerOption, size_t& controlBytePosition, unsigned char& controlByteValue)
{
    for (size_t bytePosition = 0U; bytePosition < compilerOption.size(); ++bytePosition) {
        const unsigned char byteValue = static_cast<unsigned char>(compilerOption[bytePosition]);
        if (byteValue < 0x20U || byteValue == 0x7fU) {
            controlBytePosition = bytePosition;
            controlByteValue = byteValue;
            return true;
        }
    }
    return false;
}

bool OverridesCompilerOutputPath(const std::string& compilerOption)
{
    return compilerOption == "-o" || compilerOption == "--output" ||
           (compilerOption.size() > 2U && compilerOption.compare(0, 2, "-o") == 0) ||
           compilerOption.compare(0, sizeof("--output=") - 1U, "--output=") == 0;
}

aclError ValidateCompilerOptionCharacters(
    const std::string& compilerOption, const char* optionFieldName, uint64_t optionIndex)
{
    size_t controlBytePosition = 0U;
    unsigned char controlByteValue = 0U;
    if (FindAsciiControlCharacter(compilerOption, controlBytePosition, controlByteValue)) {
        ASCENDLOGE(
            "%s[%llu] contains prohibited control byte 0x%02x at byte %zu; pass each compiler option as one "
            "plain argv string without control characters",
            optionFieldName, static_cast<unsigned long long>(optionIndex), static_cast<unsigned int>(controlByteValue),
            controlBytePosition);
        return ACLRTC_ERROR_INVALID_OPTION;
    }
    return ACLRTC_SUCCESS;
}

aclError ValidateForwardedCompilerOptionDoesNotRedirectFiles(
    const std::string& compilerOption, const char* optionFieldName, uint64_t optionIndex)
{
    if (compilerOption.front() == '@') {
        ASCENDLOGE(
            "%s[%llu] cannot use a compiler response file; pass its contents as separate %s entries", optionFieldName,
            static_cast<unsigned long long>(optionIndex), optionFieldName);
        return ACLRTC_ERROR_INVALID_OPTION;
    }
    if (OverridesCompilerOutputPath(compilerOption)) {
        ASCENDLOGE(
            "%s[%llu] cannot override compiler output; remove this option because aclrtc manages the output path",
            optionFieldName, static_cast<unsigned long long>(optionIndex));
        return ACLRTC_ERROR_INVALID_OPTION;
    }
    return ACLRTC_SUCCESS;
}

aclError CopyCompilerOptionsAndRejectControlCharacters(
    const char* const* apiOptions, uint64_t apiOptionCount, const char* optionFieldName,
    const char* optionCountFieldName, std::vector<std::string>& copiedOptions, size_t& copiedRequestTextByteCount)
{
    if (apiOptionCount == 0U) {
        return ACLRTC_SUCCESS;
    }
    if (apiOptions == nullptr) {
        ASCENDLOGE(
            "%s must not be null when %s=%llu; provide %llu entries or set %s to 0", optionFieldName,
            optionCountFieldName, static_cast<unsigned long long>(apiOptionCount),
            static_cast<unsigned long long>(apiOptionCount), optionCountFieldName);
        return ACLRTC_ERROR_INVALID_INPUT;
    }

    copiedOptions.reserve(static_cast<size_t>(apiOptionCount));
    for (uint64_t optionIndex = 0; optionIndex < apiOptionCount; ++optionIndex) {
        std::string copiedOption;
        const std::string optionLocation = std::string(optionFieldName) + "[" + std::to_string(optionIndex) + "]";
        if (!CopyRequiredBoundedCString(
                apiOptions[optionIndex], optionLocation.c_str(), copiedOption, copiedRequestTextByteCount)) {
            return ACLRTC_ERROR_INVALID_OPTION;
        }
        const aclError validationResult = ValidateCompilerOptionCharacters(copiedOption, optionFieldName, optionIndex);
        if (validationResult != ACLRTC_SUCCESS) {
            return validationResult;
        }
        copiedOptions.emplace_back(std::move(copiedOption));
    }
    return ACLRTC_SUCCESS;
}

aclError ValidateForwardedCompilerOptionsDoNotRedirectFiles(
    const std::vector<std::string>& compilerOptions, const char* optionFieldName)
{
    for (size_t optionIndex = 0U; optionIndex < compilerOptions.size(); ++optionIndex) {
        const aclError validationResult = ValidateForwardedCompilerOptionDoesNotRedirectFiles(
            compilerOptions[optionIndex], optionFieldName, optionIndex);
        if (validationResult != ACLRTC_SUCCESS) {
            return validationResult;
        }
    }
    return ACLRTC_SUCCESS;
}

aclError NormalizeBasicCompilerOptions(
    const aclrtcKernelSpecRequest& apiRequest, NormalizedKernelSpecializationRequest& normalizedRequest,
    size_t& copiedRequestTextByteCount)
{
    aclError result = CopyCompilerOptionsAndRejectControlCharacters(
        apiRequest.options, apiRequest.optionCount, "options", "optionCount",
        normalizedRequest.compilerOptions.basicOptions, copiedRequestTextByteCount);
    if (result != ACLRTC_SUCCESS) {
        return result;
    }
    return ValidateForwardedCompilerOptionsDoNotRedirectFiles(
        normalizedRequest.compilerOptions.basicOptions, "options");
}

aclError NormalizeSuperKernelCompilerOptions(
    const aclrtcKernelSpecRequest& apiRequest, NormalizedKernelSpecializationRequest& normalizedRequest,
    size_t& copiedRequestTextByteCount)
{
    std::vector<std::string> copiedSuperKernelOptions;
    aclError result = CopyCompilerOptionsAndRejectControlCharacters(
        apiRequest.skOptions, apiRequest.skOptionCount, "skOptions", "skOptionCount", copiedSuperKernelOptions,
        copiedRequestTextByteCount);
    if (result != ACLRTC_SUCCESS) {
        return result;
    }

    const size_t superKernelEnableOptionCount = static_cast<size_t>(
        std::count(copiedSuperKernelOptions.begin(), copiedSuperKernelOptions.end(), SUPER_KERNEL_ENABLE_OPTION));
    if (superKernelEnableOptionCount > 1U) {
        ASCENDLOGE(
            "skOptions contains --enable-super-kernel %zu times; remove duplicates and provide it exactly once",
            superKernelEnableOptionCount);
        return ACLRTC_ERROR_INVALID_OPTION;
    }
    if (superKernelEnableOptionCount == 0U) {
        if (!copiedSuperKernelOptions.empty()) {
            ASCENDLOGW(
                "skOptions were provided without --enable-super-kernel; ignoring skOptions and compiling basic only");
        }
        return ACLRTC_SUCCESS;
    }

    normalizedRequest.compilationVariant = CompilationVariant::BasicWithSuperKernel;
    for (size_t optionIndex = 0U; optionIndex < copiedSuperKernelOptions.size(); ++optionIndex) {
        std::string& superKernelOption = copiedSuperKernelOptions[optionIndex];
        if (superKernelOption == SUPER_KERNEL_ENABLE_OPTION) {
            continue;
        }
        result = ValidateForwardedCompilerOptionDoesNotRedirectFiles(superKernelOption, "skOptions", optionIndex);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
        normalizedRequest.compilerOptions.superKernelOptions.emplace_back(std::move(superKernelOption));
    }
    return ACLRTC_SUCCESS;
}

std::string CreateSpecializationSessionId()
{
    static std::atomic<uint64_t> specializationSessionSequence{0};
    return std::to_string(getpid()) + "_" +
           std::to_string(specializationSessionSequence.fetch_add(1U, std::memory_order_relaxed));
}
} // namespace

aclError NormalizeKernelSpecializationRequest(
    const aclrtcKernelSpecRequest& apiRequest, const char* outputElfPath,
    NormalizedKernelSpecializationRequest& normalizedRequest)
{
    const auto firstNonZeroReservedByte = std::find_if(
        std::begin(apiRequest.reserved), std::end(apiRequest.reserved), [](uint8_t value) { return value != 0U; });
    if (firstNonZeroReservedByte != std::end(apiRequest.reserved)) {
        ASCENDLOGE(
            "aclrtcKernelSpecRequest.reserved[%zu] must be zero; zero-initialize the request before use",
            static_cast<size_t>(firstNonZeroReservedByte - std::begin(apiRequest.reserved)));
        return ACLRTC_ERROR_INVALID_INPUT;
    }

    size_t copiedRequestTextByteCount = 0U;
    NormalizedKernelSpecializationRequest request;
    if (!CopyRequiredBoundedCString(
            apiRequest.resourceId, "resourceId", request.resourceId, copiedRequestTextByteCount)) {
        return ACLRTC_ERROR_INVALID_INPUT;
    }
    if (!CopyRequiredBoundedCString(
            apiRequest.kernelEntry, "kernelEntry", request.kernelName, copiedRequestTextByteCount)) {
        return ACLRTC_ERROR_INVALID_INPUT;
    }

    std::string copiedOutputElfPath;
    if (!CopyRequiredBoundedCString(outputElfPath, "outPath", copiedOutputElfPath, copiedRequestTextByteCount)) {
        return ACLRTC_ERROR_INVALID_INPUT;
    }
    boost::system::error_code outputPathError;
    const boost::filesystem::path normalizedOutputElfPath =
        boost::filesystem::absolute(copiedOutputElfPath, outputPathError).lexically_normal();
    if (outputPathError) {
        ASCENDLOGE(
            "Failed to resolve outPath '%s' against the current working directory: error=%d message=%s",
            copiedOutputElfPath.c_str(), outputPathError.value(), outputPathError.message().c_str());
        return ACLRTC_ERROR_INVALID_INPUT;
    }
    request.outputElfPath = normalizedOutputElfPath;

    if (apiRequest.optionCount > MAX_USER_OPTION_COUNT ||
        apiRequest.skOptionCount > MAX_USER_OPTION_COUNT - apiRequest.optionCount) {
        ASCENDLOGE(
            "optionCount + skOptionCount must not exceed %zu: optionCount=%llu skOptionCount=%llu; remove excess "
            "compiler options",
            MAX_USER_OPTION_COUNT, static_cast<unsigned long long>(apiRequest.optionCount),
            static_cast<unsigned long long>(apiRequest.skOptionCount));
        return ACLRTC_ERROR_INVALID_INPUT;
    }
    aclError result = NormalizeBasicCompilerOptions(apiRequest, request, copiedRequestTextByteCount);
    if (result != ACLRTC_SUCCESS) {
        return result;
    }

    result = NormalizeSuperKernelCompilerOptions(apiRequest, request, copiedRequestTextByteCount);
    if (result != ACLRTC_SUCCESS) {
        return result;
    }

    request.specializationSessionId = CreateSpecializationSessionId();
    request.kernelArgumentCount = apiRequest.argsCount;
    request.borrowedKernelArgumentDataPointers = apiRequest.argsAddr;
    request.borrowedKernelArgumentByteCounts = apiRequest.argsBytes;
    normalizedRequest = std::move(request);
    return ACLRTC_SUCCESS;
}

} // namespace aclrtc
} // namespace ascendc
