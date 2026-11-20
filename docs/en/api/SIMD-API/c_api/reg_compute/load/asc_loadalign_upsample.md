---
okftype: reference.api
description: "Reads contiguous data from UB at the corresponding granularity, performs 2x upsampling, and loads it into a vector or mask register: each element is repeated twice in a vector register, and each bit is repeated twice in a mask register. Three offset modes are supported."
tag:
  - roles: [data_movement]
  - operations: [broadcast, load]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body describes vector data registers and mask registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign_mask_upsample.md
    kind: related
    condition: "Use this API instead when the aligned load mode needs to return a mask register via the function return value"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: related
    condition: "In address register offset mode, it must be called first to generate the offset"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: capability_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added low-precision data type overloads: CANN 9.1.0 Release Notes record that APIs such as asc_loadalign_upsample add int4b_t and vector_int4x2_t overloads, corresponding to int4b_t in the dtype list in the body (the actual dst type is vector_int4x2_t)."
  - since: "9.1.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added vector_bool overload for loading into a mask register: CANN 9.1.0 Release Notes record that asc_loadalign_upsample adds a vector_bool overload, corresponding to the form with vector_bool& dst and __ubuf__ uint32_t* src as parameters in the three modes in the function prototype."
---
# asc_loadalign_upsample

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.412710+00:00 pushedAt=2026-11-16T02:10:55.571Z nl=2 -->

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

Reads contiguous data from a start address in the Unified Buffer (UB) that meets the corresponding alignment requirement and performs 2x upsampling. The result is returned through the function return value or written to the destination register.

- When `dst` is a vector data register, this API reads VL/2 bytes of data from a 32-byte-aligned start address in the UB, repeats each source element twice, and writes VL bytes of data.
- When `dst` is a mask register, this API reads VL/16 bytes of data from a 16-byte-aligned start address in the UB, repeats each bit twice, and writes VL/8 bytes of data.

This API supports three modes:

- **Aligned load mode**: Upsamples the data at the UB source address and loads it into the vector data register or mask register. The user updates the source address.
- **Immediate offset load mode**: Loads data from a position offset by a specified distance from the source start address. This API does not automatically update the source address.
- **Address register offset load mode**: Specifies the offset relative to the source start address through an address register. This is commonly used in aligned load scenarios within a hardware loop where the offset changes with the loop count. It must be used together with [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md).

In aligned load mode, when returning a mask register through the function return value, use [asc_loadalign_mask_upsample](asc_loadalign_mask_upsample.md).

**Figure 1** Upsampling load to a mask register

![](../../../../figures/capi_loadalign_mask_upsample.png)

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Aligned Load Mode

```c
// Return a vector data register through the function return value.
__simd_callee__ inline vector_<dtype> asc_loadalign_upsample(__ubuf__ <dtype>* src)

// Load into a vector data register.
__simd_callee__ inline void asc_loadalign_upsample(vector_<dtype>& dst,
                                                   __ubuf__ <dtype>* src)
// Load into a mask register.
__simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst,
                                                   __ubuf__ uint32_t* src)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, or `bfloat16_t`. When `dtype` is `int4b_t`, the actual type of dst is `vector_int4x2_t`.

#### Typical Prototype Example

```c
// Example: half type.
__simd_callee__ inline vector_half asc_loadalign_upsample(__ubuf__ half* src)

__simd_callee__ inline void asc_loadalign_upsample(vector_half& dst,
                                                   __ubuf__ half* src)
```

### Immediate Offset Load Mode

```c
// Load into a vector data register.
__simd_callee__ inline void asc_loadalign_upsample(vector_<dtype>& dst,
                                                   __ubuf__ <dtype>* src,
                                                   int32_t offset)
// Load into a mask register.
__simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst,
                                                   __ubuf__ uint32_t* src,
                                                   int32_t offset)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, or `bfloat16_t`. When `dtype` is `int4b_t`, the actual type of dst is `vector_int4x2_t`.

#### Typical Prototype Example

```c
// Example: half type.
__simd_callee__ inline void asc_loadalign_upsample(vector_half& dst,
                                                   __ubuf__ half* src,
                                                   int32_t offset)
```

### Address Register Offset Load Mode

