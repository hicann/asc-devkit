---
okftype: reference.api
description: "Converts half elements in the vector data register to int4x2_t elements according to the mask, packing two consecutive half values into the same int4x2_t storage unit (1 byte), mas."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode, split_high_low]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Rounding and saturation mode description"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "page"
    scope_basis: kind_default
    note: "CANN 9.2.0 Release Notes item 61 adds explicit quarter-position tag overloads for asc_half2int4x2 (asc_position_quarter_mode with four position values, combined with 5 rounding modes and saturation/non-saturation for a total of 80 declarations, all with archs=3510). 9.0.0 has only the position-mode overload, with unchanged parameter and return value forms."
---
# asc_half2int4x2

<!-- md-trans-meta sourceCommit=59317b6fda711b248a190c30d2db6306b5b584f1 translatedAt=2026-11-16T01:40:02.816156+00:00 pushedAt=2026-11-16T02:10:56.183Z nl=2 -->

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

Converts the `half` elements in `src` that participate in the computation to the `int4x2_t` type according to `mask`. The result can be output through a reference parameter or the function return value.

Each `half` element in `src` corresponds to one mask bit in `mask`. Two consecutive `half` elements are converted to two consecutive 4-bit signed integers, and the conversion results are packed into the same `int4x2_t` storage unit in input order, occupying 1 byte in total. When the mask bit in `mask` is 0, the corresponding 4-bit conversion result is set to 0.

Because the bit-width ratio of the source operand type to the destination operand type is 4:1, the `int4x2_t` storage units in `dst` are divided into four groups by index. The `dst_quarter_pos` parameter discretely selects index positions at intervals of 4 storage units in `dst` to write the conversion results, specifically indices 0, 4, 8, ..., indices 1, 5, 9, ..., indices 2, 6, 10, ..., or indices 3, 7, 11, ...; the other positions are set to zero.

For details about rounding modes and saturation/non-saturation modes, see [Rounding Modes and Saturation Modes](rounding_mode.md).

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```cpp
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_half2int4x2_<round_mode><sat_mode>(vector_int4x2_t& dst,
                                                                   vector_half src,
                                                                   vector_bool mask,
                                                                   std::integral_constant<asc_position_quarter_mode, <quarter_pos>> dst_quarter_pos)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_int4x2_t asc_half2int4x2_<round_mode><sat_mode>(vector_half src,
                                                                              vector_bool mask,
                                                                              std::integral_constant<asc_position_quarter_mode, <quarter_pos>> dst_quarter_pos)
```

**Placeholder description:**

- `<round_mode>` supports `rd` (FLOOR), `rn` (RINT), `rna` (ROUND), `ru` (CEIL), and `rz` (TRUNC).
- `<sat_mode>` indicates the saturation/non-saturation mode. When empty, it indicates non-saturation mode; when set to `_sat`, it indicates saturation mode.

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

```cpp
// ROUND rounding mode, non-saturation mode, output the result through a reference parameter.
__simd_callee__ inline void asc_half2int4x2_rna(vector_int4x2_t& dst,
                                                vector_half src,
                                                vector_bool mask,
                                                std::integral_constant<asc_position_quarter_mode,
                                                                       asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_quarter_pos)

// ROUND rounding mode, non-saturation mode, return the result through the function return value.
__simd_callee__ inline vector_int4x2_t asc_half2int4x2_rna(vector_half src,
                                                           vector_bool mask,
                                                           std::integral_constant<asc_position_quarter_mode,
                                                                                  asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_quarter_pos)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Mask register, which controls whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |
| dst_quarter_pos | Input | Quarter-position selection tag (compile-time tag dispatch, with the implementation selected through compile-time overloading), used to select the write position of the destination operand. Other positions are set to zero. The following compile-time tags can be passed: `ASC_DISPERSE_FIRST_QUARTER`, `ASC_DISPERSE_SECOND_QUARTER`, `ASC_DISPERSE_THIRD_QUARTER`, and `ASC_DISPERSE_FOURTH_QUARTER`. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the function return value returns the computation result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The position selection tag parameter can only use the compile-time constants described above.
- When using saturation or non-saturation mode, configure it through the [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md) API. For configuration requirements, see the API description.
- When the mask bit in `mask` is 0, the corresponding element in `dst` is set to 0.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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
    uint32_t remaining_length = TOTAL_LENGTH / sizeof(uint16_t);
    uint32_t remaining_length_store = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        vector_bool store_mask = asc_update_mask_b8(remaining_length_store);
        vector_int4x2_t dst;
        vector_half src;
        asc_loadalign(dst, reinterpret_cast<__ubuf__ int4b_t*>(output + offset));
        asc_loadalign(src, reinterpret_cast<__ubuf__ half*>(input + offset));
        asc_half2int4x2_rna(dst, src, mask, ASC_DISPERSE_FIRST_QUARTER);
        asc_storealign(reinterpret_cast<__ubuf__ int4b_t*>(output + offset), dst, store_mask);
    }
}
} // namespace

__global__ __vector__ void asc_half2int4x2_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_BUFFER_BYTES];
    __ubuf__ uint8_t input_local[PADDED_BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, output, PADDED_BUFFER_BYTES);
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

int main()
{
    constexpr uint8_t input_pattern[] = {0x00, 0x3c};
    constexpr uint8_t golden_pattern[] = {0x11, 0x00, 0x00, 0x00};
    std::vector<uint8_t> input(PADDED_BUFFER_BYTES);
    std::vector<uint8_t> output(PADDED_BUFFER_BYTES, 0);
    std::vector<uint8_t> golden(PADDED_BUFFER_BYTES);
    for (size_t i = 0; i < PADDED_BUFFER_BYTES; ++i) {
        input[i] = input_pattern[i % sizeof(input_pattern)];
        golden[i] = (i < TOTAL_LENGTH) ? golden_pattern[i % sizeof(golden_pattern)] : output[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, PADDED_BUFFER_BYTES, input.data(), PADDED_BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, PADDED_BUFFER_BYTES, output.data(), PADDED_BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_half2int4x2_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), PADDED_BUFFER_BYTES, output_device, PADDED_BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (size_t i = 0; i < PADDED_BUFFER_BYTES; ++i) passed = passed && (output[i] == golden[i]);
    std::cout << (passed ? "[Success] asc_half2int4x2_rna passed."
                         : "[Failed] asc_half2int4x2_rna failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
