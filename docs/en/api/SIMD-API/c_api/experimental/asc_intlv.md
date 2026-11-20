# asc_intlv

<!-- md-trans-meta sourceCommit=d9775184a456840aa55019c36424cc9fc4fb25d9 translatedAt=2026-11-16T15:31:24.404Z pushedAt=2026-11-18T03:12:16.711Z nl=2 -->

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

The header file path is `"c_api/experimental/reg_permute_sel_exp.h"`.

This is an experimental interface, which may be adjusted or removed in future versions, and compatibility is not guaranteed. When compiling, define the `CANN_ASC_USE_EXPERIMENTAL` macro and explicitly include the specified experimental header file.

Interleaves the data in the two source operand vector data registers and writes the results to two destination vector data registers respectively. Depending on the operand type, this interface provides two functional modes:

- **When the operands are vector data registers:** Interleaving is performed element by element. The elements in the first half of `src0` and `src1` are alternately written to `result.first`, and the elements in the second half are alternately written to `result.second`. Taking the `int8_t` type as an example, the interleaving process is shown in the following figure.
- **When the operands are mask registers:** Interleaving is performed at the granularity specified by the interface suffix. `asc_intlv_b8` uses 1 bit as a group, `asc_intlv_b16` uses 2 bits as a group, and `asc_intlv_b32` uses 4 bits as a group.

**Figure 1** Interleaving process for the int8_t type

![](../../../figures/asc_intlv.png)

This interface is a Reg vector computation interface and takes effect only on the AIV.

For the formal interface that outputs results through reference parameters, see [asc_intlv](../reg_compute/reg_permute_sel/asc_intlv.md).

## Prototype

### Vector Data Register Interleaving Mode

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline asc_<dtype>_<dtype>_pair asc_intlv(vector_<dtype> src0,
                                                          vector_<dtype> src1)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

#### Prototype Example

```c
// Example of returning the result through the function return value: interleave int8_t vector data registers.
__simd_callee__ inline asc_int8_t_int8_t_pair asc_intlv(vector_int8_t src0,
                                                        vector_int8_t src1)
```

### Mask Register Interleaving Mode

```c
// Return the result through the function return value.
__simd_callee__ inline asc_bool_bool_pair asc_intlv_b8(vector_bool src0,
                                                       vector_bool src1)
__simd_callee__ inline asc_bool_bool_pair asc_intlv_b16(vector_bool src0,
                                                        vector_bool src1)
__simd_callee__ inline asc_bool_bool_pair asc_intlv_b32(vector_bool src0,
                                                        vector_bool src1)
```

## Parameters

### Vector Data Register Interleaving Mode

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| src0 | Input | Source vector data register, the operand participating in interleaving. The first half of its elements are alternately written to `result.first` with the first half of `src1` elements, and the second half of its elements are alternately written to `result.second` with the second half of `src1` elements. In each pair of elements, the element from `src0` comes first. |
| src1 | Input | Source vector data register, the operand participating in interleaving. Its data type must be the same as that of `src0`. In each pair of elements, the element from `src1` comes second. |

### Mask Register Interleaving Mode

**Table 2** Parameter Description

| Parameter | Input/Output | Description |
|---|---|---|
| src0 | Input | Source operand (mask register). |
| src1 | Input | Source operand (mask register). |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

Returns a [dual return value structure](pair.md), and the return value is received using the variable `result`.

- When the operands are vector data registers, the return type is `asc_<dtype>_<dtype>_pair`, and the types of both members are `vector_<dtype>`. `result.first` stores the lower half of the interleaved data, and `result.second` stores the upper half.
- When the operands are mask registers, the return type for b8, b16, and b32 modes is `asc_bool_bool_pair`, and the types of both members are `vector_bool`, which store the lower half and upper half masks after interleaving at the corresponding granularity, respectively.

## Constraints

### General Constraints

