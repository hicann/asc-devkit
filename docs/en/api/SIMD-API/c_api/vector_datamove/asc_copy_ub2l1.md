---
okftype: reference.api
description: "Moves data from Unified Buffer to L1 Buffer, providing two prototypes: contiguous movement by size and high-dimensional slicing movement by burst_count, burst_len, and DataBlock intervals. In contiguous mode, size must be a multiple of 32 and between 32 and 4095 times 32 bytes."
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
version: ">=9.1.0"
version_changes: []
---
# asc_copy_ub2l1

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.098484+00:00 pushedAt=2026-11-16T02:10:59.510Z nl=2 -->

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

Provides data movement functionality to copy data from Unified Buffer (UB) to the L1 Buffer.

This API supports the following two data movement modes:

- Contiguous data movement

    Copies a segment of consecutive data from UB to the L1 Buffer, as shown in Figure 1.

    **Figure 1** Contiguous data movement diagram
    ![](../../../figures/asc_copy_ub2l1_continuous.png "Contiguous data movement diagram")

- High-dimensional slicing data movement

    Copies multiple consecutive data chunks from UB to the L1 Buffer. The length of each data chunk and the gap between adjacent data chunks are specified in units of DataBlock (32 bytes), as shown in Figure 2.

    **Figure 2** High-dimensional slicing data movement diagram
    ![](../../../figures/asc_copy_ub2l1_highdim_split.png "Contiguous data movement diagram")

This API takes effect only on AIV.

## Prototype

### Contiguous Data Movement

```c
__aicore__ inline void asc_copy_ub2l1(__cbuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

### High-Dimensional Slicing Data Movement

```c
__aicore__ inline void asc_copy_ub2l1(__cbuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_gap,
                                      uint16_t dst_gap)
```

## Parameters

### Contiguous Data Movement

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination L1 Buffer. It must be 32-byte aligned. |
| src | Input | Start address of the source UB. Must be 32-byte aligned. |
| size | Input | Size of the data to be copied, in bytes. Value range: [32, 4095×32], and it must be an integer multiple of 32. |

### High-Dimensional Slicing Data Movement

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination L1 Buffer. It must be 32-byte aligned. |
| src | Input | Start address of the source UB. Must be 32-byte aligned. |
| burst_count | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| burst_len | Input | Length of each consecutive data chunk to be copied. The unit is DataBlock (32 bytes). Value range: [1, $2^{16}−1$]. |
| src_gap | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes). Value range: [0, $2^{16}−1$]. |
| dst_gap | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes). Value range: [0, $2^{16}−1$]. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

### General Constraints

- Calling this API on a non-AIV returns directly.
- This API applies only to Mix operators and supports only a Cube-to-Vector core count ratio of 1:2.
- For the capacity and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If there is a data dependency between this instruction and other instructions, you must insert a [synchronization instruction](../sync/overview.md#synchronization-control-overview) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Consecutive Copy Mode Constraints

- `size` must be a multiple of 32; otherwise, truncating `size/32` results in fewer bytes being copied, and the trailing bytes that do not form a complete 32-byte block are discarded.

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

constexpr uint32_t DIM = 16;
constexpr uint32_t ELEMENTS = DIM * DIM;
constexpr uint32_t INPUT_BYTES = ELEMENTS * sizeof(half);
constexpr uint64_t SYNC_ID = 8;

__global__ __mix__(1, 2) void asc_copy_ub2l1_kernel(
    __gm__ float* output, __gm__ uint16_t* a, __gm__ uint16_t* b)
{
    asc_init();
    __ubuf__ half a_ub[ELEMENTS], b_ub[ELEMENTS];
    __cbuf__ half a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ half a_l0[ELEMENTS];
    __cb__ half b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
    constexpr int64_t l1_ready = 2;
    if ASC_IS_AIV {
        asc_copy_gm2ub_align(a_ub, reinterpret_cast<__gm__ half*>(a), INPUT_BYTES);
        asc_copy_gm2ub_align(b_ub, reinterpret_cast<__gm__ half*>(b), INPUT_BYTES);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        // Copy two contiguous Nz matrices from UB to the L1 buffers consumed by AIC.
        asc_copy_ub2l1(a_l1, a_ub, INPUT_BYTES);
        asc_copy_ub2l1(b_l1, b_ub, INPUT_BYTES);
        asc_sync_block_arrive(PIPE_MTE3, l1_ready);
        asc_sync_intra_arrive(PIPE_MTE3, SYNC_ID);
    }
    if ASC_IS_AIC {
        asc_sync_intra_wait(PIPE_MTE1, SYNC_ID);
        // AIC starts reading L1 only after both AIV MTE3 pipelines arrive.
        asc_sync_block_wait(PIPE_MTE1, l1_ready);
        asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
        asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, false, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_set_l0c_copy_nz_para(1, 0, 0);
        asc_copy_l0c2gm(output, c_l0, DIM, DIM, DIM, DIM, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
            asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
        asc_sync_pipe(PIPE_FIX);
    }
    asc_sync();
}

void print_row(const char* name, const std::vector<float>& data)
{
    std::cout << name << ":";
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    constexpr uint16_t half_bits[] = {0x3c00, 0x4000, 0x4200, 0x4400};
    std::vector<uint16_t> a(ELEMENTS), b(ELEMENTS, 0);
    std::vector<float> input(ELEMENTS), output(ELEMENTS, 0), golden(ELEMENTS);
    for (uint32_t row = 0; row < DIM; ++row) {
        for (uint32_t column = 0; column < DIM; ++column) {
            const uint32_t value = (row + column) % 4;
            a[row * DIM + column] = half_bits[value];
            input[row * DIM + column] = static_cast<float>(value + 1);
            golden[row * DIM + column] = input[row * DIM + column];
        }
        b[row * DIM + row] = half_bits[0];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, INPUT_BYTES, a.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, INPUT_BYTES, b.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_ub2l1_kernel<<<1, 0>>>(output_device, a_device, b_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Input A row 0", input);
    print_row("Output row 0", output);
    print_row("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_ub2l1 passed." : "[Failed] asc_copy_ub2l1 failed.") << std::endl;
    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
