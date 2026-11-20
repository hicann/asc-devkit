---
okftype: reference.api
description: "Writes the valid data length saved in the AR special register after asc_squeeze_and_storeunalign to the Unified Buffer, and automatically updates the destination address pointer by byte offset after writing."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body explains vector data registers and unaligned registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "The text uses this to reference the general constraints of Reg vector computation"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md
    kind: requires
    condition: "Before the call, the squeeze operation must be completed by this API to generate the valid length in AR."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
version: ">=9.2.0"
version_changes: []
---
# asc_storealign_squeeze_status_postupdate

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.364583+00:00 pushedAt=2026-11-16T02:10:57.286Z nl=2 -->

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

Writes the valid data length saved in the AR special register after the [asc_squeeze_and_storeunalign](../reg_permute_sel/asc_squeeze_and_storeunalign.md) operation to the Unified Buffer (UB), and automatically updates the destination address pointer by byte offset after the write. This API is suitable for continuously saving squeeze status in a hardware loop.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```cpp
__simd_callee__ inline void asc_storealign_squeeze_status_postupdate(__ubuf__ uint32_t*& dst,
                                                                     int32_t offset)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Input/output | Base address of the destination operand in UB, of type `__ubuf__ uint32_t*&`. When the API executes, the current AR value is written to this address; after execution, the pointer is incremented by `offset` bytes. |
| offset | Input | Destination address update amount, of type `int32_t`, in bytes. |

## Return Value

None

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The initial destination address and the destination address after each Post Update must be 4-byte aligned and must not exceed the actual available UB range.
- Before calling this API, use [asc_squeeze_and_storeunalign](../reg_permute_sel/asc_squeeze_and_storeunalign.md) to complete the squeeze operation. The value written is the current accumulated valid data byte count.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

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
#include "acl/acl.h"
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(uint32_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint32_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t STATUS_WORD_COUNT = 128;
constexpr int32_t POST_UPDATE_OFFSET = 32;
constexpr uint32_t SQUEEZE_STATUS_BYTES = TOTAL_LENGTH * sizeof(float);

__simd_vf__ inline void store_squeeze_status(
    __ubuf__ uint32_t* status, __ubuf__ float* packed, __ubuf__ float* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float squeezed;
    vector_store_unalign unalign_reg;

    asc_squeeze_and_storeunalign_init();
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_float src;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src, input + offset); // Load the input data into the vector data register.
        asc_squeeze_and_storeunalign(packed, squeezed, unalign_reg, src, mask);
    }
    asc_squeeze_and_storeunalign_finalize(packed, unalign_reg);

    // After each status write, status automatically increases by POST_UPDATE_OFFSET bytes.
    asc_storealign_squeeze_status_postupdate(status, POST_UPDATE_OFFSET);
    asc_storealign_squeeze_status_postupdate(status, POST_UPDATE_OFFSET);
}

__global__ __vector__ void store_squeeze_status_kernel(__gm__ uint32_t* output, __gm__ float* input)
{
    asc_init();
    __ubuf__ float input_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float packed_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t status_local[STATUS_WORD_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, sizeof(input_local));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<store_squeeze_status>(status_local, packed_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, status_local, sizeof(status_local));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(PADDED_ELEMENT_COUNT, 0.0F);
    std::vector<uint32_t> output(STATUS_WORD_COUNT, 0U);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input[i] = static_cast<float>(i);
    }

    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS) {
        return 1;
    }
    float* input_device = nullptr;
    uint32_t* output_device = nullptr;
    bool success =
        aclrtMalloc(reinterpret_cast<void**>(&input_device), input.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST) ==
            ACL_SUCCESS &&
        aclrtMalloc(
            reinterpret_cast<void**>(&output_device), output.size() * sizeof(uint32_t), ACL_MEM_MALLOC_HUGE_FIRST) ==
            ACL_SUCCESS;
    if (success) {
        success = aclrtMemcpy(
                      input_device, input.size() * sizeof(float), input.data(), input.size() * sizeof(float),
                      ACL_MEMCPY_HOST_TO_DEVICE) == ACL_SUCCESS;
    }
    if (success) {
        store_squeeze_status_kernel<<<1, 0>>>(output_device, input_device);
        success = aclrtSynchronizeDevice() == ACL_SUCCESS;
    }
    if (success) {
        success = aclrtMemcpy(
                      output.data(), output.size() * sizeof(uint32_t), output_device, output.size() * sizeof(uint32_t),
                      ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    }
    if (input_device != nullptr) {
        aclrtFree(input_device);
    }
    if (output_device != nullptr) {
        aclrtFree(output_device);
    }
    aclrtResetDevice(0);
    aclFinalize();
    if (!success) {
        return 1;
    }
    success = success && output[0] == SQUEEZE_STATUS_BYTES && output[8] == SQUEEZE_STATUS_BYTES;
    std::cout << (success ? "PASS" : "FAIL") << '\n';
    return success ? 0 : 1;
}
```
