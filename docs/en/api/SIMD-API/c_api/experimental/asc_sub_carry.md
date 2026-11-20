# asc_sub_carry

<!-- md-trans-meta sourceCommit=ceded3e2df49735f6a8a8ecb1b10a25352b15e43 translatedAt=2026-11-16T15:36:13.010Z pushedAt=2026-11-18T03:12:16.720Z nl=2 -->

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

This API is an experimental interface and may be adjusted or removed in later versions, with no guarantee of backward compatibility. When compiling, you must define the `CANN_ASC_USE_EXPERIMENTAL` macro and explicitly include the specified experimental header file.

This API performs element-wise subtraction on `src0` and `src1` according to `mask`, and returns both the 32-bit subtraction result and the carry flag through the function return value.

The API supports two modes: without input carry and with input carry. The subtraction is implemented through 32-bit two's complement addition. The carry flag indicates whether the two's complement addition produces a carry out of the most significant bit, and does not indicate signed overflow. The API stores the carry output flag of each element through `result.second` (mask register). Each 32-bit data element corresponds to a group of 4 bits, and the lowest bit of the group stores the carry flag: 1 indicates no borrow in the subtraction, and 0 indicates borrow.

Without input carry, the calculation formula is as follows:

$$
\{carry_i, result_i\} = \{1'b0, src0_i\} + \{1'b0, \sim src1_i\} + 33'b1
$$

With input carry, the two's complement addition uses the input carry (0 or 1) of the corresponding element. When the input carry is 1, `src0 - src1` is computed; when it is 0, `src0 - src1 - 1` is computed. The calculation formula is as follows:

$$
\{carry_i, result_i\} = \{1'b0, src0_i\} + \{1'b0, \sim src1_i\} + \{32'b0, carry\_src_i\}
$$

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Return the result through the function return value (placeholder form), without input carry.
__simd_callee__ inline asc_<dtype>_bool_pair asc_sub_carry(vector_<dtype> src0,
                                                           vector_<dtype> src1,
                                                           vector_bool mask)

// Return the result through the function return value (placeholder form), with input carry.
__simd_callee__ inline asc_<dtype>_bool_pair asc_sub_carry(vector_<dtype> src0,
                                                           vector_<dtype> src1,
                                                           vector_bool carry_src,
                                                           vector_bool mask)
```

### Data Types Supported by dtype

`dtype` can be `int32_t` or `uint32_t`.

### Prototype Example

```c
// Example of returning the result through the function return value: uint32_t subtraction without input carry.
__simd_callee__ inline asc_uint32_t_bool_pair asc_sub_carry(vector_uint32_t src0,
                                                               vector_uint32_t src1,
                                                               vector_bool mask)

// Example of returning the result through the function return value: uint32_t subtraction with input carry.
__simd_callee__ inline asc_uint32_t_bool_pair asc_sub_carry(vector_uint32_t src0,
                                                               vector_uint32_t src1,
                                                               vector_bool carry_src,
                                                               vector_bool mask)
```

## Parameters

**Table 1** Parameter description for the version without input carry

| Parameter | Input/Output | Description |
|---|---|---|
| src0 | Input | Source operand (vector data register). The data type is `int32_t` or `uint32_t`. |
| src1 | Input | Source operand (vector data register). The data type must be the same as that of `src0`. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the computation. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. Unselected elements are set to zero in the output. |

**Table 2** Parameter description for the version with input carry

| Parameter | Input/Output | Description |
|---|---|---|
| src0 | Input | Source operand (vector data register). The data type is `int32_t` or `uint32_t`. |
| src1 | Input | Source operand (vector data register). The data type must be the same as that of `src0`. |
| carry_src | Input | Input carry flag that participates in the two's complement addition (mask register). Each 32-bit element corresponds to the lowest bit of 4 bits. When this bit is 1, `src0 - src1` is computed; when it is 0, `src0 - src1 - 1` is computed. It cannot be used directly as an input borrow. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the computation. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. Unselected elements are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

For the common definition of the returned structure, see [Double Return Value Structure](pair.md).

The function prototype that outputs the result through the function return value returns a common result structure, with the return type `asc_<dtype>_bool_pair`. For example, use the variable `result` to receive the function return value. `result.first` stores the lower 32 bits of the computation result, with the data type consistent with the input vector data register, which is `vector_int32_t` or `vector_uint32_t`. `result.second` has the data type `vector_bool` and stores the carry flag generated by the two's complement addition, where 1 indicates no borrow and 0 indicates borrow. The addition and subtraction carry APIs with the same dtype share this structure type.

## Constraints

- Before using this API, include the header file: `#include "c_api/experimental/reg_arith_exp.h"`.
- Before use, add the compilation option `-DCANN_ASC_USE_EXPERIMENTAL` to enable the experimental feature.
- For the general constraints of the Reg vector computation C API, see [General Constraints](../reg_compute/overview.md#general-constraints).
- `src0` and `src1` must use the same data type.
- The operation output contains the lower 32-bit subtraction result and the carry flag, and is not affected by [asc_set_ctrl](../spr/asc_set_ctrl.md). The hardware does not perform saturating processing on the result.
- The carry output is generated by two's complement addition and does not indicate signed overflow. Without input carry, the two's complement addition is `src0_i + ~src1_i + 1`; with input carry, it is `src0_i + ~src1_i + carry_src_i`, where `~src1_i` is the 32-bit bitwise inversion. After computing the above addition with sufficient bit width, the carry out of the most significant bit is written to the corresponding flag bit in `result.second`.
- The return value `result.first` stores only the lower 32 bits of the above two's complement addition result. The part exceeding 32 bits is reflected by the carry output of `result.second`.

## Examples

The following is a complete example without input carry. The VF function processes 128 elements in batches through a for loop, processing one vector register length of data per iteration and writing the carry mask of that iteration to a separate output area. Each `uint32_t` mask word stores the carry flags of 8 elements. Elements with even indices have no borrow and output a flag of 1; elements with odd indices have borrow and output a flag of 0. Therefore, each golden mask word is `0x01010101U`.

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng -DCANN_ASC_USE_EXPERIMENTAL example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

This example processes 128 `uint32_t` elements, with 64 elements processed per Repeat. The Kernel computes `one_repeat_size` and `repeat_time` through `asc_get_vf_len()`, launches the Vector Function using `asc_vf_call`, and completes two computations and carry flag output within it through a `for` loop.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "c_api/experimental/reg_arith_exp.h"
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
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected)
{
    return actual == expected;
}

constexpr uint32_t ELEMENT_COUNT = 128;
constexpr uint32_t CARRY_WORDS = ELEMENT_COUNT / 8;

__simd_vf__ inline void sub_carry_vf(__ubuf__ uint32_t* dst,
                                     __ubuf__ uint32_t* carry_out,
                                     __ubuf__ uint32_t* src0,
                                     __ubuf__ uint32_t* src1,
                                     uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint32_t src0_reg;
    vector_uint32_t src1_reg;
    uint32_t count = ELEMENT_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b32(count);
        asc_loadalign(src0_reg, src0 + i * one_repeat_size);
        asc_loadalign(src1_reg, src1 + i * one_repeat_size);
        auto result = asc_sub_carry(src0_reg, src1_reg, mask);
        asc_storealign(dst + i * one_repeat_size, result.first, mask);
        asc_storealign(carry_out + i * (one_repeat_size / 8), result.second);
    }
}

