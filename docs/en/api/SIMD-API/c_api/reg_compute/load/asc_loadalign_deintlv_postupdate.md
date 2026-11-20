---
okftype: reference.api
description: "Post Update form of deinterleaved load: reads data of 2x VL length from a 32-byte aligned address in the UB, writes even-indexed and odd-indexed elements to dst0 and dst1 respectively, and automatically updates the source UB pointer src by the offset stride after the load, making the address change visible to the caller."
tag:
  - roles: [data_movement]
  - operations: [deinterleave, load]
  - traits: [aligned_transfer, post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definition of the vector data register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints of Reg vector compute C APIs and UB capacity constraints"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "Insert this synchronization instruction when there is a data dependency with other instructions"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: parameter_changed
    scope: "param:src"
    scope_basis: resolved
    note: "src is changed from __ubuf__ <dtype>* src to a pointer reference __ubuf__ <dtype>*& src (upstream 9.1.0 first introduces vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t*& src, int32_t offset), making the address update after Post Update visible to the caller; before the change, only the local copy was updated. The offset unit remains the number of elements, and src advances by offset x sizeof(dtype) bytes."
---
# asc_loadalign_deintlv_postupdate

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.352077+00:00 pushedAt=2026-11-16T02:10:55.494Z nl=2 -->

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

Reads 2×VL bytes of data from the Unified Buffer (UB) at a 32-byte-aligned start address, writes the even-indexed elements to the vector data register `dst0`, and writes the odd-indexed elements to the vector data register `dst1`. This API uses the Post Update mode and automatically updates the source address pointer after the load is complete.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_<dtype>& dst0,
                                                             vector_<dtype>& dst1,
                                                             __ubuf__ <dtype>*& src,
                                                             int32_t offset)
```

### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, or `float`. When `dtype` is `int4b_t`, both `dst0` and `dst1` are of type `vector_int4x2_t`.

### Typical Prototype Example

```c
// Example: int16_t type.
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int16_t& dst0,
                                                             vector_int16_t& dst1,
                                                             __ubuf__ int16_t*& src,
                                                             int32_t offset)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst0 | Output | Destination vector data register. The dtype must be the same as `src`. Loads the even-indexed elements from the source data, with a data length of VL. |
| dst1 | Output | Destination vector data register. The dtype must be the same as `src`. Loads the odd-indexed elements from the source data, with a data length of VL. |
| src | Input/output | Source UB address. The actual read address must be aligned to 32 bytes. |
| offset | Input | Post Update stride, in elements. After the load from the current `src` address is complete, the API updates `src`; `src` increases by `offset × sizeof(dtype)` bytes. |

For details about vector data registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The current actual read address and the next actual read address after the Post Update must be 32-byte aligned, and the actual read range must be within the UB address space without exceeding the boundary; otherwise, an error is reported.
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The UB address after applying the offset must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Instruction Constraints

`dst0` and `dst1` must be different vector data registers.

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

constexpr uint32_t INPUT_ELEMENTS = 1030;
constexpr uint32_t OUTPUT_ELEMENTS = 1030;
constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t PAIR_COUNT = (OUTPUT_ELEMENTS + 1) / 2;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = AscendC::Std::ceil_div(PAIR_COUNT, ELEMENTS_PER_REGISTER);
constexpr uint32_t STORAGE_ELEMENTS = REPEAT_COUNT * ELEMENTS_PER_REGISTER * 2;
constexpr uint32_t INPUT_BYTES = STORAGE_ELEMENTS * sizeof(uint8_t);
constexpr uint32_t OUTPUT_BYTES = STORAGE_ELEMENTS * sizeof(uint8_t);

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = PAIR_COUNT;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
__simd_vf__ inline void asc_loadalign_deintlv_postupdate_vf(__ubuf__ uint8_t* output,
                                                            __ubuf__ uint8_t* input,
                                                            uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint8_t even_reg;
    vector_uint8_t odd_reg;
    uint32_t remaining_length_even = (OUTPUT_ELEMENTS + 1) / 2;
    uint32_t remaining_length_odd = OUTPUT_ELEMENTS / 2;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool even_mask = asc_update_mask_b8(remaining_length_even);
        vector_bool odd_mask = asc_update_mask_b8(remaining_length_odd);
        asc_loadalign_deintlv_postupdate(even_reg, odd_reg, input, 512);
        asc_storealign(output + i * 512, even_reg, even_mask);
        asc_storealign(output + i * 512 + one_repeat_size, odd_reg, odd_mask);
    }
}

__global__ __vector__ void asc_loadalign_deintlv_postupdate_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[STORAGE_ELEMENTS];
    __ubuf__ uint8_t output_local[STORAGE_ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, INPUT_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_deintlv_postupdate_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(STORAGE_ELEMENTS, 0);
    std::vector<uint8_t> output(STORAGE_ELEMENTS, 0xff);
    std::vector<uint8_t> golden(STORAGE_ELEMENTS, 0xff);
    for (uint16_t i = 0; i < INPUT_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);
    for (uint32_t i = 0; i < PAIR_COUNT; ++i) {
        const uint32_t repeat = i / ELEMENTS_PER_REGISTER;
        const uint32_t offset = i % ELEMENTS_PER_REGISTER;
        golden[repeat * 512 + offset] = input[repeat * 512 + 2 * offset];
        if (2 * i + 1 < OUTPUT_ELEMENTS) {
            golden[repeat * 512 + ELEMENTS_PER_REGISTER + offset] = input[repeat * 512 + 2 * offset + 1];
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_deintlv_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < PAIR_COUNT; ++i) {
        const uint32_t repeat = i / ELEMENTS_PER_REGISTER;
        const uint32_t offset = i % ELEMENTS_PER_REGISTER;
        passed = passed && output[repeat * 512 + offset] == golden[repeat * 512 + offset];
        if (2 * i + 1 < OUTPUT_ELEMENTS) {
            passed = passed && output[repeat * 512 + ELEMENTS_PER_REGISTER + offset] ==
                golden[repeat * 512 + ELEMENTS_PER_REGISTER + offset];
        }
    }
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadalign_deintlv_postupdate example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
