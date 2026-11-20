---
okftype: reference.api
description: "Deinterleaves the elements of two vector data registers or two mask registers by even and odd indices and writes them into two destination registers. Even-indexed elements go into the first half and odd-indexed elements go into the second half. This API takes effect only on the AI Vector."
tag:
  - roles: [compute]
  - operations: [deinterleave]
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
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location was unified from include/c_api/reg_compute/reg_vector.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_permute_sel.h (starting from 9.2.0), consistent with the header file path in the body; this page has no function return value form, and there are no overload additions or removals in 9.1.0/9.2.0."
---
# asc_deintlv

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.170533+00:00 pushedAt=2026-11-16T02:10:56.898Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_permute_sel.h"`.

Deinterleaves the elements in src0 and src1 and stores them into dst0 and dst1. This API provides two functional modes: vector data deinterleaving and mask deinterleaving:

- **Vector Data Deinterleaving**: Deinterleaves the valid elements in two source vector data registers pair by pair. The even-indexed elements of src0 are written to the first half of dst0, and the odd-indexed elements are written to the first half of dst1. The even-indexed elements of src1 are written to the second half of dst0, and the odd-indexed elements are written to the second half of dst1.
- **Mask deinterleaving**: Groups the bits in the two source mask registers according to the granularity specified by the API suffix and then deinterleaves them. `asc_deintlv_b8` uses 1 bit per group, `asc_deintlv_b16` uses 2 bits per group, and `asc_deintlv_b32` uses 4 bits per group. The order of bits within a group remains unchanged. Groups are numbered from 0 starting from the low bits to the high bits. Even-indexed groups of src0 are written to the first half of dst0, and odd-indexed groups are written to the first half of dst1. Even-indexed groups of src1 are written to the second half of dst0, and odd-indexed groups are written to the second half of dst1.

Taking the int8_t data type as an example, the implementation process of asc_deintlv is shown in Figure 1:

**Figure 1** Deinterleaving implementation process

![Deinterleaving implementation process](../../../../figures/capi_deintlv_reg.png)

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

### Vector Data Deinterleaving

```c
// Placeholder form.
__simd_callee__ inline void asc_deintlv(vector_<dtype>& dst0,
                                        vector_<dtype>& dst1,
                                        vector_<dtype> src0,
                                        vector_<dtype> src1)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```c
// Example: Deinterleave a half vector data register.
__simd_callee__ inline void asc_deintlv(vector_half& dst0,
                                        vector_half& dst1,
                                        vector_half src0,
                                        vector_half src1)
