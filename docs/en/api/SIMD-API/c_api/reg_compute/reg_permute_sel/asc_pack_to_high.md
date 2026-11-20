---
okftype: reference.api
description: "Truncates the low bits of the source register elements and packs them into the high half of the destination register. For a mask source, the low bit of every 2 bits is taken and packed. Both reference output and return value forms are supported."
tag:
  - roles: [compute]
  - operations: [compression]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "Type definitions of the vector data register and mask register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "General constraints for Reg vector computation C APIs"
version: ">=9.2.0"
version_changes: []
---
# asc_pack_to_high

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.183040+00:00 pushedAt=2026-11-16T02:10:56.920Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_permute_sel.h"`.

Selects the low 8 bits (b16) or low 16 bits (b32) of each element in the source operand and writes them into the upper half of the destination operand.
When the source operand is vector_bool, it is compressed bit by bit, and the lower bit of every 2 bits is stored into the upper half of the destination operand.

![](../../../../figures/pack_high_draft.png)

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
// Output the result through a reference parameter (placeholder form).
__simd_callee__ inline void asc_pack_to_high(vector_<dtype_dst>& dst,
                                             vector_<dtype_src> src)

// Return the result through the function return value (placeholder form).
__simd_callee__ inline vector_<dtype_dst> asc_pack_to_high(vector_<dtype_src> src)

// Operand is a mask register.
__simd_callee__ inline void asc_pack_to_high(vector_bool& dst,
                                             vector_bool src)

// Operand is a mask register, and the result is returned through the function return value.
__simd_callee__ inline vector_bool asc_pack_to_high(vector_bool src)
```

### Data Types Supported by dtype

The supported data type pairs for dtype_src and dtype_dst are as follows:

| dtype_src | dtype_dst |
|---|---|
| uint16_t | uint8_t |
| int16_t | uint8_t |
| uint32_t | uint16_t |
| int32_t | uint16_t |

### Typical Prototype Example

```c
// Example: Compress a uint16_t vector data register into uint8_t.
__simd_callee__ inline void asc_pack_to_high(vector_uint8_t& dst,
                                             vector_uint16_t src)

// Example: Compress a uint16_t vector data register into uint8_t, and return the result through the function return value.
__simd_callee__ inline vector_uint8_t asc_pack_to_high(vector_uint16_t src)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --------- | ----- | ---------------- |
| dst | Output | Destination operand (vector data register/mask register). |
| src | Input | Source operand (vector data register/mask register). |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

- The function prototype that outputs the result through a reference parameter has no return value, and the compressed result is written to `dst`.
- The prototype with a return value returns the compressed result as a vector data register or mask register with the same data type as `dst`.

## Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The source and destination operands are vector data registers or mask registers.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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
void PrintData(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

template <typename T>
bool CompareRangeData(const std::vector<T>& actual, const std::vector<T>& expected,
    size_t begin, size_t count, double tolerance = 0.0)
{
    if (begin + count > actual.size() || begin + count > expected.size()) return false;
    for (size_t i = begin; i < begin + count; ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t SRC_COUNT = 133;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (SRC_COUNT + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_SRC_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t DST_ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DST_COUNT = REPEAT_COUNT * DST_ELEMENTS_PER_REGISTER;

constexpr uint32_t DATA_BYTES = SRC_COUNT * sizeof(uint16_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = SRC_COUNT;
__simd_vf__ inline void Pack(__ubuf__ uint8_t* low, __ubuf__ uint8_t* high, __ubuf__ uint16_t* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_bool dst_mask = asc_create_mask_b8(PAT_ALL);
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = SRC_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool src_mask = asc_update_mask_b16(remaining_length);
        const uint32_t dst_offset = (offset / ELEMENTS_PER_REGISTER) * DST_ELEMENTS_PER_REGISTER;
        vector_uint8_t low_reg;
        vector_uint8_t high_reg;
        vector_uint16_t src_reg;
        vector_uint16_t valid_src;
        asc_loadalign(src_reg, src + offset);
        asc_duplicate_scalar(valid_src, static_cast<uint16_t>(0), asc_create_mask_b16(PAT_ALL));
        asc_copy(valid_src, src_reg, src_mask);
        asc_duplicate_scalar(low_reg, static_cast<uint8_t>(0), dst_mask);
        asc_duplicate_scalar(high_reg, static_cast<uint8_t>(0), dst_mask);

        asc_pack_to_high(high_reg, valid_src);

        asc_storealign(high + dst_offset, high_reg, dst_mask);
    }
}

__global__ __vector__ void AscPackToHighKernel(__gm__ uint8_t* low, __gm__ uint8_t* high, __gm__ uint16_t* src)
{
    asc_init();
    __ubuf__ uint8_t low_local[DST_COUNT];
    __ubuf__ uint8_t high_local[DST_COUNT];
    __ubuf__ uint16_t src_local[PADDED_SRC_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, PADDED_SRC_COUNT * sizeof(uint16_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<Pack>(low_local, high_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(low, low_local, DST_COUNT);
    asc_copy_ub2gm_align(high, high_local, DST_COUNT);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> src(PADDED_SRC_COUNT, 0);
    std::vector<uint8_t> low(DST_COUNT, 0);
    std::vector<uint8_t> high(DST_COUNT, 0);
    std::vector<uint8_t> golden_low(DST_COUNT, 0);
    std::vector<uint8_t> golden_high(DST_COUNT, 0);
    for (uint32_t i = 0; i < SRC_COUNT; ++i) {
        src[i] = static_cast<uint16_t>(0x1200U + i);
        const uint32_t dst_offset = (i / ELEMENTS_PER_REGISTER) * DST_ELEMENTS_PER_REGISTER +
            ELEMENTS_PER_REGISTER + i % ELEMENTS_PER_REGISTER;
        golden_high[dst_offset] = static_cast<uint8_t>(src[i]);
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), PADDED_SRC_COUNT * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* low_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&low_device), (DST_COUNT) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* high_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&high_device), (DST_COUNT) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(uint16_t), src.data(), src.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    AscPackToHighKernel<<<1, 0>>>(low_device, high_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(low.data(), low.size() * sizeof(uint8_t), low_device, low.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(high.data(), high.size() * sizeof(uint8_t), high_device, high.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Input", src);
    PrintData("Low output", low);
    PrintData("Low Golden", golden_low);
    PrintData("High output", high);
    PrintData("High Golden", golden_high);
    const bool passed = CompareRangeData(high, golden_high, 0, DST_COUNT);
    std::cout << (passed ? "[Success] asc_pack_to_high passed." : "[Failed] asc_pack_to_high failed.") << std::endl;
    aclrtFree(high_device);
    aclrtFree(low_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
