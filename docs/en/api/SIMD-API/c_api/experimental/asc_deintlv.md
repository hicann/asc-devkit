# asc_deintlv

<!-- md-trans-meta sourceCommit=d9775184a456840aa55019c36424cc9fc4fb25d9 translatedAt=2026-11-16T15:26:39.964Z pushedAt=2026-11-18T03:12:16.709Z nl=2 -->

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

This interface is an experimental interface and may be adjusted or removed in later versions, with no guarantee of future compatibility. When compiling, you need to define the `CANN_ASC_USE_EXPERIMENTAL` macro and explicitly include the specified experimental header file.

Deinterleaves the elements in src0 and src1 and stores them into `result.first` and `result.second`. This interface provides two functional modes: vector data deinterleave and mask deinterleave:

- **Vector data deinterleave**: Deinterleaves the valid elements in the two source vector data registers pair by pair. The even-indexed elements of src0 are written to the first half of `result.first`, and the odd-indexed elements are written to the first half of `result.second`. The even-indexed elements of src1 are written to the second half of `result.first`, and the odd-indexed elements are written to the second half of `result.second`.
- **Mask deinterleave**: According to the granularity specified by the interface suffix, deinterleaves the bits in the two source mask registers after grouping them. `asc_deintlv_b8` uses 1 bit per group, `asc_deintlv_b16` uses 2 bits per group, and `asc_deintlv_b32` uses 4 bits per group. The order of bits within a group remains unchanged. Groups are numbered starting from 0 from the low bit to the high bit. The even-indexed groups of src0 are written to the first half of `result.first`, and the odd-indexed groups are written to the first half of `result.second`. The even-indexed groups of src1 are written to the second half of `result.first`, and the odd-indexed groups are written to the second half of `result.second`.

Taking the int8_t data type as an example, the implementation flow of asc_deintlv is shown in Figure 1:

**Figure 1** Deinterleave implementation flow

![Deinterleave implementation flow](../../../figures/capi_deintlv_reg.png)

This interface is a Reg vector compute interface and takes effect only on the AIV.

For the formal interface that outputs results through reference parameters, see [asc_deintlv](../reg_compute/reg_permute_sel/asc_deintlv.md).

## Prototype

### Vector Data Register Deinterleave Mode

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline asc_<dtype>_<dtype>_pair asc_deintlv(vector_<dtype> src0,
                                                            vector_<dtype> src1)
```

#### Data Types Supported by dtype

`dtype` can be `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, or `float`.

#### Prototype Example

```c
// Example of returning the result through the function return value: perform deinterleave on a half vector data register.
__simd_callee__ inline asc_half_half_pair asc_deintlv(vector_half src0,
                                                      vector_half src1)
```

### Mask Register Deinterleave Mode

```c
// Return the result through the function return value
__simd_callee__ inline asc_bool_bool_pair asc_deintlv_b8(vector_bool src0,
                                                         vector_bool src1)

__simd_callee__ inline asc_bool_bool_pair asc_deintlv_b16(vector_bool src0,
                                                          vector_bool src1)

__simd_callee__ inline asc_bool_bool_pair asc_deintlv_b32(vector_bool src0,
                                                          vector_bool src1)
```

## Parameters

**Table 1** Parameter Description (operands are vector data registers)

| Parameter | Input/Output | Description |
|---|---|---|
| src0 | Input | Source vector data register, the operand participating in deinterleaving. `dtype` must be consistent with `src1`. Its even-indexed elements are written to the first half of `result.first`, and odd-indexed elements are written to the first half of `result.second`. |
| src1 | Input | Source vector data register, the operand participating in deinterleaving. `dtype` must be consistent with `src0`. Its even-indexed elements are written to the second half of `result.first`, and odd-indexed elements are written to the second half of `result.second`. |

**Table 2** Parameter Description (operands are mask registers)

| Parameter | Input/Output | Description |
|---|---|---|
| src0 | Input | Source mask register, the operand participating in deinterleaving. Its even-indexed groups are written to the first half of `result.first`, and odd-indexed groups are written to the first half of `result.second`. |
| src1 | Input | Source mask register, the operand participating in deinterleaving. Its even-indexed groups are written to the second half of `result.first`, and odd-indexed groups are written to the second half of `result.second`. |

For detailed descriptions of vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

Returns a [dual return value structure](pair.md), and the return value is received using the variable `result`.

