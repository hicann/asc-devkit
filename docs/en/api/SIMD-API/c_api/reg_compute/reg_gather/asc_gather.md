---
okftype: reference.api
description: "Reg gather API that gathers source vector elements into the destination vector element by element according to an index vector register. Each element of dst is taken from the position specified by the index in src. Provides both write-to-dst-reference and function-return-value forms. Supports multiple 8/16/32-bit data and index types. Takes effect only on the AI Vector."
tag:
  - roles: [compute]
  - operations: [gather]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of vector data registers and Vector Length description"
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
    scope: "overload:asc_gather__Rvf32_vf32_vu32"
    scope_basis: resolved
    note: "Added float data overloads for register-source gather mode: dst/src are vector_float and index is vector_uint32_t. Based on CANN_9_2_0 Release Notes line 55 (register-source asc_gather adds vector_float overloads) and the first appearance of this declaration in upstream/9.2.0 (this combination does not exist in 9.1.0 or earlier)."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added function-return-value overloads for 13 dtype/index_dtype combinations (asc_gather__vi8_vu8 through asc_gather__vf32_vu32). The original dst-reference forms are retained. These overloads belong to the batch in CANN_9_2_0 that supplements function-return-value forms for Reg gather and other APIs (Release Notes line 54), and are confirmed by the first appearance of these declarations in upstream/9.2.0. Because the same batch contains 13 overloads, they are registered together under the Prototype section."
---
# asc_gather

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.081016+00:00 pushedAt=2026-11-16T02:10:56.709Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_gather.h"`.

Collects the source operand `src` into the destination operand `dst` element-wise based on the index position `index`.

**Register source gather mode**: Both the source operand and the destination operand are vector data registers. Based on the element-wise indices stored in the `index` vector register, elements at the corresponding positions are selected from the `src` vector data register and written to the corresponding positions of `dst`. No mask is used.

**Figure 1** Register source gather mode

![Register source gather mode](../../../../figures/asc_gather_vec.png)

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_gather(vector_<dtype>& dst,
                                       vector_<dtype> src,
                                       vector_<index_dtype> index)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_gather(vector_<dtype> src,
                                                 vector_<index_dtype> index)
```

### Supported Data Types

**Table 1** Supported data types for register source gather mode

| dtype | index_dtype |
| :---------- | :-------------- |
| int8_t | uint8_t |
| uint8_t | uint8_t |
| hifloat8_t | uint8_t |
| fp8_e8m0_t | uint8_t |
| fp8_e5m2_t | uint8_t |
| fp8_e4m3fn_t | uint8_t |
| int16_t | uint16_t |
| uint16_t | uint16_t |
| half | uint16_t |
| bfloat16_t | uint16_t |
| int32_t | uint32_t |
| uint32_t | uint32_t |
| float | uint32_t |

### Typical Prototype Example

```c
// Example: dtype/index_dtype data types are int16_t/uint16_t, respectively.
// Output the result through a reference parameter.
__simd_callee__ inline void asc_gather(vector_int16_t& dst,
                                       vector_int16_t src,
                                       vector_uint16_t index)

// Return the result through the function return value.
__simd_callee__ inline vector_int16_t asc_gather(vector_int16_t src,
                                                 vector_uint16_t index)
```

## Parameters

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). The dtype must be the same as dst. |
| index | Input | Data index (vector data register). The unit is the number of elements, dst[i] = src[index[i]]. |

For details about vector data registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the return value returns the gathered result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Both the source and destination are vector data registers.

### Register Source Gather Mode Constraints

- `src` is of vector data register type, with a fixed bit width of [Vector Length (VL)](../../defs/type/data_type_definition.md) and a fixed number of stored elements. If an index value in `index` exceeds the maximum number of elements that can be stored in the current vector data register, it is handled as follows: let the maximum number of data elements that can be stored in the current vector data register be `VL_T`, and let the index value in `index` be `i`; the index value is updated to `i % VL_T`.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t DATA_BYTES = 133 * sizeof(uint16_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint16_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_BUFFER_BYTES = PADDED_ELEMENT_COUNT * sizeof(uint16_t);

__simd_vf__ inline void gather_vf(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input, __ubuf__ uint16_t* indices, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        vector_uint16_t src;
        asc_loadalign(src, input + offset);
        vector_uint16_t index;
        asc_loadalign(index, indices + offset);
        vector_uint16_t dst;
        asc_gather(dst, src, index);
        asc_storealign(output + offset, dst, mask);
    }
}

__global__ __vector__ void asc_gather_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input,
                                             __gm__ uint16_t* indices)
{
    asc_init();
    __ubuf__ uint16_t output_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint16_t index_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(index_local, indices, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<gather_vf>(output_local, input_local, index_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint16_t> indices(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint16_t> output(PADDED_ELEMENT_COUNT, 0xff);
    std::vector<uint16_t> golden(PADDED_ELEMENT_COUNT, 0xff);
    for (uint32_t i = 0; i < PADDED_ELEMENT_COUNT; ++i) {
        input[i] = static_cast<uint16_t>(i);
    }
    for (uint32_t offset = 0; offset < TOTAL_LENGTH; offset += ELEMENTS_PER_REGISTER) {
        const uint32_t valid_count = (TOTAL_LENGTH - offset > ELEMENTS_PER_REGISTER) ?
            ELEMENTS_PER_REGISTER : TOTAL_LENGTH - offset;
        for (uint32_t i = 0; i < valid_count; ++i) {
            indices[offset + i] = static_cast<uint16_t>(valid_count - 1 - i);
            golden[offset + i] = input[offset + indices[offset + i]];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* index_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&index_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint16_t), input.data(), input.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(index_device, indices.size() * sizeof(uint16_t), indices.data(), indices.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_gather_kernel<<<1, 0>>>(output_device, input_device, index_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint16_t), output_device, output.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden", golden);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) passed = passed && output[i] == golden[i];
    std::cout << (passed ? "[Success] asc_gather completed." : "[Failed] asc_gather output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(index_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
