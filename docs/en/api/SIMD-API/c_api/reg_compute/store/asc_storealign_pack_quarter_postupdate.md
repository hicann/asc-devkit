---
okftype: reference.api
description: "Compresses and moves the low 8 bits of each 32-bit valid element of a vector data register out to the Unified Buffer according to the mask, and automatically updates the destination address pointer by element offset after writing."
tag:
  - roles: [data_movement]
  - operations: [compression, store]
  - traits: [post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body explains vector data registers and unaligned registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "The text uses this to reference the general constraints of Reg vector computation"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
version: ">=9.2.0"
version_changes: []
---
# asc_storealign_pack_quarter_postupdate

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.346794+00:00 pushedAt=2026-11-16T02:10:57.254Z nl=2 -->

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

The header file path is `"c_api/reg_compute/store/storealign.h"`.

Packs and stores data from a vector data register to the Unified Buffer (UB). After the write is complete, Post Update mode is enabled to automatically increment the destination address pointer, eliminating the need to manually maintain the destination address across multiple consecutive calls in a hardware loop. `mask` indicates the elements that participate in the store. When the corresponding `mask` bit is 1, the low 8 bits of each valid 32-bit element in `src` are written to the corresponding packed destination position; when the corresponding `mask` bit is 0, the element does not participate in the store, and the corresponding packed destination position retains its original value. The unit of `offset` is elements. After the store is complete, the destination address is incremented by `offset × sizeof(dtype)` bytes.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_pack_quarter_postupdate(__ubuf__ <dtype>*& dst,
                                                                   vector_<dtype> src,
                                                                   int32_t offset,
                                                                   vector_bool mask)
```

### Data Types Supported by dtype

`dtype` can be `int32_t`, `uint32_t`, or `float`. The corresponding vector register type is `vector_<dtype>`.

### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_pack_quarter_postupdate(__ubuf__ int32_t*& dst,
                                                                   vector_int32_t src,
                                                                   int32_t offset,
                                                                   vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Input/output | Start address of the destination operand, passed by pointer reference. The type is `__ubuf__ <dtype>*&`, and the start address must be 32-byte aligned. After the store is complete, this pointer is automatically updated by hardware. |
| src | Input | Source operand (vector data register). |
| offset | Input | Destination address update amount, of type `int32_t`, in elements. After the API is executed, the destination address is incremented by `offset × sizeof(dtype)` bytes. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the data movement. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The start address of `dst` must be 32-byte aligned.
- The total UB capacity is 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The write range of the destination operand and the address after Post Update must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Quarter Pack and Post Update Constraints

- The `offset` input parameter is of type `int32_t`, in elements. Ensure that the write area after applying the offset does not exceed the available UB capacity.
- The new `dst` address after Post Update must still be 32-byte aligned; otherwise, the next call to this API will report an error.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;


constexpr uint32_t ELEMENT_COUNT = 67;
constexpr uint32_t PACKED_BYTES = 64;
constexpr uint32_t REPEAT_TIMES = 2;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_TIMES * PACKED_BYTES;
constexpr uint32_t INPUT_BYTES = PADDED_ELEMENT_COUNT * sizeof(uint32_t);
constexpr uint32_t STEP_BYTES = 256;
constexpr uint32_t SECTION_BYTES = REPEAT_TIMES * STEP_BYTES;
constexpr uint32_t OUTPUT_BYTES = 3 * SECTION_BYTES;
constexpr int32_t OFFSET_ELEMENTS = 64;
constexpr uint8_t SENTINEL = 0xa5;

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = ELEMENT_COUNT;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint32_t);
__simd_vf__ inline void pack_quarter_postupdate_vf(__ubuf__ uint8_t* output,
                                                   __ubuf__ uint32_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int32_t src_i32;
    vector_uint32_t src_u32;
    vector_float src_f32;
    __ubuf__ int32_t* dst_i32 = reinterpret_cast<__ubuf__ int32_t*>(output);
    __ubuf__ uint32_t* dst_u32 = reinterpret_cast<__ubuf__ uint32_t*>(output + SECTION_BYTES);
    __ubuf__ float* dst_f32 = reinterpret_cast<__ubuf__ float*>(output + 2 * SECTION_BYTES);
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = ELEMENT_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadalign(src_i32, reinterpret_cast<__ubuf__ int32_t*>(input) + i * PACKED_BYTES);
        asc_loadalign(src_u32, input + i * PACKED_BYTES);
        asc_loadalign(src_f32, reinterpret_cast<__ubuf__ float*>(input) + i * PACKED_BYTES);
        vector_bool tail_mask = asc_update_mask_b32(remaining_length);
        asc_storealign_pack_quarter_postupdate(dst_i32, src_i32, OFFSET_ELEMENTS, tail_mask);
        asc_storealign_pack_quarter_postupdate(dst_u32, src_u32, OFFSET_ELEMENTS, tail_mask);
        asc_storealign_pack_quarter_postupdate(dst_f32, src_f32, OFFSET_ELEMENTS, tail_mask);
    }
}

__global__ __vector__ void asc_storealign_pack_quarter_postupdate_kernel(
    __gm__ uint8_t* output, __gm__ uint32_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[OUTPUT_BYTES];
    __ubuf__ uint32_t input_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub(output_local, output, OUTPUT_BYTES);
    asc_copy_gm2ub(input_local, input, INPUT_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<pack_quarter_postupdate_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> input(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint8_t> output(OUTPUT_BYTES, SENTINEL);
    std::vector<uint8_t> expected(OUTPUT_BYTES, SENTINEL);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) input[i] = 0x5a000000u | i;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = ELEMENT_COUNT;
    for (uint32_t repeat = 0; repeat < REPEAT_TIMES; ++repeat) {
        const uint32_t valid_count = remaining_length > PACKED_BYTES ? PACKED_BYTES : remaining_length;
        for (uint32_t i = 0; i < valid_count; ++i) {
            const uint8_t value = static_cast<uint8_t>(input[repeat * PACKED_BYTES + i]);
            for (uint32_t section = 0; section < 3; ++section) {
                const uint32_t base = section * SECTION_BYTES;
                expected[base + repeat * STEP_BYTES + i] = value;
            }
        }
        remaining_length -= valid_count;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_storealign_pack_quarter_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == expected;
    std::cout << (passed ? "[Success] asc_storealign_pack_quarter_postupdate passed."
                         : "[Failed] asc_storealign_pack_quarter_postupdate output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
