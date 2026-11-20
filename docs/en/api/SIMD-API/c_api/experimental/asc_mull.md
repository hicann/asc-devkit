# asc_mull

<!-- md-trans-meta sourceCommit=d9775184a456840aa55019c36424cc9fc4fb25d9 translatedAt=2026-11-16T15:35:16.631Z pushedAt=2026-11-18T03:12:16.715Z nl=2 -->

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

The header file path is: `"c_api/experimental/reg_arith_exp.h"`.

This interface is an experimental interface, which may be adjusted or removed in later versions, and backward compatibility is not guaranteed. When compiling, you need to define the `CANN_ASC_USE_EXPERIMENTAL` macro and explicitly include the specified experimental header file.

Multiplies the valid elements in two source vector data registers according to the mask. Elements with a data type bit width of 32 bits are multiplied to obtain a 64-bit product, which is split into the low 32 bits and high 32 bits and written to two destination vector data registers respectively. Elements whose corresponding mask position is 1 participate in the calculation, and elements whose corresponding mask position is 0 are set to zero in the destination vector data register. The calculation formula is as follows:

$$
dst0_i = (src0_i \times src1_i) \& ((1 \ll bit) - 1)
$$

$$
dst1_i = (src0_i \times src1_i) \gg bit
$$

Where `bit` indicates the data type bit width of `src0` and `src1`, and `dst0_i` and `dst1_i` indicate the `i`-th element in the `first` and `second` members of the returned structure respectively.

**Figure 1** asc_mull calculation diagram

![](../../../figures/asc_mull.png)

This interface is a Reg vector calculation interface and takes effect only on AIV.

For the formal interface that outputs results through reference parameters, see [asc_mull](../reg_compute/reg_arith/asc_mull.md).

## Prototype

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline asc_<dtype>_<dtype>_pair asc_mull(vector_<dtype> src0,
                                                         vector_<dtype> src1,
                                                         vector_bool mask)
```

### Data Types Supported by dtype

`dtype` can be `int32_t` or `uint32_t`.

### Prototype Example

```c
// Example of returning the result through the function return value: uint32_t multiplication.
__simd_callee__ inline asc_uint32_t_uint32_t_pair asc_mull(vector_uint32_t src0,
                                                           vector_uint32_t src1,
                                                           vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| src0 | Input | Source operand (vector data register). |
| src1 | Input | Source operand (vector data register). The data type must be the same as that of `src0`. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate in the computation and the corresponding elements of `result.first` and `result.second` are set to zero. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

Returns the [double return value structure](pair.md) `asc_int32_t_int32_t_pair` or `asc_uint32_t_uint32_t_pair`. When the variable `result` is used to receive the return value, `result.first` stores the lower 32 bits of the 64-bit product, and `result.second` stores the upper 32 bits. The data types of both members are the same as that of the input vector data register.

## Constraints

### General Constraints

- Before using this API, include the header file: `#include "c_api/experimental/reg_arith_exp.h"`.
- Before use, add the compile option `-DCANN_ASC_USE_EXPERIMENTAL` to enable the experimental feature.
- For the general constraints of Reg vector computation C APIs, see [General Constraints](../reg_compute/overview.md#general-constraints).
- Element positions where the mask bit is 0 do not participate in the multiplication, and the corresponding positions in `result.first` and `result.second` are written with 0.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng -DCANN_ASC_USE_EXPERIMENTAL example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

This example processes 128 `uint32_t` elements, with 64 elements processed per Repeat. After the kernel computes `one_repeat_size` and `repeat_time`, it uses `asc_vf_call` to launch the Vector Function and completes two long multiplications through a `for` loop.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "c_api/experimental/reg_arith_exp.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENT_COUNT = 128;
constexpr uint32_t BUFFER_BYTES = ELEMENT_COUNT * sizeof(uint32_t);

__simd_vf__ inline void mull_vf(__ubuf__ uint32_t* output_low,
                                __ubuf__ uint32_t* output_high,
                                __ubuf__ uint32_t* input0,
                                __ubuf__ uint32_t* input1,
                                uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint32_t src0;
    vector_uint32_t src1;
    uint32_t count = ELEMENT_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b32(count);
        asc_loadalign(src0, input0 + i * one_repeat_size);
        asc_loadalign(src1, input1 + i * one_repeat_size);
        auto result = asc_mull(src0, src1, mask);
        asc_storealign(output_low + i * one_repeat_size, result.first, mask);
        asc_storealign(output_high + i * one_repeat_size, result.second, mask);
    }
}

__global__ __vector__ void asc_mull_kernel(__gm__ uint32_t* output_low,
                                           __gm__ uint32_t* output_high,
                                           __gm__ uint32_t* input0,
                                           __gm__ uint32_t* input1)
{
    asc_init();
    __ubuf__ uint32_t output_low_local[ELEMENT_COUNT];
    __ubuf__ uint32_t output_high_local[ELEMENT_COUNT];
    __ubuf__ uint32_t input0_local[ELEMENT_COUNT];
    __ubuf__ uint32_t input1_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(input0_local, input0, BUFFER_BYTES);
    asc_copy_gm2ub_align(input1_local, input1, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    uint16_t one_repeat_size = asc_get_vf_len() / sizeof(uint32_t);
    uint16_t repeat_time = ELEMENT_COUNT / one_repeat_size;
    asc_vf_call<mull_vf>(output_low_local, output_high_local, input0_local, input1_local,
                         one_repeat_size, repeat_time);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output_low, output_low_local, BUFFER_BYTES);
    asc_copy_ub2gm_align(output_high, output_high_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> input0(ELEMENT_COUNT);
    std::vector<uint32_t> input1(ELEMENT_COUNT);
    std::vector<uint32_t> output_low(ELEMENT_COUNT, 0);
    std::vector<uint32_t> output_high(ELEMENT_COUNT, 0);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        input0[i] = i + 1;
        input1[i] = 0x10000000U + i;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* input0_device = nullptr;
    uint32_t* input1_device = nullptr;
    uint32_t* output_low_device = nullptr;
    uint32_t* output_high_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_low_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_high_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, BUFFER_BYTES, input0.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, BUFFER_BYTES, input1.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_mull_kernel<<<1, 0>>>(output_low_device, output_high_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output_low.data(), BUFFER_BYTES, output_low_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output_high.data(), BUFFER_BYTES, output_high_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        const uint64_t expected = static_cast<uint64_t>(input0[i]) * input1[i];
        if (output_low[i] != static_cast<uint32_t>(expected) ||
            output_high[i] != static_cast<uint32_t>(expected >> 32)) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_mull completed."
                         : "[Failed] asc_mull output mismatch.")
              << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output_low_device);
    aclrtFree(output_high_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
