---
okftype: reference.api
description: "API that completes masked copy through the function return value, supporting two modes: vector data register to vector data register and mask register to mask register. Positions not selected by the mask are undefined or set to zero. It takes effect only on the AI Vector."
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
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_copy/asc_copy.md
    kind: related
    condition: "For the dst reference form of the same function, see the mask-to-mask mode on this page"
version: ">=9.2.0"
version_changes: []
---
# asc_mask

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.019318+00:00 pushedAt=2026-11-16T02:10:56.582Z nl=2 -->

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

Copies the valid elements selected by mask from src to the corresponding positions of the return value. Positions not selected by mask behave differently depending on the source/destination register. The result is returned through the function return value. The following two modes are supported:

- Vector data register to vector data register: Copies the valid elements selected by mask from src to the corresponding positions of the return value. The values at positions not selected by mask are undefined.
- Mask register to mask register: Copies the bits selected by mask from src to the return value. Positions not selected by mask are set to 0. Both masked and unmasked overloads are supported:
    - With mask: Copies the bits selected by mask from src to the return value. For b8 mode, each bit of mask controls one bit of src. For b16 mode, every 2 bits of mask form a group, and only the LSB controls one bit of src. For b32 mode, every 4 bits of mask form a group, and only the LSB controls one bit of src.
    - Without mask: Copies all bits of src to the return value.

This API has the same function as the mask register to mask register copy mode of [asc_copy](asc_copy.md), except that this API returns the result through the function return value.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Vector Data Register to Vector Data Register

```c
__simd_callee__ inline vector_<dtype> asc_mask(vector_<dtype> src,
                                               vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

#### Typical Prototype Example

```c
// Example: Perform masked copy on a half vector data register.
__simd_callee__ inline vector_half asc_mask(vector_half src,
                                            vector_bool mask)
```

### Mask Register to Mask Register

```c
__simd_callee__ inline vector_bool asc_mask(vector_bool src,
                                            vector_bool mask)
__simd_callee__ inline vector_bool asc_mask(vector_bool src)
```

## Parameters

**Table 1** Parameter description (vector data register copied to vector data register)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source vector data register. |
| mask | Input | Source mask register, which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. The values at positions not selected by the mask are undefined. |

**Table 2** Parameter description (mask register copied to mask register)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source mask register. |
| mask | Input | Source mask register, which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to 0 in the return value. Only the masked overload includes this parameter. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- Vector data register to vector data register: Returns the vector data register that stores the masked copy result, with the same data type as `src`.
- Mask register to mask register: Returns the mask register that stores the masked copy result, of type `vector_bool`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- src is a vector data register or mask register.

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
    vector_int8_t result = asc_mask(arg_0_0, mask);
    asc_storealign(reinterpret_cast<__ubuf__ int8_t*>(output + offset), result, mask);
    }
}
__global__ __vector__ void asc_mask_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    asc_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = true;
    std::cout << "[Success] asc_mask completed." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
