---
okftype: reference.api
description: "Launches a Vector Function subtask in a Kernel in the SIMD C API scenario, uses the template parameter funcPtr to specify the entry function decorated with __simd_vf__ and passes arguments through variadic parameters, the entry function can call Reg vector computation APIs, and it takes effect only on Ascend 950PR&950DT."
tag:
  - roles: [control]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "Insert this synchronization API when UB address overlap or cross-pipeline dependencies exist in the Vector Function"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: related
    condition: "The example uses this instruction to synchronize with the MTE2 and MTE3 pipelines before and after launching the Vector Function"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md
    kind: related
    condition: "The example uses this API to move GM data into the UB before launching the Vector Function"
version: ">=9.0.0"
version_changes: []
---
# asc\_vf\_call

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.417637+00:00 pushedAt=2026-11-16T02:10:57.391Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"utils/base/helpers.h"`.

Used in SIMD C API programming scenarios. It is used to launch a Vector Function (VF) subtask in the Kernel. Reg vector computation C APIs can be called inside the VF function to complete register-level data movement and computation.

## Prototype

```cpp
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(Args &&...args)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| :-- | :------------ |
| funcPtr | Used to specify the Vector Function entrypoint function. |
| Args | Defines variable arguments used to pass actual arguments to the Vector Function entrypoint function. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :-- | :------------ | :-- |
| args | Input | Variable arguments used to pass actual arguments to the Vector Function entrypoint function. |

## Return Value

None

## Constraints

- When `asc_vf_call` launches a Vector Function subtask, the subtask function cannot be a class member function. A regular function or a class static function is recommended, and the entrypoint function must use the `__simd_vf__` modifier macro.
- When `asc_vf_call` launches a Vector Function subtask, the input arguments support only Ascend C built-in data types and pointers, arrays, and structures composed of them. When passing source/destination addresses to Reg vector computation C APIs, the pointer type must point to Unified Buffer (UB) memory.
- The pipeline type of the Vector Function is `PIPE_V`. If UB address overlap or cross-pipeline dependencies exist inside the Vector Function, insert [asc_mem_bar](../reg_sync/asc_mem_bar.md) according to the constraints of the specific API.

## Examples

Use Reg vector computation C APIs to perform addition on UB data in the Vector Function.

```cpp
#include <iostream>
#include <algorithm>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

constexpr uint32_t ELEMENT_COUNT = 67;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = 128;

constexpr uint32_t DATA_BYTES = ELEMENT_COUNT * sizeof(float);
constexpr uint32_t PADDED_DATA_BYTES = PADDED_ELEMENT_COUNT * sizeof(float);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_float dst_reg;
        vector_float src0_reg;
        vector_float src1_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);

        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_add(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

constexpr uint8_t MUTEX_ID = 0;

__global__ __vector__ void asc_add_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src1_local[PADDED_ELEMENT_COUNT];

    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, PADDED_DATA_BYTES);
    asc_copy_gm2ub_align(src1_local, src1, PADDED_DATA_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);


    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<add_vf>(dst_local, src0_local, src1_local, one_repeat_size, repeat_time);

    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, PADDED_DATA_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}

int main()
{
    std::vector<float> src0(PADDED_ELEMENT_COUNT), src1(PADDED_ELEMENT_COUNT);
    std::vector<float> output(PADDED_ELEMENT_COUNT, 0.0F), golden(PADDED_ELEMENT_COUNT, 0.0F);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        src0[i] = static_cast<float>(i);
        src1[i] = 100.0F + static_cast<float>(i);
        golden[i] = src0[i] + src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src0_device = nullptr;
    float* src1_device = nullptr;
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), PADDED_DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), PADDED_DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), PADDED_DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, PADDED_DATA_BYTES, src0.data(), PADDED_DATA_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, PADDED_DATA_BYTES, src1.data(), PADDED_DATA_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_add_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), PADDED_DATA_BYTES, dst_device, PADDED_DATA_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = std::equal(output.begin(), output.begin() + ELEMENT_COUNT, golden.begin());
    std::cout << (passed ? "[Success] asc_vf_call example." : "[Failed] asc_vf_call example.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src1_device);
    aclrtFree(src0_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
