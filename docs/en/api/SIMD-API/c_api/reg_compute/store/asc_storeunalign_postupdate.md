---
okftype: reference.api
description: "Contiguously stores vector or mask register data to a Unified Buffer address that is not 32-byte aligned and automatically updates the destination address or address register, providing immediate offset and address register offset modes."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [post_update, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body explains vector data registers and unaligned registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "The text uses this to reference the general constraints of Reg vector computation"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: related
    condition: "In address register offset mode, it must be called first to generate the offset"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storeunalign_post.md
    kind: related
    condition: "Address register offset mode and mask mode require this post-processing API to finish."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storeunalign_post_postupdate.md
    kind: related
    condition: "Immediate offset mode requires this post-processing API to finish."
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Completed address update overloads: CANN 9.1.0 Release Notes record that while correcting the destination parameter of asc_storeunalign to a normal pointer, the address update overloads of asc_storeunalign_postupdate were completed, consistent with the form in the body where the immediate offset mode updates the dst pointer by count."
  - since: "9.1.0"
    kind: capability_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added int4b_t and vector_int4x2_t data overloads: CANN 9.1.0 Release Notes explicitly list new low-precision overloads for asc_storeunalign_postupdate, corresponding to int4b_t in the dtype list in the body."
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Unified Buffer destination address changed to pass-by-reference: CANN 9.2.0 Release Notes record that the Unified Buffer destination address of asc_storeunalign_postupdate was changed to pass-by-reference to ensure that the updated result after API execution can be written back to the caller."
---
# asc_storeunalign_postupdate

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.385653+00:00 pushedAt=2026-11-16T02:10:57.327Z nl=2 -->

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

The header file path is `"c_api/reg_compute/store/storeunalign.h"`.

Contiguously stores data from a vector data register or mask register to a Unified Buffer (UB) address that is not aligned to 32 bytes. Enables Post Update mode to automatically increment the destination address pointer or address register after storing, eliminating the need to manually maintain the destination address during repeated calls within a hardware loop.

For vector data registers, this API provides the following two modes:

- **Immediate offset store mode:** Uses `count` to specify the number of elements to store and updates the `dst` pointer by `count` elements. Must be used with the post-processing API [asc_storeunalign_post_postupdate](asc_storeunalign_post_postupdate.md).
- **Address register offset store mode:** Stores a fixed VL (256 bytes) length. Uses `addr_reg` to specify the iterative offset of the destination address. After the instruction stores the data, the offset in the address register is incremented by VL (256 bytes). Must be used with the post-processing API [asc_storeunalign_post](asc_storeunalign_post.md).

The vector data register store principle is as follows:

Let the start address of the destination operand be `dst_start`, the end address be `dst_end`, and the number of tail block elements be `unalign_count = (dst_end - dst_end / 32 * 32) / sizeof(dtype)`. The stored data is divided into the following two parts:

- The 32-byte-aligned main block part `UB[dst_start, dst_end - unalign_count * sizeof(dtype)]`.
- The tail block part `UB[dst_end - unalign_count * sizeof(dtype), dst_end]` that is not aligned to 32 bytes.

For mask registers, this API provides one mode:

- **Mask register store mode:** When the data type at the UB address is `b16`, the hardware instruction extracts the least significant bit (LSB) from every 2 bits of data and packs the 32B data in the mask register into 16B for writing to UB. After the store is complete, the UB address is updated by a 16B offset. When the data type at the UB address is `b32`, the hardware instruction extracts the least significant bit (LSB) from every 4 bits of data and packs the 32B data in the mask register into 8B for writing to UB. After the store is complete, the UB address is updated by an 8B offset. Must be used with the post-processing API [asc_storeunalign_post](asc_storeunalign_post.md).

This API can concatenate the previous **contiguous tail block** with the main block of the current store and simultaneously update the current tail block data into the unaligned register. An uninitialized unaligned register can be used for the first store. If the tail block of the previous store and the main block of the current store are not contiguous, the post-processing API must be called for each tail block processing. If they are contiguous, the post-processing API needs to be called only for the last tail block processing.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Immediate Offset Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ <dtype>*& dst,
                                                        vector_store_unalign& src0,
                                                        vector_<dtype> src1,
                                                        uint32_t count)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, or `int64_t`. The corresponding vector data register type is `vector_<dtype>`, where `int4b_t` corresponds to `vector_int4x2_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t*& dst,
                                                        vector_store_unalign& src0,
                                                        vector_uint8_t src1,
                                                        uint32_t count)
```

### Address Register Offset Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ <dtype>* dst,
                                                        vector_store_unalign& src0,
                                                        vector_<dtype> src1,
                                                        addr_reg& count)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, or `int64_t`. The corresponding vector data register type is `vector_<dtype>`, where `int4b_t` corresponds to `vector_int4x2_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t* dst,
                                                        vector_store_unalign& src0,
                                                        vector_uint8_t src1,
                                                        addr_reg& count)
```

