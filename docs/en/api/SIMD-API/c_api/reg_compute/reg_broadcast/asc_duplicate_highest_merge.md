---
okftype: reference.api
description: "Broadcasts the highest element of the source vector data register to the positions selected by the mask in the destination register, preserving the original values at unselected positions; it is the merge-mode version of asc_duplicate_highest."
tag:
  - roles: [compute]
  - operations: [broadcast]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints for Reg vector computation C APIs"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_broadcast/asc_duplicate_highest.md
    kind: related
    condition: "The description compares and explains that the two APIs have the same broadcast source but this API preserves unselected elements."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "Synchronization is required when this API and adjacent Reg data movement APIs access the same Unified Buffer address with read-after-write or write-after-write dependencies."
version: ">=9.2.0"
version_changes: []
---
# asc_duplicate_highest_merge

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.591568+00:00 pushedAt=2026-11-16T02:10:55.787Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_broadcast.h"`.

Broadcasts the highest element of source operand `src`, that is, the element with the largest subscript, to the positions in destination operand `dst` selected by `mask`. `mask` only selects the element positions in `dst` and does not affect the element read from `src`; the elements in `dst` not selected by `mask` retain their original values.

This API has the same broadcast data source as [asc_duplicate_highest](asc_duplicate_highest.md). The difference is that this API uses merge mode to preserve the elements in `dst` that do not participate in the computation.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```cpp
// Placeholder form.
__simd_callee__ inline void asc_duplicate_highest_merge(vector_<dtype>& dst,
                                                        vector_<dtype> src,
                                                        vector_bool mask)
```

**Placeholder description:**

- `<dtype>` values are: `int8_t`, `uint8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

### Typical Prototype Example

```cpp
__simd_callee__ inline void asc_duplicate_highest_merge(vector_float& dst,
                                                        vector_float src,
                                                        vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Input/output | Destination operand (vector data register). The elements selected by `mask` are written with the broadcast value, and the unselected elements retain the values before the API call. The data type must be consistent with `src`. |
| src | Input | Source operand (vector data register), whose highest element is the data to be broadcast. The data type must be consistent with `dst`. |
| mask | Input | Effective indication of element operations on the destination operand (mask register). When the corresponding bit is 1, the element in `dst` is written with the highest element of `src`; when it is 0, the element in `dst` retains its original value. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- `mask` only selects the positions in `dst` where the broadcast value is written and does not select elements in `src`. Regardless of whether the highest bit of `mask` is valid, the data to be broadcast is always the highest element of `src`.
- Initialize `dst` before calling the API; otherwise, the values of elements not selected by `mask` are undefined.
- Data dependencies of the same register are ordered by hardware, and no additional synchronization instruction is required. Between this API and the preceding or following Reg data movement APIs, if different registers access the same Unified Buffer (UB) address and there is a read-after-write or write-after-write dependency, call [asc_mem_bar](../reg_sync/asc_mem_bar.md) for synchronization.

## Examples

Save the following code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The following example broadcasts the highest element of `src` to the first 4 elements of `dst`, while the remaining elements not selected by `mask` retain their original values.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 133 * sizeof(float);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t ACTIVE_COUNT = 16;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(float);
constexpr uint32_t PADDED_BUFFER_BYTES = PADDED_ELEMENT_COUNT * sizeof(float);
constexpr float INITIAL_VALUE = -1.0f;

__simd_vf__ inline void duplicate_highest_merge(__ubuf__ float* dst, __ubuf__ float* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float dst_reg;
    vector_float src_reg;
    // asc_update_mask_b32 updates the remaining valid-element count on each iteration; it does not need to be recalculated from offset.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        // active_mask controls where the broadcast takes effect in this iteration: at most the first ACTIVE_COUNT elements are enabled, limited to the valid elements in a shorter tail block.
        // For example, with 133 elements processed in groups of 64 and ACTIVE_COUNT set to 16, the three iterations enable 16, 16, and 5 elements, respectively.
        // remaining_length_active is an independent count for this iteration. update_mask decrements it to 0, so it must be recalculated on each iteration.
        // Calculate this before generating store_mask, because generating store_mask updates remaining_length, which is shared across iterations.
        uint32_t remaining_length_active = remaining_length < ACTIVE_COUNT ? remaining_length : ACTIVE_COUNT;
        vector_bool active_mask = asc_update_mask_b32(remaining_length_active);
        // store_mask controls the writeback range. The three iterations write back 64, 64, and 5 valid elements, respectively, and automatically update the remaining count.
        // Positions where the broadcast does not take effect may also need to be written back, so active_mask cannot directly replace store_mask.
        vector_bool store_mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(dst_reg, dst + offset);
        asc_loadalign(src_reg, src + offset);
        asc_duplicate_highest_merge(dst_reg, src_reg, active_mask);
        asc_storealign(dst + offset, dst_reg, store_mask);
    }
}

__global__ __vector__ void asc_duplicate_highest_merge_kernel(__gm__ float* dst, __gm__ float* src)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(dst_local, dst, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(src_local, src, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<duplicate_highest_merge>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src(PADDED_ELEMENT_COUNT);
    std::vector<float> output(PADDED_ELEMENT_COUNT, INITIAL_VALUE);
    std::vector<float> golden(TOTAL_LENGTH, INITIAL_VALUE);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) src[i] = static_cast<float>(i + 1);
    for (uint32_t i = TOTAL_LENGTH; i < PADDED_ELEMENT_COUNT; ++i) {
        src[i] = -1000.0f - static_cast<float>(i);
    }
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * ELEMENTS_PER_REGISTER;
        const uint32_t remaining = TOTAL_LENGTH - offset;
        // Keep the number of remaining valid elements for updating the mask to handle the tail block.
        const uint32_t remaining_length = remaining < ELEMENTS_PER_REGISTER ? remaining : ELEMENTS_PER_REGISTER;
        const uint32_t remaining_length_active = remaining_length < ACTIVE_COUNT ? remaining_length : ACTIVE_COUNT;
        const float register_highest = src[offset + ELEMENTS_PER_REGISTER - 1];
        for (uint32_t i = 0; i < remaining_length_active; ++i) golden[offset + i] = register_highest;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src_device = nullptr;
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, PADDED_BUFFER_BYTES, src.data(), PADDED_BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(dst_device, PADDED_BUFFER_BYTES, output.data(), PADDED_BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_duplicate_highest_merge_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    output.resize(TOTAL_LENGTH);
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_duplicate_highest_merge passed."
                        : "[Failed] asc_duplicate_highest_merge failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
