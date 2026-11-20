---
okftype: reference.api
description: "Packs the elements selected by squeeze_mask in the source vector register into the destination register and moves them out to the Unified Buffer contiguously and unaligned. Full 32B main blocks are written directly, and unaligned tail blocks smaller than 32B are temporarily stored in the src0 unaligned register to wait for finalize to finish. The AR register records the number of bytes already moved out as the offset for the next write. This API takes effect only on the AI Vector."
tag:
  - roles: [data_movement]
  - operations: [compact, store]
  - traits: [post_update, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "Type definitions of vector data registers, mask registers, and AR registers"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints for Reg vector computation C APIs"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md
    kind: related
    condition: "Call this API to write the tail block when ending a continuous store-out sequence"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md
    kind: requires
    condition: "Use this API to clear the AR register before the first call"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "Synchronization is required when accessing the same Unified Buffer address as preceding and following Reg data movement APIs with dependencies"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_squeeze_status.md
    kind: related
    condition: "Used to read the accumulated number of bytes stored out in the AR register"
version: ">=9.2.0"
version_changes: []
---
# asc_squeeze_and_storeunalign

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.204581+00:00 pushedAt=2026-11-16T02:10:56.960Z nl=2 -->

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

Reg computation data movement API. Squeezes the elements selected by the mask `squeeze_mask` from the source vector data register `src1` into the destination vector data register `dst`, and stores these elements contiguously to the Unified Buffer (UB). The squeezed elements are arranged contiguously from the low bits of `dst` in their order in `src1`, and the remaining elements in `dst` are set to 0.

This API uses the AR special register to record the total number of bytes of valid data currently stored contiguously. Before calling this API for the first time, call [asc_squeeze_and_storeunalign_init](asc_squeeze_and_storeunalign_init.md) to clear the AR register. Before the API executes, the value in the AR register indicates the write offset relative to `dst_addr`, in bytes; after the API executes, the total number of bytes of the elements selected this time is accumulated into the AR register. By calling this API consecutively while keeping `dst_addr` and `src0` unchanged, multiple groups of filtered results can be written to the UB contiguously without manually updating the address. The value in the AR register can be obtained through the [asc_get_squeeze_status](../../spr/asc_get_squeeze_status.md) API.

The API writes main blocks that can completely form 32B to the UB, and temporarily stores tail blocks smaller than 32B in the unaligned register `src0`. After one or more consecutive store operations, call [asc_squeeze_and_storeunalign_finalize](asc_squeeze_and_storeunalign_finalize.md) to write the tail block to the UB.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ <dtype>* dst_addr,
                                                         vector_<dtype>& dst,
                                                         vector_store_unalign& src0,
                                                         vector_<dtype> src1,
                                                         vector_bool squeeze_mask)
```

### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, or `float`.

### Typical Prototype Example

```c
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ float* dst_addr,
                                                         vector_float& dst,
                                                         vector_store_unalign& src0,
                                                         vector_float src1,
                                                         vector_bool squeeze_mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst_addr | Output | Start address of the destination operand in the UB. The address does not need to be 32B-aligned, but must be aligned to `sizeof(dtype)` bytes. During consecutive stores, the actual write position is `dst_addr` plus the byte offset recorded in the AR register. The API does not modify the value of `dst_addr`. |
| dst | Output | Destination operand (vector data register), which returns the squeezed result by reference. The selected elements are arranged contiguously in the low bits, and the remaining elements are set to 0. |
| src0 | Input/output | Unaligned register with a length of 32B, used to temporarily store tail blocks smaller than 32B. The same register must be used for consecutive stores and finalization; the register does not need to be initialized before the first call. |
| src1 | Input | Source operand (vector data register). |
| squeeze_mask | Input | Source operand mask (mask register). When the corresponding position is 1, the element is selected; when it is 0, the element is not selected. |

For details about vector data registers, mask registers, and unaligned registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The UB capacity upper limit is 256 KB, and the user-available capacity varies with compilation options and programming scenarios. The sum of `dst_addr` and the accumulated byte count recorded in the AR register must not exceed the actual available capacity; otherwise, a write out-of-bounds exception is triggered.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Instruction Constraints

- Before each group of consecutive store operations starts, call [asc_squeeze_and_storeunalign_init](asc_squeeze_and_storeunalign_init.md) once. When calling this API consecutively, keep `dst_addr` and `src0` unchanged; after the last call, call [asc_squeeze_and_storeunalign_finalize](asc_squeeze_and_storeunalign_finalize.md) once to complete finalization.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;


constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t INPUT_COUNT = 139;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (INPUT_COUNT + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_INPUT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t SELECTED_PER_REGISTER = 22;
constexpr uint32_t OUTPUT_COUNT = (INPUT_COUNT / ELEMENTS_PER_REGISTER) * SELECTED_PER_REGISTER +
    ((INPUT_COUNT % ELEMENTS_PER_REGISTER) + 2) / 3;
constexpr uint32_t PADDED_OUTPUT_COUNT = ((OUTPUT_COUNT + ELEMENTS_PER_REGISTER - 1) /
    ELEMENTS_PER_REGISTER) * ELEMENTS_PER_REGISTER;
constexpr uint32_t LAST_SELECTED_COUNT = ((INPUT_COUNT % ELEMENTS_PER_REGISTER) + 2) / 3;

constexpr uint32_t DATA_BYTES = INPUT_COUNT * sizeof(float);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = INPUT_COUNT;
__simd_vf__ inline void pack_selected(
    __ubuf__ float* output, __ubuf__ float* register_output, __ubuf__ float* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float src;
    vector_float dst;
    vector_store_unalign unalign_reg;
    vector_bool pattern_mask = asc_create_mask_b32(PAT_M3);
    vector_bool full_mask = asc_create_mask_b32(PAT_ALL);
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = INPUT_COUNT;
    vector_bool store_mask = asc_create_mask_b32(PAT_ALL);

    asc_squeeze_and_storeunalign_init();
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool valid_mask = asc_update_mask_b32(remaining_length);
        vector_bool squeeze_mask = asc_and(pattern_mask, valid_mask, full_mask);
        asc_loadalign(src, input + i * ELEMENTS_PER_REGISTER);
        asc_squeeze_and_storeunalign(output, dst, unalign_reg, src, squeeze_mask);
    }
    asc_squeeze_and_storeunalign_finalize(output, unalign_reg);
    asc_storealign(register_output, dst, store_mask);
}

__global__ __vector__ void squeeze_and_store_kernel(
    __gm__ float* output, __gm__ float* register_output, __gm__ float* input)
{
    asc_init();
    __ubuf__ float input_local[PADDED_INPUT_COUNT];
    __ubuf__ float output_local[PADDED_OUTPUT_COUNT];
    __ubuf__ float register_local[ELEMENTS_PER_REGISTER];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_INPUT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<pack_selected>(output_local, register_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_OUTPUT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(register_output, register_local, ELEMENTS_PER_REGISTER * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(PADDED_INPUT_COUNT, 0.0F);
    std::vector<float> output(PADDED_OUTPUT_COUNT, 0.0F);
    std::vector<float> register_output(ELEMENTS_PER_REGISTER, 0.0F);
    std::vector<float> golden;
    std::vector<float> register_golden(ELEMENTS_PER_REGISTER, 0.0F);
    for (uint32_t i = 0; i < INPUT_COUNT; ++i) input[i] = static_cast<float>(i) + 0.5F;
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * ELEMENTS_PER_REGISTER;
        const uint32_t valid_count = INPUT_COUNT - offset > ELEMENTS_PER_REGISTER ?
            ELEMENTS_PER_REGISTER : INPUT_COUNT - offset;
        for (uint32_t i = 0; i < valid_count; i += 3) {
            golden.push_back(input[offset + i]);
        }
    }
    const uint32_t last_offset = (REPEAT_COUNT - 1) * ELEMENTS_PER_REGISTER;
    for (uint32_t i = 0; i < LAST_SELECTED_COUNT; ++i) {
        register_golden[i] = input[last_offset + i * 3];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* input_device = nullptr;
    float* output_device = nullptr;
    float* register_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), input.size() * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_OUTPUT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&register_device), register_output.size() * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(float), input.data(), input.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    squeeze_and_store_kernel<<<1, 0>>>(output_device, register_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(register_output.data(), register_output.size() * sizeof(float), register_device,
        register_output.size() * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = std::equal(output.begin(), output.begin() + OUTPUT_COUNT, golden.begin()) &&
        std::equal(register_output.begin(), register_output.begin() + LAST_SELECTED_COUNT, register_golden.begin());
    std::cout << (passed ? "[Success] squeeze and unaligned store completed."
                         : "[Failed] output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtFree(register_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
