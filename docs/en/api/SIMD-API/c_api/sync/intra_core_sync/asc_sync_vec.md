---
okftype: reference.api
description: "A convenience API for synchronizing PIPE_V. Ensures that subsequent instructions of other pipelines can start only after preceding instructions of PIPE_V are complete. The prototype without input parameters is deprecated."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "The page declares that the prototype asc_sync_vec() without input parameters is replaced by asc_sync"
version: ">=8.5.0"
version_changes: []
---
# asc_sync_vec

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.563890+00:00 pushedAt=2026-11-16T02:10:57.819Z nl=2 -->

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

**The no-argument prototype `asc_sync_vec()` API is deprecated. Use [asc_sync](asc_sync.md) to perform synchronization for all pipelines.**

Performs synchronization for `PIPE_V`. It ensures that subsequent instructions on `PIPE_MTE2`, `PIPE_MTE3`, and `PIPE_S` can start execution only after all preceding instructions (such as vector computations) in `PIPE_V` are complete.

This API is an ease-of-use API for simplifying manual synchronization coding.

## Prototype

```c
// Prototype with arguments.
__aicore__ inline void asc_sync_vec(int id)

// Prototype without arguments.
__aicore__ inline void asc_sync_vec()
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

This example demonstrates the value of `asc_sync_vec` as an ease-of-use API: after the vector computation on `PIPE_V` is complete, the data movement in on `PIPE_MTE2`, the data movement out on `PIPE_MTE3`, and the scalar processing on `PIPE_S` all depend on that computation. With manual synchronization, you need to execute a pair of `asc_sync_notify` and `asc_sync_wait` for each of `PIPE_MTE2`, `PIPE_MTE3`, and `PIPE_S`. Using `asc_sync_vec`, a single call completes synchronization for `PIPE_MTE2`, `PIPE_MTE3`, and `PIPE_S` at the same time, reducing the amount of handwritten code. In the example, the dependency from `PIPE_MTE2` to `PIPE_V` uses `asc_sync_notify` and `asc_sync_wait` for precise synchronization, contrasting with the ease-of-use style of `asc_sync_vec`.

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

__global__ __vector__ void asc_sync_vec_kernel(__gm__ float* dst0, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS];
    asc_copy_gm2ub_align(x, src0, BYTES);
    asc_copy_gm2ub_align(y, src1, BYTES);

    // For synchronization from PIPE_MTE2 to PIPE_V, write notify and wait separately.
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    add_vf(z, x, y);

    // A single asc_sync_vec completes synchronization for PIPE_MTE2, PIPE_MTE3, and PIPE_S at the same time.
    // Ensure that subsequent data movement in, data movement out, and scalar processing execute only after the computation is complete.
    asc_sync_vec(0);

    asc_copy_ub2gm_align(dst0, z, BYTES);
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS), src1(ELEMENTS), dst0(ELEMENTS, 0.0f), golden0(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS - i) * 0.5f;
        golden0[i] = src0[i] + src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *dst0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, BYTES, src0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, BYTES, src1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_vec_kernel<<<1, 0>>>(dst0_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(dst0.data(), BYTES, dst0_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = dst0 == golden0;
    std::cout << (passed ? "[Success] asc_sync_vec passed." : "[Failed] asc_sync_vec failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(dst0_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
