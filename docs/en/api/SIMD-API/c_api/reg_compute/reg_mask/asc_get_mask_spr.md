---
okftype: reference.api
description: "Reads the mask value from a special-purpose register and formats it according to the data type. In b16 mode, each bit is expanded to 2 bits; in b32 mode, each bit is expanded to 4 bits. The bits are assembled into a 256-bit mask register and returned."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints for Reg vector computation C APIs"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_mask_config/asc_set_vector_mask.md
    kind: related
    condition: "The mask register is set by this API outside the VF scenario, and then obtained in the Reg scenario using this API"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location moved from include/c_api/reg_compute/reg_load.h (9.1.0 and earlier) to include/c_api/reg_compute/compute/reg_mask.h (starting from 9.2.0), consistent with the header file path in the body; the signature always remains parameterless and returns vector_bool."
---
# asc_get_mask_spr

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.159523+00:00 pushedAt=2026-11-16T02:10:56.867Z nl=2 -->

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

Reads the mask value from the SPR (Special Purpose Register) and returns it after formatting according to the data type.

- For the b16 data type: Reads a total of 128 bits of mask data from the SPR, expands each bit to 2 bits, and assembles them into a 256-bit mask register to return.
- For the b32 data type: Reads a total of 64 bits of mask data from the SPR, expands each bit to 4 bits, and assembles them into a 256-bit mask register to return.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline vector_bool asc_get_mask_spr_b16()
__simd_callee__ inline vector_bool asc_get_mask_spr_b32()
```

## Parameters

None

## Return Value

vector_bool, mask register.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- This API must be called within a Vector Function (a function marked with `__simd_vf__`).
- [asc_set_vector_mask](../../vector_compute/vector_mask_config/asc_set_vector_mask.md) sets the mask register outside VF scenarios. When configuring Reg vector computation, use `asc_get_mask_spr_b16` or `asc_get_mask_spr_b32` with this API to obtain the mask register value. APIs used within VF scenarios ([asc_create_mask](./asc_create_mask.md) and [asc_update_mask.md](./asc_update_mask.md)) are recommended for configuring the Reg vector computation mask.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
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
    size_t begin, size_t count, double tolerance = 0.0)
{
    if (begin + count > actual.size() || begin + count > expected.size()) return false;
    for (size_t i = begin; i < begin + count; ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t CHUNK_BYTES = 768;
constexpr uint32_t DATA_BYTES = 1028;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (DATA_BYTES + CHUNK_BYTES - 1) / CHUNK_BYTES;
constexpr uint32_t BUFFER_BYTES = CHUNK_BYTES * REPEAT_COUNT;

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
__simd_vf__ inline void exercise(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t value16;
    vector_uint32_t value32;
    uint32_t remaining = DATA_BYTES;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * CHUNK_BYTES;
        const uint32_t valid_count = remaining > CHUNK_BYTES ? CHUNK_BYTES : remaining;
        // Keep the number of remaining valid elements for updating the mask to handle the tail block.
        uint32_t remaining_length = valid_count / sizeof(uint16_t);
        if (remaining_length > 128) remaining_length = 128;
        vector_bool seed16 = asc_update_mask_b16(remaining_length);
        vector_bool mask16 = asc_get_mask_spr_b16();
        asc_duplicate_scalar(value16, static_cast<uint16_t>(1), seed16);
        asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + offset), value16, mask16);
        uint32_t remaining_length_2 = valid_count > 256 ? (valid_count - 256) / sizeof(uint32_t) : 0;
        if (remaining_length_2 > 64) remaining_length_2 = 64;
        vector_bool seed32 = asc_update_mask_b32(remaining_length_2);
        vector_bool mask32 = asc_get_mask_spr_b32();
        asc_duplicate_scalar(value32, static_cast<uint32_t>(1), seed32);
        asc_storealign(reinterpret_cast<__ubuf__ uint32_t*>(output + offset + 256), value32, mask32);
        remaining -= valid_count;
    }
}

__global__ __vector__ void asc_get_mask_spr_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
        for (uint32_t i = 0; i < end && i < 256; i += sizeof(uint16_t)) golden[offset + i] = 1;
        for (uint32_t i = 256; i < end && i < 512; i += sizeof(uint32_t)) golden[offset + i] = 1;
        if (end > 512) {
            std::copy(input.begin() + offset + 512, input.begin() + offset + end, golden.begin() + offset + 512);
        }
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
    asc_get_mask_spr_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden bytes", golden);
    const bool passed = compare_range_data(output, golden, 0, DATA_BYTES);
    std::cout << (passed ? "[Success] asc_get_mask_spr_b16/asc_get_mask_spr_b32 passed." : "[Failed] asc_get_mask_spr_b16/asc_get_mask_spr_b32 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
