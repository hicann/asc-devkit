---
okftype: reference.api
description: "Reg vector gather API: given a UB base address and an index vector register, gathers the corresponding data blocks into the destination vector register at DataBlock (32-byte) granularity, where only the first 8 elements of index are valid, provides both mask-controlled and unmasked forms, and takes effect only on the AI Vector."
tag:
  - roles: [data_movement]
  - operations: [gather]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "Vector data registers and mask registers are defined according to their data types"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints of Reg vector computation C APIs follow their overview page"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_gather_datablock__Rvi4x2_PUi4x2_vu32_vb@3510"
    scope_basis: resolved
    note: "Release Notes 9.1.0 records newly added int4b_t overloads and a uint64_t overload that does not require a mask parameter (corresponding to asc_gather_datablock__Rvu64_PUu64_vu32); this event locates the int4b_t masked form, and the unmasked forms added in the same batch are shown in the unmasked prototype table"
---
# asc_gather_datablock

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.409281+00:00 pushedAt=2026-11-16T02:10:57.373Z nl=2 -->

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

The header file path is `"c_api/reg_compute/gather/ub_gather.h"`.

Given the base address of the source operand in Unified Buffer (UB) and the index, collects the source operand into the destination operand by DataBlock based on the index position. Each DataBlock is 32B in length.

![](../../../../figures/asc_gather_datablock.png)

Only the first 8 elements in index are valid, and each element corresponds to one DataBlock. For example, if the first element is 96 (3 * 32), DataBlock3 is selected and written to the corresponding position in dst.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Mask Control Supported

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_gather_datablock(vector_<dst_dtype>& dst,
                                                 __ubuf__ <src_dtype>* src,
                                                 vector_uint32_t index,
                                                 vector_bool mask)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dst_dtype> asc_gather_datablock(__ubuf__ <src_dtype>* src,
                                                               vector_uint32_t index,
                                                               vector_bool mask)
```

**Table 1** Supported data type combinations

| dst Data Type | src Data Type | Function Return Value Form |
|---|---|---|
| `vector_int4x2_t` | `int4b_t` | `vector_int4x2_t` |
| `vector_int8_t` | `int8_t` | `vector_int8_t` |
| `vector_uint8_t` | `uint8_t` | `vector_uint8_t` |
| `vector_fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `vector_fp4x2_e2m1_t` |
| `vector_fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `vector_fp4x2_e1m2_t` |
| `vector_hifloat8_t` | `hifloat8_t` | `vector_hifloat8_t` |
| `vector_fp8_e8m0_t` | `fp8_e8m0_t` | `vector_fp8_e8m0_t` |
| `vector_fp8_e5m2_t` | `fp8_e5m2_t` | `vector_fp8_e5m2_t` |
| `vector_fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `vector_fp8_e4m3fn_t` |
| `vector_int16_t` | `int16_t` | `vector_int16_t` |
| `vector_uint16_t` | `uint16_t` | `vector_uint16_t` |
| `vector_half` | `half` | `vector_half` |
| `vector_bfloat16_t` | `bfloat16_t` | `vector_bfloat16_t` |
| `vector_int32_t` | `int32_t` | `vector_int32_t` |
| `vector_uint32_t` | `uint32_t` | `vector_uint32_t` |
| `vector_float` | `float` | `vector_float` |
| `vector_int64_t` | `int64_t` | Not supported |
| `vector_uint64_t` | `uint64_t` | Not supported |

#### Typical Prototype Example

```c
// Example of outputting the result through a reference parameter: int8_t type.
__simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst,
                                                 __ubuf__ int8_t* src,
                                                 vector_uint32_t index,
                                                 vector_bool mask)

// Example of returning the result through the function return value: int8_t type.
__simd_callee__ inline vector_int8_t asc_gather_datablock(__ubuf__ int8_t* src,
                                                          vector_uint32_t index,
                                                          vector_bool mask)
```

