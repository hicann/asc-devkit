/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>

#include "acl/acl_base.h"
#include "acl/acl_mdl.h"
#include "acl/acl_rt.h"

#define CHECK_ACL(expr)                                                                                 \
    do {                                                                                                \
        auto __ret = (expr);                                                                            \
        int32_t __code = static_cast<int32_t>(__ret);                                                   \
        if (__code != 0) {                                                                              \
            fprintf(stderr, "[ERROR] %s failed at %s:%d, ret=%d\n", #expr, __FILE__, __LINE__, __code); \
        }                                                                                               \
    } while (0)

int32_t main(int32_t argc, char** argv)
{
    CHECK_ACL(aclInit(nullptr));
    const int32_t deviceId = 0;
    CHECK_ACL(aclrtSetDevice(deviceId));
    aclrtStream stream = nullptr;
    CHECK_ACL(aclrtCreateStream(&stream));
    uint32_t modelId = 0;
    CHECK_ACL(aclmdlLoadFromFile("./leaky_relu_custom.om", &modelId));
    aclmdlDesc* modelDesc = aclmdlCreateDesc();
    CHECK_ACL(aclmdlGetDesc(modelDesc, modelId));

    aclmdlDataset* modelInputDataset = aclmdlCreateDataset();
    void* input0DeviceMem = nullptr;
    size_t modelInputSize = aclmdlGetInputSizeByIndex(modelDesc, 0);
    CHECK_ACL(aclrtMalloc(&input0DeviceMem, modelInputSize, ACL_MEM_MALLOC_HUGE_FIRST));
    aclDataBuffer* input0Buffer = aclCreateDataBuffer(input0DeviceMem, modelInputSize);
    CHECK_ACL(aclmdlAddDatasetBuffer(modelInputDataset, input0Buffer));

    aclmdlDataset* modelOutputDataset = aclmdlCreateDataset();
    void* output0DeviceMem = nullptr;
    size_t modelOutputSize = aclmdlGetOutputSizeByIndex(modelDesc, 0);
    CHECK_ACL(aclrtMalloc(&output0DeviceMem, modelOutputSize, ACL_MEM_MALLOC_HUGE_FIRST));
    aclDataBuffer* output0Buffer = aclCreateDataBuffer(output0DeviceMem, modelOutputSize);
    CHECK_ACL(aclmdlAddDatasetBuffer(modelOutputDataset, output0Buffer));

    const std::vector<int64_t> shape = {8, 16, 128};
    const int64_t elementCount = shape[0] * shape[1] * shape[2];
    std::vector<float> input0HostData(elementCount);
    for (int64_t i = 0; i < elementCount; ++i) {
        input0HostData[i] = static_cast<float>((i % 7) - 3);
    }
    std::vector<float> output0HostData(elementCount, 0.0f);
    std::vector<float> goldenData(elementCount, 0.0f);
    const float negativeSlope = 0.1f;
    for (int64_t i = 0; i < elementCount; ++i) {
        const float v = input0HostData[i];
        goldenData[i] = v >= 0.0f ? v : v * negativeSlope;
    }

    CHECK_ACL(
        aclrtMemcpy(input0DeviceMem, modelInputSize, input0HostData.data(), modelInputSize, ACL_MEMCPY_HOST_TO_DEVICE));
    CHECK_ACL(aclmdlExecute(modelId, modelInputDataset, modelOutputDataset));
    CHECK_ACL(aclrtMemcpy(
        output0HostData.data(), modelOutputSize, output0DeviceMem, modelOutputSize, ACL_MEMCPY_DEVICE_TO_HOST));

    printf("result is:\n");
    const int64_t previewCount = std::min<int64_t>(elementCount, 10);
    for (int64_t i = 0; i < previewCount; i++) {
        printf("%.6f ", output0HostData[i]);
    }
    printf("\n");

    bool pass = true;
    for (int64_t i = 0; i < elementCount; ++i) {
        if (std::fabs(output0HostData[i] - goldenData[i]) > 1e-5f) {
            printf(
                "test failed at index %ld, output0HostData[i] = %.6f, goldenData[i] = %.6f\n", i, output0HostData[i],
                goldenData[i]);
            pass = false;
            break;
        }
    }
    printf("test %s\n", pass ? "pass" : "failed");

    CHECK_ACL(aclmdlDestroyDataset(modelInputDataset));
    CHECK_ACL(aclmdlDestroyDataset(modelOutputDataset));
    CHECK_ACL(aclDestroyDataBuffer(input0Buffer));
    CHECK_ACL(aclDestroyDataBuffer(output0Buffer));
    CHECK_ACL(aclrtFree(input0DeviceMem));
    CHECK_ACL(aclrtFree(output0DeviceMem));
    CHECK_ACL(aclmdlDestroyDesc(modelDesc));
    CHECK_ACL(aclmdlUnload(modelId));
    CHECK_ACL(aclrtDestroyStream(stream));
    CHECK_ACL(aclrtResetDevice(deviceId));
    CHECK_ACL(aclFinalize());
    return 0;
}
