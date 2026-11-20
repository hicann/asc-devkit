---
okftype: reference.api
description: "Generates a b8, b16, or b32 bit-width mask register according to the element position pattern specified by pat_mode, used to filter the elements that participate in subsequent Reg vector computation."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints of Reg vector computation C APIs and the upper limit on the number of mask registers"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, include/c_api/reg_compute/compute/reg_mask.h provides this API through object-like macro aliases (#define asc_create_mask_b8 pset_b8, asc_create_mask_b16 pset_b16, asc_create_mask_b32 pset_b32). In 9.0.0/9.1.0, the include contains only comment-form call examples and no annotatable declarations; therefore, the three identifiers on this page are shown as not written to the header in doc_overrides, and this page does not use an overload scope."
---
# asc_create_mask

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.154458+00:00 pushedAt=2026-11-16T02:10:56.856Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_mask.h"`.

Generates the corresponding mask register based on the passed pat_mode, supporting three bit-width modes: b8, b16, and b32. The pat_mode parameter specifies the mask pattern, that is, which element positions participate in the computation.

Bit-width mode description:
- b8 mode: Each bit corresponds to an 8-bit element (256 elements in total), used for vector computation of 8-bit data types.
- b16 mode: Every 2 bits form a group corresponding to a 16-bit element (128 elements in total), used for vector computation of 16-bit data types.
- b32 mode: Every 4 bits form a group corresponding to a 32-bit element (64 elements in total), used for vector computation of 32-bit data types.

**Figure 1** asc_create_mask principle

![asc_create_mask principle](../../../../figures/capi_create_mask.png)

This API is a Reg data type definition API and takes effect only on AIV.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
asc_create_mask_b8(pat_mode)
asc_create_mask_b16(pat_mode)
asc_create_mask_b32(pat_mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pat_mode | Input | Mask pattern. The values are as follows:<br>&bull; PAT_ALL: All elements are set as valid data and participate in the computation.<br>&bull; PAT_VL1: The lowest 1 element is set as valid data.<br>&bull; PAT_VL2: The lowest 2 elements are set as valid data.<br>&bull; PAT_VL3: The lowest 3 elements are set as valid data.<br>&bull; PAT_VL4: The lowest 4 elements are set as valid data.<br>&bull; PAT_VL8: The lowest 8 elements are set as valid data.<br>&bull; PAT_VL16: The lowest 16 elements are set as valid data.<br>&bull; PAT_VL32: The lowest 32 elements are set as valid data.<br>&bull; PAT_VL64: The lowest 64 elements are set as valid data.<br>&bull; PAT_VL128: The lowest 128 elements are set as valid data.<br>&bull; PAT_M3: Elements whose subscripts are multiples of 3 are set as valid data.<br>&bull; PAT_M4: Elements whose subscripts are multiples of 4 are set as valid data.<br>&bull; PAT_H: The lower half of the elements are set as valid data.<br>&bull; PAT_Q: The lower quarter of the elements are set as valid data.<br>&bull; PAT_ALLF: All elements are set as invalid elements and do not participate in the computation. |

## Return Value

vector_bool, mask register.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The maximum number of mask registers is 8. Mask registers exceeding this limit are written to the reserved 8K Unified Buffer (UB) memory, which may cause performance degradation. The compiler automatically reuses registers and reserved memory whose lifecycles have ended. If both are available, registers are reused first.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id10 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id10 -->

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

constexpr uint32_t CHUNK_BYTES = 768;
constexpr uint32_t DATA_BYTES = 1541;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (DATA_BYTES + CHUNK_BYTES - 1) / CHUNK_BYTES;
constexpr uint32_t BUFFER_BYTES = CHUNK_BYTES * REPEAT_COUNT;

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
__simd_vf__ inline void exercise(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint8_t value8;
    vector_uint16_t value16;
    vector_uint32_t value32;
    uint32_t remaining = DATA_BYTES;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * CHUNK_BYTES;
        const uint32_t valid_count = remaining > CHUNK_BYTES ? CHUNK_BYTES : remaining;
        // Keep the number of remaining valid elements for updating the mask to handle the tail block.
        uint32_t remaining_length = valid_count > 256 ? 256 : valid_count;
        uint32_t remaining_length_2 = valid_count > 256 ? (valid_count - 256) / sizeof(uint16_t) : 0;
        if (remaining_length_2 > 128) remaining_length_2 = 128;
        uint32_t remaining_length_3 = valid_count > 512 ? (valid_count - 512) / sizeof(uint32_t) : 0;
        if (remaining_length_3 > 64) remaining_length_3 = 64;
        vector_bool mask8 = asc_create_mask_b8(PAT_ALL);
        vector_bool mask16 = asc_create_mask_b16(PAT_ALL);
        vector_bool mask32 = asc_create_mask_b32(PAT_ALL);
        asc_duplicate_scalar(value8, static_cast<uint8_t>(1), mask8);
        asc_duplicate_scalar(value16, static_cast<uint16_t>(1), mask16);
        asc_duplicate_scalar(value32, static_cast<uint32_t>(1), mask32);
        asc_storealign(output + offset, value8, asc_update_mask_b8(remaining_length));
        asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + offset + 256), value16,
            asc_update_mask_b16(remaining_length_2));
        asc_storealign(reinterpret_cast<__ubuf__ uint32_t*>(output + offset + 512), value32,
            asc_update_mask_b32(remaining_length_3));
        remaining -= valid_count;
    }
}

__global__ __vector__ void asc_create_mask_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES];
    __ubuf__ uint8_t input_local[BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint16_t one_repeat_size = 3 * asc_get_vf_len() / sizeof(uint8_t);
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<exercise>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_BYTES), output(BUFFER_BYTES, 0xff), golden(BUFFER_BYTES);
    for (uint32_t i = 0; i < BUFFER_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * CHUNK_BYTES;
        const uint32_t end = DATA_BYTES - offset > CHUNK_BYTES ? CHUNK_BYTES : DATA_BYTES - offset;
        for (uint32_t i = 0; i < end && i < 256; ++i) golden[offset + i] = 1;
        for (uint32_t i = 256; i < end && i < 512; i += sizeof(uint16_t)) golden[offset + i] = 1;
        for (uint32_t i = 512; i < end; i += sizeof(uint32_t)) golden[offset + i] = 1;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_create_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden bytes", golden);
    bool passed = true;
    for (uint32_t i = 0; i < DATA_BYTES; ++i) passed = passed && output[i] == golden[i];
    std::cout << (passed ? "[Success] asc_create_mask_b8/asc_create_mask_b16/asc_create_mask_b32 passed." : "[Failed] asc_create_mask_b8/asc_create_mask_b16/asc_create_mask_b32 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