```

### Mask Deinterleaving

```c
__simd_callee__ inline void asc_deintlv_b8(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
__simd_callee__ inline void asc_deintlv_b16(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
__simd_callee__ inline void asc_deintlv_b32(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
```

## Parameters

**Table 1** Parameter description (operands are vector data registers)

| Parameter | Input/Output | Description |
|---|---|---|
| dst0 | Output | Destination vector data register, which receives the set of even-indexed elements after deinterleaving. dtype must be consistent with dst1, src0, and src1. |
| dst1 | Output | Destination vector data register, which receives the set of odd-indexed elements after deinterleaving. dtype must be consistent with dst0, src0, and src1. |
| src0 | Input | Source vector data register, an operand participating in deinterleaving. dtype must be consistent with dst0. Its even-indexed elements are written to the first half of dst0, and its odd-indexed elements are written to the first half of dst1. |
| src1 | Input | Source vector data register, an operand participating in deinterleaving. dtype must be consistent with dst0. Its even-indexed elements are written to the second half of dst0, and its odd-indexed elements are written to the second half of dst1. |

**Table 2** Parameter description (operands are mask registers)

| Parameter | Input/Output | Description |
|---|---|---|
| dst0 | Output | Destination mask register, written with the set of even-indexed groups after deinterleaving. |
| dst1 | Output | Destination mask register, written with the set of odd-indexed groups after deinterleaving. |
| src0 | Input | Source mask register, an operand that participates in deinterleaving. Its even-indexed groups are written to the first half of dst0, and its odd-indexed groups are written to the first half of dst1. |
| src1 | Input | Source mask register, an operand that participates in deinterleaving. Its even-indexed groups are written to the second half of dst0, and its odd-indexed groups are written to the second half of dst1. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The data types of src0, src1, dst0, and dst1 must be consistent.
- src0 and src1 can be the same vector data register or mask register.
- dst0 and dst1 must be different vector data registers or mask registers. If both reference the same register, the two outputs will overwrite each other, and the result is undefined.
- The source operands and destination operands are allowed to be the same vector data register, for example, asc_deintlv(src0, src1, src0, src1).

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id10 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id10 -->

This example contains 133 valid float elements. The input is padded with zeros to 192 elements and deinterleaved in full registers, and all 192 output elements are checked. Deinterleaving does not compact the tail block: the even-indexed and odd-indexed elements of src1 are still written starting from the second halves of the two destination registers, respectively.

```cpp
#include <cstdint>
#include <algorithm>
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

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 133 * sizeof(float);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
__simd_vf__ inline void rearrange(__ubuf__ float* dst0, __ubuf__ float* dst1,
    __ubuf__ float* src0, __ubuf__ float* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Include the zero-padded region in the computation to ensure that all UB elements to be stored have been written.
    uint32_t remaining_length = PADDED_ELEMENT_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_float dst0_reg, dst1_reg, src0_reg, src1_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        vector_bool mask_dst0, mask_dst1;
        asc_deintlv_b32(mask_dst0, mask_dst1, mask, mask);
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_deintlv(dst0_reg, dst1_reg, src0_reg, src1_reg);
        asc_storealign(dst0 + offset, dst0_reg, mask_dst0);
        asc_storealign(dst1 + offset, dst1_reg, mask_dst1);
    }
}
__global__ __vector__ void asc_deintlv_kernel(__gm__ float* dst0, __gm__ float* dst1,
    __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst0_local[PADDED_ELEMENT_COUNT], dst1_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src0_local[PADDED_ELEMENT_COUNT], src1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<rearrange>(dst0_local, dst1_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst0, dst0_local, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(dst1, dst1_local, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src0(PADDED_ELEMENT_COUNT), src1(PADDED_ELEMENT_COUNT);
    std::vector<float> output0(PADDED_ELEMENT_COUNT, 0), output1(PADDED_ELEMENT_COUNT, 0);
    std::vector<float> golden0(PADDED_ELEMENT_COUNT, 0), golden1(PADDED_ELEMENT_COUNT, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) { src0[i] = static_cast<float>(i); src1[i] = 1000.0f + i; }
    for (uint32_t offset = 0; offset < PADDED_ELEMENT_COUNT; offset += ELEMENTS_PER_REGISTER) {
        // Deinterleaving always splits a full register into its first and second halves; the tail block is not compacted.
        for (uint32_t i = 0; i < ELEMENTS_PER_REGISTER / 2; ++i) {
            golden0[offset + i] = src0[offset + 2 * i];
            golden1[offset + i] = src0[offset + 2 * i + 1];
            golden0[offset + ELEMENTS_PER_REGISTER / 2 + i] = src1[offset + 2 * i];
            golden1[offset + ELEMENTS_PER_REGISTER / 2 + i] = src1[offset + 2 * i + 1];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), PADDED_ELEMENT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), PADDED_ELEMENT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), PADDED_ELEMENT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst1_device), PADDED_ELEMENT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(float), src0.data(), src0.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(float), src1.data(), src1.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_deintlv_kernel<<<1, 0>>>(dst0_device, dst1_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output0.data(), output0.size() * sizeof(float), dst0_device, output0.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output1.data(), output1.size() * sizeof(float), dst1_device, output1.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input 0", src0);
    print_data("Input 1", src1);
    print_data("Output 0", output0);
    print_data("Golden 0", golden0);
    print_data("Output 1", output1);
    print_data("Golden 1", golden1);
    const bool passed = std::equal(output0.begin(), output0.end(), golden0.begin()) &&
        std::equal(output1.begin(), output1.end(), golden1.begin());
    std::cout << (passed ? "[Success] asc_deintlv passed." : "[Failed] asc_deintlv failed.") << std::endl;
    aclrtFree(dst0_device);
    aclrtFree(dst1_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
