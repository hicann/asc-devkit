---
okftype: reference.api
description: "Interleaves the elements of two source vector data registers or mask registers and writes them into two destination registers respectively. In mask mode, interleaving is performed at the bit-width granularity specified by the b8, b16, or b32 suffix. This API takes effect only on the AI Vector."
tag:
  - roles: [compute]
  - operations: [interleave]
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
    note: "The declaration location was unified from include/c_api/reg_compute/reg_vector.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_permute_sel.h (starting from 9.2.0); this page has only the reference output form, with no return value overload additions or removals."
---
# asc_intlv

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.176533+00:00 pushedAt=2026-11-16T02:10:56.907Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_permute_sel.h"`.

Interleaves the data in two source operand vector data registers and writes the results into two destination vector data registers. Depending on the operand type, this API provides two functional modes:

- **Operands are vector data registers:** Interleaving is performed element by element. The elements in the first half of `src0` and `src1` are alternately written to `dst0`, and the elements in the second half are alternately written to `dst1`. Taking the `int8_t` type as an example, the interleaving process is shown in the following figure.
- **Operands are mask registers:** Interleaving is performed at the granularity specified by the API suffix. `asc_intlv_b8` uses 1-bit groups, `asc_intlv_b16` uses 2-bit groups, and `asc_intlv_b32` uses 4-bit groups.

**Figure 1** Interleaving process for the int8_t type

![](../../../../figures/asc_intlv.png)

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

### Vector Data Register Interleaving Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_intlv(vector_<dtype>& dst0,
                                      vector_<dtype>& dst1,
                                      vector_<dtype> src0,
                                      vector_<dtype> src1)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_intlv(vector_int8_t& dst0,
                                      vector_int8_t& dst1,
                                      vector_int8_t src0,
                                      vector_int8_t src1)
```

### Mask Register Interleaving Mode

```c
__simd_callee__ inline void asc_intlv_b8(vector_bool& dst0,
                                         vector_bool& dst1,
                                         vector_bool src0,
                                         vector_bool src1)
__simd_callee__ inline void asc_intlv_b16(vector_bool& dst0,
                                          vector_bool& dst1,
                                          vector_bool src0,
                                          vector_bool src1)
__simd_callee__ inline void asc_intlv_b32(vector_bool& dst0,
                                          vector_bool& dst1,
                                          vector_bool src0,
                                          vector_bool src1)
```

## Parameters

### Vector Data Register Interleaving Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst0 | Output | Destination operand (vector data register), used to store the lower half of the interleaved data. The data type must be the same as that of `src0` and `src1`. |
| dst1 | Output | Destination operand (vector data register), used to store the upper half of the interleaved data. The data type must be the same as that of `src0` and `src1`. |
| src0 | Input | Source operand (vector data register). |
| src1 | Input | Source operand (vector data register). The data type must be the same as `src0`. |

### Mask Register Interleaving Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst0 | Output | Destination operand (mask register), used to store the lower half of the interleaved data. |
| dst1 | Output | Destination operand (mask register), used to store the upper half of the interleaved data. |
| src0 | Input | Source operand (mask register). |
| src1 | Input | Source operand (mask register). |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The source and destination operands are vector data registers or mask registers.

### Register Constraints

- `src0` and `src1` can be the same vector data register or mask register.
- `dst0` and `dst1` cannot be the same vector data register or mask register.
- The source and destination operands can use the same vector data register or mask register, for example, `asc_intlv(src0, src1, src0, src1)`.

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DATA_BYTES = 557 * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t HALF_COUNT = ELEMENTS_PER_REGISTER / 2;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
constexpr uint32_t PADDED_BUFFER_BYTES = PADDED_ELEMENT_COUNT * sizeof(uint8_t);

__simd_vf__ inline void intlv_vf(__ubuf__ uint8_t* output0, __ubuf__ uint8_t* output1,
                                 __ubuf__ uint8_t* input0, __ubuf__ uint8_t* input1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint8_t src0;
    vector_uint8_t src1;
    vector_uint8_t dst0;
    vector_uint8_t dst1;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const uint32_t remaining = TOTAL_LENGTH - offset;
        uint32_t valid_count = remaining < ELEMENTS_PER_REGISTER ? remaining : ELEMENTS_PER_REGISTER;
        uint32_t remaining_length_output = valid_count < HALF_COUNT ? valid_count * 2 : ELEMENTS_PER_REGISTER;
        uint32_t remaining_length_output_2 = valid_count > HALF_COUNT ? (valid_count - HALF_COUNT) * 2 : 0;
        vector_bool output0_mask = asc_update_mask_b8(remaining_length_output);
        vector_bool output1_mask = asc_update_mask_b8(remaining_length_output_2);
        asc_loadalign(src0, input0 + offset);
        asc_loadalign(src1, input1 + offset);
        asc_intlv(dst0, dst1, src0, src1);
        asc_storealign(output0 + offset, dst0, output0_mask);
        asc_storealign(output1 + offset, dst1, output1_mask);
    }
}

__global__ __vector__ void asc_intlv_kernel(__gm__ uint8_t* output0, __gm__ uint8_t* output1,
                                            __gm__ uint8_t* input0, __gm__ uint8_t* input1)
{
    asc_init();
    __ubuf__ uint8_t output0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint8_t output1_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint8_t input0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint8_t input1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input0_local, input0, BUFFER_BYTES);
    asc_copy_gm2ub_align(input1_local, input1, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<intlv_vf>(output0_local, output1_local, input0_local, input1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output0, output0_local, PADDED_BUFFER_BYTES);
    asc_copy_ub2gm_align(output1, output1_local, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input0(TOTAL_LENGTH);
    std::vector<uint8_t> input1(TOTAL_LENGTH);
    std::vector<uint8_t> output0(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint8_t> output1(PADDED_ELEMENT_COUNT, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input0[i] = static_cast<uint8_t>(i);
        input1[i] = static_cast<uint8_t>(255 - i % 256);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input0_device = nullptr;
    uint8_t* input1_device = nullptr;
    uint8_t* output0_device = nullptr;
    uint8_t* output1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output0_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output1_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, BUFFER_BYTES, input0.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, BUFFER_BYTES, input1.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_intlv_kernel<<<1, 0>>>(output0_device, output1_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output0.data(), PADDED_BUFFER_BYTES, output0_device, PADDED_BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output1.data(), PADDED_BUFFER_BYTES, output1_device, PADDED_BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * ELEMENTS_PER_REGISTER;
        const uint32_t remaining = TOTAL_LENGTH - offset;
        const uint32_t valid_count = remaining < ELEMENTS_PER_REGISTER ? remaining : ELEMENTS_PER_REGISTER;
        const uint32_t output0_pairs = valid_count < HALF_COUNT ? valid_count : HALF_COUNT;
        const uint32_t output1_pairs = valid_count > HALF_COUNT ? valid_count - HALF_COUNT : 0;
        for (uint32_t i = 0; i < output0_pairs; ++i) {
            passed = passed && output0[offset + 2 * i] == input0[offset + i]
                     && output0[offset + 2 * i + 1] == input1[offset + i];
        }
        for (uint32_t i = 0; i < output1_pairs; ++i) {
            passed = passed && output1[offset + 2 * i] == input0[offset + i + HALF_COUNT]
                     && output1[offset + 2 * i + 1] == input1[offset + i + HALF_COUNT];
        }
    }
    std::cout << (passed ? "[Success] asc_intlv completed." : "[Failed] asc_intlv output mismatch.")
              << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output0_device);
    aclrtFree(output1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
