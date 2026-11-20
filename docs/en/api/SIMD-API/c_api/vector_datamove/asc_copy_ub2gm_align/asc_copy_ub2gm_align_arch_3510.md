---
okftype: reference.api
description: "On the 3510 architecture, moves UB data out to GM at a length that is not an integer multiple of 32 bytes, pads the UB read and discards the dummy data on the GM write, supporting Compact/Normal modes and asc_store_l2_cache_mode configuration."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_store_l2_cache_mode.md
    kind: related
    condition: "Description of the l2_cache_mode value for the high-dimensional slicing prototype"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: related
    condition: "The main text references storage units accordingly"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "Insert synchronization instructions according to this page when data dependencies exist"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added a simplified overload that takes only dst, src, and size: CANN 9.1.0 Release Notes record that asc_copy_ub2gm_align (including _sync) added a simplified overload, corresponding to the three-parameter form for contiguous data movement in the body."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added an L2 Cache policy overload: CANN 9.2.0 Release Notes record that asc_copy_ub2gm_align added an overload that uses asc_store_l2_cache_mode to configure the L2 Cache (supported only on Ascend 950PR&950DT), corresponding to the l2_cache_mode parameter of the high-dimensional slicing prototype in the body."
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Fixes the insufficient bit width of the n_burst parameter: CANN 9.2.0 Release Notes records the fix of the n_burst bit width issue for aligned data movement APIs such as asc_copy_ub2gm_align on the 3510 architecture and its extension to uint32_t, consistent with burst_count being uint32_t and taking values [1, 2^21−1] in the body text."
---
# asc_copy_ub2gm_align (NPU Architecture Version 3510)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.091491+00:00 pushedAt=2026-11-16T02:10:59.499Z nl=2 -->

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

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

Copies data from the Unified Buffer (UB) to Global Memory (GM) and supports data lengths that are not integer multiples of 32 bytes. During the copy, the data is padded to an integer multiple of 32 bytes. When writing to GM, the padding data is discarded and only valid data of the specified length is written. This API supports copying 8-bit/16-bit/32-bit data types. The copied data length must still meet the byte alignment requirement of the corresponding data type. For the alignment requirements of the source and destination start addresses, see [Parameters](#parameters).

This API supports the following two data movement methods, neither of which supports setting a padding value:

- Contiguous data movement

    If the copied data length is not 32-byte aligned, the hardware automatically pads dummy data when reading UB data to make the copied data 32-byte aligned. The padded dummy data is discarded when writing to GM.

- High-dimensional slicing data movement

    If the copied data length is not 32-byte aligned, the hardware pads dummy data when reading UB data to make the copied data 32-byte aligned. The padded dummy data is discarded when writing to GM. You can select Normal mode or Compact mode by configuring the parameter `src_stride`:

    - Compact mode: Set `src_stride` to `burst_len`. Multiple data blocks are tightly arranged in UB and are treated as a whole for 32-byte padding. If the total length of all data blocks is not 32-byte aligned, dummy data is padded only at the end of the entire copy to achieve 32-byte alignment, and the padded dummy data is discarded when writing to GM.
    - Normal mode: Set `src_stride` to a value that is not equal to `burst_len` and satisfies 32-byte alignment. Each data block is individually processed for 32-byte alignment.
        - If `burst_len` is 32-byte aligned, no dummy data is read, and all data read from UB is written to GM.
        - If `burst_len` is not 32-byte aligned, dummy data is padded to achieve 32-byte alignment when reading UB data, and the padded dummy data is discarded when writing to GM.

    For example, when copying 3 data blocks of 48 bytes each:
    - In Compact mode, set `src_stride` to 48. The start offsets of the data blocks in UB are 0, 48, and 96, and 16 bytes of dummy data are supplemented only at the end of the 144 bytes of valid data.
    - In Normal mode, set `src_stride` to 64. The start offsets of the data blocks in UB are 0, 64, and 128, and 16 bytes of dummy data are supplemented for each data block.

    **Figure 1** Compact mode 32-byte aligned read diagram
    ![](../../../../figures/asc_copy_ub2gm_align_compact_padding.png "Compact mode 32-byte padding diagram")

    **Figure 2** Normal mode 32-byte aligned read diagram
    ![](../../../../figures/asc_copy_ub2gm_align_normal_padding.png "Compact mode 32-byte padding diagram")

    In both modes, dummy data is not written to GM. When only 1 data block is copied, or when `burst_len` is already 32-byte aligned, the copy results of the two modes are the same.

This API takes effect only on AIV.

## Prototype

### Contiguous Data Movement

```c
// Placeholder form.
__aicore__ inline void asc_copy_ub2gm_align(__gm__ <dtype>* dst,
                                            __ubuf__ <dtype>* src,
                                            uint32_t size)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```c
// Example: The source and destination data type is bfloat16_t.
__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst,
                                            __ubuf__ bfloat16_t* src,
                                            uint32_t size)
```

### High-Dimensional Slicing Data Movement

```c
// Placeholder form.
__aicore__ inline void asc_copy_ub2gm_align(__gm__ <dtype>* dst,
                                            __ubuf__ <dtype>* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            asc_store_l2_cache_mode l2_cache_mode,
                                            uint64_t dst_stride,
                                            uint32_t src_stride)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```c
// Example: The source and destination data type is bfloat16_t.
__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst,
                                            __ubuf__ bfloat16_t* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            asc_store_l2_cache_mode l2_cache_mode,
                                            uint64_t dst_stride,
                                            uint32_t src_stride)
```

