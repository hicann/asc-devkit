---
okftype: reference.api
description: "Shifts the elements of the source vector data register left according to a mask. The shift amount is given element by element by the shift vector. Unsigned types perform logical left shift, and signed types perform arithmetic left shift. Covers 6 combinations of dtype0 and dtype1."
tag:
  - roles: [compute]
  - operations: [bitwise_shift]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Must comply with the general constraints of Reg vector computation C APIs"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added function-return-value overloads (returning vector_<dtype0>). The reference-parameter output forms are retained. history_probe shows that vector_uint16_t asc_shiftleft(vector_uint16_t src, vector_int16_t shift, vector_bool mask) first appeared in impl/c_api/reg_base_impl/reg_logic_intf_impl.h:316 in 9.2.0."
---
# asc_shiftleft

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.124365+00:00 pushedAt=2026-11-16T02:10:56.801Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_logic.h"`.

Performs a left shift on the elements in the source operand vector data register based on the mask. The shift amount for each element is specified by the corresponding element in the vector data register `shift`, and the result is returned through the function return value or written to the destination operand. Elements with a corresponding mask bit of 1 participate in the computation, and elements with 0 are set to zero in the output result. The calculation formula is as follows:

$$
dst_i = src_i \ll shift_i
$$

Depending on the data type of the source operand, the left shift operation is classified into the following two cases:

- **Unsigned integer: A logical left shift is performed.** A logical left shift moves the entire binary number to the left by the specified number of bits, discards the most significant bit, and fills the least significant bit with 0. For example, after the binary number 1010101010101010 (`uint16_t` type) is logically shifted left by 1 bit, the result is 0101010101010100.
- **Signed integer: An arithmetic left shift is performed.** An arithmetic left shift moves the entire binary number to the left by the specified number of bits, discards the most significant bit (the sign bit is discarded), and fills the least significant bit with 0. For example, after the binary number 1010101010101010 (`int16_t` type) is arithmetically shifted left by 1 bit, the result is 0101010101010100; after it is arithmetically shifted left by 3 bits, the result is 0101010101010000.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype0> asc_shiftleft(vector_<dtype0> src,
                                                     vector_<dtype1> shift,
                                                     vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_shiftleft(vector_<dtype0>& dst,
                                          vector_<dtype0> src,
                                          vector_<dtype1> shift,
                                          vector_bool mask)
```

### Supported Data Types for dtype0 and dtype1

| `dtype0` | `dtype1` |
|---|---|
| `int8_t` | `int8_t` |
| `uint8_t` | `int8_t` |
| `int16_t` | `int16_t` |
| `uint16_t` | `int16_t` |
| `int32_t` | `int32_t` |
| `uint32_t` | `int32_t` |

### Typical Prototype Example

```c
// Return the result through the function return value.
__simd_callee__ inline vector_uint16_t asc_shiftleft(vector_uint16_t src,
                                                     vector_int16_t shift,
                                                     vector_bool mask)

// Output the result through a reference parameter.
__simd_callee__ inline void asc_shiftleft(vector_uint16_t& dst,
                                          vector_uint16_t src,
                                          vector_int16_t shift,
                                          vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). Only the void-returning API contains this parameter, and the data type must be the same as `src`. |
| src | Input | Source operand (vector data register). |
| shift | Input | Shift amount (vector data register). The data type must have the same bit width as `src` and be a signed integer type. |
| mask | Input | Source operand mask (mask register), which indicates the elements that participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not, with the corresponding element in the output result set to zero. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

For the return-value API, the vector data register that stores the left shift result is returned, and its data type is consistent with `src`.

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Element positions with a mask bit of 0 do not participate in the left shift operation, and the corresponding positions in the output result are written with 0.

### Shift Constraints

<!-- npu="950" id8 -->
- For Ascend 950PR&950DT products, when `shift value` is set to a negative number, a right shift is performed on the source operand. The right shift rule depends on the data type of the source operand:
    - Source operand is an unsigned integer: logical right shift is performed, and the high bits are filled with `0`.
    - Source operand is a signed integer: arithmetic right shift is performed, and the high bits are filled with the sign bit.
- For Ascend 950PR&950DT products, when the `shift value` is negative and its absolute value is greater than the data type bit width of `src`, the output result depends on the data type of the source operand:
    - Source operand is an unsigned integer: The result is `0`.
    - Source operand is a signed integer: The result converges to the sign bit. If the source operand is positive or `0`, the result is `0`; if it is negative, the result is `-1`.
<!-- end id8 -->
- When the `shift value` is positive and greater than the data type bit width of `src`, the valid elements in the output result are set to `0`.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id9 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t DATA_BYTES = 257 * sizeof(uint16_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint16_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(uint16_t);

__simd_vf__ inline void shiftleft_vf(__ubuf__ uint16_t* output,
                                     __ubuf__ uint16_t* input,
                                     __ubuf__ int16_t* shift, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t src;
    vector_int16_t shift_reg;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        asc_loadalign(src, input + offset);
        asc_loadalign(shift_reg, shift + offset);
        vector_uint16_t dst = asc_shiftleft(src, shift_reg, mask);
        asc_storealign(output + offset, dst, mask);
    }
}

__global__ __vector__ void asc_shiftleft_kernel(__gm__ uint16_t* output,
                                                __gm__ uint16_t* input,
                                                __gm__ int16_t* shift)
{
    asc_init();
    __ubuf__ uint16_t output_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int16_t shift_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(shift_local, shift, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<shiftleft_vf>(output_local, input_local, shift_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(TOTAL_LENGTH);
    std::vector<int16_t> shift(TOTAL_LENGTH, 1);
    std::vector<uint16_t> output(TOTAL_LENGTH, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input[i] = static_cast<uint16_t>(i);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    int16_t* shift_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&shift_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(shift_device, BUFFER_BYTES, shift.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_shiftleft_kernel<<<1, 0>>>(output_device, input_device, shift_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        const uint16_t expected = static_cast<uint16_t>(input[i] << shift[i]);
        if (output[i] != expected) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_shiftleft completed."
                         : "[Failed] asc_shiftleft output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(shift_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
