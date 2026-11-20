---
okftype: reference.api
description: "A convenience API for synchronizing PIPE_MTE3. Ensures that subsequent instructions of PIPE_MTE2 and PIPE_V can start only after all preceding instructions of PIPE_MTE3 are complete."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies: []
version: ">=8.5.0"
version_changes: []
---
# asc_sync_mte3

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.551358+00:00 pushedAt=2026-11-16T02:10:57.790Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is `"c_api/sync/sync.h"`.

Performs synchronization on `PIPE_MTE3`. It ensures that only after all preceding instructions in `PIPE_MTE3` (such as data movement from UB to GM) are complete can subsequent instructions in `PIPE_MTE2` and `PIPE_V` begin execution.

This API is an ease-of-use API for simplifying manual synchronization coding.

## Prototype

```c
__aicore__ inline void asc_sync_mte3(int id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| id | Input | Synchronization ID for multi-pipeline synchronization within a core. The value range is [0, 7]. Its meaning is the same as the `event_t` enum values of the `id` parameter in [asc_sync_notify](asc_sync_notify.md). For each enum value, see [event\_t enum definition](asc_sync_notify.md#event_t_enum). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The synchronization ID value range is [0, 7].
- This API can be called only in AIV cores.

## Examples

This example demonstrates the value of `asc_sync_mte3` as an ease-of-use API: in each iteration, after the computed result `dst0` and the directly loaded data `dst1` are moved from UB to GM, the next iteration's `PIPE_MTE2` move-in (which reoccupies the buffer) and `PIPE_V` computation both depend on that move-out. For comparison, the synchronization from `PIPE_MTE2` to `PIPE_V` in the code illustrates the manual approach: `asc_sync_notify` and `asc_sync_wait` must be written separately for each pipeline. Using `asc_sync_mte3`, a single call completes synchronization for both `PIPE_MTE2` and `PIPE_V`, reducing the amount of handwritten code.

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(float);
constexpr uint32_t ITERATIONS = 4;

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENTS;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_sync_mte3_kernel(__gm__ float* dst0, __gm__ float* dst1, __gm__ float* src0, __gm__ float* src1, __gm__ float* src2)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS], w[ELEMENTS];
    for (uint32_t iter = 0; iter < ITERATIONS; ++iter) {
        asc_copy_gm2ub_align(x, src0 + iter * ELEMENTS, BYTES);
        asc_copy_gm2ub_align(y, src1 + iter * ELEMENTS, BYTES);
        asc_copy_gm2ub_align(w, src2 + iter * ELEMENTS, BYTES);

        // For synchronization from PIPE_MTE2 to PIPE_V, write notify and wait separately.
        asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

        add_vf(z, x, y);

        // Synchronization from PIPE_MTE2 to PIPE_MTE3 ensures that w is moved out only after its move-in is complete.
        asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_copy_ub2gm_align(dst1 + iter * ELEMENTS, w, BYTES);

        // Synchronization from PIPE_V to PIPE_MTE3 ensures that z is moved out only after its computation is complete.
        asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_copy_ub2gm_align(dst0 + iter * ELEMENTS, z, BYTES);

        // A single asc_sync_mte3 completes synchronization for both PIPE_MTE2 and PIPE_V.
        // Ensure that the move-in and computation of the next iteration begin only after the move-out of the current iteration is complete.
        // After the last iteration, there is no next iteration, so no synchronization is required.
        if (iter != ITERATIONS - 1) {
            asc_sync_mte3(0);
        }
    }
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS * ITERATIONS), src1(ELEMENTS * ITERATIONS), src2(ELEMENTS * ITERATIONS), dst0(ELEMENTS * ITERATIONS, 0.0f), dst1(ELEMENTS * ITERATIONS, 0.0f), golden0(ELEMENTS * ITERATIONS), golden1(ELEMENTS * ITERATIONS);
    for (uint32_t i = 0; i < ELEMENTS * ITERATIONS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS * ITERATIONS - i) * 0.5f;
        src2[i] = static_cast<float>(i) + 1.0f;
        golden0[i] = src0[i] + src1[i];
        golden1[i] = src2[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *src2_device = nullptr, *dst0_device = nullptr, *dst1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src2_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst1_device), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, ELEMENTS * ITERATIONS * sizeof(float), src0.data(), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, ELEMENTS * ITERATIONS * sizeof(float), src1.data(), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src2_device, ELEMENTS * ITERATIONS * sizeof(float), src2.data(), ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_mte3_kernel<<<1, 0>>>(dst0_device, dst1_device, src0_device, src1_device, src2_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(dst0.data(), ELEMENTS * ITERATIONS * sizeof(float), dst0_device, ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(dst1.data(), ELEMENTS * ITERATIONS * sizeof(float), dst1_device, ELEMENTS * ITERATIONS * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = dst0 == golden0 && dst1 == golden1;
    std::cout << (passed ? "[Success] asc_sync_mte3 passed." : "[Failed] asc_sync_mte3 failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(src2_device);
    aclrtFree(dst0_device);
    aclrtFree(dst1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
