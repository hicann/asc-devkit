# asc_update_mask

<!-- md-trans-meta sourceCommit=d9775184a456840aa55019c36424cc9fc4fb25d9 translatedAt=2026-11-16T01:40:03.164028+00:00 pushedAt=2026-11-16T02:10:56.876Z nl=2 -->

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

Generates a mask based on the element count value and automatically subtracts the number of elements processed by the current vector processing unit from value. Supports three bit-width modes: b8, b16, and b32. Since VL = 256B, the number of elements processed by the vector processing unit in each mode is:

- b8 mode: Processes 256 elements at a time, used for vector computation of 8-bit data types.
- b16 mode: Processes 128 elements at a time, used for vector computation of 16-bit data types.
- b32 mode: Processes 64 elements at a time, used for vector computation of 32-bit data types.

**Figure 1** asc_update_mask update process

![asc_update_mask update process](../../../../figures/capi_update_mask.png)

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline vector_bool asc_update_mask_b8(uint32_t& value)
__simd_callee__ inline vector_bool asc_update_mask_b16(uint32_t& value)
__simd_callee__ inline vector_bool asc_update_mask_b32(uint32_t& value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| value | Input/output | Element count. After the call, the number of elements processed by the current vector processing unit is automatically subtracted (256, 128, and 64 for b8, b16, and b32 modes, respectively).<br>After this API is executed once, value = (value < VL_T) ? 0 : (value - VL_T), where VL_T indicates the number of elements of data type T that can be stored in a vector data register with a bit width of VL.<br>For example, with 320 elements of the b16 data type and 128 elements processed at a time:<br>On the first call, all elements corresponding to the generated mask are valid data, and value = 320 - 128 = 192.<br>On the second call, all elements corresponding to the generated mask are valid data, and value = 192 - 128 = 64.<br>On the third call, only the lower half of the elements corresponding to the generated mask are valid data, and value = 0.|

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

vector_bool, mask register.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The maximum number of mask registers is 8. Mask registers exceeding this limit are written to the reserved 8K Unified Buffer (UB) memory, which may cause performance degradation. The compiler automatically reuses registers and reserved memory whose lifecycles have ended. If both are available, registers are reused first.
- This API must be called within a Vector Function (a function marked with `__simd_vf__`).

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
constexpr uint32_t DATA_BYTES = 1540;
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
        vector_bool mask8 = asc_update_mask_b8(remaining_length);
        vector_bool mask16 = asc_update_mask_b16(remaining_length_2);
        vector_bool mask32 = asc_update_mask_b32(remaining_length_3);
        asc_duplicate_scalar(value8, static_cast<uint8_t>(1), mask8);
        asc_duplicate_scalar(value16, static_cast<uint16_t>(1), mask16);
        asc_duplicate_scalar(value32, static_cast<uint32_t>(1), mask32);
        asc_storealign(output + offset, value8, mask8);
        asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + offset + 256), value16, mask16);
        asc_storealign(reinterpret_cast<__ubuf__ uint32_t*>(output + offset + 512), value32, mask32);
        remaining -= valid_count;
    }
}

__global__ __vector__ void asc_update_mask_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    asc_update_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden bytes", golden);
    const bool passed = compare_range_data(output, golden, 0, DATA_BYTES);
    std::cout << (passed ? "[Success] asc_update_mask_b8/asc_update_mask_b16/asc_update_mask_b32 passed." : "[Failed] asc_update_mask_b8/asc_update_mask_b16/asc_update_mask_b32 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
