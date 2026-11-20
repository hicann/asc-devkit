---
okftype: reference.api
description: "Stores the first element of a vector data register out to the Unified Buffer aligned to dtype, and automatically updates the destination address pointer after the write, facilitating consecutive calls within a hardware loop."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [aligned_transfer, post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The main text describes vector data registers/mask registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "The text uses this to reference the general constraints of Reg vector computation"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Destination pointer changed to pass-by-reference: The CANN 9.1.0 Release Notes record that the UB destination pointer parameters that require automatic updates in APIs such as asc_storealign_1st_postupdate were corrected to pointer references, ensuring that address changes after the call are visible to the caller (corresponding to __ubuf__ <dtype>*& dst in the main text)."
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "UB destination address changed to pass-by-reference: The CANN 9.2.0 Release Notes again record that the UB destination address of asc_storealign_1st_postupdate was changed to pass-by-reference, ensuring that the updated result after API execution can be written back to the caller."
---
# asc_storealign_1st_postupdate

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.310720+00:00 pushedAt=2026-11-16T02:10:57.194Z nl=2 -->

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

Writes the first element in the source vector data register to the destination address in the Unified Buffer (UB). A single element is stored at a time, mask configuration is not supported, the destination address must be aligned to `sizeof(dtype)`, and the data format and content remain unchanged during the store. The API uses Post Update mode and automatically updates the destination address after the store is complete, eliminating the need to manually maintain the destination address across multiple consecutive calls in a hardware loop.

Taking the **b16 bit-width single-point store** process as an example, the schematic diagram is as follows:

**Figure 1** b16 single-element store data

![](../../../../figures/reg_store_onept.png)

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ <dtype>*& dst,
                                                          vector_<dtype> src,
                                                          int32_t offset)
```

### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int8_t*& dst,
                                                          vector_int8_t src,
                                                          int32_t offset)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Input/output | Start address of the destination operand (vector), passed by pointer reference (`__ubuf__ <dtype>*&`). After the store is complete, this address is automatically updated by the hardware in Post Update mode. |
| src | Input | Source operand (vector data register). |
| offset | Input | Address offset, in elements. **After the API executes**, the underlying UB address of the `dst` pointer is updated by `offset × sizeof(dtype)` bytes. |

For details about vector data registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- UB capacity limit: The total UB capacity is 256 KB. By default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity. The sum of the start address of `dst` and the cumulative Post Update offset must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Instruction Constraints

- The start address of `dst` must be aligned to the `dtype` data type, and the new `dst` address after Post Update must still satisfy `dtype` alignment; otherwise, the next call to this API will report an error.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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


constexpr uint32_t ELEMENT_COUNT = 133;
constexpr int32_t OFFSET = 7;
constexpr uint32_t REPEAT_TIMES = 3;
constexpr uint16_t SENTINEL = 0xffff;
constexpr uint32_t BUFFER_BYTES = ELEMENT_COUNT * sizeof(uint16_t);

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = REPEAT_TIMES;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint16_t);
__simd_vf__ inline void store_first_postupdate_vf(__ubuf__ uint16_t* output,
                                                   __ubuf__ uint16_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t src;
    asc_loadalign(src, input);
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint16_t valid_count = remaining_length > one_repeat_size ? one_repeat_size : remaining_length;
        for (uint16_t j = 0; j < valid_count; ++j) {
            asc_storealign_1st_postupdate(output, src, OFFSET);
        }
        remaining_length -= valid_count;
    }
}

__global__ __vector__ void store_first_postupdate_kernel(__gm__ uint16_t* output,
                                                          __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t output_local[ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub(output_local, output, BUFFER_BYTES);
    asc_copy_gm2ub(input_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<store_first_postupdate_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(ELEMENT_COUNT);
    std::vector<uint16_t> output(ELEMENT_COUNT, SENTINEL);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) input[i] = static_cast<uint16_t>(i + 1);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, BUFFER_BYTES, output.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    store_first_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        const bool written = i < REPEAT_TIMES * OFFSET && i % OFFSET == 0;
        passed = passed && output[i] == (written ? input[0] : SENTINEL);
    }
    std::cout << (passed ? "[Success] asc_storealign_1st_postupdate completed."
                         : "[Failed] asc_storealign_1st_postupdate output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
