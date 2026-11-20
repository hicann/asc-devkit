---
okftype: reference.api
description: "Performs a reduction to find the minimum value in the source vector register according to the mask. The result and the raw bit pattern of the index of the minimum value in the source are written to elements 0 and 1 of the destination register respectively, and the remaining elements are set to zero."
tag:
  - roles: [compute]
  - operations: [minimum, reduction]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints for Reg vector computation C APIs"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "In 9.2.0, six function return value overloads are added for the six dtypes of asc_reduce_min. In 9.0.0/9.1.0, only the reference output form that writes the result to dst is available. Currently, the include contains 6 reference output + 6 return value declarations for this name."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location is unified from include/c_api/reg_compute/reg_vector.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_reduce.h (from 9.2.0), consistent with the header file path in the body."
---
# asc_reduce_min

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.250019+00:00 pushedAt=2026-11-16T02:10:57.080Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_reduce.h"`.

Performs a reduction minimum operation on the source operand `src` according to `mask` to obtain the reduction result. The result is stored in element 0 of `dst`, the original bit pattern of the index of the minimum value in `src` is stored in element 1 of `dst`, and the other elements in `dst` are set to 0. If there are multiple minimum values, the smallest index is retained. The calculation formula is as follows:

$$
\begin{aligned}
dst_0 &= \min\{src_i \mid mask_i = 1\} \\
dst_1 &= \operatorname{argmin}\{src_i \mid mask_i = 1\}
\end{aligned}
$$

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_reduce_min(vector_<dtype> src,
                                                     vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_reduce_min(vector_<dtype>& dst,
                                           vector_<dtype> src,
                                           vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values are `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, and `float`.

### Typical Prototype Example

```c
// Example: Perform a reduction minimum operation on a half vector data register.
__simd_callee__ inline void asc_reduce_min(vector_half& dst,
                                           vector_half src,
                                           vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | --------- | ---------------------------------------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Mask register, used to control whether each element participates in the reduction. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- Outputting the result through a reference parameter: the reduction result is written to `dst`.
- Returning the result through the function return value: the reduction result is returned, and its type is a vector data register consistent with the data type of `src`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Elements not selected by `mask` are treated as the maximum value of the corresponding data type, and the maximum value of a floating-point type is `+inf`. If none of the elements in `src` are selected by `mask`, this maximum value is written to element 0 of `dst`, and the remaining elements are set to 0.
- The comparison follows $min(-0, +0) = -0$.
- If the input data contains nan, nan is written to element 0 of `dst`, and the index of the first nan is written to element 1 of `dst`.

## Key Features

**The reduction produces two results**: value and index. The index value requires a forced type conversion:

The index of `dst` is stored according to the data type of `dst`. For example, when `dst` is of the half type, the index is stored as the half type. Therefore, to read the index, you need to use the reinterpret\_cast method to convert it to an integer type. If the data type is half, use reinterpret\_cast\<uint16_t*\>; if the data type is float, use reinterpret\_cast\<uint32_t*\>. For how to extract the value and index separately, see [Examples](#examples).

## Examples

Save the following code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
// This example uses the half type to demonstrate how to extract the "value + index" from the reduction result separately.
// In the reduction result, element 0 of dst is the minimum value, and element 1 is the original bit pattern of the minimum value index.
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"
namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t DATA_BYTES = 133 * sizeof(half);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(half);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
__simd_vf__ inline void reduce(__ubuf__ half* dst, __ubuf__ half* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_half dst_reg, src_reg;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        asc_loadalign(src_reg, src + offset);
        asc_reduce_min(dst_reg, src_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}
__global__ __vector__ void asc_reduce_min_kernel(__gm__ half* dst, __gm__ half* src)
{
    asc_init();
    __ubuf__ half dst_local[PADDED_ELEMENT_COUNT], src_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, TOTAL_LENGTH * sizeof(half));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(half));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<reduce>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(half));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    std::vector<half> src(TOTAL_LENGTH), output(TOTAL_LENGTH, half(0));
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src[i] = half(static_cast<float>(i) + 1.0f);  // 1, 2, ..., 64
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    half* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (TOTAL_LENGTH) * sizeof(half),
        ACL_MEM_MALLOC_HUGE_FIRST);
    half* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(half),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(half), src.data(), src.size() * sizeof(half),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_reduce_min_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(half), dst_device, output.size() * sizeof(half),
        ACL_MEMCPY_DEVICE_TO_HOST);
    half min_value = output[0];                                      // Extract the minimum value.
    uint16_t min_index = *reinterpret_cast<uint16_t*>(&output[1]);   // Extract the minimum value index: the index is stored as the half type and must be forcibly converted to an integer type.
    const bool passed = (static_cast<float>(min_value) == 1.0f) && (min_index == 0);
    std::cout << (passed ? "[Success] asc_reduce_min passed." : "[Failed] asc_reduce_min failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
