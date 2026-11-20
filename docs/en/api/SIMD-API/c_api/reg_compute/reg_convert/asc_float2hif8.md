---
okftype: reference.api
description: "Converts a float vector to hifloat8_t using rna or rh rounding and saturation or non-saturation mode, and writes the result to discrete quarter positions such as indices 0, 4, 8 or 1, 5, 9 according to the dst_quarter_pos tag, with the remaining positions cleared to zero. The result can also be returned as the return value. This API takes effect only on the AI Vector."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "The general constraints of Reg vector computation C APIs must be followed"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Rounding and saturation mode description"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_flag.md
    kind: requires
    condition: "Saturation or non-saturation mode requires configuration through the asc_set_saturation_flag API before use"
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: parameter_added
    scope: "param:dst_quarter_pos"
    scope_basis: resolved
    note: "The destination position is changed from the function name suffix (_v2/_v3/_v4, including _sat combinations) to the explicit compile-time tag parameter dst_quarter_pos (std::integral_constant<asc_position_quarter_mode, DISPERSE_*_QUARTER>); the old three-parameter form is marked ASC_DEPRECATED since 9.2.0 (starting at reg_convert.h:4150)."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added an overload that outputs the result through the function return value (returning vector_hifloat8_t, with parameters src, mask, and dst_quarter_pos); since 9.2.0, the two forms total 32 declarations (four modes rna/rna_sat/rh/rh_sat × 4 discrete positions × 2 result forms), whereas previously there were only 4 three-parameter reference output declarations."
---
# asc_float2hif8

<!-- md-trans-meta sourceCommit=59317b6fda711b248a190c30d2db6306b5b584f1 translatedAt=2026-11-16T01:40:02.759758+00:00 pushedAt=2026-11-16T02:10:56.076Z nl=2 -->

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

Converts the `float` elements in `src` that participate in the computation to the `hifloat8_t` type according to `mask`, and writes the results to `dst`. Because the bit-width ratio of the source operand type to the destination operand type is 4:1, the elements in `dst` are divided into four groups by index. The `dst_quarter_pos` parameter discretely selects index positions at intervals of 4 elements in `dst` to write the conversion results, specifically indices 0, 4, 8, ..., indices 1, 5, 9, ..., indices 2, 6, 10, ..., or indices 3, 7, 11, ...; the other positions are set to zero.

For details about rounding modes and saturation/non-saturation modes, see [Rounding Modes and Saturation Modes](rounding_mode.md).

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_float2hif8_<round_mode><sat_mode>(vector_hifloat8_t& dst,
                                                                  vector_float src,
                                                                  vector_bool mask,
                                                                  std::integral_constant<asc_position_quarter_mode, <quarter_pos>> dst_quarter_pos)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_hifloat8_t asc_float2hif8_<round_mode><sat_mode>(vector_float src,
                                                                               vector_bool mask,
                                                                               std::integral_constant<asc_position_quarter_mode, <quarter_pos>> dst_quarter_pos)
```

**Placeholder description:**
- `<round_mode>` supports `rna` (ROUND) and `rh` (HIFLOAT8-specific rounding mode).
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

```c
// ROUND rounding mode, non-saturation mode, output the result through a reference parameter.
__simd_callee__ inline void asc_float2hif8_rna(vector_hifloat8_t& dst,
                                               vector_float src,
                                               vector_bool mask,
                                               std::integral_constant<asc_position_quarter_mode,
                                                                      asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_quarter_pos)

// ROUND rounding mode, non-saturation mode, return the result through the function return value.
__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna(vector_float src,
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
| dst_quarter_pos | Input | Quarter-position selection tag (compile-time tag dispatch, with the implementation selected through compile-time overloading), used to select the write address of the destination operand. Other positions are set to zero. The following compile-time tags can be passed: `ASC_DISPERSE_FIRST_QUARTER`, `ASC_DISPERSE_SECOND_QUARTER`, `ASC_DISPERSE_THIRD_QUARTER`, and `ASC_DISPERSE_FOURTH_QUARTER`. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the function return value returns the computation result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The position selection tag parameter can only use compile-time constants. The compiler dispatches to the corresponding overload at compile time based on this parameter.
- The position selection tag selects the write address of the destination operand. Other positions are set to zero.
- When the mask bit in `mask` is 0, the corresponding element in `dst` is set to 0.
- When using saturation or non-saturation mode, configure it through the [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md) API. For configuration requirements, see the API description.

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
    uint32_t remaining_length = TOTAL_LENGTH / sizeof(uint32_t);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        vector_hifloat8_t dst;
        vector_float src;
        asc_loadalign(dst, reinterpret_cast<__ubuf__ hifloat8_t*>(output + offset));
        asc_loadalign(src, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2hif8_rna(dst, src, mask, ASC_DISPERSE_FIRST_QUARTER);
        asc_storealign(reinterpret_cast<__ubuf__ hifloat8_t*>(output + offset), dst, mask);
    }
}

__global__ __vector__ void asc_float2hif8_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    asc_float2hif8_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), PADDED_BUFFER_BYTES, output_device, PADDED_BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_float2hif8_rna passed." : "[Failed] asc_float2hif8_rna failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
