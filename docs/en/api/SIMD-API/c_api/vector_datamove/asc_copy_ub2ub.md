---
okftype: reference.api
description: "Moves data from UB to UB within Unified Buffer, providing two prototypes: contiguous movement by size and high-dimensional slicing movement by burst_count, burst_len, and DataBlock intervals. In contiguous mode, the upper limit of size is 65535 times 32 bytes."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Must follow the storage unit description and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: requires
    condition: "Insert synchronization instructions when data dependencies exist with other instructions"
version: ">=9.0.0"
version_changes: []
---
# asc_copy_ub2ub

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.104000+00:00 pushedAt=2026-11-16T02:10:59.519Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

Copies data from Unified Buffer (UB) to UB.

This API supports contiguous data movement and high-dimensional slicing data movement. Contiguous data movement copies data in the UB according to the number of bytes specified by `size`, as shown in Figure 1. High-dimensional slicing data movement copies data blocks according to the count specified by `burst_count`, with each data block copied according to the number of DataBlocks specified by `burst_len`, and uses `src_gap` and `dst_gap` to specify the intervals between adjacent data blocks of the source operand and destination operand, as shown in Figure 2.

**Figure 1** Contiguous data movement diagram
![](../../../figures/asc_copy_ub2ub_continuous.png "Contiguous data movement diagram")

**Figure 2** High-dimensional slicing data movement diagram
![](../../../figures/asc_copy_ub2ub_highdim_split.png "Contiguous data movement diagram")

This API takes effect only on AIV.

## Prototype

### Contiguous Data Movement

```c
__aicore__ inline void asc_copy_ub2ub(__ubuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

### High-Dimensional Slicing Data Movement

```cpp
__aicore__ inline void asc_copy_ub2ub(__ubuf__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap)
```

## Parameters

### Contiguous Data Movement

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination UB. Must be 32-byte aligned. |
| src | Input | Start address of the source UB. Must be 32-byte aligned. |
| size | Input | Size of the data to be copied, in bytes. Value range: [32, 65535×32], and it must be a multiple of 32. |

### High-Dimensional Slicing Data Movement

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination UB. Must be 32-byte aligned. |
| src | Input | Start address of the source UB. Must be 32-byte aligned. |
| burst_count | Input | Number of consecutive data blocks to be copied. Value range: [1, 65535]. |
| burst_len | Input | Length of each consecutive data block, in DataBlock (32 bytes). Value range: [1, 65535]. |
| src_gap | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes). Value range: [0, $2^{16}−1$]. |
| dst_gap | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes). Value range: [0, $2^{16}−1$]. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

### General Constraints

- Calling this API on a non-AIV returns directly.
- For the capacity and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- The effective DataBlocks copied for the source operand and destination operand must not overlap. In high-dimensional slicing data movement mode, if `src_gap` and `dst_gap` make the actual copied DataBlocks of the source and destination non-overlapping, the overall address spans covered by `src` and `dst` are allowed to overlap.
<!-- npu="950" id9 -->
- For Ascend 950PR&950DT products, internal synchronization of `PIPE_V` is automatically guaranteed by hardware, and no additional synchronization is required between multiple instructions of this API.
<!-- end id9 -->
<!-- npu="A3,910b" id10 -->
- If there is a data dependency between this instruction and other instructions, you must insert a [synchronization instruction](../sync/overview.md#synchronization-control-overview) to ensure that the instructions execute serially and prevent abnormal data from occurring.
<!-- end id10 -->

### Consecutive Copy Mode Constraints

- `size` must be a multiple of 32; otherwise, the hardware uses integer division (`size/32`) to determine the number of complete blocks to copy, and the trailing bytes that do not form a complete 32-byte block are discarded.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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
#include "acl/acl.h"

namespace {

constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 256;

__global__ __vector__ void asc_copy_ub2ub_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t source[INPUT_BYTES], copied[INPUT_BYTES];
    asc_copy_gm2ub_align(source, input, INPUT_BYTES);
    // Data movement within UB uses PIPE_V and must wait for the PIPE_MTE2 load to complete.
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    // Create an independent copy within UB, then store that copy to verify that the source and destination data have not been confused.
    asc_copy_ub2ub(copied, source, INPUT_BYTES);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, copied, INPUT_BYTES);
    asc_sync();
}

void print_data(const char* name, const std::vector<uint8_t>& data)
{
    std::cout << name << ":";
    const uint32_t count = data.size() < 32 ? data.size() : 32;
    for (uint32_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i + 1);
    for (uint32_t i = 0; i < 256; ++i) golden[i] = input[i];
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_ub2ub_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_ub2ub passed." : "[Failed] asc_copy_ub2ub failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
