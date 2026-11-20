---
okftype: reference.api
description: "Computes the element-wise maximum of two vector data registers according to a mask, writes the result to the destination data register or returns it as the function return value, and supports 8- to 32-bit integers, half, bfloat16_t, and float."
tag:
  - roles: [compute]
  - operations: [maximum]
  - traits: [elementwise]
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
    note: "9.2.0 adds function-return-value overloads for asc_max: 9.0.0/9.1.0 only have 9 reference-output (void) forms; starting from 9.2.0, include adds 9 overloads returning vector_<dtype> (int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/half/bfloat16_t/float); Release Notes CANN_9_2_0 \"Adds function-return-value overloads for Reg arithmetic APIs\" explicitly lists asc_max."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "In 9.1.0 and earlier, this family is declared in include/c_api/reg_compute/reg_vector.h; starting from 9.2.0, it is unified into include/c_api/reg_compute/compute/reg_arith.h (Release Notes CANN_9_2_0 also describes the unified public header file), and the header file path in the body is written according to the new location."
---
# asc_max

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.518941+00:00 pushedAt=2026-11-16T02:10:55.696Z nl=2 -->

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

This API performs an element-wise maximum operation on the source operands src0 and src1 based on mask, and writes the result to the destination operand dst.

The calculation formula is as follows:

$$
dst_i = max(src0_i, src1_i)
$$

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_max(vector_<dtype>& dst,
                                    vector_<dtype> src0,
                                    vector_<dtype> src1,
                                    vector_bool mask)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_max(vector_<dtype> src0,
                                              vector_<dtype> src1,
                                              vector_bool mask)
```

### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Typical Prototype Example

```c
// Example of outputting the result through a reference parameter: float type.
__simd_callee__ inline void asc_max(vector_float& dst,
                                    vector_float src0,
                                    vector_float src1,
                                    vector_bool mask)

// Example of returning the result through the function return value: float type.
__simd_callee__ inline vector_float asc_max(vector_float src0,
                                            vector_float src1,
                                            vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand (vector data register), applicable only to the function prototype that outputs the result through a reference parameter. |
| src0 | Input | Source operand (vector data register). |
| src1 | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The prototype that returns the result through the function return value returns the element-wise maximum computation result, with the return value type consistent with the source operand type.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- max(-0, +0) = +0.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
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

// Reserve UB space based on the 256-byte register width. Each register holds 256 / sizeof(int32_t) = 64 elements.
constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
// Use 137 valid elements to cover two full registers and a tail block with only 9 valid elements.
constexpr uint32_t DATA_BYTES = 137 * sizeof(int32_t);
// Convert the byte count to an element count for calculating the iteration count and valid-element mask.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(int32_t);
// Round up to obtain 3 register blocks; integer division yields only 2 and would leave the tail block unprocessed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to 3 * 64 = 192 elements so that full-register loads in the final iteration stay within the allocated space.
// The extra space is only for safe loads; the mask and TOTAL_LENGTH still control the valid range for computation and writeback.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;

__simd_vf__ inline void max_vf(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int32_t dst_reg;
    vector_int32_t src0_reg;
    vector_int32_t src1_reg;
    // Record the number of valid elements remaining, which asc_update_mask_b32 decrements on each iteration.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        // Process 64, 64, and 9 elements in the three iterations, respectively; only the first 9 valid elements are computed and stored in the final iteration.
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_max(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_max_kernel(__gm__ int32_t* dst, __gm__ int32_t* src0, __gm__ int32_t* src1)
{
    asc_init();
    __ubuf__ int32_t dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t src1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, TOTAL_LENGTH * sizeof(int32_t));
    asc_copy_gm2ub_align(src1_local, src1, TOTAL_LENGTH * sizeof(int32_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(int32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<max_vf>(dst_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    // Store only 137 valid elements; do not write the extra UB space reserved for full-register loads to GM.
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(int32_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}

} // namespace

int main()
{
    std::vector<int32_t> src0(TOTAL_LENGTH);
    std::vector<int32_t> src1(TOTAL_LENGTH);
    std::vector<int32_t> output(TOTAL_LENGTH, 0);
    std::vector<int32_t> golden(TOTAL_LENGTH);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src0[i] = static_cast<int32_t>(i) - 64;
        src1[i] = 32 - static_cast<int32_t>(i);
        golden[i] = src0[i] > src1[i] ? src0[i] : src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(int32_t), src0.data(), src0.size() * sizeof(int32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(int32_t), src1.data(), src1.size() * sizeof(int32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_max_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), dst_device, output.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);

    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_max passed." : "[Failed] asc_max failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
