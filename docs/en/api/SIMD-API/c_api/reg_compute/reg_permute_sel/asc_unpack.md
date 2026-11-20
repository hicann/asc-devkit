---
okftype: reference.api
description: "Expands the elements in the lower or upper half of the source vector register by bit width and writes them to the destination register or returns them as the return value. Unsigned integers are zero-extended in the high bits, and signed integers are sign-extended."
tag:
  - roles: [compute]
  - operations: [conversion]
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
    note: "In 9.2.0, function return value overloads are added for asc_unpack_lower and asc_unpack_upper (__simd_callee__ inline vector_<dtype_dst> asc_unpack_lower/upper(vector_<dtype_src> src)). In 9.0.0/9.1.0, only the reference output form that writes the result to dst is available. Currently, the include contains 10 declarations for each of the two names (5 reference + 5 return)."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location was unified from include/c_api/reg_compute/reg_vector.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_permute_sel.h (starting from 9.2.0)."
---
# asc_unpack

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.221096+00:00 pushedAt=2026-11-16T02:10:56.993Z nl=2 -->

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

Writes the elements in the lower half or upper half of src to dst with expanded bit width, or returns them through the function return value. The following operations are supported:

- asc_unpack_lower: Takes the lower half of src (LOWER), fills the high bits of each element with 0 to expand the bit width, and writes the result to dst or returns it through the function return value.
- asc_unpack_upper: Takes the upper half of src (HIGHER), fills the high bits of each element with 0 to expand the bit width, and writes the result to dst or returns it through the function return value.

Expansion methods for different data types:

- Unsigned integer: Expands by filling the high bits with 0. For example, from vector_uint8_t to vector_uint16_t, each uint8_t element in src is expanded to uint16_t by padding the high bits with 0.
- Signed integer: Expands while preserving the sign bit. For example, from vector_int8_t to vector_int16_t, each int8_t element in src is expanded to int16_t according to the sign bit.
- bool type (mask register): Expands each bit to 2 bits, with the high bit filled with 0.

The unpack process of a vector data register is shown in Figure 1:

**Figure 1** Vector data register unpack process

![Vector data register unpack process](../../../../figures/capi_unpack_reg.png)

The unpack process of a mask data register is shown in Figure 2:

**Figure 2** Mask data register unpack process

![Mask data register unpack process](../../../../figures/capi_unpack_mask.png)

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

### Vector Data Register Unpack

```c
// Unpack the lower half and return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype_dst> asc_unpack_lower(vector_<dtype_src> src)
// Unpack the upper half and return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype_dst> asc_unpack_upper(vector_<dtype_src> src)

// Unpack the lower half and output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_unpack_lower(vector_<dtype_dst>& dst,
                                             vector_<dtype_src> src)
// Unpack the upper half and output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_unpack_upper(vector_<dtype_dst>& dst,
                                             vector_<dtype_src> src)
```

#### Data Types Supported by dtype

The supported data type pairs for dtype_src and dtype_dst are as follows:

| dtype_src | dtype_dst |
|---|---|
| uint8_t | uint16_t |
| int8_t | int16_t |
| uint16_t | uint32_t |
| int16_t | int32_t |

#### Typical Prototype Example

```c
// Example: Unpack a uint16_t vector data register into uint32_t and return the result through the function return value.
__simd_callee__ inline vector_uint32_t asc_unpack_lower(vector_uint16_t src)

// Example: Unpack a uint16_t vector data register into uint32_t.
__simd_callee__ inline void asc_unpack_lower(vector_uint32_t& dst,
                                             vector_uint16_t src)
```

### Mask Register Unpack

```c
// Unpack the lower half and return the result through the function return value.
__simd_callee__ inline vector_bool asc_unpack_lower(vector_bool src)
// Unpack the upper half and return the result through the function return value.
__simd_callee__ inline vector_bool asc_unpack_upper(vector_bool src)

// Unpack the lower half and output the result through a reference parameter.
__simd_callee__ inline void asc_unpack_lower(vector_bool& dst,
                                             vector_bool src)
// Unpack the upper half and output the result through a reference parameter.
__simd_callee__ inline void asc_unpack_upper(vector_bool& dst,
                                             vector_bool src)
```

## Parameters

**Table 1** Parameter description (operands are vector data registers)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination vector data register. This parameter is included only in the no-return-value API. Reads the low-half or high-half elements of src, expands the bit width, and writes them to all valid positions of dst. All valid positions of dst are overwritten. |
| src | Input | Source vector data register. Takes the low-half or high-half elements of src, expands the bit width, and writes them to dst. |

**Table 2** Parameter description (operands are mask registers)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination mask register. This parameter is included only in the no-return-value API. Reads the low-half or high-half elements of src, expands the bit width, and writes them to all valid positions of dst. All valid positions of dst are overwritten. |
| src | Input | Source mask register. Takes the low-half or high-half elements of src, expands the bit width, and writes them to dst. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

