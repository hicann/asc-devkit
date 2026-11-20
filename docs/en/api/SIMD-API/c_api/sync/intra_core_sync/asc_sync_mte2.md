---
okftype: reference.api
description: "A convenience API for synchronizing PIPE_MTE2. Ensures that subsequent instructions of PIPE_MTE3 and PIPE_V can start only after all preceding instructions of PIPE_MTE2 are complete."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies: []
version: ">=8.5.0"
version_changes: []
---
# asc_sync_mte2

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.547861+00:00 pushedAt=2026-11-16T02:10:57.782Z nl=2 -->

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

Performs synchronization on `PIPE_MTE2`. It ensures that subsequent instructions on `PIPE_MTE3` and `PIPE_V` start only after all preceding instructions in `PIPE_MTE2` (such as GM-to-UB data movement) are complete.

This API is an ease-of-use API for simplifying manual synchronization coding.

## Prototype

```c
__aicore__ inline void asc_sync_mte2(int id)
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

This example demonstrates the value of `asc_sync_mte2` as an ease-of-use API: after a single GM-to-UB move-in of three data blocks `src0`, `src1`, and `src2`, two independent paths—vector computation on `PIPE_V` (depending on `src0` and `src1`) and data move-out on `PIPE_MTE3` (depending on `src2`)—both depend on that move-in. With manual synchronization, a pair of `asc_sync_notify` and `asc_sync_wait` would be required for each of `PIPE_V` and `PIPE_MTE3`. Using `asc_sync_mte2`, a single call synchronizes both `PIPE_MTE3` and `PIPE_V` at once, reducing the amount of handwritten code. In the example, the dependency from `PIPE_V` to `PIPE_MTE3` uses `asc_sync_notify` and `asc_sync_wait` for precise synchronization, contrasting with the ease-of-use style of `asc_sync_mte2`.

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

__global__ __vector__ void asc_sync_mte2_kernel(__gm__ float* dst0, __gm__ float* dst1, __gm__ float* src0, __gm__ float* src1, __gm__ float* src2)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS], w[ELEMENTS];
    asc_copy_gm2ub_align(x, src0, BYTES);
    asc_copy_gm2ub_align(y, src1, BYTES);
    asc_copy_gm2ub_align(w, src2, BYTES);

    // A single asc_sync_mte2 synchronizes both PIPE_MTE3 and PIPE_V at once.
    // Ensure that after the data movement into the buffer is complete, the computation on PIPE_V and the data movement out on PIPE_MTE3 are executed.
    asc_sync_mte2(0);

    add_vf(z, x, y);

    // For synchronization from PIPE_V to PIPE_MTE3, write notify and wait separately.
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(dst1, w, BYTES);
    asc_copy_ub2gm_align(dst0, z, BYTES);
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS), src1(ELEMENTS), src2(ELEMENTS), dst0(ELEMENTS, 0.0f), dst1(ELEMENTS, 0.0f), golden0(ELEMENTS), golden1(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS - i) * 0.5f;
        src2[i] = static_cast<float>(i) + 1.0f;
        golden0[i] = src0[i] + src1[i];
        golden1[i] = src2[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *src2_device = nullptr, *dst0_device = nullptr, *dst1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src2_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, BYTES, src0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, BYTES, src1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src2_device, BYTES, src2.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_mte2_kernel<<<1, 0>>>(dst0_device, dst1_device, src0_device, src1_device, src2_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(dst0.data(), BYTES, dst0_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(dst1.data(), BYTES, dst1_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = dst0 == golden0 && dst1 == golden1;
    std::cout << (passed ? "[Success] asc_sync_mte2 passed." : "[Failed] asc_sync_mte2 failed.") << std::endl;
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
