---
okftype: reference.api
description: "Reads an fp4x2_e2m1_t vector from discrete quarter positions such as indices 0, 4, 8 or 1, 5, 9 according to the src_quarter_pos tag and converts it to bfloat16_t. The result is written to the destination register or returned as the return value. This API takes effect only on the AI Vector."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Vector data register and mask register definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "The general constraints of Reg vector computation C APIs must be followed"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "The source type alias is standardized from vector_f4e2m1x2 to vector_fp4x2_e2m1_t (in the same batch as the CANN_9_1_0 type alias correction for the same family); the API capability itself has existed since 9.0.0."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added an overload that returns the result through the function return value (returning vector_bfloat16_t); the reference parameter form has been available since 9.0.0, and in 9.2.0 the two forms total 8 declarations."
  - since: "9.3.0-beta.1"
    kind: parameter_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "The read position is changed to the explicit compile-time tag src_quarter_pos (std::integral_constant<asc_position_quarter_mode,...> with four discrete positions); the original position suffix prototypes are deprecated since 9.2.0. See the corresponding deprecated_interface page."
---
# asc_e2m1x22bfloat16

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.720207+00:00 pushedAt=2026-11-16T02:10:56.005Z nl=2 -->

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

The header file path is `"c_api/reg_compute/reg_convert.h"`.

Converts each `fp4x2_e2m1_t` element in `src` that participates in the computation to the `bfloat16_t` type according to `mask`, and writes the result to `dst`. Because the bit-width ratio of the source operand type to the destination operand type is 1:4, the elements in `src` are divided into four groups by index. The `src_quarter_pos` parameter discretely selects index positions at intervals of 4 elements from `src` for reading, specifically indices 0, 4, 8, ..., indices 1, 5, 9, ..., indices 2, 6, 10, ..., or indices 3, 7, 11, ...

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_e2m1x22bfloat16(vector_bfloat16_t& dst,
                                                vector_fp4x2_e2m1_t src,
                                                vector_bool mask,
                                                std::integral_constant<asc_position_quarter_mode, <quarter_pos>> src_quarter_pos)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_bfloat16_t asc_e2m1x22bfloat16(vector_fp4x2_e2m1_t src,
                                                             vector_bool mask,
                                                             std::integral_constant<asc_position_quarter_mode, <quarter_pos>> src_quarter_pos)
```

**Placeholder description:**

`<quarter_pos>` supports four values of the `asc_position_quarter_mode` enum class:

```c
enum class asc_position_quarter_mode {
    DISPERSE_FIRST_QUARTER = 0,
    DISPERSE_SECOND_QUARTER,
    DISPERSE_THIRD_QUARTER,
    DISPERSE_FOURTH_QUARTER
};
```

### Typical Prototype Example

```c
// Output the result through a reference parameter.
__simd_callee__ inline void asc_e2m1x22bfloat16(vector_bfloat16_t& dst,
                                                vector_fp4x2_e2m1_t src,
                                                vector_bool mask,
                                                std::integral_constant<asc_position_quarter_mode,
                                                                       asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> src_quarter_pos)

// Return the result through the function return value.
__simd_callee__ inline vector_bfloat16_t asc_e2m1x22bfloat16(vector_fp4x2_e2m1_t src,
                                                             vector_bool mask,
                                                             std::integral_constant<asc_position_quarter_mode,
                                                                                    asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> src_quarter_pos)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Mask register, which controls whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |
| src_quarter_pos | Input | Quarter-position selection tag (compile-time tag dispatch, with the implementation selected through compile-time overloading), used to select the read position of the source operand. The following compile-time tags can be passed: `ASC_DISPERSE_FIRST_QUARTER`, `ASC_DISPERSE_SECOND_QUARTER`, `ASC_DISPERSE_THIRD_QUARTER`, and `ASC_DISPERSE_FOURTH_QUARTER`. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the function return value returns the computation result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The position selection tag parameter can only use compile-time constants. The compiler dispatches to the corresponding overload at compile time based on this parameter.
- When the mask bit in `mask` is 0, the corresponding element in `dst` is set to 0.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "acl/acl.h"
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DATA_BYTES = 260;
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_BUFFER_BYTES = REPEAT_COUNT * ELEMENTS_PER_REGISTER;

__simd_vf__ inline void convert(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH / sizeof(uint8_t);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b8(remaining_length);
        vector_bfloat16_t dst;
        vector_fp4x2_e2m1_t src;
        asc_loadalign(dst, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src, reinterpret_cast<__ubuf__ fp4x2_e2m1_t*>(input + offset));
        asc_e2m1x22bfloat16(dst, src, mask, ASC_DISPERSE_FIRST_QUARTER);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst, mask);
    }
}

__global__ __vector__ void asc_e2m1x22bfloat16_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_BUFFER_BYTES];
    __ubuf__ uint8_t input_local[PADDED_BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<convert>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(PADDED_BUFFER_BYTES, 0);
    std::vector<uint8_t> output(PADDED_BUFFER_BYTES, 0xff);
    const std::vector<uint8_t> golden(PADDED_BUFFER_BYTES, 0);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, PADDED_BUFFER_BYTES, input.data(), PADDED_BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_e2m1x22bfloat16_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), PADDED_BUFFER_BYTES, output_device, PADDED_BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_e2m1x22bfloat16 passed." : "[Failed] asc_e2m1x22bfloat16 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
