---
okftype: reference.api
description: "Converts the bfloat16_t vector elements participating in mask filtering to half using rd, rn, rna, ru, or rz rounding and saturation or non-saturation mode, writes the result to the destination register or returns it, requires configuration through the asc_set_saturation_flag API before using saturation mode, and takes effect only on AI Vector."
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
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Adds overloads that return the result through a function return value (returning vector_half); the reference parameter form has been available since 9.0.0, and in 9.2.0 the two forms total 20 declarations (5 rounding modes x saturation and non-saturation x 2 result forms)."
---
# asc_bfloat162half

<!-- md-trans-meta sourceCommit=59317b6fda711b248a190c30d2db6306b5b584f1 translatedAt=2026-11-16T01:40:02.699402+00:00 pushedAt=2026-11-16T02:10:55.968Z nl=2 -->

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

Converts each `bfloat16_t` element in `src` that participates in the computation to the `half` type according to `mask`, and writes the results to `dst`.

For details about rounding modes and saturation/non-saturation modes, see [Rounding Modes](rounding_mode.md).

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_bfloat162half_<round_mode><sat_mode>(vector_half& dst,
                                                                     vector_bfloat16_t src,
                                                                     vector_bool mask)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_half asc_bfloat162half_<round_mode><sat_mode>(vector_bfloat16_t src,
                                                                            vector_bool mask)
```

**Placeholder description:**
- `<round_mode>` supports `rd` (FLOOR), `rn` (RINT), `rna` (ROUND), `ru` (CEIL), and `rz` (TRUNC).
- `<sat_mode>` indicates the saturation/non-saturation mode. When empty, it indicates non-saturation mode; when set to `_sat`, it indicates saturation mode.

### Typical Prototype Example

```c
// Output the result through a reference parameter.
__simd_callee__ inline void asc_bfloat162half_rn(vector_half& dst,
                                                 vector_bfloat16_t src,
                                                 vector_bool mask)

// Return the result through the function return value.
__simd_callee__ inline vector_half asc_bfloat162half_rn(vector_bfloat16_t src,
                                                        vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Mask register, which controls whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the function return value returns the computation result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
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

template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t DATA_BYTES = 133 * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(uint16_t);
constexpr uint32_t PADDED_BUFFER_BYTES = PADDED_ELEMENT_COUNT * sizeof(uint16_t);

__simd_vf__ inline void convert_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        vector_half dst;
        vector_bfloat16_t src;
        asc_loadalign(dst, reinterpret_cast<__ubuf__ half*>(output + offset * sizeof(uint16_t)));
        asc_loadalign(src, reinterpret_cast<__ubuf__ bfloat16_t*>(input + offset * sizeof(uint16_t)));
        asc_bfloat162half_rn(dst, src, mask);
        asc_storealign(reinterpret_cast<__ubuf__ half*>(output + offset * sizeof(uint16_t)), dst, mask);
    }
}
__global__ __vector__ void asc_bfloat162half_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_BUFFER_BYTES];
    __ubuf__ uint8_t input_local[PADDED_BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<convert_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    constexpr uint8_t input_pattern[] = {0x80, 0x3f};
    constexpr uint8_t golden_pattern[] = {0x00, 0x3c};
    std::vector<uint8_t> input(PADDED_BUFFER_BYTES);
    std::vector<uint8_t> output(PADDED_BUFFER_BYTES, 0xff);
    std::vector<uint8_t> golden(PADDED_BUFFER_BYTES);
    for (size_t i = 0; i < PADDED_BUFFER_BYTES; ++i) {
        input[i] = input_pattern[i % sizeof(input_pattern)];
        golden[i] = (i < BUFFER_BYTES) ? golden_pattern[i % sizeof(golden_pattern)] : input[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_bfloat162half_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    std::cout << "Conversion: vector_bfloat16_t -> vector_half" << std::endl;
    print_data("Input 1.0 (raw bytes)", input);
    print_data("Output (raw bytes)", output);
    print_data("Golden 1.0 (raw bytes)", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_bfloat162half_rn passed." : "[Failed] asc_bfloat162half_rn failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
