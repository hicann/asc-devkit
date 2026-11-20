---
okftype: reference.api
description: "Unaligned load API: reads VL-length contiguous data from a dtype-aligned start address in UB, concatenates it with the preceding data cached in the 32-byte unaligned register by asc_loadunalign_pre, and loads it into a vector register. Before each load, source address preprocessing initialization must be performed first with the same src1. This API takes effect only on the AI Vector."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body explains vector data registers and unaligned registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadunalign_pre.md
    kind: requires
    condition: "Before each load, initialization must be performed in source address preprocessing mode, using the same src1 as this API"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
version: ">=9.0.0"
version_changes: []
---
# asc_loadunalign

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.423721+00:00 pushedAt=2026-11-16T02:10:55.584Z nl=2 -->

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

The header file path is `"c_api/reg_compute/load/loadunalign.h"`.

Reads VL bytes of contiguous data from a dtype-aligned start address in the Unified Buffer (UB) and loads it into a vector data register. This API concatenates the preceding data cached in the unaligned register (32 bytes) by [asc_loadunalign_pre](asc_loadunalign_pre.md) with the subsequent data read from the UB to obtain VL bytes of data, then loads the resulting data into the vector data register. For consecutive loads, manually update the source address before each call.

Let the start byte address of this actual read be `src_start_addr` and the end byte address be `src_end_addr`, where `src_end_addr = src_start_addr + VL`; align `src_start_addr` downward to a 32-byte boundary to obtain `aligned_src_start_addr`. `asc_loadunalign_pre` buffers the data in the byte address range `[aligned_src_start_addr, aligned_src_start_addr + 32)` in the unaligned register. This API concatenates that buffered data with the subsequent data read from the UB to obtain the data in the byte address range `[src_start_addr, src_end_addr)`.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline void asc_loadunalign(vector_<dtype>& dst,
                                            vector_load_unalign& src0,
                                            __ubuf__ <dtype>* src1)
```

### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, or `int64_t`. When `dtype` is `int4b_t`, the actual type of `dst` is `vector_int4x2_t`.

### Typical Prototype Example

```c
// Example: int8_t type.
__simd_callee__ inline void asc_loadunalign(vector_int8_t& dst,
                                            vector_load_unalign& src0,
                                            __ubuf__ int8_t* src1)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register. The dtype must be the same as `src1`, and VL bytes of data are loaded. |
| src0 | Input | Unaligned register, 32 bytes in size, which stores the preceding data required for this load. |
| src1 | Input | Effective start UB address for this load, which must be aligned to dtype. This API does not update this pointer. |

## Return Value

None

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The `src1` start address must be aligned to dtype, and the actual access range must be within the UB address space; otherwise, an error is reported.
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The UB address after applying the offset must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.
- Before each load, call the source address preprocessing mode in [asc_loadunalign_pre](asc_loadunalign_pre.md) for initialization, and the initialization API and this API must use the same `src1`.

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

constexpr uint32_t DATA_ELEMENTS = 517;
constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t PADDED_DATA_ELEMENTS =
    AscendC::Std::ceil_div(DATA_ELEMENTS, ELEMENTS_PER_REGISTER) * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_ELEMENTS = PADDED_DATA_ELEMENTS + 32;
constexpr uint32_t BUFFER_BYTES = BUFFER_ELEMENTS * sizeof(uint8_t);

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_ELEMENTS;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
__simd_vf__ inline void asc_loadunalign_vf(__ubuf__ uint8_t* dst, __ubuf__ uint8_t* src,
                                           uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_load_unalign load_unalign_reg;
    vector_store_unalign store_unalign_reg;
    vector_uint8_t dst_reg;
    asc_loadunalign_pre(load_unalign_reg, src);
    uint32_t remaining = DATA_ELEMENTS;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint16_t valid_count = remaining > one_repeat_size ? one_repeat_size : remaining;
        asc_loadunalign(dst_reg, load_unalign_reg, src + i * one_repeat_size);
        asc_storeunalign(dst + i * one_repeat_size, store_unalign_reg, dst_reg, valid_count);
        remaining -= valid_count;
    }
    asc_storeunalign_post(dst, store_unalign_reg, DATA_ELEMENTS);
}

__global__ __vector__ void asc_loadunalign_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[BUFFER_ELEMENTS];
    __ubuf__ uint8_t output_local[BUFFER_ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadunalign_vf>(output_local + 1, input_local + 1, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_ELEMENTS);
    std::vector<uint8_t> output(BUFFER_ELEMENTS, 0xff);
    for (uint16_t i = 0; i < BUFFER_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadunalign_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint16_t i = 0; i < DATA_ELEMENTS; ++i) passed = passed && output[i + 1] == input[i + 1];
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadunalign example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