```c
// Load into a vector data register.
__simd_callee__ inline void asc_loadalign_upsample(vector_<dtype>& dst,
                                                   __ubuf__ <dtype>* src,
                                                   addr_reg offset)
// Load into a mask register.
__simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst,
                                                   __ubuf__ uint32_t* src,
                                                   addr_reg offset)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, or `bfloat16_t`. When `dtype` is `int4b_t`, the actual type of dst is `vector_int4x2_t`.

#### Typical Prototype Example

```c
// Example: half type.
__simd_callee__ inline void asc_loadalign_upsample(vector_half& dst,
                                                   __ubuf__ half* src,
                                                   addr_reg offset)
```

## Parameters

### Aligned Load Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register or mask register. This parameter is included only in the API without a return value.<br>&bull; When `dst` is a vector data register, dtype must be consistent with `src`, and VL bytes of data are loaded.<br>&bull; When `dst` is a mask register, VL/8 bytes of data are loaded. |
| src | Input | Source UB address.<br>&bull; When `dst` is a vector data register, the actual read address must be 32-byte aligned.<br>&bull; When `dst` is a mask register, the actual read address must be 16-byte aligned. |

### Immediate Offset Load Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register or mask register.<br>&bull; When `dst` is a vector data register, the dtype must be the same as `src`, and VL bytes of data are loaded.<br>&bull; When `dst` is a mask register, VL/8 bytes of data are loaded. |
| src | Input | Source UB address.<br>&bull; When `dst` is a vector data register, the actual read address must be 32-byte aligned.<br>&bull; When `dst` is a mask register, the actual read address must be 16-byte aligned. |
| offset | Input | Offset relative to the `src` start address.<br>&bull; When `dst` is a vector data register, the unit is elements.<br>&bull; When `dst` is a mask register, the unit is bytes. |

### Address Register Offset Load Mode

**Table 3** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register or mask register.<br>&bull; When `dst` is a vector data register, the dtype must be the same as `src`, and VL bytes of data are loaded.<br>&bull; When `dst` is a mask register, VL/8 bytes of data are loaded. |
| src | Input | Source UB address.<br>&bull; When `dst` is a vector data register, the actual read address must be 32-byte aligned.<br>&bull; When `dst` is a mask register, the actual read address must be 16-byte aligned. |
| offset | Input | Address register of type `addr_reg`, which must be generated by `asc_update_addr_reg`. This register records the offset relative to the `src` start address, in elements. The b8, b16, and b32 modes indicate that each element occupies 1 byte, 2 bytes, and 4 bytes, respectively. When `dst` is `vector_bool`, use the b32 mode. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

For the return-value API, the vector data register that stores the upsampled load result is returned, and its data type is consistent with `src`.

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- When `dst` is a vector data register, the actual read address in each functional mode must be 32-byte aligned; when `dst` is a mask register, the actual read address must be 16-byte aligned. The actual read range must be within the UB address space without exceeding the boundary; otherwise, an error is reported.
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

constexpr uint32_t INPUT_ELEMENTS = 260;
constexpr uint32_t OUTPUT_ELEMENTS = 520;
constexpr uint32_t INPUT_BYTES = INPUT_ELEMENTS * sizeof(uint8_t);
constexpr uint32_t OUTPUT_BYTES = OUTPUT_ELEMENTS * sizeof(uint8_t);
constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = AscendC::Std::ceil_div(OUTPUT_ELEMENTS, ELEMENTS_PER_REGISTER);
constexpr uint32_t OUTPUT_STORAGE_ELEMENTS = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t INPUT_STORAGE_ELEMENTS = REPEAT_COUNT * 128;

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = OUTPUT_ELEMENTS;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
__simd_vf__ inline void asc_loadalign_upsample_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input,
                                                 uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = OUTPUT_ELEMENTS;
    vector_uint8_t dst_reg;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b8(remaining_length);
        asc_loadalign_upsample(dst_reg, input + i * 128);
        asc_storealign(output + i * one_repeat_size, dst_reg, mask);
    }
}

__global__ __vector__ void asc_loadalign_upsample_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[INPUT_STORAGE_ELEMENTS];
    __ubuf__ uint8_t output_local[OUTPUT_STORAGE_ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, INPUT_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_upsample_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_ELEMENTS);
    std::vector<uint8_t> output(OUTPUT_ELEMENTS, 0xff);
    std::vector<uint8_t> golden(OUTPUT_ELEMENTS, 0);
    for (uint16_t i = 0; i < INPUT_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);
    for (uint16_t i = 0; i < OUTPUT_ELEMENTS; ++i) golden[i] = input[i / 2];

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_upsample_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadalign_upsample example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
