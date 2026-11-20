---
okftype: reference.api
description: "Shifts the elements of the source vector data register right by the number of bits given by the int16_t scalar shift, with masking. Supports six integer types from int8_t to uint32_t. Negative shifts are undefined, and output elements are set to zero when the shift exceeds the bit width."
tag:
  - roles: [compute]
  - operations: [bitwise_shift]
  - traits: [scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Must comply with the general constraints of Reg vector computation C APIs"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: parameter_changed
    scope: "param:shift"
    scope_basis: resolved
    note: "The scalar shift parameter name is changed from value to shift (in 9.0.0 and 9.1.0, include/c_api/reg_compute/reg_vector.h declares it as int16_t value; from 9.2.0, reg_logic.h declares it as int16_t shift). The type and semantics remain unchanged."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Adds an overload in function return value form (returning vector_<dtype>); the reference parameter output form is retained. history_probe shows that vector_int8_t asc_shiftright_scalar(vector_int8_t src, int16_t shift, vector_bool mask) first appeared in 9.2.0 in impl/c_api/reg_base_impl/reg_logic_intf_impl.h:220."
---
# asc_shiftright_scalar

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.141942+00:00 pushedAt=2026-11-16T02:10:56.828Z nl=2 -->

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

Performs a right shift on the elements in the source operand vector data register according to the mask, with the number of right shift bits specified by a scalar immediate value, and returns the result through the function return value or writes it to the destination operand. Elements at positions where the mask is 1 participate in the computation, and elements at positions where the mask is 0 are set to zero in the output result. The calculation formula is as follows:

$$
dst_i = src_i \gg shift
$$

Depending on the data type of the source operand, the right shift operation is classified into the following two cases:

- **Unsigned integer: A logical right shift is performed.** A logical right shift moves the entire binary number to the right by the specified number of bits, discards the least significant bit, and fills the most significant bit with 0. For example, after the binary number 1010101010101010 (`uint16_t` type) is logically shifted right by 1 bit, the result is 0101010101010101.
- **Signed integer: An arithmetic right shift is performed.** An arithmetic right shift moves the entire binary number to the right by the specified number of bits, discards the least significant bit, and fills the most significant bit with a copy of the sign bit. For example, after the binary number 1010101010101010 (`int16_t` type) is arithmetically shifted right by 1 bit, the result is 1101010101010101; after it is arithmetically shifted right by 3 bits, the result is 1111010101010101.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_shiftright_scalar(vector_<dtype> src,
                                                            int16_t shift,
                                                            vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_shiftright_scalar(vector_<dtype>& dst,
                                                  vector_<dtype> src,
                                                  int16_t shift,
                                                  vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`.

### Typical Prototype Example

```c
// Return the result through the function return value.
__simd_callee__ inline vector_int8_t asc_shiftright_scalar(vector_int8_t src,
                                                           int16_t shift,
                                                           vector_bool mask)

// Output the result through a reference parameter.
__simd_callee__ inline void asc_shiftright_scalar(vector_int8_t& dst,
                                                  vector_int8_t src,
                                                  int16_t shift,
                                                  vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand (vector data register). Only the void-returning API contains this parameter, and the data type must be the same as `src`. |
| src | Input | Source operand (vector data register). |
| shift | Input | Shift amount (scalar), of type `int16_t`. |
| mask | Input | Source operand mask (mask register), which indicates the elements that participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not, with the corresponding element in the output result set to zero. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

For the return-value API, the vector data register that stores the right shift result is returned, and its data type is consistent with `src`.

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Element positions with a mask bit of 0 do not participate in the right shift operation, and the corresponding positions in the output result are written with 0.

### Shift Constraints

<!-- npu="950" id8 -->
- For Ascend 950PR&950DT products, when `shift` is set to a negative number, a left shift operation is performed on the source operand, padding the low bits with 0. If the absolute value of `shift` is greater than the bit width of the source operand, the output result is `0`.
<!-- end id8 -->
- When `shift` is set to a positive number and is greater than the data type bit width of `src`, the output result depends on the data type of the source operand:
    - For unsigned integers, the valid elements in the output result are set to zero.
    - For signed integers, if an element in `src` is less than `0`, `-1` is written to the corresponding position in the output result; if an element in `src` is greater than or equal to `0`, `0` is written to the corresponding position in the output result.

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

__simd_vf__ inline void shiftright_scalar_vf(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t src;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        asc_loadalign(src, input + offset);
        vector_uint16_t dst = asc_shiftright_scalar(src, 1, mask);
        asc_storealign(output + offset, dst, mask);
    }
}

__global__ __vector__ void asc_shiftright_scalar_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t output_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<shiftright_scalar_vf>(output_local, input_local, one_repeat_size, repeat_time);
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
    std::vector<uint16_t> output(TOTAL_LENGTH, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input[i] = static_cast<uint16_t>(i);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_shiftright_scalar_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        const uint16_t expected = static_cast<uint16_t>(input[i] >> 1);
        if (output[i] != expected) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_shiftright_scalar completed."
                         : "[Failed] asc_shiftright_scalar output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
