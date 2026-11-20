---
okftype: reference.api
description: "Converts a float vector to int16_t using rn, rna, rd, ru, or rz rounding and saturation or non-saturation mode, and writes the result to even or odd index positions of the destination register according to the dst_pos tag, with the remaining positions cleared to zero. The result can also be returned as the return value. This API takes effect only on the AI Vector."
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
    kind: parameter_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added the explicit compile-time position tag dst_pos (std::integral_constant<asc_position_mode, EVEN/ODD>), which selects writing to even or odd index positions of the destination under the 2:1 bit-width ratio, with other positions cleared to zero; the original position suffix form is deprecated since 9.2.0. See the corresponding deprecated_interface page."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added overloads that return the result through the function return value (returning vector_int16_t). In 9.2.0, the two forms total 40 declarations (5 rounding modes × saturation and non-saturation × 2 positions × 2 result forms)."
---
# asc_float2int16

<!-- md-trans-meta sourceCommit=59317b6fda711b248a190c30d2db6306b5b584f1 translatedAt=2026-11-16T01:40:02.765797+00:00 pushedAt=2026-11-16T02:10:56.087Z nl=2 -->

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

Converts data of the `float` type to the `int16_t` type and supports multiple rounding modes as well as saturation and non-saturation modes.

For details about rounding modes and saturation/non-saturation modes, see [Rounding Modes and Saturation Modes](rounding_mode.md).

Because the bit-width ratio of the source operand type to the destination operand type is 2:1, data of one `VL` size must be split into two parts when writing. The `dst_pos` parameter selects whether to write to even-indexed or odd-indexed positions in `dst`.

```python
# The position check is only used to explain the API semantics. The actual position parameter is a compile-time constant.
# VL indicates the vector data register bit width, which is 256 bytes.
def asc_float2int16_rn(dst, src, mask, dst_pos):
    offset = 0 if dst_pos == ASC_POSITION_EVEN else 1
    for i in range(VL // 4):
        dst[2 * i] = 0
        dst[2 * i + 1] = 0
        if mask[i]:
            dst[2 * i + offset] = int16_t(src[i])  # float -> int16_t, RINT rounding.
```

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_float2int16_<round_mode><sat_mode>(vector_int16_t& dst,
                                                                   vector_float src,
                                                                   vector_bool mask,
                                                                   std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)

__simd_callee__ inline void asc_float2int16_<round_mode><sat_mode>(vector_int16_t& dst,
                                                                   vector_float src,
                                                                   vector_bool mask,
                                                                   std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_int16_t asc_float2int16_<round_mode><sat_mode>(vector_float src,
                                                                             vector_bool mask,
                                                                             std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)

__simd_callee__ inline vector_int16_t asc_float2int16_<round_mode><sat_mode>(vector_float src,
                                                                             vector_bool mask,
                                                                             std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
```

**Placeholder description:**

- `<round_mode>` indicates the supported rounding modes, including `rn` (`RINT`), `rna` (`ROUND`), `rd` (`FLOOR`), `ru` (`CEIL`), and `rz` (`TRUNC`).
- `<sat_mode>` indicates the saturation/non-saturation mode. When empty, it indicates non-saturation mode; when set to `_sat`, it indicates saturation mode.

### Typical Prototype Example

```c
// RINT rounding mode, non-saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2int16_rn(vector_int16_t& dst,
                                               vector_float src,
                                               vector_bool mask,
                                               std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register).|
| src | Input | Source operand (vector data register).|
| mask | Input | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by `mask` are set to zero in the output. |
| dst_pos | Input | Position selection tag (compile-time tag dispatch; the corresponding implementation is selected through compile-time overloading). The values are as follows:<br>&bull; `ASC_POSITION_EVEN`: writes the result to even-indexed positions of the destination operand and sets other positions to zero.<br>&bull; `ASC_POSITION_ODD`: writes the result to odd-indexed positions of the destination operand and sets other positions to zero. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the function return value returns the computation result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The position selection tag parameter can only use compile-time constants. The compiler dispatches to the corresponding overload at compile time based on this parameter.
- The position selection tag selects the write address of the destination operand. Other positions are set to zero.
- The data types of `src` and `dst` must match the function prototype.
- When the mask bit in `mask` is 0, the corresponding element in `dst` is set to 0.
- When the result is written to odd-indexed positions of `dst`, even-indexed positions are set to zero; when the result is written to even-indexed positions, odd-indexed positions are set to zero.
- When using saturation or non-saturation mode, configure it through the [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md) API. For configuration requirements, see the API description.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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
        vector_int16_t dst;
        vector_float src;
        asc_loadalign(dst, reinterpret_cast<__ubuf__ int16_t*>(output + offset));
        asc_loadalign(src, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2int16_rn(dst, src, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ int16_t*>(output + offset), dst, mask);
    }
}

__global__ __vector__ void asc_float2int16_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    constexpr uint8_t input_pattern[] = {0x00, 0x00, 0x80, 0x3f};
    constexpr uint8_t golden_pattern[] = {0x01, 0x00, 0x00, 0x00};
    std::vector<uint8_t> input(PADDED_BUFFER_BYTES);
    std::vector<uint8_t> output(PADDED_BUFFER_BYTES, 0xff);
    std::vector<uint8_t> golden(PADDED_BUFFER_BYTES);
    for (size_t i = 0; i < PADDED_BUFFER_BYTES; ++i) {
        input[i] = input_pattern[i % sizeof(input_pattern)];
        golden[i] = (i < TOTAL_LENGTH) ? golden_pattern[i % sizeof(golden_pattern)] : input[i];
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
    asc_float2int16_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    std::cout << "Conversion: vector_float -> vector_int16_t" << std::endl;
    print_data("Input 1.0 (raw bytes)", input);
    print_data("Output (raw bytes)", output);
    print_data("Golden 1.0 (raw bytes)", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_float2int16_rn/ASC_POSITION_EVEN passed."
                         : "[Failed] asc_float2int16_rn/ASC_POSITION_EVEN failed.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
