---
okftype: reference.api
description: "Generates an increasing or decreasing index sequence in a vector data register starting from scalar value. Supports int8_t, int16_t, half, int32_t, and float. Integer results that exceed the representable range are handled by wraparound."
tag:
  - roles: [compute]
  - operations: [fill]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Must comply with the general constraints of Reg vector computation C APIs"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: parameter_changed
    scope: "param:value"
    scope_basis: resolved
    note: "The scalar parameter name in the public header declaration was unified from index to value (include/c_api/reg_compute/reg_vector.h in 9.0.0 uses index, and value is used from 9.1.0 onward). The signature type and semantics are unchanged. This rename is recorded in the CANN_9_1_0 Bug Fixes section."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added function-return-value overloads for asc_arange and asc_arange_descend (returning vector_<dtype>). The reference-output forms are retained. history_probe shows that return-value forms such as vector_int32_t asc_arange(int32_t value) first appeared in impl/c_api/reg_base_impl/reg_index_intf_impl.h:107 in 9.2.0. The CANN_9_2_0 Enhancements section lists this batch of return-value overloads."
---
# asc_arange

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.100323+00:00 pushedAt=2026-11-16T02:10:56.755Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_index.h"`.

Generates ascending or descending indices starting from the input `value` and stores the generated indices in `dst`. [Vector Length (VL)](../../defs/type/data_type_definition.md) indicates the bit width of the Vector Data Register, and `VL_T` indicates the number of elements that the register can store. The algorithm logic is as follows:

```cpp
// Ascending.
{value, value + 1, value + 2, ... value + VL_T - 2, value + VL_T - 1}
// Descending.
{value + VL_T - 1, value + VL_T - 2, value + VL_T - 3, ... value + 1, value}
```

Taking the int16_t data type with a start value of `value=10` as an example:
The ascending indices are {10, 11, 12, 13, ... 135, 136, 137}, and the descending indices are {137, 136, 135, 134, ... 12, 11, 10}.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

### Ascending Mode

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_arange(vector_<dtype>& dst,
                                       <dtype> value)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_arange(<dtype> value)
```

#### Data Types Supported by dtype

`dtype` values are `int8_t`, `int16_t`, `half`, `int32_t`, and `float`.

#### Typical Prototype Example

```c
// Example: Generate an ascending sequence based on a half scalar.
// Output the result through a reference parameter.
__simd_callee__ inline void asc_arange(vector_half& dst,
                                       half value)

// Return the result through the function return value.
__simd_callee__ inline vector_half asc_arange(half value)
```

### Descending Mode

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_arange_descend(vector_<dtype>& dst,
                                               <dtype> value)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype> asc_arange_descend(<dtype> value)
```

#### Data Types Supported by dtype

`dtype` values are `int8_t`, `int16_t`, `half`, `int32_t`, and `float`.

#### Typical Prototype Example

```c
// Example: Generate a descending sequence based on a half scalar.
// Output the result through a reference parameter.
__simd_callee__ inline void asc_arange_descend(vector_half& dst,
                                               half value)

// Return the result through the function return value.
__simd_callee__ inline vector_half asc_arange_descend(half value)
```

## Parameters

### Ascending Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --------- | ----- | ----------------- |
| dst | Output | Destination operand (vector data register). |
| value | Input | Source operand (scalar), whose dtype must be the same as that of dst. Serves as the starting point of the ascending sequence. The 0th element of the sequence equals value, and subsequent elements increase by 1. The value range is the representable range of the dtype. |

### Descending Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --------- | ----- | ----------------- |
| dst | Output | Destination operand (vector data register). |
| value | Input | Source operand (scalar), whose dtype must be the same as that of dst. Serves as the starting point of the descending sequence. The 0th element of the sequence equals `value + VL_T - 1`, and subsequent elements decrease by 1. The value range is the representable range of the dtype. |

For details about vector data registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The prototype that outputs the result through a reference parameter has no return value.
- The function prototype that outputs the result through the function return value returns the computation result. The return value type is the same as the type of the `dst` parameter in the corresponding reference output function prototype (with the reference removed).

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- For integer dtypes (int8_t, int16_t, int32_t), results wrap around when they exceed the representable range of the dtype, without triggering an exception. For example, for int8_t with value=127, the first 128 elements of the sequence are 127, −128, −127, …, -2; with value=−128, the first 128 elements of the sequence are −128, −127, …, −1.

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

template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
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
constexpr int32_t START_VALUE = 10;

__simd_vf__ inline void arange_vf(__ubuf__ int32_t* ascending, __ubuf__ int32_t* descending, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_int32_t ascending_reg;
        vector_int32_t descending_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_arange(ascending_reg, START_VALUE + static_cast<int32_t>(offset));
        asc_arange_descend(descending_reg, START_VALUE + static_cast<int32_t>(offset));
        asc_storealign(ascending + offset, ascending_reg, mask);
        asc_storealign(descending + offset, descending_reg, mask);
    }
}

__global__ __vector__ void asc_arange_kernel(__gm__ int32_t* ascending, __gm__ int32_t* descending)
{
    asc_init();
    __ubuf__ int32_t ascending_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t descending_local[PADDED_ELEMENT_COUNT];
    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(int32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<arange_vf>(ascending_local, descending_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(ascending, ascending_local, TOTAL_LENGTH * sizeof(int32_t));
    asc_copy_ub2gm_align(descending, descending_local, TOTAL_LENGTH * sizeof(int32_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int32_t> input = {START_VALUE};
    std::vector<int32_t> ascending(TOTAL_LENGTH, 0), descending(TOTAL_LENGTH, 0);
    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t* ascending_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&ascending_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* descending_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&descending_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    asc_arange_kernel<<<1, 0>>>(ascending_device, descending_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(ascending.data(), ascending.size() * sizeof(int32_t), ascending_device, ascending.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(descending.data(), descending.size() * sizeof(int32_t), descending_device, descending.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input start", input);
    print_data("Ascending output", ascending);
    print_data("Descending output", descending);
    aclrtFree(ascending_device);
    aclrtFree(descending_device);
    aclrtResetDevice(0);
    aclFinalize();
    return 0;
}
```