For the return-value API, the vector data register or mask register that stores the unpacking result is returned, and its data type is consistent with `dst`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Both dst and src are vector data registers or mask registers.
- Unpacking the lower half and unpacking the upper half can be used together to unpack the first half and the second half of the source register respectively. Two calls can expand all narrow-type data in the entire source register to wide type and write them to two destination registers.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510&& ./main
```
<!-- end id8 -->

```cpp
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

template <typename T>
bool compare_range_data(const std::vector<T>& actual, const std::vector<T>& expected,
    size_t begin, size_t remaining_length, double tolerance = 0.0)
{
    if (begin + remaining_length > actual.size() || begin + remaining_length > expected.size()) return false;
    for (size_t i = begin; i < begin + remaining_length; ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t SRC_ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DST_ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t SRC_COUNT = 269;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (SRC_COUNT + SRC_ELEMENTS_PER_REGISTER - 1) / SRC_ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_SRC_COUNT = REPEAT_COUNT * SRC_ELEMENTS_PER_REGISTER;
constexpr uint32_t DST_COUNT = REPEAT_COUNT * DST_ELEMENTS_PER_REGISTER;
constexpr uint32_t DATA_BYTES = SRC_COUNT * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = SRC_COUNT;
__simd_vf__ inline void unpack(__ubuf__ uint16_t* lower, __ubuf__ uint16_t* upper, __ubuf__ uint8_t* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = DST_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const uint32_t dst_offset = (offset / SRC_ELEMENTS_PER_REGISTER) * DST_ELEMENTS_PER_REGISTER;
        vector_uint16_t lower_reg;
        vector_uint16_t upper_reg;
        vector_uint8_t src_reg;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        asc_loadalign(src_reg, src + offset);
        asc_unpack_lower(lower_reg, src_reg);
        asc_unpack_upper(upper_reg, src_reg);
        asc_storealign(lower + dst_offset, lower_reg, mask);
        asc_storealign(upper + dst_offset, upper_reg, mask);
    }
}
__global__ __vector__ void asc_unpack_kernel(__gm__ uint16_t* lower, __gm__ uint16_t* upper, __gm__ uint8_t* src)
{
    asc_init();
    __ubuf__ uint16_t lower_local[DST_COUNT];
    __ubuf__ uint16_t upper_local[DST_COUNT];
    __ubuf__ uint8_t src_local[PADDED_SRC_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, PADDED_SRC_COUNT);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<unpack>(lower_local, upper_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(lower, lower_local, DST_COUNT * sizeof(uint16_t));
    asc_copy_ub2gm_align(upper, upper_local, DST_COUNT * sizeof(uint16_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint8_t> src(PADDED_SRC_COUNT, 0);
    std::vector<uint16_t> lower(DST_COUNT, 0), upper(DST_COUNT, 0);
    std::vector<uint16_t> golden_lower(DST_COUNT, 0), golden_upper(DST_COUNT, 0);
    for (uint32_t i = 0; i < SRC_COUNT; ++i) src[i] = static_cast<uint8_t>(i);
    for (uint32_t offset = 0; offset < SRC_COUNT; offset += SRC_ELEMENTS_PER_REGISTER) {
        const uint32_t dst_offset = (offset / SRC_ELEMENTS_PER_REGISTER) * DST_ELEMENTS_PER_REGISTER;
        const uint32_t chunk_count = (offset + SRC_ELEMENTS_PER_REGISTER < SRC_COUNT) ?
            SRC_ELEMENTS_PER_REGISTER : SRC_COUNT - offset;
        const uint32_t lower_count = chunk_count < DST_ELEMENTS_PER_REGISTER ? chunk_count : DST_ELEMENTS_PER_REGISTER;
        for (uint32_t i = 0; i < lower_count; ++i) golden_lower[dst_offset + i] = src[offset + i];
        if (chunk_count > DST_ELEMENTS_PER_REGISTER) {
            for (uint32_t i = 0; i < chunk_count - DST_ELEMENTS_PER_REGISTER; ++i) {
                golden_upper[dst_offset + i] = src[offset + DST_ELEMENTS_PER_REGISTER + i];
            }
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), PADDED_SRC_COUNT * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* lower_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&lower_device), (DST_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* upper_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&upper_device), (DST_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(uint8_t), src.data(), src.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_unpack_kernel<<<1, 0>>>(lower_device, upper_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(lower.data(), lower.size() * sizeof(uint16_t), lower_device, lower.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(upper.data(), upper.size() * sizeof(uint16_t), upper_device, upper.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", src);
    print_data("Lower output", lower);
    print_data("Lower Golden", golden_lower);
    print_data("Upper output", upper);
    print_data("Upper Golden", golden_upper);
    const bool passed = compare_data(lower, golden_lower) && compare_data(upper, golden_upper);
    std::cout << (passed ? "[Success] asc_unpack_lower/asc_unpack_upper passed."
                         : "[Failed] asc_unpack_lower/asc_unpack_upper failed.") << std::endl;
    aclrtFree(lower_device);
    aclrtFree(src_device);
    aclrtFree(upper_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
