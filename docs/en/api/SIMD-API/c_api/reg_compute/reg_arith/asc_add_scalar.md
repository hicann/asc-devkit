---
okftype: reference.api
description: "Performs element-wise addition of vector register elements and a scalar of the same type according to the mask. The result is written to the destination register or returned by the function return value. It supports common dtypes from int8 to float."
tag:
  - roles: [compute]
  - operations: [add]
  - traits: [elementwise, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body describes vector data registers and mask registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "The text uses this to reference the general constraints of Reg vector computation"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "New function return value overload: the probed inline vector_* asc_add_scalar first appears in reg_base_impl/reg_arith_intf_impl.h:1275 of 9.2.0 (the version-map does not provide this draft; this is based on the declaration history and the Reg arithmetic return value overload entry in the CANN 9.2.0 Release Notes), corresponding to the vector_<dtype> asc_add_scalar(vector_<dtype> src, <dtype> value, vector_bool mask) in the text."
---
# asc_add_scalar

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.488642+00:00 pushedAt=2026-11-16T02:10:55.657Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_arith.h"`.

Adds the source operand `src` element-wise with the scalar `value` based on `mask` to obtain the computation result. The calculation formula is as follows:

$$
dst_i = src_i + value
$$

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_add_scalar(vector_<dtype> src,
                                                     <dtype> value,
                                                     vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_add_scalar(vector_<dtype>& dst,
                                           vector_<dtype> src,
                                           <dtype> value,
                                           vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Typical Prototype Example

```c
// Example: Perform an addition operation on a half vector data register and a half scalar.
__simd_callee__ inline void asc_add_scalar(vector_half& dst,
                                           vector_half src,
                                           half value,
                                           vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| value | Input | Source operand (scalar). |
| mask | Input | Mask register, which controls whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- Outputting the result through a reference parameter: the computation result is written to `dst`.
- Returning the result through the function return value: returns the computation result as a vector data register with the same data type as `src`. Elements whose `mask` bit is 0 are set to 0 in the return value.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The data types of `dst` and `src` must be consistent.
- When the `mask` bit is 0, the corresponding element of the result is set to 0.

## Examples

Save the following code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <algorithm>
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
constexpr float SCALAR_VALUE = 2.0f;

__simd_vf__ inline void compute(__ubuf__ float* dst, __ubuf__ float* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float dst_reg;
    vector_float src_reg;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src_reg, src + offset);
        asc_add_scalar(dst_reg, src_reg, SCALAR_VALUE, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_add_scalar_kernel(__gm__ float* dst, __gm__ float* src)
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
    std::vector<float> golden(TOTAL_LENGTH);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src[i] = (static_cast<int>(i) - 16) * 0.25f;
        golden[i] = src[i] + SCALAR_VALUE;
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

    asc_add_scalar_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = compare_data(output, golden, 1e-6);
    std::cout << (passed ? "[Success] asc_add_scalar passed." : "[Failed] asc_add_scalar failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
