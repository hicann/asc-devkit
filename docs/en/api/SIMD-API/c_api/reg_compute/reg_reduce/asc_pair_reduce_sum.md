---
okftype: reference.api
description: "Adds every two adjacent elements in the source vector register according to the mask, writes the results contiguously to the lower half of the destination register, and sets the upper half to zero. Supports half and float."
tag:
  - roles: [compute]
  - operations: [reduction]
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
    note: "In 9.2.0, two function return value overloads are added for the half and float dtypes of asc_pair_reduce_sum. In 9.0.0/9.1.0, only the reference output form that writes the result to dst is available. Currently, the include contains 2 reference output + 2 return value declarations for this name."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location is unified from include/c_api/reg_compute/reg_vector.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_reduce.h (from 9.2.0), consistent with the header file path in the body."
---
# asc_pair_reduce_sum

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.234450+00:00 pushedAt=2026-11-16T02:10:57.039Z nl=2 -->

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

Adds every two adjacent elements in the source operand `src` according to `mask` to obtain the computation result. The result is stored consecutively in the low half of the destination operand `dst`, and the high half of `dst` is set to 0. Taking the half data type as an example, the addition diagram is as follows:

![asc_pair_reduce_sum diagram](../../../../figures/reg_pair_reduce_elem.png)

The calculation formula is as follows:

$$
dst_i = src_{2i} + src_{2i+1} \quad (0 \le i < \frac{N}{2})
$$

Here, $N$ is the number of elements in the vector data register.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_pair_reduce_sum(vector_<dtype> src,
                                                          vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_pair_reduce_sum(vector_<dtype>& dst,
                                                vector_<dtype> src,
                                                vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values: `half`, `float`.

### Typical Prototype Example

```c
// Example: Add adjacent elements of a half vector data register.
__simd_callee__ inline void asc_pair_reduce_sum(vector_half& dst,
                                                vector_half src,
                                                vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | --------- | ---------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Mask register, which controls whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- Outputting the result through a reference parameter: the computation result is written to `dst`.
- Returning the result through the function return value: returns the computation result as a vector data register with the same data type as `src`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The data types of `dst` and `src` must be consistent.
- `mask` specifies whether a source operand participates in the computation. Elements that do not participate are treated as 0 during accumulation. For example:
  - If neither of two consecutive elements a and b participates in the computation, the destination operand result is 0.
  - If only element a of two consecutive elements a and b participates in the computation, the destination operand result is a.
  - If only element b of two consecutive elements a and b participates in the computation, the destination operand result is b.
- After the sum, only the first half of the elements in `dst` are valid data, and the second half is filled with 0. Avoid data overwriting when storing data to the Unified Buffer (UB).

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
constexpr uint32_t DATA_BYTES = 137 * sizeof(float);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;

__simd_vf__ inline void compute(__ubuf__ float* dst, __ubuf__ float* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_float dst_reg;
        vector_float src_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src_reg, src + offset);
        asc_pair_reduce_sum(dst_reg, src_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_pair_reduce_sum_kernel(__gm__ float* dst, __gm__ float* src)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, TOTAL_LENGTH * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<compute>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src(TOTAL_LENGTH);
    std::vector<float> output(TOTAL_LENGTH, 0.0f);
    std::vector<float> golden(TOTAL_LENGTH, 0.0f);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src[i] = static_cast<float>(i) * 0.25f;
    }
    for (uint32_t offset = 0; offset < TOTAL_LENGTH; offset += ELEMENTS_PER_REGISTER) {
        const uint32_t end = (offset + ELEMENTS_PER_REGISTER < TOTAL_LENGTH) ?
            offset + ELEMENTS_PER_REGISTER : TOTAL_LENGTH;
        for (uint32_t i = offset; i + 1 < end; i += 2) {
            golden[offset + (i - offset) / 2] = src[i] + src[i + 1];
        }
        if (((end - offset) & 1U) != 0) golden[offset + (end - offset) / 2] = src[end - 1];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(float), src.data(), src.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_pair_reduce_sum_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = compare_data(output, golden, 1e-6);
    std::cout << (passed ? "[Success] asc_pair_reduce_sum passed." : "[Failed] asc_pair_reduce_sum failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
