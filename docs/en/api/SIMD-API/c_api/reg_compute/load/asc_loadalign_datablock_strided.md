---
okftype: reference.api
description: "Loads eight DataBlocks non-contiguously from a 32-byte aligned address in the UB with a 32-byte stride specified by block_stride. mask determines whether to load at DataBlock granularity. The result is returned to the vector data register through the function return value. This API takes effect only on the AI Vector."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign.md
    kind: related
    condition: "Destination register form of the non-contiguous aligned load mode; this API is its return value form"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints of Reg vector compute C APIs"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "Insert this synchronization instruction when there is a data dependency with other instructions"
version: ">=9.2.0"
version_changes: []
---
# asc_loadalign_datablock_strided

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.340034+00:00 pushedAt=2026-11-16T02:10:55.479Z nl=2 -->

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

Loads 8 `DataBlock`s non-contiguously from the Unified Buffer (UB) at a 32-byte-aligned start address and returns the vector data register that stores the loaded result through the function return value. Each `DataBlock` has a data size of 32 bytes. You can configure the address stride between adjacent data blocks and the start read position of this load.

This API provides the same functionality as the non-contiguous aligned load mode of [asc_loadalign](asc_loadalign.md), except that this API returns the result through the function return value.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline vector_<dtype> asc_loadalign_datablock_strided(__ubuf__ <dtype>* src,
                                                                      uint16_t block_stride,
                                                                      uint16_t repeat_stride,
                                                                      vector_bool mask)
```

### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, or `float`. When `dtype` is `int4b_t`, the actual type of the return value is `vector_int4x2_t`.

### Typical Prototype Example

```c
// Example: uint8_t type.
__simd_callee__ inline vector_uint8_t asc_loadalign_datablock_strided(__ubuf__ uint8_t* src,
                                                                      uint16_t block_stride,
                                                                      uint16_t repeat_stride,
                                                                      vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| src | Input | Source UB address. The actual read address must be aligned to 32 bytes. |
| block_stride | Input | Stride between the start addresses of adjacent `DataBlock` units in the source operand, in 32-byte units. |
| repeat_stride | Input | Offset of the start read address for this load relative to `src`, in 32-byte units. The actual start read address is `src` offset by `repeat_stride × 32` bytes. |
| mask | Input | Mask register that indicates which elements participate in the computation. This API uses `DataBlock` as the data movement unit.<br>&bull; When any element in a `DataBlock` is selected as valid by `mask`, all data in that `DataBlock` is loaded into the vector data register.<br>&bull; When all elements in a `DataBlock` are selected as invalid by `mask`, the data in that `DataBlock` is not loaded into the vector data register, and the elements at the corresponding positions are set to 0. No error is reported even if the UB access is out of bounds. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

Returns the vector data register that stores the non-contiguous aligned load result, with the data type consistent with `src`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The actual read address must be 32-byte aligned, and the read range of valid `DataBlock`s must be within the UB address space without exceeding the boundary; otherwise, an error is reported.
- When all elements in a `DataBlock` are set to invalid by `mask`, no error is reported even if the `DataBlock` is out of bounds.
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

constexpr uint32_t DATABLOCK_BYTES = 32;
constexpr uint32_t DATABLOCKS_PER_REGISTER = 8;
constexpr uint32_t DATABLOCK_COUNT = 11;
constexpr uint16_t BLOCK_STRIDE = 2;
constexpr uint16_t REPEAT_STRIDE = 0;
constexpr uint32_t START_BLOCK = 1;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (DATABLOCK_COUNT + DATABLOCKS_PER_REGISTER - 1) / DATABLOCKS_PER_REGISTER;
constexpr uint32_t INPUT_BYTES_PER_REPEAT = 16 * DATABLOCK_BYTES;
constexpr uint32_t OUTPUT_BYTES_PER_REPEAT = DATABLOCKS_PER_REGISTER * DATABLOCK_BYTES;
constexpr uint32_t INPUT_BYTES = INPUT_BYTES_PER_REPEAT * REPEAT_COUNT;
constexpr uint32_t OUTPUT_BYTES = OUTPUT_BYTES_PER_REPEAT * REPEAT_COUNT;

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATABLOCK_COUNT;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * DATABLOCK_BYTES;
constexpr uint8_t MUTEX_ID = 0;

__simd_vf__ inline void asc_loadalign_datablock_strided_vf(
    __ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = DATABLOCK_COUNT * DATABLOCK_BYTES;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b8(remaining_length);
        const uint32_t input_offset = i * INPUT_BYTES_PER_REPEAT;
        const uint32_t output_offset = i * OUTPUT_BYTES_PER_REPEAT;
        // Specify the starting block offset through the pointer; repeat_stride is not the starting offset of this read.
        vector_uint8_t dst = asc_loadalign_datablock_strided(input + input_offset + START_BLOCK * DATABLOCK_BYTES, BLOCK_STRIDE, REPEAT_STRIDE, mask);
        asc_storealign(output + output_offset, dst, mask);
    }
}

__global__ __vector__ void asc_loadalign_datablock_strided_kernel(
    __gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[INPUT_BYTES];
    __ubuf__ uint8_t output_local[OUTPUT_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, INPUT_BYTES);
    asc_copy_gm2ub_align(output_local, output, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = asc_get_vf_len() / DATABLOCK_BYTES;
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_datablock_strided_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES);
    std::vector<uint8_t> output(OUTPUT_BYTES, 0);
    std::vector<uint8_t> golden(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t input_base = repeat * INPUT_BYTES_PER_REPEAT;
        const uint32_t output_base = repeat * OUTPUT_BYTES_PER_REPEAT;
        const uint32_t valid_blocks = repeat + 1 == REPEAT_COUNT ?
            DATABLOCK_COUNT - repeat * DATABLOCKS_PER_REGISTER : DATABLOCKS_PER_REGISTER;
        for (uint32_t block = 0; block < valid_blocks; ++block) {
            const uint32_t src_offset = input_base + (START_BLOCK + block * BLOCK_STRIDE) * DATABLOCK_BYTES;
            const uint32_t dst_offset = output_base + block * DATABLOCK_BYTES;
            for (uint32_t i = 0; i < DATABLOCK_BYTES; ++i) {
                golden[dst_offset + i] = input[src_offset + i];
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_datablock_strided_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success]" : "[Failed]")
              << " asc_loadalign_datablock_strided example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