- When the operands are vector data registers, the return type is `asc_<dtype>_<dtype>_pair`, and the types of both members are `vector_<dtype>`. `result.first` stores the set of even-indexed elements after deinterleaving, and `result.second` stores the set of odd-indexed elements.
- When the operands are mask registers, the return types for b8, b16, and b32 modes are all `asc_bool_bool_pair`, and the types of both members are `vector_bool`, which store the even-indexed groups and odd-indexed groups after deinterleaving at the corresponding granularity, respectively.

## Constraints

- For general constraints of Reg vector computation C APIs, see [General Constraints](../reg_compute/overview.md#general-constraints).
- The data types of `src0` and `src1` must be the same, and they can be the same vector data register or mask register.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id10 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng -DCANN_ASC_USE_EXPERIMENTAL example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id10 -->

This example processes two groups of 128 `float` elements each. A single Repeat reads 64 elements from each of the two source registers. After the kernel computes `one_repeat_size` and `repeat_time`, it uses `asc_vf_call` to launch the Vector Function, performs deinterleave twice in a loop, and the Host generates the expected result according to the range of each Repeat.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "c_api/experimental/reg_permute_sel_exp.h"
#include "acl/acl.h"

namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENT_COUNT = 128;
__simd_vf__ inline void rearrange(__ubuf__ float* dst0, __ubuf__ float* dst1,
    __ubuf__ float* src0, __ubuf__ float* src1,
    uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_float src0_reg;
    vector_float src1_reg;
    vector_bool mask = asc_create_mask_b32(PAT_ALL);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadalign(src0_reg, src0 + i * one_repeat_size);
        asc_loadalign(src1_reg, src1 + i * one_repeat_size);
        auto result = asc_deintlv(src0_reg, src1_reg);
        asc_storealign(dst0 + i * one_repeat_size, result.first, mask);
        asc_storealign(dst1 + i * one_repeat_size, result.second, mask);
    }
}
__global__ __vector__ void asc_deintlv_kernel(__gm__ float* dst0, __gm__ float* dst1,
    __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst0_local[ELEMENT_COUNT], dst1_local[ELEMENT_COUNT];
    __ubuf__ float src0_local[ELEMENT_COUNT], src1_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(src0_local, src0, ELEMENT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, ELEMENT_COUNT * sizeof(float));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    uint16_t one_repeat_size = asc_get_vf_len() / sizeof(float);
    uint16_t repeat_time = ELEMENT_COUNT / one_repeat_size;
    asc_vf_call<rearrange>(dst0_local, dst1_local, src0_local, src1_local,
                           one_repeat_size, repeat_time);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst0, dst0_local, ELEMENT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(dst1, dst1_local, ELEMENT_COUNT * sizeof(float));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src0(ELEMENT_COUNT), src1(ELEMENT_COUNT);
    std::vector<float> output0(ELEMENT_COUNT, 0), output1(ELEMENT_COUNT, 0);
    std::vector<float> golden0(ELEMENT_COUNT), golden1(ELEMENT_COUNT);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        src0[i] = static_cast<float>(i);
        src1[i] = 1000.0f + i;
    }
    const uint32_t one_repeat_size = 256 / sizeof(float);
    for (uint32_t repeat = 0; repeat < ELEMENT_COUNT / one_repeat_size; ++repeat) {
        const uint32_t base = repeat * one_repeat_size;
        for (uint32_t offset = 0; offset < one_repeat_size / 2; ++offset) {
            golden0[base + offset] = src0[base + 2 * offset];
            golden1[base + offset] = src0[base + 2 * offset + 1];
            golden0[base + one_repeat_size / 2 + offset] = src1[base + 2 * offset];
            golden1[base + one_repeat_size / 2 + offset] = src1[base + 2 * offset + 1];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), (ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst1_device), (ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(float), src0.data(), src0.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(float), src1.data(), src1.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_deintlv_kernel<<<1, 0>>>(dst0_device, dst1_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output0.data(), output0.size() * sizeof(float), dst0_device, output0.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output1.data(), output1.size() * sizeof(float), dst1_device, output1.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input 0", src0);
    print_data("Input 1", src1);
    print_data("Output 0", output0);
    print_data("Golden 0", golden0);
    print_data("Output 1", output1);
    print_data("Golden 1", golden1);
    const bool passed = compare_data(output0, golden0) && compare_data(output1, golden1);
    std::cout << (passed ? "[Success] asc_deintlv passed." : "[Failed] asc_deintlv failed.") << std::endl;
    aclrtFree(dst0_device);
    aclrtFree(dst1_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