- For general constraints on Reg vector compute C APIs, see [General Constraints](../reg_compute/overview.md#general-constraints).
- The source operands and destination operands are vector data registers or mask registers.

### Register Constraints

`src0` and `src1` can be the same vector data register or mask register.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng -DCANN_ASC_USE_EXPERIMENTAL example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

This example processes two groups of 512 `uint8_t` elements each. A single Repeat reads 256 elements from each of the two source registers. After the Kernel computes `one_repeat_size` and `repeat_time`, it uses `asc_vf_call` to launch the Vector Function, which performs interleaving twice in a loop. The Host verifies the results according to the range of each Repeat.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "c_api/experimental/reg_permute_sel_exp.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENT_COUNT = 512;
constexpr uint32_t BUFFER_BYTES = ELEMENT_COUNT * sizeof(uint8_t);

__simd_vf__ inline void intlv_vf(__ubuf__ uint8_t* output0, __ubuf__ uint8_t* output1,
                                 __ubuf__ uint8_t* input0, __ubuf__ uint8_t* input1,
                                 uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint8_t src0;
    vector_uint8_t src1;
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadalign(src0, input0 + i * one_repeat_size);
        asc_loadalign(src1, input1 + i * one_repeat_size);
        auto result = asc_intlv(src0, src1);
        asc_storealign(output0 + i * one_repeat_size, result.first, mask);
        asc_storealign(output1 + i * one_repeat_size, result.second, mask);
    }
}

__global__ __vector__ void asc_intlv_kernel(__gm__ uint8_t* output0, __gm__ uint8_t* output1,
                                            __gm__ uint8_t* input0, __gm__ uint8_t* input1)
{
    asc_init();
    __ubuf__ uint8_t output0_local[ELEMENT_COUNT];
    __ubuf__ uint8_t output1_local[ELEMENT_COUNT];
    __ubuf__ uint8_t input0_local[ELEMENT_COUNT];
    __ubuf__ uint8_t input1_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(input0_local, input0, BUFFER_BYTES);
    asc_copy_gm2ub_align(input1_local, input1, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    uint16_t one_repeat_size = asc_get_vf_len() / sizeof(uint8_t);
    uint16_t repeat_time = ELEMENT_COUNT / one_repeat_size;
    asc_vf_call<intlv_vf>(output0_local, output1_local, input0_local, input1_local,
                          one_repeat_size, repeat_time);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output0, output0_local, BUFFER_BYTES);
    asc_copy_ub2gm_align(output1, output1_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input0(ELEMENT_COUNT);
    std::vector<uint8_t> input1(ELEMENT_COUNT);
    std::vector<uint8_t> output0(ELEMENT_COUNT, 0);
    std::vector<uint8_t> output1(ELEMENT_COUNT, 0);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        input0[i] = static_cast<uint8_t>(i % 251);
        input1[i] = static_cast<uint8_t>(250 - i % 251);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input0_device = nullptr;
    uint8_t* input1_device = nullptr;
    uint8_t* output0_device = nullptr;
    uint8_t* output1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output0_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output1_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, BUFFER_BYTES, input0.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, BUFFER_BYTES, input1.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_intlv_kernel<<<1, 0>>>(output0_device, output1_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output0.data(), BUFFER_BYTES, output0_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output1.data(), BUFFER_BYTES, output1_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    const uint32_t one_repeat_size = 256 / sizeof(uint8_t);
    for (uint32_t repeat = 0; repeat < ELEMENT_COUNT / one_repeat_size; ++repeat) {
        const uint32_t base = repeat * one_repeat_size;
        for (uint32_t offset = 0; offset < one_repeat_size / 2; ++offset) {
            passed = passed && output0[base + 2 * offset] == input0[base + offset]
                     && output0[base + 2 * offset + 1] == input1[base + offset];
            passed = passed && output1[base + 2 * offset] == input0[base + one_repeat_size / 2 + offset]
                     && output1[base + 2 * offset + 1] == input1[base + one_repeat_size / 2 + offset];
        }
    }
    std::cout << (passed ? "[Success] asc_intlv completed." : "[Failed] asc_intlv output mismatch.")
              << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output0_device);
    aclrtFree(output1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
