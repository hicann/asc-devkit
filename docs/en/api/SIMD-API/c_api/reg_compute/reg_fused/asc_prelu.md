---
okftype: reference.api
description: "Reg fused activation API that performs PReLU on the source vector according to a mask. The negative half-axis slope is given element-wise by the vector alpha. It provides both dst reference and function return value forms, supporting half and float. It takes effect only on the AI Vector."
tag:
  - roles: [compute]
  - operations: [prelu]
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
    scope: "overload:asc_prelu__vf16_vf16_vb"
    scope_basis: resolved
    note: "Added a function return value overload for half data: asc_prelu(vector_half, vector_half, vector_bool). Elements with mask 0 are set to 0 in the return value; based on line 54 of the CANN_9_2_0 Release Notes (adding function return value overloads for Reg fused and other APIs) and the first appearance of this declaration in upstream/9.2.0."
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_prelu__vf32_vf32_vb"
    scope_basis: resolved
    note: "Added a function return value overload for float data: asc_prelu(vector_float, vector_float, vector_bool), added in the same batch as the half return value overload."
---
# asc_prelu

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.069500+00:00 pushedAt=2026-11-16T02:10:56.682Z nl=2 -->

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

Performs PReLU (Parametric ReLU) on the elements in the source operand vector data register `src0` according to the mask. The negative half-axis slope is specified by the corresponding elements in the vector data register `alpha`, and the result is returned through the function return value or written to the destination operand vector data register. When `src0` is greater than or equal to 0, the value is written directly to the output result; otherwise, `src0` is multiplied by `alpha` and then written to the output result. Note: When the data in the source vector data register is floating-point positive zero (+0), it is treated as a positive number and output directly; when it is floating-point negative zero (-0), it is treated as a negative number and multiplied by `alpha`. Elements with mask 1 participate in the computation, and elements with mask 0 are set to zero in the output result. The calculation formula is as follows:

$$
dst_i = \begin{cases}
src0_i, & src0_i >= 0 \\
src0_i \times \alpha_i, & src0_i < 0
\end{cases}
$$

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_prelu(vector_<dtype> src0,
                                                vector_<dtype> alpha,
                                                vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_prelu(vector_<dtype>& dst,
                                      vector_<dtype> src0,
                                      vector_<dtype> alpha,
                                      vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values: `half`, `float`.

### Typical Prototype Example

```c
// Return the result through the function return value.
__simd_callee__ inline vector_half asc_prelu(vector_half src0,
                                             vector_half alpha,
                                             vector_bool mask)

// Output the result through a reference parameter.
__simd_callee__ inline void asc_prelu(vector_half& dst,
                                      vector_half src0,
                                      vector_half alpha,
                                      vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand (vector data register). Only the void-returning API contains this parameter. The data type must be the same as `src0` and `alpha`. |
| src0 | Input | Source operand (vector data register). |
| alpha | Input | Negative half-axis slope (vector data register). The data type must be the same as `src0`. |
| mask | Input | Mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by `mask` are set to zero in the output. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

For the return-value API, the vector data register that stores the PReLU computation result is returned, and its data type is consistent with `src0` and `alpha`.

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Element positions with a mask bit of 0 do not participate in the PReLU operation, and the corresponding positions in the output result are written as 0.

### Computation Constraints

- When the data in the source vector data register is floating-point positive zero (+0), it is treated as a positive number and output directly; when it is floating-point negative zero (-0), it is treated as a negative number and multiplied by `alpha`, and the multiplication satisfies IEEE 754 floating-point multiplication rules.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(float);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(float);

__simd_vf__ inline void prelu_vf(__ubuf__ float* output,
                                 __ubuf__ float* input,
                                 __ubuf__ float* alpha_input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float src0;
    vector_float alpha;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src0, input + offset);
        asc_loadalign(alpha, alpha_input + offset);
        vector_float dst = asc_prelu(src0, alpha, mask);
        asc_storealign(output + offset, dst, mask);
    }
}

__global__ __vector__ void asc_prelu_kernel(__gm__ float* output,
                                            __gm__ float* input,
                                            __gm__ float* alpha)
{
    asc_init();
    __ubuf__ float output_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float input_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float alpha_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(alpha_local, alpha, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<prelu_vf>(output_local, input_local, alpha_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(TOTAL_LENGTH);
    std::vector<float> alpha(TOTAL_LENGTH, 0.1f);
    std::vector<float> output(TOTAL_LENGTH, 0.0f);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input[i] = static_cast<float>(static_cast<int32_t>(i) - 64);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* input_device = nullptr;
    float* alpha_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&alpha_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(alpha_device, BUFFER_BYTES, alpha.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_prelu_kernel<<<1, 0>>>(output_device, input_device, alpha_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        const float expected = input[i] > 0.0f ? input[i] : input[i] * alpha[i];
        if (std::fabs(output[i] - expected) > 1e-6f) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_prelu completed."
                         : "[Failed] asc_prelu output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(alpha_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