### No Mask Control

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_gather_datablock(vector_<dst_dtype>& dst,
                                                 __ubuf__ <src_dtype>* src,
                                                 vector_uint32_t index)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dst_dtype> asc_gather_datablock(__ubuf__ <src_dtype>* src,
                                                               vector_uint32_t index)
```

**Table 2** Supported data type combinations

| dst Data Type | src Data Type | Function Return Value Form |
|---|---|---|
| `vector_int4x2_t` | `int4b_t` | `vector_int4x2_t` |
| `vector_int8_t` | `int8_t` | `vector_int8_t` |
| `vector_uint8_t` | `uint8_t` | `vector_uint8_t` |
| `vector_fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `vector_fp4x2_e2m1_t` |
| `vector_fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `vector_fp4x2_e1m2_t` |
| `vector_hifloat8_t` | `hifloat8_t` | `vector_hifloat8_t` |
| `vector_fp8_e8m0_t` | `fp8_e8m0_t` | `vector_fp8_e8m0_t` |
| `vector_fp8_e5m2_t` | `fp8_e5m2_t` | `vector_fp8_e5m2_t` |
| `vector_fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `vector_fp8_e4m3fn_t` |
| `vector_int16_t` | `int16_t` | `vector_int16_t` |
| `vector_uint16_t` | `uint16_t` | `vector_uint16_t` |
| `vector_half` | `half` | `vector_half` |
| `vector_bfloat16_t` | `bfloat16_t` | `vector_bfloat16_t` |
| `vector_int32_t` | `int32_t` | `vector_int32_t` |
| `vector_uint32_t` | `uint32_t` | `vector_uint32_t` |
| `vector_float` | `float` | `vector_float` |
| `vector_int64_t` | `int64_t` | Not supported |
| `vector_uint64_t` | `uint64_t` | Not supported |

#### Typical Prototype Example

```c
// Example of outputting the result through a reference parameter: int8_t type.
__simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst,
                                                 __ubuf__ int8_t* src,
                                                 vector_uint32_t index)

// Example of returning the result through the function return value: int8_t type.
__simd_callee__ inline vector_int8_t asc_gather_datablock(__ubuf__ int8_t* src,
                                                          vector_uint32_t index)
```

## Parameters

### Mask Control Supported

**Table 3** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand (vector data register), applicable only to the function prototype that outputs the result through a reference parameter. |
| src | Input | Start address of the source operand (vector). |
| index | Input | Source operand (vector data register), indicating the index position of each DataBlock in dst relative to src in UB. The index position must be greater than or equal to 0 and 32B-aligned. Index values may be identical. **Only the first 8 elements of index are valid, and the unit is bytes.** |
| mask | Input | Source operand mask (mask register). **Valid indication for DataBlock movement, interpreted in b32 format. One DataBlock corresponds to 4 bits, and only the lowest bit of each 4-bit group is valid. Since only the first 8 elements of index are valid, mask uses only bits 0, 4, 8, 12, 16, 20, 24, and 28 corresponding to the first 8 b32 elements to control whether DataBlock0 through DataBlock7 in dst are updated. The remaining bits are invalid.** |

### No Mask Control

**Table 4** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand (vector data register), applicable only to the function prototype that outputs the result through a reference parameter. |
| src | Input | Start address of the source operand (vector). |
| index | Input | Source operand (vector data register), indicating the index position of each DataBlock in dst relative to src in UB. The index position must be greater than or equal to 0 and 32B-aligned. Index values may be identical. **Only the first 8 elements of index are valid, and the unit is bytes.** |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the return value returns the gathered result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The b64 data type is implemented through compiler software emulation, and the function prototype that outputs results through the function return value is not supported.
- The start address of the source operand in UB must be 32B-aligned.
- The index position must be greater than or equal to 0 and 32B-aligned, meaning one index value corresponds to one DataBlock.
- Index values may be identical, meaning the data of the same DataBlock in the source operand can be read multiple times.
- The data corresponding to the index value must be within the valid address range of UB.
- Only the first 8 elements of index are valid.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md).

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


