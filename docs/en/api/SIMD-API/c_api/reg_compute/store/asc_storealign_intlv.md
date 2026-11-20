---
okftype: reference.api
description: "Interleaves two vector data registers in element order and stores them contiguously and aligned out to the Unified Buffer. A single store moves 2×VL, totaling 512 bytes, and masks are not supported. It provides three offset modes."
tag:
  - roles: [data_movement]
  - operations: [interleave, store]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The main text describes vector data registers/mask registers accordingly"
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
    note: "Added int4b_t and vector_int4x2_t data overloads: CANN 9.1.0 Release Notes explicitly list asc_storealign_intlv as adding low-precision overloads, corresponding to int4b_t in the dtype list in the body."
---
# asc_storealign_intlv

<!-- md-trans-meta sourceCommit=cf83f96b5ed7be9d4192c3dc624907102dc19cc4 translatedAt=2026-11-16T01:40:03.315718+00:00 pushedAt=2026-11-16T02:10:57.206Z nl=2 -->

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

Interleaves the data in two vector data registers element by element and performs a contiguous aligned store of the interleaved data to the Unified Buffer (UB). The amount stored per operation is `2 × VL` (512 bytes in total), and mask configuration is not supported. The data format and content remain unchanged during the store.

This API provides three functional modes with different parameter lists:

- **Aligned store mode**: The offset is fixed at 0. The data in `src0` and `src1` is interleaved and stored to the UB start address, and the user updates the destination address manually.
- **Immediate offset store mode**: An `int32_t offset` specifies the offset relative to the destination start address. The user can choose to update the offset or the destination address.
- **Address register offset store mode**: Specifies the offset relative to the destination start address through the address register `addr_reg offset`. It is commonly used in aligned store scenarios within a Hardware Loop where the offset changes with the loop count. It must be used together with [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md).

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Aligned Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ <dtype>* dst,
                                                 vector_<dtype> src0,
                                                 vector_<dtype> src1)
```

#### Data Types Supported by dtype

`dtype` values: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e1m2_t`, `fp4x2_e2m1_t`, `hifloat8_t`, `fp8_e4m3fn_t`, `fp8_e5m2_t`, `fp8_e8m0_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`. The corresponding vector register type is `vector_<dtype>`, and `int4b_t` corresponds to `vector_int4x2_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst,
                                                 vector_int8_t src0,
                                                 vector_int8_t src1)
```

### Immediate Offset Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ <dtype>* dst,
                                                 vector_<dtype> src0,
                                                 vector_<dtype> src1,
                                                 int32_t offset)
```

#### Data Types Supported by dtype

`dtype` values: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e1m2_t`, `fp4x2_e2m1_t`, `hifloat8_t`, `fp8_e4m3fn_t`, `fp8_e5m2_t`, `fp8_e8m0_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`. The corresponding vector register type is `vector_<dtype>`, and `int4b_t` corresponds to `vector_int4x2_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst,
                                                 vector_int8_t src0,
                                                 vector_int8_t src1,
                                                 int32_t offset)
```

### Address Register Offset Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ <dtype>* dst,
                                                 vector_<dtype> src0,
                                                 vector_<dtype> src1,
                                                 addr_reg offset)
```

#### Data Types Supported by dtype

`dtype` values: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e1m2_t`, `fp4x2_e2m1_t`, `hifloat8_t`, `fp8_e4m3fn_t`, `fp8_e5m2_t`, `fp8_e8m0_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`. The corresponding vector register type is `vector_<dtype>`, and `int4b_t` corresponds to `vector_int4x2_t`.

`float` corresponds to `vector_float`, interleaved out by 32-bit elements without numeric conversion. A single store moves 128 `float` elements (64 each from `src0` and `src1`), and all three store modes are supported. The implementation is consistent with the basic API `StoreAlign<float, StoreDist::DIST_INTLV_B32>`, reinterprets the source register and destination address as `uint32_t` bits before calling the underlying instruction, and preserves the original bit pattern of the floating-point data.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst,
                                                 vector_int8_t src0,
                                                 vector_int8_t src1,
                                                 addr_reg offset)