## Parameters

### Contiguous Data Movement

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination GM. Must be 1-byte aligned. |
| src | Input | Start address of the source UB. Must be 32-byte aligned. |
| size | Input | Size of data to be copied, in bytes. Value range: [1, $2^{21}−1$]. |

### High-Dimensional Slicing Data Movement

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination GM. Must be 1-byte aligned. |
| src | Input | Start address of the source UB. Must be 32-byte aligned. |
| burst_count | Input | Number of consecutive data blocks to be copied. Value range: [1, $2^{21}−1$]. |
| burst_len | Input | Length of each consecutive data block to be copied, in bytes. Value range: [1, $2^{21}−1$]. |
| l2_cache_mode | Input | Enum value of the [asc_store_l2_cache_mode](../../defs/enum/asc_store_l2_cache_mode.md) type, which configures the data management policy in L2 Cache. |
| dst_stride | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the head of the preceding data block and the head of the following data block). The unit is bytes. Value range: [0, $2^{40}−1$].<br>When only 1 data block is copied, that is, when `burst_count` is set to 1, this parameter can be set to 0. |
| src_stride | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the head of the preceding data block and the head of the following data block). The unit is bytes. It is used to select the data copy mode. Value range: [0, $2^{21}−1$].<br>&bull; Equal to `burst_len`: Compact mode. Source data blocks are tightly arranged in UB, and `src_stride` supports byte alignment.<br>&bull; Not equal to `burst_len`: Normal mode. `src_stride` must satisfy the 32-byte alignment requirement.<br>When only 1 data block is copied, that is, when `burst_count` is set to 1, this parameter can be set to 0. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

### General Constraints

- Calling this API on a non-AIV returns directly.
- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If there is a data dependency between this instruction and other instructions, you must insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Contiguous Data Movement Constraints

- `size` must satisfy dtype byte alignment: when dtype is b16, it must be a multiple of 2; when dtype is b32, it must be a multiple of 4.

### High-Dimensional Slicing Data Movement Constraints

- `len_burst` must meet the dtype byte alignment requirement: when dtype is b16, it must be a multiple of 2; when dtype is b32, it must be a multiple of 4.
- When `src_stride` is not equal to `burst_len`, `src_stride` must be 32-byte aligned.
- When `src_stride` is set to 0, **hardware undefined behavior** may occur. To broadcast the same source data block to multiple consecutive locations in GM, use the loop function. For details, see [Key Feature Description](#key-feature-description).

## Key Feature Description

### Broadcast Scenario

When using the high-dimensional slicing data movement API, it is not recommended to implement Broadcast by setting `src_stride` to 0. Broadcast can be implemented through the loop function by setting `burst_count` to 1 and configuring loop copy through [asc_set_ub2gm_loop_size](../asc_set_ub2gm_loop_size.md), [asc_set_ub2gm_loop1_stride](../asc_set_ub2gm_loop1_stride.md), and [asc_set_ub2gm_loop2_stride](../asc_set_ub2gm_loop2_stride.md):

- Set `loop1_size` to the number of Broadcast times and `loop2_size` to 1.
- Set `loop1_src_stride` to 0 so that each inner loop iteration reads the same source data block from the UB.
- Set `loop1_dst_stride` to the number of bytes in a single data block so that each inner loop writes to the next consecutive location in GM.
- Set `loop2_src_stride` and `loop2_dst_stride` to 0. The outer loop executes only 1 time, so these two parameters do not produce an address offset.
- Set `burst_count` of `asc_copy_ub2gm_align` to 1 and set `src_stride` to a non-0 value. After the loop copy completes, call `asc_set_ub2gm_loop_size` to reset the loop count to 1.

The following example repeatedly copies a 256-byte source data block from UB 2048 times, generating 512 KB of consecutive data in GM. Before use, ensure that the UB space pointed to by `src` contains at least 256 bytes of valid data and that the GM space pointed to by `dst` contains at least 512 KB of writable space.

```cpp
constexpr uint32_t block_bytes = 256;
constexpr uint32_t broadcast_count = 2048;

asc_set_ub2gm_loop_size(broadcast_count, 1);
asc_set_ub2gm_loop1_stride(0, block_bytes);
asc_set_ub2gm_loop2_stride(0, 0);
asc_copy_ub2gm_align(dst, src, 1, block_bytes,
    asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, block_bytes, block_bytes);
asc_set_ub2gm_loop_size(1, 1);
```

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
#include "acl/acl.h"

namespace {

constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 256;

__global__ __vector__ void asc_copy_ub2gm_align_arch3510_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[INPUT_BYTES];
    asc_copy_gm2ub_align(local, input, INPUT_BYTES);
    // PIPE_MTE3 waits for the PIPE_MTE2 load to complete before copying UB data to GM.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    // Use 256 bytes to cover a full register of data, allowing the aligned store result to be checked byte by byte.
    asc_copy_ub2gm_align(output, local, INPUT_BYTES);
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
    for (uint32_t i = 0; i < OUTPUT_BYTES; ++i) golden[i] = input[i];
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_ub2gm_align_arch3510_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_ub2gm_align passed." : "[Failed] asc_copy_ub2gm_align failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
