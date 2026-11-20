# asc_loadalign_deintlv

<!-- md-trans-meta sourceCommit=98f65e2752282a4f7a74e447bb6ee0a2566925c8 translatedAt=2026-11-16T15:33:01.681Z pushedAt=2026-11-18T03:12:16.713Z nl=2 -->

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

The header file path is: `"c_api/experimental/loadalign_exp.h"`.

This interface is an experimental interface and may be adjusted or removed in later versions, with compatibility not guaranteed. When compiling, you need to define the `CANN_ASC_USE_EXPERIMENTAL` macro and explicitly include the specified experimental header file.

Reads 2×VL length data from a 32-byte aligned start address in the Unified Buffer (UB), and returns the even-indexed and odd-indexed elements through the `first` and `second` members of the returned structure respectively. The source address is updated by the user, and only the offset-free aligned load mode is provided.

This interface is a Reg vector transfer interface and takes effect only on AIV.

For the formal interface that outputs results through reference parameters, see [asc_loadalign_deintlv](../reg_compute/load/asc_loadalign_deintlv.md).

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline asc_<dtype>_<dtype>_pair asc_loadalign_deintlv(__ubuf__ <dtype>* src)
```

### Data Types Supported by dtype

The values of `dtype` are: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

When `dtype` is `int4b_t`, the return type is `asc_int4x2_t_int4x2_t_pair`, and the member types are both `vector_int4x2_t`.

### Prototype Example

```c
// Example of returning the result through the function return value: perform deinterleave load on float data.
__simd_callee__ inline asc_float_float_pair asc_loadalign_deintlv(__ubuf__ float* src)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| src | Input | Source UB address. The actual read address must be 32-byte aligned. |

## Return Value

Returns the [pair structure](pair.md) `asc_<dtype>_<dtype>_pair`, whose two members are both of type `vector_<dtype>`. When the variable `result` is used to receive the return value, `result.first` stores the even-indexed elements of the source data, and `result.second` stores the odd-indexed elements. When `dtype` is `int4b_t`, the return type is `asc_int4x2_t_int4x2_t_pair`, and the member type is `vector_int4x2_t`.

## Constraints

- For the general constraints of Reg vector compute C APIs, see [General Constraints](../reg_compute/overview.md#general-constraints).
- The actual read address must be 32-byte aligned, and the actual read range must be within the UB address space without going out of bounds; otherwise, an error is reported.
- The UB capacity upper limit is 256 KB. The user-available capacity varies with compilation options and programming scenarios (by default, 6 KB SIMD VF stack + 2 KB Ascend C reserved, leaving 248 KB available; in SIMD+SIMT mixed programming, 32 KB to 128 KB is further allocated as Data Cache, further reducing the available capacity). After the UB address offset, the offset must not exceed the actual available capacity; otherwise, an error is reported.
- If this instruction overlaps with other instructions in UB addresses, a synchronization instruction [asc_mem_bar](../reg_compute/reg_sync/asc_mem_bar.md) must be inserted to serialize multiple instructions and prevent abnormal data.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng -DCANN_ASC_USE_EXPERIMENTAL example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

This example processes 1024 `uint8_t` elements, where `one_repeat_size` indicates the number of elements that one register can hold. Each Repeat reads `2 * one_repeat_size` elements, and the deinterleave load is executed twice in a loop. The Host generates the expected result according to the range of each Repeat.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "c_api/experimental/loadalign_exp.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t INPUT_ELEMENTS = 1024;
constexpr uint32_t OUTPUT_ELEMENTS = 1024;
constexpr uint32_t INPUT_BYTES = INPUT_ELEMENTS * sizeof(uint8_t);
constexpr uint32_t OUTPUT_BYTES = OUTPUT_ELEMENTS * sizeof(uint8_t);

__simd_vf__ inline void asc_loadalign_deintlv_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input,
                                                 uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        auto result = asc_loadalign_deintlv(input + i * 2 * one_repeat_size);
        asc_storealign(output + i * 2 * one_repeat_size, result.first, mask);
        asc_storealign(output + i * 2 * one_repeat_size + one_repeat_size, result.second, mask);
    }
}

__global__ __vector__ void asc_loadalign_deintlv_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[INPUT_ELEMENTS];
    __ubuf__ uint8_t output_local[OUTPUT_ELEMENTS];
    asc_copy_gm2ub_align(input_local, input, INPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    uint16_t one_repeat_size = asc_get_vf_len() / sizeof(uint8_t);
    uint16_t repeat_time = OUTPUT_ELEMENTS / (2 * one_repeat_size);
    asc_vf_call<asc_loadalign_deintlv_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_ELEMENTS);
    std::vector<uint8_t> output(OUTPUT_ELEMENTS, 0xff);
    std::vector<uint8_t> golden(OUTPUT_ELEMENTS, 0);
    for (uint16_t i = 0; i < INPUT_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);
    const uint32_t one_repeat_size = 256 / sizeof(uint8_t);
    for (uint32_t repeat = 0; repeat < OUTPUT_ELEMENTS / (2 * one_repeat_size); ++repeat) {
        const uint32_t base = repeat * 2 * one_repeat_size;
        for (uint32_t offset = 0; offset < one_repeat_size; ++offset) {
            golden[base + offset] = input[base + 2 * offset];
            golden[base + one_repeat_size + offset] = input[base + 2 * offset + 1];
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_deintlv_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadalign_deintlv example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
