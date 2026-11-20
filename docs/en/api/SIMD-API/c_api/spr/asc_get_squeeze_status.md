---
okftype: reference.api
description: "Reads the valid data length saved in the AR special register by the squeeze operation, and works with the contiguous move-out API to obtain the amount of valid data that has been moved out."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md
    kind: requires
    condition: "Data filtering and store must be completed first"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md
    kind: related
    condition: "Store the tail block after the filtering pipeline ends"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md
    kind: requires
    condition: "Clear the AR special registers before calling"
version: ">=9.2.0"
version_changes: []
---
# asc_get_squeeze_status

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.474316+00:00 pushedAt=2026-11-16T02:10:57.598Z nl=2 -->

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

The header file path is `"c_api/spr/spr.h"`.

Reads the valid data length value saved to the AR special register after a squeeze operation, for use with [asc_squeeze_and_storeunalign](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md) to obtain the amount of valid data that has been contiguously moved out.

## Prototype

```cpp
__aicore__ inline int64_t asc_get_squeeze_status()
```

## Parameters

None

## Return Value

Returns the squeeze valid data length value of type int64_t.

## Pipeline Type

PIPE_S

## Constraints

Before calling this API, call [asc_squeeze_and_storeunalign_init](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md) to clear the AR special register, and call [asc_squeeze_and_storeunalign](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md) to complete data filtering.

## Examples

This example simulates a sparse data compression scenario: the input contains 256 `uint16_t` data elements, the mask selects the 1st of every 4 data elements, and 64 valid data elements are continuously filtered and moved out. The kernel function obtains the number of valid bytes moved out through `asc_get_squeeze_status`, and the host checks whether the return value is 128 bytes. After the filtering process is complete, call [asc_squeeze_and_storeunalign_finalize](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md) to move out the tail block.

Save the code as `example.asc` and compile and run it with the `bisheng` command. For the mapping between products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t REPEAT_COUNT = 2;
constexpr uint32_t INPUT_COUNT = ELEMENTS_PER_REGISTER * REPEAT_COUNT;
constexpr uint32_t OUTPUT_COUNT = INPUT_COUNT / 4;
constexpr uint64_t EXPECTED_VALID_BYTES = OUTPUT_COUNT * sizeof(uint16_t);

/*
 * This example shows:
 * - The PAT_M4 mask selects the 1st of every 4 uint16_t data elements.
 * - asc_squeeze_and_storeunalign writes the filtering result of each round contiguously to the Unified Buffer.
 * - asc_get_squeeze_status reads the accumulated valid byte count from the AR special register.
 */
__simd_vf__ inline void pack_selected(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input)
{
    __ubuf__ uint16_t* dst = output;
    vector_uint16_t src;
    vector_uint16_t squeezed;
    vector_store_unalign unalign;
    vector_bool every_fourth = asc_create_mask_b16(PAT_M4);

    asc_squeeze_and_storeunalign_init();
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        asc_loadalign(src, input + repeat * ELEMENTS_PER_REGISTER);
        asc_squeeze_and_storeunalign(dst, squeezed, unalign, src, every_fourth);
    }
    asc_squeeze_and_storeunalign_finalize(dst, unalign);
}

__global__ __vector__ void asc_get_squeeze_status_kernel(
    __gm__ uint16_t* output, __gm__ uint16_t* input, __gm__ uint64_t* valid_bytes)
{
    asc_init();
    __ubuf__ uint16_t input_local[INPUT_COUNT];
    __ubuf__ uint16_t output_local[OUTPUT_COUNT];

    asc_copy_gm2ub_align(input_local, input, INPUT_COUNT * sizeof(uint16_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    pack_selected(output_local, input_local);
    valid_bytes[0] = static_cast<uint64_t>(asc_get_squeeze_status());
    asc_dcci_single(valid_bytes);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_COUNT * sizeof(uint16_t));
    asc_sync();
}

} // namespace

int main()
{
    std::vector<uint16_t> input(INPUT_COUNT), output(OUTPUT_COUNT, 0);
    for (uint32_t i = 0; i < INPUT_COUNT; ++i) {
        input[i] = static_cast<uint16_t>(i + 1);
    }
    uint64_t valid_bytes = 0;
    uint16_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    uint64_t* valid_bytes_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_COUNT * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_COUNT * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&valid_bytes_device), sizeof(uint64_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_COUNT * sizeof(uint16_t), input.data(),
        INPUT_COUNT * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_get_squeeze_status_kernel<<<1, 0>>>(output_device, input_device, valid_bytes_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(&valid_bytes, sizeof(uint64_t), valid_bytes_device, sizeof(uint64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Valid bytes after squeeze: " << valid_bytes << std::endl;
    const bool passed = valid_bytes == EXPECTED_VALID_BYTES;
    std::cout << (passed ? "[Success] asc_get_squeeze_status passed."
                          : "[Failed] asc_get_squeeze_status failed.") << std::endl;

    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtFree(valid_bytes_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
