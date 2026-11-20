---
okftype: reference.api
description: "Reads eight consecutive elements from the UB and broadcasts each element to the corresponding DataBlock (32 bytes) of the output result, providing three load modes."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: requires
    condition: "The address register offset mode must be used with asc_update_addr_reg"
version: ">=9.2.0"
version_changes: []
---
# asc_loadalign_brc_elem2datablock

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.324974+00:00 pushedAt=2026-11-16T02:10:55.450Z nl=2 -->

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

Reads 8 consecutive elements from the Unified Buffer (UB) at a 16-byte-aligned (b16 type) or 32-byte-aligned (b32 type) start address and broadcasts each element to a corresponding `DataBlock` (32 bytes) in the output result. The result is returned through the function return value or written to the destination vector data register. The data format and content remain unchanged during the load. This API provides three function modes:

- **Aligned load mode**: Loads data from the UB source address into the vector data register, with the source address updated by the user.
- **Immediate offset load mode**: Loads data from a position offset by a specified distance from the source start address. This API does not automatically update the source address.
- **Address register offset load mode**: Specifies the offset relative to the source start address through an address register. This is commonly used in aligned load scenarios within a hardware loop where the offset changes with the loop count. It must be used together with [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md).

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Aligned Load Mode

```c
// Return the result through the function return value.
__simd_callee__ inline vector_<dtype> asc_loadalign_brc_elem2datablock(__ubuf__ <dtype>* src)

// Output the result through a reference parameter.
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_<dtype>& dst,
                                                             __ubuf__ <dtype>* src)
```

#### Data Types Supported by dtype

`dtype` values are: `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

#### Typical Prototype Example

```c
// Example: half type.
__simd_callee__ inline vector_half asc_loadalign_brc_elem2datablock(__ubuf__ half* src)

__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_half& dst,
                                                             __ubuf__ half* src)
```

### Immediate Offset Load Mode

```c
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_<dtype>& dst,
                                                            __ubuf__ <dtype>* src,
                                                            int32_t offset)
```

#### Data Types Supported by dtype

`dtype` values are: `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

#### Typical Prototype Example

```c
// Example: half type.
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_half& dst,
                                                             __ubuf__ half* src,
                                                             int32_t offset)
```

### Address Register Offset Load Mode

```c
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_<dtype>& dst,
                                                            __ubuf__ <dtype>* src,
                                                            addr_reg offset)
```

#### Data Types Supported by dtype

`dtype` values are: `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

#### Typical Prototype Example

```c
// Example: half type.
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_half& dst,
                                                             __ubuf__ half* src,
                                                             addr_reg offset)
```

## Parameters

### Aligned Load Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register. Only the void-returning API contains this parameter. The dtype must be the same as `src`, and VL bytes of data are loaded. |
| src | Input | Source UB address. The actual read address must meet the alignment requirement: the b16 type must be 16-byte aligned, and the b32 type must be 32-byte aligned. |

### Immediate Offset Load Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register. The dtype must be the same as `src`, and VL bytes of data are loaded. |
| src | Input | Source UB address. The actual read address must meet the alignment requirement: the b16 type must be 16-byte aligned, and the b32 type must be 32-byte aligned. |
| offset | Input | Offset relative to the `src` start address, in elements. |

### Address Register Offset Load Mode

**Table 3** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register. The dtype must be the same as `src`, and VL bytes of data are loaded. |
| src | Input | Source UB address. The actual read address must meet the alignment requirement: the b16 type must be 16-byte aligned, and the b32 type must be 32-byte aligned. |
| offset | Input | Address register of type `addr_reg`, which must be generated by `asc_update_addr_reg`. This register records the offset relative to the `src` start address, in elements. The b16 and b32 modes indicate that each element occupies 2 bytes and 4 bytes, respectively. |

For details about vector data registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

For the return-value API, the vector data register that stores the broadcast load result is returned, and its data type is consistent with `src`.

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- In each function mode, the actual read address of the b16 type must be 16-byte aligned, and the actual read address of the b32 type must be 32-byte aligned. The actual read range must be within the UB address space without exceeding the boundary; otherwise, an error is reported.
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The UB address after applying the offset must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Address Register Offset Load Mode

`asc_update_addr_reg` must be called first.

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

constexpr uint32_t OUTPUT_ELEMENTS = 260;
constexpr uint32_t INPUT_ELEMENTS = AscendC::Std::ceil_div(OUTPUT_ELEMENTS, 128) * 8;
constexpr uint32_t INPUT_BYTES = INPUT_ELEMENTS * sizeof(uint16_t);
constexpr uint32_t OUTPUT_BYTES = OUTPUT_ELEMENTS * sizeof(uint16_t);
constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = AscendC::Std::ceil_div(OUTPUT_ELEMENTS, ELEMENTS_PER_REGISTER);
constexpr uint32_t OUTPUT_STORAGE_ELEMENTS = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t INPUT_STORAGE_ELEMENTS = REPEAT_COUNT * 8;

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = OUTPUT_ELEMENTS;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint16_t);
__simd_vf__ inline void asc_loadalign_brc_elem2datablock_vf(
    __ubuf__ uint16_t* output, __ubuf__ uint16_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = OUTPUT_ELEMENTS;
    vector_uint16_t dst_reg;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b16(remaining_length);
        asc_loadalign_brc_elem2datablock(dst_reg, input + i * 8);
        asc_storealign(output + i * one_repeat_size, dst_reg, mask);
    }
}

__global__ __vector__ void asc_loadalign_brc_elem2datablock_kernel(
    __gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t input_local[INPUT_STORAGE_ELEMENTS];
    __ubuf__ uint16_t output_local[OUTPUT_STORAGE_ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, INPUT_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_brc_elem2datablock_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(INPUT_ELEMENTS);
    std::vector<uint16_t> output(OUTPUT_ELEMENTS, 0xffff);
    std::vector<uint16_t> golden(OUTPUT_ELEMENTS, 0);
    for (uint16_t i = 0; i < INPUT_ELEMENTS; ++i) input[i] = static_cast<uint16_t>(i + 1);
    for (uint16_t i = 0; i < OUTPUT_ELEMENTS; ++i) golden[i] = input[i / 16];

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_brc_elem2datablock_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadalign_brc_elem2datablock example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
