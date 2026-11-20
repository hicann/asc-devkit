---
okftype: reference.api
description: "Reg vector gather API: uses the UB as the source and vector data registers as the destination, reads data from the UB according to the element-wise indices saved in the index vector register and writes it to dst, controlled by the mask with 0 written to unfiltered positions, provides reference output and function return value forms, and takes effect only on the AI Vector."
tag:
  - roles: [data_movement]
  - operations: [gather]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "Vector data registers and mask registers are defined according to their data types"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints of Reg vector computation C APIs follow their overview page"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_gather__Rvi16_PUi16_vu32_vb@3510"
    scope_basis: resolved
    note: "Release Notes 9.1.0 records newly added overloads using vector_uint32_t indices for int16_t, uint16_t, half, and bfloat16_t; this event covers that group of uint32 index overloads"
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_gather__Rvi8_PUi8_vu16_vb@3510"
    scope_basis: resolved
    note: "Release Notes 9.2.0 records newly added int8_t and uint8_t same-width output overloads for the UB-source asc_gather, and simultaneously splits the documentation and header files of the UB-source and register-source APIs into ub_gather.h and reg_gather.h"
---
# asc_gather

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.402756+00:00 pushedAt=2026-11-16T02:10:57.359Z nl=2 -->

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

The header file path is `"c_api/reg_compute/gather/ub_gather.h"`.

Collects the source operand `src` into the destination operand `dst` element-wise based on the index position `index`.

**UB source gather mode**: The source operand is a Unified Buffer (UB) address, and the destination operand is a vector data register. Based on the element-wise indices stored in the `index` vector register, elements at the corresponding positions are read from UB and written to the corresponding positions of `dst`, controlled by `mask`. Positions where the `mask` bit is 0 have 0 written to the corresponding `dst` position.

**Figure 1** UB source gather mode

![UB source gather mode](../../../../figures/asc_gather_ub.png)

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_gather(vector_<dst_dtype>& dst,
                                       __ubuf__ <src_dtype>* src,
                                       vector_<index_dtype> index,
                                       vector_bool mask)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dst_dtype> asc_gather(__ubuf__ <src_dtype>* src,
                                                     vector_<index_dtype> index,
                                                     vector_bool mask)
```

### Data Type Combination List

**Table 1** Supported data type combinations for UB source gather mode

| dst_dtype | src_dtype | index_dtype |
| :---------- | :---------- | :-------------- |
| int16_t | int8_t | uint16_t |
| int8_t | int8_t | uint16_t |
| uint16_t | uint8_t | uint16_t |
| uint8_t | uint8_t | uint16_t |
| hifloat8_t | hifloat8_t | uint16_t |
| fp8_e8m0_t | fp8_e8m0_t | uint16_t |
| fp8_e5m2_t | fp8_e5m2_t | uint16_t |
| fp8_e4m3fn_t | fp8_e4m3fn_t | uint16_t |
| int16_t | int16_t | uint16_t |
| int16_t | int16_t | uint32_t |
| uint16_t | uint16_t | uint16_t |
| uint16_t | uint16_t | uint32_t |
| half | half | uint16_t |
| half | half | uint32_t |
| bfloat16_t | bfloat16_t | uint16_t |
| bfloat16_t | bfloat16_t | uint32_t |
| int32_t | int32_t | uint32_t |
| uint32_t | uint32_t | uint32_t |
| float | float | uint32_t |

> Note: The two bit-width extension combinations `int16_t/int8_t` and `uint16_t/uint8_t` support only the "output result through a reference parameter" form; the "return result through the function return value" form is not supported.

### Typical Prototype Example

```c
// Example: dst_dtype/src_dtype/index_dtype data types are int16_t/int16_t/uint16_t respectively.
// Output the result through a reference parameter.
__simd_callee__ inline void asc_gather(vector_int16_t& dst,
                                       __ubuf__ int16_t* src,
                                       vector_uint16_t index,
                                       vector_bool mask)

// Return the result through the function return value.
__simd_callee__ inline vector_int16_t asc_gather(__ubuf__ int16_t* src,
                                                 vector_uint16_t index,
                                                 vector_bool mask)
```

## Parameters

**Table 2** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Start address of the source operand (vector). The start address in UB must be 32B-aligned and must fall within the UB address space. |
| index | Input | Data index (vector data register). Index position of each element in dst relative to src in UB, in elements. |
| mask | Input | Source operand mask (mask register). Indicates which elements participate in the computation. Elements at positions where the corresponding bit is 1 participate in the computation, and elements at positions where the corresponding bit is 0 do not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the return value returns the gathered result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The source operand is a UB address, and the destination operand is a vector data register.

### UB Source Gather Mode Constraints

- Pointers outside the UB address space cannot be passed as `src`. The start address of the source operand in UB must be 32B-aligned.
- For elements selected by `mask` that need to be transferred, the corresponding addresses must be within the valid UB range. For elements not selected by `mask`, the corresponding addresses do not trigger any address out-of-bounds exception, and the corresponding elements in `dst` are set to zero.
- When `src` is of the b8 data type and `dst` is of the b16 data type, the implementation is a software emulation by the compiler, and the function prototype that outputs the result through the function return value is not supported. The lower 8 bits of the destination operand are the same as the source operand, and the upper 8 bits are automatically padded with 0. For example, when `src` is of the `int8_t` data type and `dst` is of the `int16_t` data type:

    src: 40 = 0b00101000 -> 0b0000000000101000, which equals 40 after extension to 16 bits, so the corresponding dst is 40.

    src: -40 = 0b11011000 -> 0b0000000011011000, which equals 216 after extension to 16 bits, so the corresponding dst is 216.

- When `src` and `dst` have the same data type but differ from the `index` data type, data is written to even-indexed positions of `dst`, and odd-indexed positions are set to zero. For example, when `src` is of the `int8_t` data type and `index` is of the `uint16_t` data type, the applicable scenario is shown in the following figure:

![](../../../../figures/asc_gather_different_type.png)

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

__simd_vf__ inline void gather_vf(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        vector_uint16_t index;
        asc_loadalign(index, input + offset);
        vector_uint16_t dst;
        asc_gather(dst, input, index, mask);
        asc_storealign(output + offset, dst, mask);
    }
}

__global__ __vector__ void asc_gather_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t output_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_ELEMENT_COUNT * sizeof(uint16_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<gather_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, TOTAL_LENGTH * sizeof(uint16_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint16_t> output(TOTAL_LENGTH, 0xff);
    std::vector<uint16_t> golden(TOTAL_LENGTH);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input[i] = static_cast<uint16_t>(TOTAL_LENGTH - 1 - i);
    }
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        golden[i] = input[input[i]];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_ELEMENT_COUNT * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (TOTAL_LENGTH) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint16_t), input.data(), input.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_gather_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint16_t), output_device, output.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_gather completed." : "[Failed] asc_gather output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