template <typename T>
void PrintData(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t DATA_BLOCK_SIZE = 32;
constexpr uint32_t DATA_BLOCKS_PER_REGISTER = 8;
constexpr uint32_t DATA_BLOCK_COUNT = 9;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (DATA_BLOCK_COUNT + DATA_BLOCKS_PER_REGISTER - 1) / DATA_BLOCKS_PER_REGISTER;
constexpr uint32_t PADDED_DATA_BLOCK_COUNT = REPEAT_COUNT * DATA_BLOCKS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = DATA_BLOCK_SIZE * DATA_BLOCK_COUNT;
constexpr uint32_t PADDED_BUFFER_BYTES = DATA_BLOCK_SIZE * PADDED_DATA_BLOCK_COUNT;
constexpr uint32_t INDEX_WORDS = 64;

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BLOCK_COUNT;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * DATA_BLOCK_SIZE;
__simd_vf__ inline void gather_datablock_vf(
    __ubuf__ uint8_t* output, __ubuf__ uint8_t* data, __ubuf__ uint32_t* index, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = BUFFER_BYTES;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t block_offset = i * one_repeat_size;
        uint32_t valid_count = DATA_BLOCK_COUNT - block_offset;
        if (valid_count > DATA_BLOCKS_PER_REGISTER) {
            valid_count = DATA_BLOCKS_PER_REGISTER;
        }
        uint32_t remaining_length_gather = valid_count;
        vector_bool gather_mask = asc_update_mask_b32(remaining_length_gather);
        vector_bool store_mask = asc_update_mask_b8(remaining_length);
        vector_uint32_t index_reg;
        asc_loadalign(index_reg, index + block_offset);
        vector_uint8_t dst;
        asc_gather_datablock(dst, data, index_reg, gather_mask);
        asc_storealign(output + block_offset * DATA_BLOCK_SIZE, dst, store_mask);
    }
}

__global__ __vector__ void asc_gather_datablock_kernel(
    __gm__ uint8_t* output, __gm__ uint8_t* data, __gm__ uint32_t* index)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_BUFFER_BYTES];
    __ubuf__ uint8_t data_local[PADDED_BUFFER_BYTES];
    __ubuf__ uint32_t index_local[INDEX_WORDS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(data_local, data, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(index_local, index, INDEX_WORDS * sizeof(uint32_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = asc_get_vf_len() / DATA_BLOCK_SIZE;
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<gather_datablock_vf>(output_local, data_local, index_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> data(PADDED_BUFFER_BYTES, 0);
    std::vector<uint32_t> index(INDEX_WORDS);
    std::vector<uint8_t> output(BUFFER_BYTES, 0xff);
    std::vector<uint8_t> golden(BUFFER_BYTES);
    for (uint32_t i = 0; i < BUFFER_BYTES; ++i) {
        data[i] = static_cast<uint8_t>(i);
    }
    for (uint32_t b = 0; b < DATA_BLOCK_COUNT; ++b) {
        index[b] = (DATA_BLOCK_COUNT - 1 - b) * DATA_BLOCK_SIZE;
    }
    for (uint32_t b = 0; b < DATA_BLOCK_COUNT; ++b) {
        for (uint32_t j = 0; j < DATA_BLOCK_SIZE; ++j) {
            golden[b * DATA_BLOCK_SIZE + j] =
                static_cast<uint8_t>((DATA_BLOCK_COUNT - 1 - b) * DATA_BLOCK_SIZE + j);
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* data_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&data_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* index_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&index_device), INDEX_WORDS * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(data_device, data.size() * sizeof(uint8_t), data.data(), data.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(index_device, index.size() * sizeof(uint32_t), index.data(), index.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_gather_datablock_kernel<<<1, 0>>>(output_device, data_device, index_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Data", data);
    PrintData("Output bytes", output);
    PrintData("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_gather_datablock completed."
                         : "[Failed] asc_gather_datablock output mismatch.")
              << std::endl;
    aclrtFree(data_device);
    aclrtFree(index_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
