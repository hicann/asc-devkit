# asc_copy_ub2gm (NPU architecture version 3510)

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T01:40:04.079966+00:00 pushedAt=2026-11-16T02:10:59.467Z nl=2 -->

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

Copies data from Unified Buffer (UB) to Global Memory (GM).

This API supports contiguous data movement and high-dimensional slicing data movement. Contiguous data movement copies the number of bytes specified by `size` from the source UB to the destination GM, as shown in Figure 1. High-dimensional slicing data movement copies data blocks according to the count specified by `burst_count`, with each data block copied according to the number of bytes specified by `burst_len`, and uses `src_stride` and `dst_stride` to specify the start address gaps between adjacent data blocks of the source and destination operands, as shown in Figure 2.

When the valid length is not 32-byte aligned, the hardware pads the read from the UB to the 32-byte boundary, but writes only the valid data to GM. Contiguous movement pads the read only at the end of the entire copy. In high-dimensional slicing data movement, when `src_stride` equals `burst_len`, the read is padded only at the end of all data blocks as a whole; otherwise, each data block is padded separately. The source UB must reserve valid space for the actual read range.

**Figure 1** Contiguous data movement diagram
![](../../../../figures/asc_copy_ub2gm_continuous.png "Contiguous data movement diagram")

**Figure 2** High-dimensional slicing data movement diagram
![](../../../../figures/asc_copy_ub2gm_highdim_split.png "Contiguous data movement diagram")

This API takes effect only on AIV.

## Prototype

### Contiguous Data Movement

```c
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

### High-Dimensional Slicing Data Movement

```c
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t burst_count,
                                      uint32_t burst_len,
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
| dst_stride | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the head of the preceding data block and the head of the following data block). The unit is bytes. Value range: [0, $2^{40}−1$].<br>When only 1 data block is copied, that is, when `burst_count` is set to 1, this parameter can be set to 0. |
| src_stride | Input | Distance between adjacent contiguous data blocks of the source operand (the interval between the head of the preceding data block and the head of the following data block), in bytes. Value range: [0, $2^{21}−1$].<br>When moving only one data block, that is, when `burst_count` is set to 1, this parameter can be set to 0. |

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
- When `size` is not 32-byte aligned, the source UB must reserve a readable range padded to the 32-byte boundary.

### High-Dimensional Slicing Data Movement Constraints

- `burst_len` must satisfy dtype byte alignment: when dtype is b16, it must be a multiple of 2; when dtype is b32, it must be a multiple of 4.
- When `src_stride` is not equal to `burst_len`, `src_stride` must be 32-byte aligned.
- The source UB capacity must be calculated based on the actual read range after Compact whole padding or Normal per-block padding.

## Key Feature Description

### Broadcast Scenario

When using the high-dimensional slicing data movement API, it is not recommended to implement Broadcast by setting `src_stride` to 0. Broadcast can be implemented through the loop function by setting `burst_count` to 1 and configuring loop copy through [asc_set_ub2gm_loop_size](../asc_set_ub2gm_loop_size.md), [asc_set_ub2gm_loop1_stride](../asc_set_ub2gm_loop1_stride.md), and [asc_set_ub2gm_loop2_stride](../asc_set_ub2gm_loop2_stride.md):

- Set `loop1_size` to the number of Broadcast times and `loop2_size` to 1.
- Set `loop1_src_stride` to 0 so that each inner loop iteration reads the same source data block from the UB.
- Set `loop1_dst_stride` to the number of bytes in a single data block so that each inner loop writes to the next consecutive location in GM.
- Set `loop2_src_stride` and `loop2_dst_stride` to 0. The outer loop executes only 1 time, so these two parameters do not produce an address offset.
- Set `burst_count` of `asc_copy_ub2gm` to 1 and set `src_stride` to a non-0 value. After the loop copy completes, call `asc_set_ub2gm_loop_size` to reset the loop count to 1.

The following example repeatedly copies a 256-byte source data block from UB 2048 times, generating 512 KB of consecutive data in GM. Before use, ensure that the UB space pointed to by `src` contains at least 256 bytes of valid data and that the GM space pointed to by `dst` contains at least 512 KB of writable space.

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

__global__ __vector__ void asc_copy_ub2gm_arch_3510_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[INPUT_BYTES];
    asc_copy_gm2ub_align(local, input, INPUT_BYTES);
    // PIPE_MTE3 waits for the PIPE_MTE2 load to complete before copying UB data to GM.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    // Use 256 bytes to cover a full register of data, allowing the store result to be checked byte by byte.
    asc_copy_ub2gm(output, local, INPUT_BYTES);
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
    asc_copy_ub2gm_arch_3510_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_ub2gm passed." : "[Failed] asc_copy_ub2gm failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
