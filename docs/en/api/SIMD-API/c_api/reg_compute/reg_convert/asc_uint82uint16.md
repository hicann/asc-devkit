---
okftype: reference.api
description: "Converts uint8_t elements at even or odd indices in the source vector data register to uint16_t according to the mask, writes the result to the destination vector data register or returns it from the function, selects the position by the compile-time tag asc_position_mode, sets destination elements with mask bit 0 to 0, and takes effect only on AI Vector."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_position_mode.md
    kind: requires
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_uint82uint16__vu8_vb_icposeven"
    scope_basis: resolved
    note: "Adds a function return value overload asc_uint82uint16(src, mask, src_pos), where src_pos is fixed to asc_position_mode::EVEN, returns vector_uint16_t, and reads elements at even indices 2*i."
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_uint82uint16__vu8_vb_icposodd"
    scope_basis: resolved
    note: "Adds a function return value overload, where src_pos is fixed to asc_position_mode::ODD, reads elements at odd indices 2*i+1, and replaces the position suffix form of asc_uint82uint16_v2."
  - since: "9.2.0"
    kind: parameter_added
    scope: "param:src_pos"
    scope_basis: resolved
    note: "The reference output prototype adds a fourth parameter src_pos (std::integral_constant<asc_position_mode, EVEN/ODD>), with the position now explicitly expressed by a compile-time tag; the three-parameter prototypes asc_uint82uint16/_v2 are marked with ASC_DEPRECATED(9.2.0) starting from this version, deprecated but not removed, with no availability upper limit."
---
# asc_uint82uint16

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.989037+00:00 pushedAt=2026-11-16T02:10:56.524Z nl=2 -->

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

Converts each `uint8_t` element in the source operand `src` to the `uint16_t` type according to `mask`, and writes the results to `dst`. Because the bit-width ratio of the source operand type to the destination operand type is 1:2, when reading data, one `VL`-sized data block needs to be divided into two parts. According to the `src_pos` parameter, data is read from the even-indexed positions or the odd-indexed positions of `src`. The reference pseudocode is as follows:

```python
# The position check is only used to explain the API semantics. The actual position parameter is a compile-time constant.
# VL indicates the vector data register bit width, which is 256 bytes.
def asc_uint82uint16(dst, src, mask, src_pos):
    offset = 0 if src_pos == ASC_POSITION_EVEN else 1
    for i in range(VL // 2):
        src_index = 2 * i + offset
        dst[i] = uint16(src[src_index]) if mask[src_index] else 0
```

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter.
__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst,
                                             vector_uint8_t src,
                                             vector_bool mask,
                                             std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)

__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst,
                                             vector_uint8_t src,
                                             vector_bool mask,
                                             std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)

// Return the result through the function return value.
__simd_callee__ inline vector_uint16_t asc_uint82uint16(vector_uint8_t src,
                                                        vector_bool mask,
                                                        std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)

__simd_callee__ inline vector_uint16_t asc_uint82uint16(vector_uint8_t src,
                                                        vector_bool mask,
                                                        std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Mask register, which controls whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |
| src_pos | Input | Position selection tag (compile-time tag dispatch; the corresponding implementation is selected through compile-time overloading). Values are as follows:<br>&bull; `ASC_POSITION_EVEN`: Selects the even-indexed positions of the source operand.<br>&bull; `ASC_POSITION_ODD`: Selects the odd-indexed positions of the source operand. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the function return value returns the computation result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The position selection tag parameter can only use compile-time constants. The compiler dispatches to the corresponding overload at compile time based on this parameter.
- The data types of `src` and `dst` must match the function prototype.
- When the mask bit in `mask` is 0, the corresponding element in `dst` is set to 0.
- When reading the odd-indexed positions of `src`, the elements at even-indexed positions do not participate in the computation. When reading the even-indexed positions, the elements at odd-indexed positions do not participate in the computation.

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
        vector_uint16_t dst_0;
        vector_uint8_t src_0;
        asc_loadalign(dst_0, reinterpret_cast<__ubuf__ uint16_t*>(output + offset));
        asc_loadalign(src_0, reinterpret_cast<__ubuf__ uint8_t*>(input + offset));
        asc_uint82uint16(dst_0, src_0, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + offset), dst_0, mask);
        vector_uint16_t dst_1;
        vector_uint8_t src_1;
        // Different destination registers reuse the same UB region. Wait for the preceding write to complete before reading.
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_1, reinterpret_cast<__ubuf__ uint16_t*>(output + offset));
        asc_loadalign(src_1, reinterpret_cast<__ubuf__ uint8_t*>(input + offset));
        asc_uint82uint16(dst_1, src_1, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + offset), dst_1, mask);
    }
}
__global__ __vector__ void asc_uint82uint16_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    constexpr uint8_t input_pattern[] = {0x01};
    constexpr uint8_t golden_pattern[] = {0x01, 0x00};
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
    asc_uint82uint16_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    std::cout << "Conversion: vector_uint8_t -> vector_uint16_t" << std::endl;
    print_data("Input 1.0 (raw bytes)", input);
    print_data("Output (raw bytes)", output);
    print_data("Golden 1.0 (raw bytes)", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_uint82uint16/ASC_POSITION_ODD/ASC_POSITION_EVEN passed."
                         : "[Failed] asc_uint82uint16/ASC_POSITION_ODD/ASC_POSITION_EVEN failed.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
