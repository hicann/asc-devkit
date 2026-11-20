---
okftype: reference.api
description: "Multiplies elements of two 32-bit vector data registers according to a mask to obtain 64-bit products, and writes the low 32 bits and high 32 bits to two destination data registers respectively, supporting int32_t and uint32_t."
tag:
  - roles: [compute]
  - operations: [multiply]
  - traits: [elementwise, split_high_low, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints for Reg vector computation C APIs"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location is unified from include/c_api/reg_compute/reg_vector.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_arith.h (starting from 9.2.0); this page has only 2 reference-output overloads, and 9.2.0 does not add function-return-value forms for asc_mull (the include always has 2 declarations), so overload_added is not registered."
---
# asc_mull

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.551072+00:00 pushedAt=2026-11-16T02:10:55.735Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_arith.h"`.

Multiplies the valid elements in two source vector data registers based on the mask. Elements with a data type bit width of 32 bits are multiplied to obtain a 64-bit product, which is split into the low 32 bits and high 32 bits and written to two destination vector data registers respectively. Elements with a mask value of 1 at the corresponding position participate in the computation, and elements with a mask value of 0 are set to zero in the destination vector data registers. The calculation formula is as follows:

$$
dst0_i = (src0_i \times src1_i) \& ((1 \ll bit) - 1)
$$

$$
dst1_i = (src0_i \times src1_i) \gg bit
$$

Here, `bit` indicates the data type bit width of `src0` and `src1`.

**Figure 1** asc_mull computation diagram

![](../../../../figures/asc_mull.png)

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Placeholder form.
__simd_callee__ inline void asc_mull(vector_<dtype>& dst0,
                                     vector_<dtype>& dst1,
                                     vector_<dtype> src0,
                                     vector_<dtype> src1,
                                     vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values: `int32_t`, `uint32_t`.

### Typical Prototype Example

```c
__simd_callee__ inline void asc_mull(vector_uint32_t& dst0,
                                     vector_uint32_t& dst1,
                                     vector_uint32_t src0,
                                     vector_uint32_t src1,
                                     vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst0 | Output | Destination operand (vector data register). The data type must be the same as `src0`. |
| dst1 | Output | Destination operand (vector data register). The data type must be the same as `src0`. |
| src0 | Input | Source operand (vector data register). |
| src1 | Input | Source operand (vector data register). The data type must be the same as `src0`. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the computation. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation, with the corresponding elements of `dst0` and `dst1` set to zero. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Element positions with a mask bit of 0 do not participate in the multiplication operation, and the corresponding positions of `dst0` and `dst1` are written with 0.

### Computation Constraints

- `dst0` and `dst1` must be different vector data registers; otherwise, undefined behavior occurs.
- The source operands and destination operands can be the same vector data registers, for example, `asc_mull(src0, src1, src0, src1, mask);`.

## Examples

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
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(uint32_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint32_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(uint32_t);

__simd_vf__ inline void mull_vf(__ubuf__ uint32_t* output_low,
                                __ubuf__ uint32_t* output_high,
                                __ubuf__ uint32_t* input0,
                                __ubuf__ uint32_t* input1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint32_t src0;
    vector_uint32_t src1;
    vector_uint32_t dst0;
    vector_uint32_t dst1;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src0, input0 + offset);
        asc_loadalign(src1, input1 + offset);
        asc_mull(dst0, dst1, src0, src1, mask);
        asc_storealign(output_low + offset, dst0, mask);
        asc_storealign(output_high + offset, dst1, mask);
    }
}

__global__ __vector__ void asc_mull_kernel(__gm__ uint32_t* output_low,
                                           __gm__ uint32_t* output_high,
                                           __gm__ uint32_t* input0,
                                           __gm__ uint32_t* input1)
{
    asc_init();
    __ubuf__ uint32_t output_low_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t output_high_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t input0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t input1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input0_local, input0, BUFFER_BYTES);
    asc_copy_gm2ub_align(input1_local, input1, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<mull_vf>(output_low_local, output_high_local, input0_local, input1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output_low, output_low_local, BUFFER_BYTES);
    asc_copy_ub2gm_align(output_high, output_high_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> input0(TOTAL_LENGTH);
    std::vector<uint32_t> input1(TOTAL_LENGTH);
    std::vector<uint32_t> output_low(TOTAL_LENGTH, 0);
    std::vector<uint32_t> output_high(TOTAL_LENGTH, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input0[i] = i + 1;
        input1[i] = 0x10000000U + i;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* input0_device = nullptr;
    uint32_t* input1_device = nullptr;
    uint32_t* output_low_device = nullptr;
    uint32_t* output_high_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_low_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_high_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, BUFFER_BYTES, input0.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, BUFFER_BYTES, input1.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_mull_kernel<<<1, 0>>>(output_low_device, output_high_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output_low.data(), BUFFER_BYTES, output_low_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output_high.data(), BUFFER_BYTES, output_high_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        const uint64_t expected = static_cast<uint64_t>(input0[i]) * input1[i];
        if (output_low[i] != static_cast<uint32_t>(expected) ||
            output_high[i] != static_cast<uint32_t>(expected >> 32)) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_mull completed."
                         : "[Failed] asc_mull output mismatch.")
              << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output_low_device);
    aclrtFree(output_high_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
