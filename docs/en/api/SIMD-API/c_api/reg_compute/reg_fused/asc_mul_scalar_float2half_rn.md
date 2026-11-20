---
okftype: reference.api
description: "Reg fused API that multiplies float vector elements by a scalar, converts them to half, and writes them to even or odd index positions according to dst_pos. Unwritten positions are set to 0. It supports both return value and dst reference forms. It takes effect only on the AI Vector."
tag:
  - roles: [compute]
  - operations: [conversion, multiply]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "For general constraints of Reg vector computation C APIs, see the general constraints section of this page"
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: related
    condition: "Mapping between product models and NPU architectures"
version: ">=9.2.0"
version_changes: []
---
# asc_mul_scalar_float2half_rn

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.058984+00:00 pushedAt=2026-11-16T02:10:56.656Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_fused.h"`.

Multiplies source operand `src` (float type) element-wise by scalar `value` according to `mask`, and then converts the result to the half type according to the ROUND rounding mode to obtain the computation result. The calculation formula is as follows:

$$
dst_i = round\_to\_half(src_i \times value)
$$

Because the bit width ratio of the source operand (float, 32 bits) to the destination operand (half, 16 bits) is 2:1, the converted data of one `vector_float` fills one `vector_half`. The `dst_pos` parameter specifies whether the result is written to even or odd indices in `dst`, and unwritten positions are set to 0.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value, with data written to even indices.
__simd_callee__ inline vector_half asc_mul_scalar_float2half_rn(vector_float src,
                                                                float value,
                                                                vector_bool mask,
                                                                std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)

// Return the result through the function return value, with data written to odd indices.
__simd_callee__ inline vector_half asc_mul_scalar_float2half_rn(vector_float src,
                                                                float value,
                                                                vector_bool mask,
                                                                std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)

// Output the result through a reference parameter, with data written to even indices.
__simd_callee__ inline void asc_mul_scalar_float2half_rn(vector_half& dst,
                                                         vector_float src,
                                                         float value,
                                                         vector_bool mask,
                                                         std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)

// Output the result through a reference parameter, with data written to odd indices.
__simd_callee__ inline void asc_mul_scalar_float2half_rn(vector_half& dst,
                                                         vector_float src,
                                                         float value,
                                                         vector_bool mask,
                                                         std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| -------- | --------- | -------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand (vector data register, half type). |
| src | Input | Source operand (vector data register, float type). |
| value | Input | Source operand (scalar, float type). |
| mask | Input | Mask register, which controls whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |
| dst_pos | Input | Data write position mode, with values of `ASC_POSITION_EVEN` (even positions) or `ASC_POSITION_ODD` (odd positions). When even positions are selected, odd positions are set to 0; when odd positions are selected, even positions are set to 0. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- Outputting the result through a reference parameter: the computation result is written to `dst`.
- Returning the result through the function return value: the computation result is returned, with the type `vector_half`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- When the `mask` bit is 0, the corresponding element of the result is set to 0.
- When `dst_pos` is `ASC_POSITION_EVEN`, the result is written to the even index positions of `dst`, and odd index positions are set to 0; when `dst_pos` is `ASC_POSITION_ODD`, the result is written to the odd index positions of `dst`, and even index positions are set to 0.

## Examples

Save the following code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 133 * sizeof(half);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(half);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr float SCALAR_VALUE = 2.0f;

__simd_vf__ inline void compute(__ubuf__ half* dst, __ubuf__ float* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    vector_bool mask_b16_all = asc_create_mask_b16(PAT_ALL);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_half dst_reg;
        vector_float src_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src_reg, src + offset);
        asc_mul_scalar_float2half_rn(dst_reg, src_reg, SCALAR_VALUE, mask, ASC_POSITION_EVEN);
        asc_storealign(dst + offset * 2, dst_reg, mask_b16_all);
    }
}

__global__ __vector__ void asc_mul_scalar_float2half_rn_kernel(__gm__ half* dst, __gm__ float* src)
{
    asc_init();
    __ubuf__ half dst_local[PADDED_ELEMENT_COUNT * 2];
    __ubuf__ float src_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<compute>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src(PADDED_ELEMENT_COUNT, 0.0f);
    std::vector<half> output(PADDED_ELEMENT_COUNT * 2, half(0));
    std::vector<half> golden(PADDED_ELEMENT_COUNT * 2, half(0));
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src[i] = (static_cast<int>(i) - 16) * 0.25f;
    }
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        golden[2 * i] = static_cast<half>(src[i] * SCALAR_VALUE);
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (PADDED_ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    half* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (PADDED_ELEMENT_COUNT * 2) * sizeof(half),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(float), src.data(), src.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_mul_scalar_float2half_rn_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(half), dst_device, output.size() * sizeof(half),
        ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH * 2; ++i) {
        passed = passed && compare_data(std::vector<half>{output[i]}, std::vector<half>{golden[i]}, 1e-3);
    }
    std::cout << (passed ? "[Success] asc_mul_scalar_float2half_rn passed." : "[Failed] asc_mul_scalar_float2half_rn failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
