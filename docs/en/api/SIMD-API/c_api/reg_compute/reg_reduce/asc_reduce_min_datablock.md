---
okftype: reference.api
description: "Computes the minimum value of elements in each 32-byte DataBlock according to the mask, writes the results consecutively to the first 8 elements of the low bits of the destination register, and sets the remaining elements to zero."
tag:
  - roles: [compute]
  - operations: [minimum, reduction]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints for Reg vector computation C APIs"
version: ">=9.1.0"
version_changes:
  - since: "9.1.0"
    kind: renamed
    scope: "page"
    scope_basis: kind_default
    note: "In 9.0.0, the header declared it as asc_reduce_min_datablock_ (the trailing underscore was a typo). Starting from 9.1.0, it is unified as asc_reduce_min_datablock. The Release Notes CANN_9_1_0 record that asc_reduce_min_datablock_ was corrected to asc_reduce_min_datablock, removing the redundant trailing underscore. The underscored form from 9.0.0 no longer exists in the current header."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "In 9.2.0, six function return value overloads were added for the six dtypes of asc_reduce_min_datablock. In 9.0.0/9.1.0, only the reference output form that writes the result to dst existed. The current include contains six reference output declarations and six return value declarations for this name."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location is unified from include/c_api/reg_compute/reg_vector.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_reduce.h (from 9.2.0), consistent with the header file path in the body."
---
# asc_reduce_min_datablock

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.256018+00:00 pushedAt=2026-11-16T02:10:57.090Z nl=2 -->

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

Finds the minimum value of the elements in each `DataBlock` (32B) according to `mask` to obtain the reduction result of each `DataBlock`. The results are stored sequentially in the low bits of `dst`. The calculation formula is as follows:

$$
dst_k = \min\{src_i \mid kB \le i < (k + 1)B,\ mask_i = 1\}
$$

Where $k$ represents the index of the `DataBlock`, and $B$ is the number of elements in one `DataBlock`.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_reduce_min_datablock(vector_<dtype> src,
                                                               vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_reduce_min_datablock(vector_<dtype>& dst,
                                                     vector_<dtype> src,
                                                     vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values are `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, and `float`.

### Typical Prototype Example

```c
// Example: Perform a DataBlock reduction minimum operation on a half vector data register.
__simd_callee__ inline void asc_reduce_min_datablock(vector_half& dst,
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
- The comparison follows $min(-0, +0) = -0$.
- The minimum value in each `DataBlock` is written consecutively to the first 8 elements of `dst`. These 8 elements are valid outputs, and the other elements in `dst` are set to 0.
- Elements not selected by `mask` are treated as the maximum value of the corresponding data type. For floating-point types, the maximum value is `+inf`. If none of the elements in a `DataBlock` are selected by `mask`, this maximum value is written to the corresponding position in `dst`.
- Only the minimum value is output, without the index.

## Examples

Save the following code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <algorithm>
#include <limits>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"
namespace {
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
constexpr uint32_t DATA_BYTES = 133 * sizeof(uint32_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint32_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint8_t MUTEX_ID = 0;

__simd_vf__ inline void reduce(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_uint32_t dst_reg, src_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src_reg, src + offset);
        asc_reduce_min_datablock(dst_reg, src_reg, mask);
        // Write only 8 reduction results per iteration; the input mask cannot be used as the output mask.
        uint32_t result_count = 8;
        vector_bool dst_mask = asc_update_mask_b32(result_count);
        asc_storealign(dst + i * (ELEMENTS_PER_REGISTER / 8), dst_reg, dst_mask);
    }
}
__global__ __vector__ void asc_reduce_min_datablock_kernel(__gm__ uint32_t* dst, __gm__ uint32_t* src)
{
    asc_init();
    __ubuf__ uint32_t dst_local[PADDED_ELEMENT_COUNT], src_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, PADDED_ELEMENT_COUNT * sizeof(uint32_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<reduce>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, PADDED_ELEMENT_COUNT * sizeof(uint32_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint32_t> src(PADDED_ELEMENT_COUNT), output(PADDED_ELEMENT_COUNT, 0), golden(PADDED_ELEMENT_COUNT, std::numeric_limits<uint32_t>::max());
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) src[i] = i + 1;
    for (uint32_t offset = 0; offset < TOTAL_LENGTH; offset += ELEMENTS_PER_REGISTER) {
        const uint32_t end = (offset + ELEMENTS_PER_REGISTER < TOTAL_LENGTH) ?
            offset + ELEMENTS_PER_REGISTER : TOTAL_LENGTH;
        for (uint32_t block = 0; block < 8 && offset + block * 8 < end; ++block) {
            const uint32_t block_end = (offset + block * 8 + 8 < end) ? offset + block * 8 + 8 : end;
            uint32_t value = src[offset + block * 8];
            for (uint32_t i = offset + block * 8 + 1; i < block_end; ++i) value = value < src[i] ? value : src[i];
            golden[(offset / ELEMENTS_PER_REGISTER) * (ELEMENTS_PER_REGISTER / 8) + block] = value;
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), PADDED_ELEMENT_COUNT * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), PADDED_ELEMENT_COUNT * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(uint32_t), src.data(), src.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_reduce_min_datablock_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint32_t), dst_device, output.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = std::equal(output.begin(), output.begin() + (REPEAT_COUNT * ELEMENTS_PER_REGISTER / 8), golden.begin());
    std::cout << (passed ? "[Success] asc_reduce_min_datablock passed." : "[Failed] asc_reduce_min_datablock failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
