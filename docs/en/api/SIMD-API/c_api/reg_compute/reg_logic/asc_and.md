---
okftype: reference.api
description: "Performs bitwise AND on two vector data registers or two mask registers according to a mask. The result can be output through the function return value or a destination parameter. The vector form supports six integer types from int8_t to uint32_t. Output bits whose mask bit is 0 are written as 0."
tag:
  - roles: [compute]
  - operations: [bitwise_and]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Must comply with the general constraints of Reg vector computation C APIs"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added function-return-value overloads (returning vector_<dtype> or vector_bool). The reference-parameter output forms are retained. history_probe shows that vector_int32_t asc_and(vector_int32_t src0, vector_int32_t src1, vector_bool mask) first appeared in impl/c_api/reg_base_impl/reg_logic_intf_impl.h:699 in 9.2.0. The CANN_9_2_0 Enhancements section lists this batch of return-value overloads."
---
# asc_and

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.106845+00:00 pushedAt=2026-11-16T02:10:56.770Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_logic.h"`.

Performs a bitwise AND (&) operation on the source operands `src0` and `src1` based on `mask`, and returns the result or writes it to the destination operand `dst`.

- Vector Data Register bitwise AND: Performs a bitwise AND (&) on two Vector Data Registers according to the `dtype` bit width, and the result is a Vector Data Register.

- Mask register bitwise AND: Performs a bitwise AND (&) on two mask registers, and the result is a mask register.

The calculation formula is as follows:

$$
dst_i = src0_i \& src1_i
$$

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

### Vector Data Register Bitwise AND

```c
// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_and(vector_<dtype> src0,
                                              vector_<dtype> src1,
                                              vector_bool mask)

// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_and(vector_<dtype>& dst,
                                    vector_<dtype> src0,
                                    vector_<dtype> src1,
                                    vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`.

#### Typical Prototype Example

```c
// Example: Perform bitwise AND on an int32_t vector data register.
__simd_callee__ inline vector_int32_t asc_and(vector_int32_t src0,
                                              vector_int32_t src1,
                                              vector_bool mask)

__simd_callee__ inline void asc_and(vector_int32_t& dst,
                                    vector_int32_t src0,
                                    vector_int32_t src1,
                                    vector_bool mask)
```

### Mask Register Bitwise AND

```c
// Return the result through the function return value.
__simd_callee__ inline vector_bool asc_and(vector_bool src0,
                                           vector_bool src1,
                                           vector_bool mask)

// Output the result through a reference parameter.
__simd_callee__ inline void asc_and(vector_bool& dst,
                                    vector_bool src0,
                                    vector_bool src1,
                                    vector_bool mask)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register or mask register), used only in the prototype without a return value. |
| src0 | Input | Source operand (vector data register or mask register). |
| src1 | Input | Source operand (vector data register or mask register). |
| mask | Input | Source operand mask (mask register).<br>&bull;When the source operand is a vector data register, elements with a corresponding bit of 1 participate in the computation, and elements with 0 do not. Elements not selected by mask are set to zero in the output.<br>&bull;When the source operand is a mask register, it indicates which bits are valid during the computation. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that returns the result through the function return value returns the bitwise AND result, with a return type consistent with the data types of `src0` and `src1`.
- The prototype that outputs the result through a reference parameter has no return value.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The number of elements participating in the computation is determined by the vector length (VL):
    - Number of elements in vector data register bitwise AND = VL ÷ sizeof(dtype);
    - Number of bits in mask register bitwise AND = VL.
- When a bit of `mask` is 0, the corresponding bit of the computation result is written as 0.

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(int32_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(int32_t);
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// Extend UB capacity to full register blocks to prevent full-register accesses in the final iteration from going out of bounds; the mask still controls the valid range.
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;

__simd_vf__ inline void compute(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int32_t dst_reg;
    vector_int32_t src0_reg;
    vector_int32_t src1_reg;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_and(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_and_kernel(__gm__ int32_t* dst, __gm__ int32_t* src0, __gm__ int32_t* src1)
{
    asc_init();
    __ubuf__ int32_t dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t src1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, TOTAL_LENGTH * sizeof(int32_t));
    asc_copy_gm2ub_align(src1_local, src1, TOTAL_LENGTH * sizeof(int32_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(int32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<compute>(dst_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(int32_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int32_t> src0(TOTAL_LENGTH);
    std::vector<int32_t> src1(TOTAL_LENGTH);
    std::vector<int32_t> output(TOTAL_LENGTH, 0);
    std::vector<int32_t> golden(TOTAL_LENGTH);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src0[i] = static_cast<int32_t>(0x101010U + i);
        src1[i] = static_cast<int32_t>(0x0f0f0000U + i);
        golden[i] = src0[i] & src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(int32_t), src0.data(), src0.size() * sizeof(int32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(int32_t), src1.data(), src1.size() * sizeof(int32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_and_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), dst_device, output.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_and passed." : "[Failed] asc_and failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