__global__ __vector__ void asc_sub_carry_kernel(__gm__ uint32_t* dst,
                                                __gm__ uint32_t* carry,
                                                __gm__ uint32_t* src0,
                                                __gm__ uint32_t* src1)
{
    asc_init();
    __ubuf__ uint32_t dst_local[ELEMENT_COUNT];
    __ubuf__ uint32_t carry_local[CARRY_WORDS];
    __ubuf__ uint32_t src0_local[ELEMENT_COUNT];
    __ubuf__ uint32_t src1_local[ELEMENT_COUNT];

    asc_copy_gm2ub_align(src0_local, src0, ELEMENT_COUNT * sizeof(uint32_t));
    asc_copy_gm2ub_align(src1_local, src1, ELEMENT_COUNT * sizeof(uint32_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    uint16_t one_repeat_size = asc_get_vf_len() / sizeof(uint32_t);
    uint16_t repeat_time = ELEMENT_COUNT / one_repeat_size;
    asc_vf_call<sub_carry_vf>(dst_local, carry_local, src0_local, src1_local,
                            one_repeat_size, repeat_time);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, ELEMENT_COUNT * sizeof(uint32_t));
    asc_copy_ub2gm_align(carry, carry_local, CARRY_WORDS * sizeof(uint32_t));
    asc_sync();
}

} // namespace

int main()
{
    std::vector<uint32_t> src0(ELEMENT_COUNT);
    std::vector<uint32_t> src1(ELEMENT_COUNT);
    std::vector<uint32_t> output(ELEMENT_COUNT, 0);
    std::vector<uint32_t> carry(CARRY_WORDS, 0);
    std::vector<uint32_t> golden(ELEMENT_COUNT, 0);
    std::vector<uint32_t> golden_carry(CARRY_WORDS, 0);

    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        src0[i] = i % 2 == 0 ? i + 2 : i;
        src1[i] = i % 2 == 0 ? i : i + 2;
        const uint64_t sum = static_cast<uint64_t>(src0[i])
                           + static_cast<uint32_t>(~src1[i]) + 1;
        golden[i] = static_cast<uint32_t>(sum);
        golden_carry[i / 8] |= static_cast<uint32_t>(sum >> 32) << (4 * (i % 8));
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* carry_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&carry_device), (CARRY_WORDS) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(uint32_t), src0.data(), src0.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(uint32_t), src1.data(), src1.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_sub_carry_kernel<<<1, 0>>>(dst_device, carry_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint32_t), dst_device, output.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(carry.data(), carry.size() * sizeof(uint32_t), carry_device, carry.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);

    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Carry words", carry);
    print_data("Golden output", golden);
    print_data("Golden carry words", golden_carry);
    const bool passed = compare_data(output, golden) && compare_data(carry, golden_carry);
    std::cout << (passed ? "[Success] asc_sub_carry passed." : "[Failed] asc_sub_carry failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(carry_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
