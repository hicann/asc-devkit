---
okftype: reference.api
description: "Writes the tail block temporarily stored in the non-aligned register to the Unified Buffer for finalizing contiguous non-aligned movement. Two modes are provided: immediate offset and address register offset."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body explains vector data registers and unaligned registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "The text uses this to reference the general constraints of Reg vector computation"
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
    note: "Added int4b_t and vector_int4x2_t data overloads: CANN 9.1.0 Release Notes explicitly list asc_storeunalign_post as adding low-precision overloads, corresponding to int4b_t in the dtype list in the body."
---
# asc_storeunalign_post

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.374436+00:00 pushedAt=2026-11-16T02:10:57.309Z nl=2 -->

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

The header file path is `"c_api/reg_compute/store/storeunalign.h"`.

Writes the tail block temporarily stored in the unaligned register to the Unified Buffer (UB) to finish a contiguous unaligned store. This API provides the following two modes:

- **Immediate offset store mode**: Specifies the offset relative to the destination start address through an immediate value, in units of elements. You can choose to manually update the offset or update the destination address. It is used with the preceding unaligned store API [asc_storeunalign](asc_storeunalign.md) or [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md) to finish the store.
- **Address register offset store mode**: Specifies the offset relative to the destination start address through an address register. It is commonly used in Hardware Loop scenarios where the offset changes with the loop count. It must be used together with [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md). It is used with the preceding unaligned store API [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md) to finish the store.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Immediate Offset Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ <dtype>* dst,
                                                  vector_store_unalign src,
                                                  int32_t offset)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, or `int64_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst,
                                                  vector_store_unalign src,
                                                  int32_t offset)
```

### Address Register Offset Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ <dtype>* dst,
                                                  vector_store_unalign src,
                                                  addr_reg offset)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, or `int64_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst,
                                                  vector_store_unalign src,
                                                  addr_reg offset)
```

## Parameters

### Immediate Offset Store Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Base address of the destination operand in UB, of type `__ubuf__ <dtype>*`. The address does not need to be 32-byte aligned, but must be aligned to `sizeof(dtype)` bytes. |
| src | Input | Unaligned register of type `vector_store_unalign`, with a length of 32 bytes. The same register must be used as in the preceding main store API. |
| offset | Input | Offset of the end address of the destination operand, of type `int32_t`, in units of elements. |

### Address Register Offset Store Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Base address of the destination operand in UB, of type `__ubuf__ <dtype>*`. The address does not need to be 32-byte aligned, but must be aligned to `sizeof(dtype)` bytes. |
| src | Input | Unaligned register of type `vector_store_unalign`, with a length of 32 bytes. The same register must be used as in the preceding main store API. |
| offset | Input | Offset of the end address of the destination operand, of type `addr_reg`. It must be the same register as that used by the preceding main store API. |

For details about unaligned registers and address registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- Ensure that the address obtained by adding `offset` to the destination operand address is within the actual available UB range.
- The destination address in this API does not need to be 32B-aligned, but `dst` of data type `dtype` must be aligned to `sizeof(dtype)` bytes.
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The destination operand address must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Immediate Offset Store Mode

- Use this API with the **mask register store mode** of [asc_storeunalign](asc_storeunalign.md) or [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md), and reuse the same `src`.
- Ensure that the address obtained by adding the offset to the destination address equals the end address of the store performed by the **mask register store mode** of [asc_storeunalign](asc_storeunalign.md) or [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md); otherwise, the store cannot be finished correctly.

### Address Register Offset Store Mode

- Use this API with the **address register offset store mode** of [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md), and reuse the same unaligned register `src` and address register.

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DATA_BYTES = 260 * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t START_OFFSET = 8;
constexpr uint32_t OUTPUT_BYTES = START_OFFSET + TOTAL_LENGTH;
constexpr uint32_t PADDED_OUTPUT_BYTES = START_OFFSET + PADDED_ELEMENT_COUNT + 32;

__simd_vf__ inline void storeunalign_post_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_store_unalign unalign_reg;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const uint32_t valid_count = (offset + ELEMENTS_PER_REGISTER < TOTAL_LENGTH) ?
            ELEMENTS_PER_REGISTER : TOTAL_LENGTH - offset;
        vector_uint8_t src;
        asc_loadalign(src, input + offset);
        asc_storeunalign(output + START_OFFSET + offset, unalign_reg, src, valid_count);
        asc_storeunalign_post(output + START_OFFSET + offset, unalign_reg, valid_count);
    }
}

__global__ __vector__ void storeunalign_post_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_OUTPUT_BYTES];
    __ubuf__ uint8_t input_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_ELEMENT_COUNT);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<storeunalign_post_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint8_t> output(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) input[i] = static_cast<uint8_t>(i);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_ELEMENT_COUNT, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, PADDED_ELEMENT_COUNT, input.data(), PADDED_ELEMENT_COUNT, ACL_MEMCPY_HOST_TO_DEVICE);
    storeunalign_post_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) passed = passed && output[START_OFFSET + i] == input[i];
    std::cout << (passed ? "[Success] asc_storeunalign_post completed."
                         : "[Failed] asc_storeunalign_post output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
