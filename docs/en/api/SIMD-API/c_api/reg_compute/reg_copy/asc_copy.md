---
okftype: reference.api
description: "API for completing data movement between registers, supporting three modes: vector data register to vector data register, mask register to mask register, and extracting bits from a specified data block to generate a mask, taking effect only on AI Vector."
tag:
  - roles: [data_movement]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "For general constraints of Reg vector computation C APIs, see the general constraints section of this page"
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: related
    condition: "Product model to NPU architecture mapping and macro descriptions required for compilation commands"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_copy__Rvb_vu16_i16"
    scope_basis: resolved
    note: "Adds an overload for copying a vector data register to a mask register: the source is vector_uint16_t, the second parameter is the compile-time constant part (value range 0 to 15), and each bit of the selected 16B data block is broadcast to 2 bits to generate a 32B mask; based on line 128 of the CANN_9_1_0 Release Notes and the first appearance of this declaration in upstream/9.1.0."
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_copy__Rvb_vu32_i16"
    scope_basis: resolved
    note: "Adds an overload for copying a vector data register to a mask register: the source is vector_uint32_t, the second parameter is the compile-time constant part (value range 0 to 31), and each bit of the selected 8B data block is broadcast to 4 bits to generate a 32B mask; added in the same batch as the uint16_t overload (line 128 of the CANN_9_1_0 Release Notes)."
---
# asc_copy

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.008793+00:00 pushedAt=2026-11-16T02:10:56.562Z nl=2 -->

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

The header file path is `"c_api/reg_compute/reg_copy.h"`.

Completes data movement between registers and supports the following three modes:

- Vector data register to vector data register: Copies the valid elements selected by the mask in src to the corresponding positions in dst. Positions not selected by the mask retain the original values of dst.

- Mask register to mask register: Copies all bits of src to dst. Supports two overloads, with and without a mask:
    - With mask: Copies the bits selected by the mask in src to dst. For b8 mode, each bit of the mask controls one bit of src; for b16 mode, every 2 bits of the mask form a group and only the LSB controls one bit of src; for b32 mode, every 4 bits of the mask form a group and only the LSB controls one bit of src.
    - Without mask: Copies all bits of src to dst.

- Vector data register to mask register:
    - When the data type is uint16_t, the vector data register (256B) is divided into 16 data blocks (16B). The user can select one data block through a parameter. Each bit of the data block is broadcast to 2 bits and becomes 32B, which is the output mask.
    - When the data type is uint32_t, the vector data register (256B) is divided into 32 data blocks (8B). The user can select one data block through a parameter. Each bit of the data block is broadcast to 4 bits and becomes 32B, which is the output mask.

    For the specific data movement principle, see [Key Feature Description](#key-feature-description).

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Vector Data Register to Vector Data Register

```c
// Placeholder form.
__simd_callee__ inline void asc_copy(vector_<dtype>& dst,
                                     vector_<dtype> src,
                                     vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

#### Typical Prototype Example

```c
// Example: Copy a half vector data register.
__simd_callee__ inline void asc_copy(vector_half& dst,
                                     vector_half src,
                                     vector_bool mask)
```

### Mask Register to Mask Register

```c
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_bool src,
                                     vector_bool mask)
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_bool src)
```

### Vector Data Register to Mask Register

**The value of part must be determined at compile time. It can be an integer literal or a constexpr integer constant. Runtime variables are not supported.**

```c
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_uint16_t src,
                                     int16_t part)
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_uint32_t src,
                                     int16_t part)
```

## Parameters

**Table 1** Parameter description (vector data register copied to vector data register)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination vector data register. Its dtype must be exactly the same as that of src. Positions where the mask bit is 1 are written with the corresponding elements (or corresponding bits) of src; positions where the mask bit is 0 are written with the original value of dst in Merging mode. dst must be initialized; otherwise, unknown values will appear when the mask is 0. |
| src | Input | Source vector data register. Its dtype must be exactly the same as that of dst. |
| mask | Input | Source mask register, which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask keep their original values in the output. |

**Table 2** Parameter description (mask register copied to mask register)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination mask register. |
| src | Input | Source mask register. |
| mask | Input | Source mask register, which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

**Table 3** Parameter description (vector data register copied to mask register)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination mask register, into which all bits of src are written. |
| src | Input | Source vector data register. |
| part | Input | part determines the data block to be copied from src. For details, see [Key feature description](#key-feature-description).<br>&bull; When the data type is uint16_t, part∈[0, 15].<br>&bull; When the data type is uint32_t, part∈[0, 31]. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Both dst and src are vector data registers or mask registers.
- For vector data register copied to vector data register: positions in dst that are not selected by the mask keep their original values.
- For mask register copied to mask register: in the API with a mask, positions in dst that are not selected by the mask are filled with 0.
- For vector register copied to mask register: **the value of part must be determined at compile time. It can be an integer literal or a constexpr integer constant. Runtime variables are not supported.**
    - When the data type is uint16_t, part∈[0, 15].
    - When the data type is uint32_t, part∈[0, 31].

## Key Feature Description

**For vector data register copied to mask register:**

According to the principle of [Mask Register](../../defs/type/data_type_definition.md#mask-register), when the operand type is b16, each element corresponds to 2 bits of the mask register, and only the lowest bit of the 2 bits is valid. When the operand type is b32, each element corresponds to 4 bits of the mask register, and only the lowest bit of the 4 bits is valid.

The data movement principle is shown in [Figure 1](#fig-maskgen-b16) and [Figure 2](#fig-maskgen-b32).

- When the operand type is uint16_t, one data block of the vector data register is 16B. After each bit is broadcast to 2 bits, it becomes 32B, that is, one MaskReg (32B). In addition, a vector data register has 256B/16B = 16 data blocks, and the offset part ranges from [0, 15].
- When the operand type is uint32_t, one data block of the vector data register is 8B. After each bit is broadcast to 4 bits, it becomes 32B, that is, one MaskReg (32B). In addition, a vector data register has 256B/8B = 32 data blocks, and the offset part ranges from [0, 31].

**Figure 1** Vector register copied to mask register (uint16_t)<a id="fig-maskgen-b16"></a>

![Vector register copied to mask register (uint16_t)](../../../../figures/capi_copy_reg2mask_b16.png)

**Figure 2** Vector register copied to mask register (uint32_t)<a id="fig-maskgen-b32"></a>

![Vector register copied to mask register (uint32_t)](../../../../figures/capi_copy_reg2mask_b32.png)

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


template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t ELEMENT_COUNT = 260;
constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (ELEMENT_COUNT + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t DATA_BYTES = ELEMENT_COUNT * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = ELEMENT_COUNT;
__simd_vf__ inline void load(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = ELEMENT_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
    const uint32_t offset = i * ELEMENTS_PER_REGISTER;
    vector_bool mask = asc_update_mask_b8(remaining_length);
    vector_int8_t arg_0_0;
    asc_loadalign(arg_0_0, reinterpret_cast<__ubuf__ int8_t*>(input + offset));
    vector_int8_t arg_0_1;
    asc_loadalign(arg_0_1, reinterpret_cast<__ubuf__ int8_t*>(input + offset));
    asc_copy(arg_0_0, arg_0_1, mask);
    asc_storealign(reinterpret_cast<__ubuf__ int8_t*>(output + offset), arg_0_0, mask);
    }
}
__global__ __vector__ void asc_copy_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES], input_local[BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<load>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint8_t> input(BUFFER_BYTES), output(BUFFER_BYTES, 0xff);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = true;
    std::cout << "[Success] asc_copy completed." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
