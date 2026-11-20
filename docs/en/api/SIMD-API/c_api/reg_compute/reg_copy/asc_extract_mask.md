---
okftype: reference.api
description: "API that extracts bits from a specified data block of a vector data register, expands them into a mask register, and returns the result through the function return value, supporting uint16_t and uint32_t source types, where part must be a compile-time constant, taking effect only on AI Vector."
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
    condition: "For the movement principle and the dst reference form API with the same function, see the key feature description of this page"
version: ">=9.2.0"
version_changes: []
---
# asc_extract_mask

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.015317+00:00 pushedAt=2026-11-16T02:10:56.571Z nl=2 -->

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

Extracts a mask register from a vector data register and returns the result through the function return value.

- When the data type is uint16_t, the vector data register (256B) is divided into 16 data blocks (16B). The user can select a data block through a parameter. After each bit of the data block is broadcast to 2 bits, it becomes 32B, which is the output mask.
- When the data type is uint32_t, the vector data register (256B) is divided into 32 data blocks (8B). You can select a data block through a parameter. Each bit of the data block is broadcast to 4 bits to become 32B, which is the output mask.

For the detailed data movement principle, see [Key Feature Description of asc_copy](asc_copy.md#key-feature-description).

This API has the same function as the vector data register to mask register copy mode of [asc_copy](asc_copy.md), except that this API returns the result through the function return value.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

**The value of part must be determined at compile time. It can be an integer literal or a constexpr integer constant. Runtime variables are not supported.**

```c
__simd_callee__ inline vector_bool asc_extract_mask(vector_uint16_t src,
                                                    int16_t part)
__simd_callee__ inline vector_bool asc_extract_mask(vector_uint32_t src,
                                                    int16_t part)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source vector data register. |
| part | Input | part determines the data block to be moved from src. For details, see [Key feature description of asc_copy](asc_copy.md#key-feature-description).<br>&bull; When the data type is uint16_t, part ∈ [0, 15].<br>&bull; When the data type is uint32_t, part ∈ [0, 31]. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

Returns the mask register that stores the extraction result, of type `vector_bool`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- src is a vector data register.
- **The value of part must be determined at compile time. It can be an integer literal or a constexpr integer constant. Runtime variables are not supported.**
    - When the data type is uint16_t, part∈[0, 15].
    - When the data type is uint32_t, part∈[0, 31].

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510&& ./main
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

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t ELEMENT_COUNT = 323;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (ELEMENT_COUNT + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t STORAGE_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t MASK_WORDS_PER_REGISTER = 8;
constexpr uint32_t OUTPUT_WORD_COUNT = REPEAT_COUNT * MASK_WORDS_PER_REGISTER;
constexpr uint32_t DATA_BYTES = ELEMENT_COUNT * sizeof(uint16_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = ELEMENT_COUNT;
__simd_vf__ inline void extract_mask(__ubuf__ uint32_t* output, __ubuf__ uint16_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t src_reg;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = ELEMENT_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * ELEMENTS_PER_REGISTER;
        vector_bool valid_mask = asc_update_mask_b16(remaining_length);
        asc_loadalign(src_reg, input + offset);
        vector_bool mask = asc_extract_mask(src_reg, 0);
        mask = asc_and(mask, valid_mask, asc_create_mask_b16(PAT_ALL));
        asc_storealign(output + i * MASK_WORDS_PER_REGISTER, mask);
    }
}
__global__ __vector__ void asc_extract_mask_kernel(__gm__ uint32_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint32_t output_local[OUTPUT_WORD_COUNT];
    __ubuf__ uint16_t input_local[STORAGE_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, STORAGE_COUNT * sizeof(uint16_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<extract_mask>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_WORD_COUNT * sizeof(uint32_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint16_t> input(STORAGE_COUNT, 0);
    std::vector<uint32_t> output(OUTPUT_WORD_COUNT, 0);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) input[i] = static_cast<uint16_t>(i);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (STORAGE_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (OUTPUT_WORD_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint16_t), input.data(), input.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_extract_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint32_t), output_device, output.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    const bool passed = true;
    std::cout << "[Success] asc_extract_mask completed." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
