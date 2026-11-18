/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "acl/acl_rt_compile.h"
#include "custom_op/op_kernel/matmul_custom_tiling.h"
#include "runtime_resources.h"
#include "tiling/tiling_api.h"
#include <array>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <new>
#include <utility>
#include <vector>

constexpr size_t kElementCount = static_cast<size_t>(kMatmulM) * kMatmulN;
constexpr size_t kInputBytes = kElementCount * sizeof(aclFloat16);
constexpr size_t kOutputBytes = kElementCount * sizeof(float);

#define ASCENDC_CHECK(expr)                                           \
    do {                                                              \
        aclError checkRet = (expr);                                   \
        if (checkRet != ACL_SUCCESS) {                                \
            std::fprintf(stderr, "%s failed: %d\n", #expr, checkRet); \
            return 1;                                                 \
        }                                                             \
    } while (false)

bool ParseDevice(const char* text, int32_t& device)
{
    char* end = nullptr;
    errno = 0;
    long value = std::strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value < 0 || value > INT32_MAX) {
        return false;
    }
    device = static_cast<int32_t>(value);
    return true;
}

bool VerifyResult(const float* output, const float* golden, size_t count)
{
    if (output == nullptr || golden == nullptr || count == 0) {
        return false;
    }
    for (size_t i = 0; i < count; ++i) {
        if (!std::isfinite(output[i]) || output[i] != golden[i]) {
            std::fprintf(stderr, "Mismatch at %zu: output=%f expected=%f\n", i, output[i], golden[i]);
            return false;
        }
    }
    std::printf("Numerical check passed: %zu float elements\n", count);
    return true;
}

bool InitializeRuntime(RuntimeResources& resources, int32_t device, size_t tilingBytes)
{
    aclError ret = aclInit(nullptr);
    if (ret != ACL_SUCCESS) {
        std::fprintf(stderr, "aclInit failed: %d\n", ret);
        return false;
    }
    resources.initialized = true;
    ret = aclrtCreateContext(&resources.context, device);
    if (ret != ACL_SUCCESS) {
        std::fprintf(stderr, "aclrtCreateContext failed: %d\n", ret);
        return false;
    }
    ret = aclrtCreateStream(&resources.stream);
    if (ret != ACL_SUCCESS) {
        std::fprintf(stderr, "aclrtCreateStream failed: %d\n", ret);
        return false;
    }
    const std::array<std::pair<void**, size_t>, 3> hostBuffers = {
        {{&resources.xHost, kInputBytes}, {&resources.yHost, kInputBytes}, {&resources.zHost, kOutputBytes}}};
    for (const auto& [buffer, bytes] : hostBuffers) {
        *buffer = std::malloc(bytes);
        if (*buffer == nullptr) {
            std::fprintf(stderr, "Host memory allocation failed: %zu bytes\n", bytes);
            return false;
        }
    }
    const std::array<std::pair<void**, size_t>, 4> allocations = {
        {{&resources.xDevice, kInputBytes},
         {&resources.yDevice, kInputBytes},
         {&resources.zDevice, kOutputBytes},
         {&resources.tilingDevice, tilingBytes}}};
    const std::array<const char*, 4> names = {"xDevice", "yDevice", "zDevice", "tilingDevice"};
    for (size_t i = 0; i < allocations.size(); ++i) {
        ret = aclrtMalloc(allocations[i].first, allocations[i].second, ACL_MEM_MALLOC_NORMAL_ONLY);
        if (ret != ACL_SUCCESS) {
            std::fprintf(stderr, "aclrtMalloc(%s) failed: %d\n", names[i], ret);
            return false;
        }
    }
    return true;
}

