---
okftype: reference.api
description: "Contiguously moves the first count elements of a vector data register out to a Unified Buffer address that is not 32-byte aligned. The main block is written directly, and the tail block is temporarily stored in a non-aligned register for the next concatenation. The destination address is updated by the user."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [unaligned_transfer]
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
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storeunalign_post.md
    kind: related
    condition: "When the previous tail block and the current main block are non-contiguous, this post-processing API is required for finalization."
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Destination parameter corrected from pointer reference to plain pointer: CANN 9.1.0 Release Notes record that the destination parameter of asc_storeunalign, which does not update the address, is corrected from a pointer reference to a plain pointer (consistent with the non-reference form of __ubuf__ <dtype>* dst in the body)."
  - since: "9.1.0"
    kind: capability_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added int4b_t and vector_int4x2_t data overloads: CANN 9.1.0 Release Notes explicitly list asc_storeunalign as adding low-precision overloads, corresponding to int4b_t in the dtype list in the body."
---
# asc_storeunalign

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.368908+00:00 pushedAt=2026-11-16T02:10:57.296Z nl=2 -->

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

Contiguously stores the first `count` elements of a vector data register to an address in the Unified Buffer (UB) that is not aligned to 32 bytes. `src0` is an unaligned register passed by reference. After this API is called, it is updated with the tail data of the current store for concatenation by the next call to this API or an unaligned store finalization API. After each call, the user must manually update the destination operand address in UB.

The vector data register store principle is as follows:
Let the start address of the destination operand be `dst_start`, the end address be `dst_end`, and the number of tail block elements be `unalign_count = (dst_end - dst_end / 32 * 32) / sizeof(T)`.
The stored data is then divided into two parts:
- The 32B-aligned main block part `UB[dst_start, dst_end - unalign_count * sizeof(T)]`.
- The tail block part `UB[dst_end - unalign_count * sizeof(T), dst_end]` that is not aligned to 32 bytes.

When this API executes, it stores the main block to UB and temporarily stores the tail block in the unaligned register [0, `unalign_count`]. In consecutive calls, this API can concatenate the previous **contiguous tail block** with the main block of the current store, and simultaneously update the current tail block data to the unaligned register. The first store can use an uninitialized unaligned register. If the tail block of the previous store and the main block of the current store are not contiguous, the post-processing API [asc_storeunalign_post](asc_storeunalign_post.md) must be called for each tail block processing. If they are contiguous, the post-processing API needs to be called only for the last tail block processing.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
// Placeholder form.
__simd_callee__ inline void asc_storeunalign(__ubuf__ <dtype>* dst,
                                             vector_store_unalign& src0,
                                             vector_<dtype> src1,
                                             uint32_t count)
```

### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, or `int64_t`. The corresponding vector register type is `vector_<dtype>`, where `int4b_t` corresponds to `vector_int4x2_t`.

### Typical Prototype Example

```c
__simd_callee__ inline void asc_storeunalign(__ubuf__ uint8_t* dst,
                                             vector_store_unalign& src0,
                                             vector_uint8_t src1,
                                             uint32_t count)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand in UB, of type `__ubuf__ <dtype>*`. The address does not need to be 32-byte aligned, but must be aligned to `sizeof(dtype)` bytes. |
| src0 | Input/output | Unaligned register of type `vector_store_unalign`, with a length of 32 bytes. Holds the tail block from the preceding store and temporarily stores the tail block from the current store. No initialization is required before the first call. |
| src1 | Input | Source operand (vector data register). The data type must be the same as that of `dst`. |
| count | Input | Number of elements stored this time, of type `uint32_t`. The maximum value does not exceed the number of elements that a single vector data register can hold. |

For details about vector data registers and unaligned registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The destination operand address after applying the offset must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Instruction Constraints

- In consecutive calls, you must reuse the same `src0` and update `dst` according to the number of elements stored.
- After calling this API, call [asc_storeunalign_post](asc_storeunalign_post.md) to process the tail block of the unaligned store. The tail block data is stored in the same unaligned register.
- The maximum value of `count` does not exceed the number of elements that a single vector data register can hold. The destination access range must not exceed the available UB capacity; otherwise, an error is reported.

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
constexpr uint32_t DATA_BYTES = 300 * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t START_OFFSET = 8;
constexpr uint32_t PADDED_OUTPUT_BYTES = START_OFFSET + PADDED_ELEMENT_COUNT;

__simd_vf__ inline void storeunalign_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_store_unalign unalign_reg;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const uint32_t count = (TOTAL_LENGTH - offset > ELEMENTS_PER_REGISTER) ?
            ELEMENTS_PER_REGISTER : TOTAL_LENGTH - offset;
        vector_uint8_t src;
        asc_loadalign(src, input + offset);
        asc_storeunalign(output + START_OFFSET + offset, unalign_reg, src, count);
    }
    asc_storeunalign_post(output + START_OFFSET, unalign_reg, TOTAL_LENGTH);
}

__global__ __vector__ void storeunalign_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    asc_vf_call<storeunalign_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint8_t> output(PADDED_OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) input[i] = static_cast<uint8_t>(i);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_ELEMENT_COUNT, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, PADDED_ELEMENT_COUNT, input.data(), PADDED_ELEMENT_COUNT, ACL_MEMCPY_HOST_TO_DEVICE);
    storeunalign_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), PADDED_OUTPUT_BYTES, output_device, PADDED_OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) passed = passed && output[START_OFFSET + i] == input[i];
    std::cout << (passed ? "[Success] asc_storeunalign completed."
                         : "[Failed] asc_storeunalign output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
