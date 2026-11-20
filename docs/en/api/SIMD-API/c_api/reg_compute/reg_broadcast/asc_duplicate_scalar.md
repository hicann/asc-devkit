---
okftype: reference.api
description: "Broadcasts the scalar value to a vector data register; the masked form writes only the selected positions and zeroes the remaining positions, while the unmasked form writes all elements, and both forms can return the result."
tag:
  - roles: [compute]
  - operations: [broadcast]
  - traits: [elementwise, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints for Reg vector computation C APIs"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "Synchronization is required when this API and adjacent Reg data movement APIs access the same Unified Buffer address with read-after-write or write-after-write dependencies."
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "9.1.0 adds 12 reference output overloads without mask (for example, void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value)); the 11 overloads in 9.0.0 all require a vector_bool mask. This group of overloads writes value to all element positions of dst."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "9.2.0 adds a masked fp8_e8m0_t reference output overload (the dtype set expands from 11 to 12). Release Notes CANN_9_2_0 records that asc_duplicate_scalar adds an fp8_e8m0_t overload."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "9.2.0 adds function return value forms for both masked and unmasked overload groups (24 overloads returning vector_<dtype> in total); 9.0.0/9.1.0 only have the reference output form. The Release Notes 9.2.0 list of added return value forms includes asc_duplicate_scalar."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location is unified from include/c_api/reg_compute/reg_vector.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_broadcast.h (from 9.2.0)."
---
# asc_duplicate_scalar

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.601580+00:00 pushedAt=2026-11-16T02:10:55.801Z nl=2 -->

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

Broadcasts the scalar `value` to the destination operand. Two API forms are supported: outputting the result through a reference parameter and returning the result through the function return value. Each form supports the following two overloads:

- With `mask`: Writes `value` to the positions in `dst` selected by `mask`, and sets the positions not selected by `mask` to 0.
- Without `mask`: Writes `value` to all element positions of `dst`.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

### With mask

```cpp
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_duplicate_scalar(<dtype> value,
                                                           vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_duplicate_scalar(vector_<dtype>& dst,
                                                 <dtype> value,
                                                 vector_bool mask)
```

**Placeholder description:**

- `<dtype>` values are: `int8_t`, `uint8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```cpp
// Return the result through the function return value.
__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value,
                                                          vector_bool mask)
// Output the result through a reference parameter.
__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst,
                                                 int8_t value,
                                                 vector_bool mask)
```

### Without mask

```cpp
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_duplicate_scalar(<dtype> value)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_duplicate_scalar(vector_<dtype>& dst,
                                                 <dtype> value)
```

**Placeholder description:**

- `<dtype>` values are: `int8_t`, `uint8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```cpp
// Return the result through the function return value.
__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value)

// Output the result through a reference parameter.
__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst,
                                                 int8_t value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register), which stores the broadcast result. Only the API that outputs the result through a reference parameter includes this parameter. The element data type of `dst` is used to determine the function overload. |
| value | Input | Source operand (scalar), which is the data to be broadcast. For the API with a return value, the data type of `value` is used to determine the function overload and return type. It is recommended to pass a variable with an explicit type or use an explicit type conversion. |
| mask | Input | Effective indication of element operations on the destination operand (mask register). Only the overload with `mask` includes this parameter. Elements selected by `mask` are filled with `value` in `dst`, and unselected elements are set to 0 in `dst`. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The function prototype that outputs the result through a reference parameter has no return value, and the broadcast result is written to `dst`.
- The function prototype that returns the result through the function return value returns the broadcast result, with a return type of `vector_<dtype>` corresponding to `dtype`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Data dependencies of the same register are ordered by hardware, and no additional synchronization instruction is required. Between this API and the preceding or following Reg data movement APIs, if different registers access the same Unified Buffer (UB) address and there is a read-after-write or write-after-write dependency, call [asc_mem_bar](../reg_sync/asc_mem_bar.md) for synchronization.
- When calling the API with a return value, use a variable with an explicit type or an explicit type conversion, such as `static_cast<uint8_t>(1)`, to match the correct function prototype.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The following example code is supported only on Ascend 950PR&950DT products.

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
constexpr float FILL_VALUE = 3.5f;

__simd_vf__ inline void duplicate_scalar(__ubuf__ float* dst, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float dst_reg;
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
        asc_duplicate_scalar(dst_reg, FILL_VALUE, active_mask);
        asc_storealign(dst + offset, dst_reg, store_mask);
    }
}

__global__ __vector__ void asc_duplicate_scalar_kernel(__gm__ float* dst)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<duplicate_scalar>(dst_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> output(TOTAL_LENGTH);
    std::vector<float> golden(TOTAL_LENGTH, 0.0f);
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * ELEMENTS_PER_REGISTER;
        const uint32_t remaining = TOTAL_LENGTH - offset;
        // Keep the number of remaining valid elements for updating the mask to handle the tail block.
        const uint32_t remaining_length = remaining < ELEMENTS_PER_REGISTER ? remaining : ELEMENTS_PER_REGISTER;
        const uint32_t remaining_length_active = remaining_length < ACTIVE_COUNT ? remaining_length : ACTIVE_COUNT;
        for (uint32_t i = 0; i < remaining_length_active; ++i) golden[offset + i] = FILL_VALUE;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    asc_duplicate_scalar_kernel<<<1, 0>>>(dst_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_duplicate_scalar passed."
                        : "[Failed] asc_duplicate_scalar failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