bool PrepareInputs(const RuntimeResources& resources, std::vector<float>& golden)
{
    auto* xHost = static_cast<aclFloat16*>(resources.xHost);
    auto* yHost = static_cast<aclFloat16*>(resources.yHost);
    for (size_t i = 0; i < kElementCount; ++i) {
        const float value = static_cast<float>(static_cast<int>(i % 17) - 8) * 0.25F;
        xHost[i] = aclFloatToFloat16(value);
        yHost[i] = aclFloatToFloat16(i / kMatmulN == i % kMatmulN ? 1.0F : 0.0F);
        golden[i] = value;
    }
    aclError ret = aclrtMemcpy(resources.xDevice, kInputBytes, resources.xHost, kInputBytes, ACL_MEMCPY_HOST_TO_DEVICE);
    if (ret != ACL_SUCCESS) {
        std::fprintf(stderr, "aclrtMemcpy(x) failed: %d\n", ret);
        return false;
    }
    ret = aclrtMemcpy(resources.yDevice, kInputBytes, resources.yHost, kInputBytes, ACL_MEMCPY_HOST_TO_DEVICE);
    if (ret != ACL_SUCCESS) {
        std::fprintf(stderr, "aclrtMemcpy(y) failed: %d\n", ret);
        return false;
    }
    return true;
}

bool PrepareTiling(optiling::TCubeTiling& tiling, size_t& workspaceBytes)
{
    const char* socName = aclrtGetSocName();
    if (socName == nullptr) {
        return false;
    }
    auto* platform = platform_ascendc::PlatformAscendCManager::GetInstance(socName);
    if (platform == nullptr || platform->GetCoreNumAic() == 0) {
        return false;
    }
    matmul_tiling::MultiCoreMatmulTiling tilingApi(*platform);
    tilingApi.SetDim(platform->GetCoreNumAic());
    tilingApi.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, false);
    tilingApi.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, false);
    tilingApi.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetShape(kMatmulM, kMatmulN, kMatmulK);
    tilingApi.SetOrgShape(kMatmulM, kMatmulN, kMatmulK);
    tilingApi.SetBias(false);
    tilingApi.SetBufferSpace(-1, -1, -1);
    workspaceBytes = platform->GetLibApiWorkSpaceSize();
    return tilingApi.GetTiling(tiling) == 0 && tiling.get_usedCoreNum() > 0 && tiling.get_singleCoreM() > 0 &&
           tiling.get_singleCoreN() > 0;
}

struct ProgramOptions {
    const char* resourceId = nullptr;
    const char* kernelEntry = nullptr;
    const char* outputKernelPath = nullptr;
    const char* inputKernelPath = nullptr;
    int32_t device = 0;
};

bool ParseArguments(int argc, char** argv, ProgramOptions& options)
{
    if ((argc != 5 && argc != 6) || !ParseDevice(argv[4], options.device) || std::strlen(argv[1]) != 64 ||
        argv[3][0] == '\0') {
        return false;
    }
    options.resourceId = argv[1];
    options.kernelEntry = argv[2];
    options.outputKernelPath = argv[3];
    if (argc == 6) {
        options.inputKernelPath = argv[5];
        std::error_code error;
        if (!std::filesystem::is_regular_file(options.inputKernelPath, error)) {
            std::fprintf(
                stderr, "Input Kernel file does not exist or is not a regular file: %s\n", options.inputKernelPath);
            return false;
        }
    }
    return options.kernelEntry[0] != '\0';
}

int LoadAndRunKernel(
    RuntimeResources& resources, const ProgramOptions& options, const void* tiling, size_t tilingBytes,
    uint32_t blockCount)
{
    const char* kernelPath = options.inputKernelPath != nullptr ? options.inputKernelPath : options.outputKernelPath;
    ASCENDC_CHECK(aclrtBinaryLoadFromFile(kernelPath, nullptr, &resources.binary));
    aclrtFuncHandle function = nullptr;
    ASCENDC_CHECK(aclrtBinaryGetFunction(resources.binary, options.kernelEntry, &function));

    // Preserve the original five-parameter ABI when launching the Kernel.
    ASCENDC_CHECK(aclrtMemcpy(resources.tilingDevice, tilingBytes, tiling, tilingBytes, ACL_MEMCPY_HOST_TO_DEVICE));
    void* kernelArgs[] = {
        &resources.xDevice, &resources.yDevice, &resources.zDevice, &resources.workspaceDevice,
        &resources.tilingDevice};
    ASCENDC_CHECK(aclrtMemset(resources.zDevice, kOutputBytes, 0xff, kOutputBytes));
    ASCENDC_CHECK(aclrtLaunchKernelWithArgsArray(function, blockCount, resources.stream, nullptr, kernelArgs));
    ASCENDC_CHECK(aclrtSynchronizeStream(resources.stream));
    return 0;
}

