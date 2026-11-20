---
okftype: reference.api
description: "Reads VL/8 length data from a 32-byte aligned address in UB, keeps the data format and content unchanged, and returns the mask register through the function return value. For consecutive loads, the source address must be manually updated before each call."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign.md
    kind: related
    condition: "Prototype whose destination is a mask register in the consecutive aligned load mode; this API is its return value form"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints of Reg vector compute C APIs"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "Insert this synchronization instruction when there is a data dependency with other instructions"
version: ">=9.2.0"
version_changes: []
---
# asc_loadalign_mask

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.370111+00:00 pushedAt=2026-11-16T02:10:55.517Z nl=2 -->

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

The header file path is `"c_api/reg_compute/load/loadalign.h"`.

Reads VL/8 bytes of data from a 32-byte-aligned start address in the Unified Buffer (UB) and returns a mask register through the function return value. The data format and content remain unchanged during the load. For contiguous load, manually update the source address before each call.

This API has the same prototype functionality as the contiguous aligned load mode of [asc_loadalign](asc_loadalign.md) when the destination operand is a mask register, except that this API returns the result through the function return value.

**Figure 1** Contiguous load to a mask register

![](../../../../figures/capi_loadalign_mask_norm.png)

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline vector_bool asc_loadalign_mask(__ubuf__ uint32_t* src)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| src | Input | Source UB address. The actual read address must be 32-byte-aligned, and VL/8 bytes of data are loaded. |

## Return Value

Returns the mask register that stores the contiguous aligned load result, of type `vector_bool`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The actual read address of `src` must be 32-byte-aligned, and the actual read range must be within the UB address space without exceeding the boundary; otherwise, an error is reported.
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
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;


constexpr uint32_t MASK_WORD_COUNT = 11;
constexpr uint32_t WORDS_PER_MASK = 8;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (MASK_WORD_COUNT + WORDS_PER_MASK - 1) / WORDS_PER_MASK;
constexpr uint32_t STORAGE_WORD_COUNT = REPEAT_COUNT * WORDS_PER_MASK;
constexpr uint32_t TOTAL_MASK_BYTES = STORAGE_WORD_COUNT * sizeof(uint32_t);

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = MASK_WORD_COUNT;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint32_t);
__simd_vf__ inline void asc_loadalign_mask_vf(
    __ubuf__ uint32_t* output, __ubuf__ uint32_t* mask_input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    uint32_t remaining = MASK_WORD_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t valid_count = remaining > WORDS_PER_MASK ? WORDS_PER_MASK : remaining;
        const uint32_t offset = i * WORDS_PER_MASK;
        vector_bool mask = asc_loadalign_mask(mask_input + offset);
        asc_storealign(output + offset, mask);
        remaining -= valid_count;
    }
}

__global__ __vector__ void asc_loadalign_mask_kernel(
    __gm__ uint32_t* output, __gm__ uint32_t* mask_input)
{
    asc_init();
    __ubuf__ uint32_t output_local[STORAGE_WORD_COUNT];
    __ubuf__ uint32_t mask_local[STORAGE_WORD_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(mask_local, mask_input, TOTAL_MASK_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = asc_get_vf_len() / sizeof(uint32_t) / 8;
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_mask_vf>(output_local, mask_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, TOTAL_MASK_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> output(STORAGE_WORD_COUNT, 0);
    std::vector<uint32_t> mask_input(STORAGE_WORD_COUNT, 0x55555555u);

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* output_device = nullptr;
    uint32_t* mask_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), TOTAL_MASK_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&mask_device), TOTAL_MASK_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(mask_device, TOTAL_MASK_BYTES, mask_input.data(), TOTAL_MASK_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_mask_kernel<<<1, 0>>>(output_device, mask_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), TOTAL_MASK_BYTES, output_device, TOTAL_MASK_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < MASK_WORD_COUNT; ++i) passed = passed && output[i] == mask_input[i];
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadalign_mask example." << std::endl;
    aclrtFree(output_device);
    aclrtFree(mask_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
