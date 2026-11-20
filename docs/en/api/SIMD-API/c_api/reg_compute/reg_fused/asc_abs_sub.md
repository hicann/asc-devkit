---
okftype: reference.api
description: "Reg fused compute API that performs element-wise subtraction of two vector data registers according to a mask and takes the absolute value, supporting half and float, and providing both dst reference and function return value forms. It takes effect only on the AI Vector."
tag:
  - roles: [compute]
  - operations: [absolute, subtract]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "For general constraints of Reg vector computation C APIs, see the general constraints section of this page"
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: related
    condition: "Mapping between product models and NPU architectures"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_abs_sub__vf16_vf16_vb"
    scope_basis: resolved
    note: "Added a function return value overload for half data: asc_abs_sub(vector_half, vector_half, vector_bool). Elements with mask 0 are set to 0 in the return value; based on line 54 of the CANN_9_2_0 Release Notes (adding function return value overloads for Reg fused and other APIs, naming asc_abs_sub) and the first appearance of this declaration in upstream/9.2.0."
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_abs_sub__vf32_vf32_vb"
    scope_basis: resolved
    note: "Added a function return value overload for float data: asc_abs_sub(vector_float, vector_float, vector_bool), added in the same batch as the half return value overload (line 54 of the CANN_9_2_0 Release Notes)."
---
# asc_abs_sub

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.025351+00:00 pushedAt=2026-11-16T02:10:56.599Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_fused.h"`.

Subtracts source operands `src0` and `src1` element-wise according to `mask` and takes the absolute value of the difference to obtain the computation result. The calculation formula is as follows:

$$
dst_i = \lvert src0_i - src1_i \rvert
$$

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_abs_sub(vector_<dtype> src0,
                                                  vector_<dtype> src1,
                                                  vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_abs_sub(vector_<dtype>& dst,
                                        vector_<dtype> src0,
                                        vector_<dtype> src1,
                                        vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values: `half`, `float`.

### Typical Prototype Example

```c
// Example: Perform absolute difference computation on a float vector data register.
__simd_callee__ inline void asc_abs_sub(vector_float& dst,
                                        vector_float src0,
                                        vector_float src1,
                                        vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | --------- | ---------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand (vector data register). |
| src0 | Input | Source operand 0 (vector data register). |
| src1 | Input | Source operand 1 (vector data register). |
| mask | Input | Mask register, which controls whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- Outputting the result through a reference parameter: the computation result is written to `dst`.
- Returning the result through the function return value: returns the computation result, which is a vector data register with the same data type as `src0` and `src1`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The data types of `dst`, `src0`, and `src1` must be consistent.
- When the `mask` bit is 0, the corresponding element of the result is set to 0.

## Examples

Save the following code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(float);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;

__simd_vf__ inline void compute(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(dst_reg, dst + offset);
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_abs_sub(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_abs_sub_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(dst_local, dst, TOTAL_LENGTH * sizeof(float));
    asc_copy_gm2ub_align(src0_local, src0, TOTAL_LENGTH * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, TOTAL_LENGTH * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<compute>(dst_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src0(TOTAL_LENGTH);
    std::vector<float> src1(TOTAL_LENGTH);
    std::vector<float> output(TOTAL_LENGTH);
    std::vector<float> golden(TOTAL_LENGTH);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src0[i] = static_cast<float>(i % 8) * 0.25f;
        src1[i] = static_cast<float>((i + 3) % 8) * 0.25f;
        golden[i] = std::fabs(src0[i] - src1[i]);
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(float), src0.data(), src0.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(float), src1.data(), src1.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_abs_sub_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = compare_data(output, golden, 1e-6);
    std::cout << (passed ? "[Success] asc_abs_sub passed." : "[Failed] asc_abs_sub failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