int CheckOutput(const RuntimeResources& resources, const std::vector<float>& golden)
{
    ASCENDC_CHECK(
        aclrtMemcpy(resources.zHost, kOutputBytes, resources.zDevice, kOutputBytes, ACL_MEMCPY_DEVICE_TO_HOST));
    const auto* output = static_cast<const float*>(resources.zHost);
    return VerifyResult(output, golden.data(), golden.size()) ? 0 : 1;
}

int main(int argc, char** argv)
try {
    ProgramOptions options;
    if (!ParseArguments(argc, argv, options)) {
        std::fprintf(stderr, "Usage: demo resource_id kernel_entry output_kernel_path device_id [input_kernel_path]\n");
        return 1;
    }
    std::vector<float> golden(kElementCount);
    optiling::TCubeTiling cubeTiling;
    std::array<uint8_t, sizeof(AscendC::tiling::TCubeTiling)> tiling{};
    RuntimeResources resources;
    if (!InitializeRuntime(resources, options.device, sizeof(tiling)) || !PrepareInputs(resources, golden)) {
        return 1;
    }
    size_t workspaceBytes = 0;
    if (!PrepareTiling(cubeTiling, workspaceBytes) || cubeTiling.GetDataSize() > tiling.size()) {
        std::fprintf(stderr, "Matmul Tiling generation failed\n");
        return 1;
    }
    if (workspaceBytes != 0) {
        ASCENDC_CHECK(aclrtMalloc(&resources.workspaceDevice, workspaceBytes, ACL_MEM_MALLOC_NORMAL_ONLY));
    }
    cubeTiling.SaveToBuffer(tiling.data(), tiling.size());
    const uint32_t blockCount = static_cast<uint32_t>(cubeTiling.get_usedCoreNum());
    std::printf(
        "Matmul: M=%d N=%d K=%d blocks=%u singleCore=%dx%dx%d base=%dx%dx%d\n", kMatmulM, kMatmulN, kMatmulK,
        blockCount, cubeTiling.get_singleCoreM(), cubeTiling.get_singleCoreN(), cubeTiling.get_singleCoreK(),
        cubeTiling.get_baseM(), cubeTiling.get_baseN(), cubeTiling.get_baseK());
    if (options.inputKernelPath == nullptr) {
        // Only tiling (parameter 4) is constant; provide its actual Host data.
        const void* argsAddr[] = {nullptr, nullptr, nullptr, nullptr, tiling.data()};
        const uint64_t argsBytes[] = {0, 0, 0, 0, sizeof(tiling)};
        const char* compileOptions[] = {"-g", "-fno-ident"};
        aclrtcKernelSpecRequest request{};
        request.resourceId = options.resourceId;
        request.kernelEntry = options.kernelEntry;
        request.argsCount = 5;
        request.argsAddr = argsAddr;
        request.argsBytes = argsBytes;
        request.options = compileOptions;
        request.optionCount = 2;
        ASCENDC_CHECK(aclrtcKernelSpecialization(&request, options.outputKernelPath));
    }
    if (LoadAndRunKernel(resources, options, tiling.data(), tiling.size(), blockCount) != 0) {
        return 1;
    }
    if (CheckOutput(resources, golden) != 0) {
        return 1;
    }
    if (!resources.Close()) {
        std::fprintf(stderr, "Resource cleanup failed\n");
        return 1;
    }
    std::printf("Example passed\n");
    return 0;
} catch (const std::bad_alloc&) {
    std::fprintf(stderr, "Host allocation failed\n");
    return 1;
}