```

## Parameters

### Aligned Store Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand (vector). The type is `__ubuf__ <dtype>*`, and the start address must be 32-byte aligned. |
| src0 | Input | Source operand 0 (vector data register). `dtype` must be consistent with `dst`. |
| src1 | Input | Source operand 1 (vector data register). `dtype` must be consistent with `dst`. |

### Immediate Offset Store Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand (vector). The type is `__ubuf__ <dtype>*`, and the start address must be 32-byte aligned. |
| src0 | Input | Source operand 0 (vector data register). `dtype` must be consistent with `dst`. |
| src1 | Input | Source operand 1 (vector data register). `dtype` must be consistent with `dst`. |
| offset | Input | Offset relative to the start address of `dst`, of type `int32_t`, in elements. |

### Address Register Offset Store Mode

**Table 3** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand (vector). The type is `__ubuf__ <dtype>*`, and the start address must be 32-byte aligned. |
| src0 | Input | Source operand 0 (vector data register). `dtype` must be consistent with `dst`. |
| src1 | Input | Source operand 1 (vector data register). `dtype` must be consistent with `dst`. |
| offset | Input | Address register offset, of type `addr_reg`, which must be constructed in advance through the corresponding address register construction API. |

For details about vector data registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The start address of `dst` must be 32-byte aligned; otherwise, an error is reported.
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The destination operand address after applying the offset must not exceed the actual available capacity; otherwise, an error is reported.
- The actual access address after the `offset` parameter must be within the UB address range and must still be 32-byte aligned; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

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


constexpr uint32_t INPUT_COUNT = 128;
constexpr uint32_t STORE_COUNT = INPUT_COUNT * 2;
constexpr uint32_t REPEAT_TIMES = 2;
constexpr uint32_t DIRECT_STRIDE = 272;
constexpr uint32_t IMMEDIATE_BASE = 544;
constexpr uint32_t IMMEDIATE_OFFSET = 272;
constexpr uint32_t ADDR_BASE = 1360;
constexpr uint32_t ADDR_STRIDE = 272;
constexpr uint32_t OUTPUT_COUNT = 1904;
constexpr uint16_t SENTINEL = 0xffff;
constexpr uint32_t INPUT_BYTES = INPUT_COUNT * sizeof(uint16_t);
constexpr uint32_t OUTPUT_BYTES = OUTPUT_COUNT * sizeof(uint16_t);

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = STORE_COUNT + 5;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint16_t);
__simd_vf__ inline void store_intlv_vf(__ubuf__ uint16_t* output,
                                       __ubuf__ uint16_t* input0,
                                       __ubuf__ uint16_t* input1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t src0;
    vector_uint16_t src1;
    asc_loadalign(src0, input0);
    asc_loadalign(src1, input1);
    addr_reg offset;
    uint32_t remaining = STORE_COUNT + 5;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t valid_count = remaining > STORE_COUNT ? STORE_COUNT : remaining;
        asc_storealign_intlv(output + i * DIRECT_STRIDE, src0, src1);
        asc_storealign_intlv(output + IMMEDIATE_BASE + i * DIRECT_STRIDE,
                             src0, src1, static_cast<int32_t>(IMMEDIATE_OFFSET));
        offset = asc_update_addr_reg_b16(ADDR_STRIDE);
        asc_storealign_intlv(output + ADDR_BASE, src0, src1, offset);
        remaining -= valid_count;
    }
}

__global__ __vector__ void store_intlv_kernel(__gm__ uint16_t* output,
                                               __gm__ uint16_t* input0,
                                               __gm__ uint16_t* input1)
{
    asc_init();
    __ubuf__ uint16_t output_local[OUTPUT_COUNT];
    __ubuf__ uint16_t input0_local[INPUT_COUNT];
    __ubuf__ uint16_t input1_local[INPUT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub(output_local, output, OUTPUT_BYTES);
    asc_copy_gm2ub(input0_local, input0, INPUT_BYTES);
    asc_copy_gm2ub(input1_local, input1, INPUT_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint16_t one_repeat_size = 2 * asc_get_vf_len() / sizeof(uint16_t);
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<store_intlv_vf>(output_local, input0_local, input1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input0(INPUT_COUNT), input1(INPUT_COUNT);
    std::vector<uint16_t> output(OUTPUT_COUNT, SENTINEL);
    for (uint32_t i = 0; i < INPUT_COUNT; ++i) {
        input0[i] = static_cast<uint16_t>(i + 1);
        input1[i] = static_cast<uint16_t>(i + 1001);
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input0_device = nullptr;
    uint16_t* input1_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, INPUT_BYTES, input0.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, INPUT_BYTES, input1.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    store_intlv_kernel<<<1, 0>>>(output_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < OUTPUT_COUNT; ++i) {
        uint32_t relative = 0;
        bool written = false;
        const uint32_t starts[] = {
            0, DIRECT_STRIDE,
            IMMEDIATE_BASE + IMMEDIATE_OFFSET,
            IMMEDIATE_BASE + DIRECT_STRIDE + IMMEDIATE_OFFSET,
            ADDR_BASE, ADDR_BASE + ADDR_STRIDE
        };
        for (uint32_t start : starts) {
            if (i >= start && i < start + STORE_COUNT) {
                relative = i - start;
                written = true;
            }
        }
        const uint16_t expected = !written ? SENTINEL :
            (relative % 2 == 0 ? input0[relative / 2] : input1[relative / 2]);
        passed = passed && output[i] == expected;
    }
    std::cout << (passed ? "[Success] asc_storealign_intlv completed."
                         : "[Failed] asc_storealign_intlv output mismatch.") << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