### Mask Register Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ <dtype>*& dst,
                                                        vector_store_unalign& src0,
                                                        vector_bool src1)
```

#### Data Types Supported by dtype

`dtype` can be `uint16_t`, or `uint32_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t*& dst,
                                                        vector_store_unalign& src0,
                                                        vector_bool src1)
```

## Parameters

### Immediate Offset Store Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Input/output | Start address of the destination operand in UB, of type `__ubuf__ <dtype>*&`. |
| src0 | Input/output | Unaligned register of type `vector_store_unalign&`, with a length of 32 bytes. |
| src1 | Input | Source operand (vector data register) of type `vector_<dtype>`. `dtype` must be the same as the data type pointed to by `dst`. |
| count | Input | Number of elements to store this time, of type `uint32_t`. |

### Address Register Offset Store Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand in UB, of type `__ubuf__ <dtype>*`. |
| src0 | Input/output | Unaligned register of type `vector_store_unalign&`, with a length of 32 bytes. |
| src1 | Input | Source operand (vector data register) of type `vector_<dtype>`. `dtype` must be the same as the data type pointed to by `dst`. |
| count | Input/output | Iterative offset of the destination address, of type `addr_reg&`, pre-constructed by the matching address register construction API. This parameter is used to update the destination address and does not control the amount of data stored in a single operation. After the instruction is executed, the offset in the address register is incremented by one VL (256 bytes). |

### Mask Register Store Mode

**Table 3** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Input/output | Start address of the destination operand in UB, of type `__ubuf__ <dtype>*&`. For the `b16` type, the destination address must be 16-byte aligned; for the `b32` type, the destination address must be 8-byte aligned. After storing, `dst` is automatically updated to the end address of this store operation. |
| src0 | Input/output | Unaligned register of type `vector_store_unalign&`, with a length of 32 bytes. |
| src1 | Input | Source operand (mask register) of type `vector_bool`. |

For details about vector data registers, mask registers, unaligned registers, and address registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). After the destination operand address is updated, it must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Instruction Constraints

- In consecutive calls, you must reuse the same `src0`.
- The contiguous mask register store mode must be finalized using the immediate offset store mode of [asc_storeunalign_post](asc_storeunalign_post.md), reusing the same `src0`.
- The immediate offset store mode must be finalized using [asc_storeunalign_post_postupdate](asc_storeunalign_post_postupdate.md), reusing the same `src0`.
- In address register offset store mode, `addr_reg` must be constructed in advance through [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md) and assigned for use within the inner loop body. Use [asc_storeunalign_post](asc_storeunalign_post.md) for finalization, reusing the same `src0` and address register.
- In immediate offset store mode, the maximum value of `count` must not exceed the number of elements that a single vector data register can hold.
- In vector data register store scenarios, the destination address does not need to be 32-byte aligned, but it must be aligned to `sizeof(dtype)` bytes.
- In mask register store scenarios, the destination address of the `b16` type must be 16-byte aligned, and the destination address of the `b32` type must be 8-byte aligned.

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DATA_BYTES = 260 * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t START_OFFSET = 8;
constexpr uint32_t OUTPUT_BYTES = START_OFFSET + TOTAL_LENGTH;
constexpr uint32_t PADDED_OUTPUT_BYTES = START_OFFSET + PADDED_ELEMENT_COUNT + 32;

__simd_vf__ inline void storeunalign_postupdate_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_store_unalign unalign_reg;
    __ubuf__ uint8_t* dst = output + START_OFFSET;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const uint32_t valid_count = (offset + ELEMENTS_PER_REGISTER < TOTAL_LENGTH) ?
            ELEMENTS_PER_REGISTER : TOTAL_LENGTH - offset;
        vector_uint8_t src;
        asc_loadalign(src, input + offset);
        asc_storeunalign_postupdate(dst, unalign_reg, src, valid_count);
    }
    asc_storeunalign_post_postupdate(dst, unalign_reg, 0);
}

__global__ __vector__ void storeunalign_postupdate_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_OUTPUT_BYTES];
    __ubuf__ uint8_t input_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_ELEMENT_COUNT);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<storeunalign_postupdate_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint8_t> output(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) input[i] = static_cast<uint8_t>(i);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_ELEMENT_COUNT, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, PADDED_ELEMENT_COUNT, input.data(), PADDED_ELEMENT_COUNT, ACL_MEMCPY_HOST_TO_DEVICE);
    storeunalign_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) passed = passed && output[START_OFFSET + i] == input[i];
    std::cout << (passed ? "[Success] asc_storeunalign_postupdate completed."
                         : "[Failed] asc_storeunalign_postupdate output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
