---
okftype: reference.api
description: "Performs cumulative frequency statistics on uint8_t source vector register elements and accumulates the results into a uint16_t destination register. In low mode, counts the number of elements whose value is not greater than n. In high mode, counts the number of elements whose value is not greater than 128 plus n. A mask can be used to filter participating elements."
tag:
  - roles: [compute]
  - operations: [histogram]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
version: ">=9.0.0"
version_changes: []
---
# asc_cumulative_histogram

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.087551+00:00 pushedAt=2026-11-16T02:10:56.727Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_histogram.h"`.

Performs cumulative frequency statistics on the elements in the input vector data register to generate a cumulative histogram. The statistical result is accumulated on top of the existing data in the destination vector data register. A mask can be configured to indicate the elements that participate in the statistics. When a mask bit is 1, the corresponding element participates in the statistics; when it is 0, the element is not counted. The mask does not affect the write behavior of the destination vector data register.

Because the source Vector Data Register `src` has the data type `uint8_t` with a value range of [0, 255], while the destination Vector Data Register `dst` has the element type `uint16_t` and one Vector Length can store 128 `uint16_t` data elements, this API supports the following two modes, where n is the element index of `dst` and n ∈ {0, 1, ..., 127}:

- **Low mode (`BIN0`)**: `dst[n]` accumulates the number of elements in `src` whose values are not greater than `n`, with a cumulative threshold range of [0, 127].
- **High mode (`BIN1`)**: `dst[n]` accumulates the number of elements in `src` whose values are not greater than `128 + n`, with a cumulative threshold range of [128, 255].

As shown in the figure below:

![Cumulative statistics](../../../../figures/asc_cumulative_histogram.png)

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

### Low Mode

```c
__simd_callee__ inline void asc_cumulative_histogram_bin0(vector_uint16_t& dst,
                                                          vector_uint8_t src,
                                                          vector_bool mask)
```

### High Mode

```c
__simd_callee__ inline void asc_cumulative_histogram_bin1(vector_uint16_t& dst,
                                                          vector_uint8_t src,
                                                          vector_bool mask)
```

`src` supports the data type `uint8_t`, and `dst` supports the data type `uint16_t`.

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Input/output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the bit corresponding to an element in mask is 1, the element participates in the computation; when it is 0, the element does not participate in the computation. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).

### Computation Constraints

- `mask` is used to filter the source operands and does not affect the original data or write behavior of positions in `dst` that are not accumulated. When a mask bit is 0, the value at the corresponding position of the source operand `src` is ignored, and the value at the corresponding position of `dst` is the statistical result obtained after ignoring `src` at that position.
- The statistical results in `dst` are accumulated on top of the original data. `dst` must be initialized before the first statistics operation. When this API is called multiple times, the later statistical results are accumulated into the previous statistical results.
- The data type of `dst` is `uint16_t`, and the maximum value of a single statistical result is 65535. The user must ensure that the accumulated statistical results do not exceed this range; otherwise, overflow occurs.

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

constexpr uint32_t SRC_ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DST_ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t SRC_COUNT = 517;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (SRC_COUNT + SRC_ELEMENTS_PER_REGISTER - 1) / SRC_ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_SRC_COUNT = REPEAT_COUNT * SRC_ELEMENTS_PER_REGISTER;
constexpr uint32_t DST_COUNT = REPEAT_COUNT * DST_ELEMENTS_PER_REGISTER;

constexpr uint32_t DATA_BYTES = SRC_COUNT * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = SRC_COUNT;
__simd_vf__ inline void histogram(__ubuf__ uint16_t* low, __ubuf__ uint16_t* high, __ubuf__ uint8_t* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    uint32_t remaining_length_src = SRC_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const uint32_t dst_offset = (offset / SRC_ELEMENTS_PER_REGISTER) * DST_ELEMENTS_PER_REGISTER;
        // The input tail block has only 5 valid elements, but all 128 cumulative thresholds are still generated.
        uint32_t remaining_length_dst = DST_ELEMENTS_PER_REGISTER;
        vector_uint16_t low_reg, high_reg;
        vector_uint8_t src_reg;
        vector_bool src_mask = asc_update_mask_b8(remaining_length_src);
        vector_bool dst_mask = asc_update_mask_b16(remaining_length_dst);
        asc_duplicate_scalar(low_reg, static_cast<uint16_t>(0), dst_mask);
        asc_duplicate_scalar(high_reg, static_cast<uint16_t>(0), dst_mask);
        asc_loadalign(src_reg, src + offset);
        asc_cumulative_histogram_bin0(low_reg, src_reg, src_mask);
        asc_cumulative_histogram_bin1(high_reg, src_reg, src_mask);
        asc_storealign(low + dst_offset, low_reg, dst_mask);
        asc_storealign(high + dst_offset, high_reg, dst_mask);
    }
}

__global__ __vector__ void asc_cumulative_histogram_kernel(__gm__ uint16_t* low, __gm__ uint16_t* high, __gm__ uint8_t* src)
{
    asc_init();
    __ubuf__ uint16_t low_local[DST_COUNT], high_local[DST_COUNT];
    __ubuf__ uint8_t src_local[PADDED_SRC_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, PADDED_SRC_COUNT);
    asc_copy_gm2ub_align(low_local, low, DST_COUNT * sizeof(uint16_t));
    asc_copy_gm2ub_align(high_local, high, DST_COUNT * sizeof(uint16_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<histogram>(low_local, high_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(low, low_local, DST_COUNT * sizeof(uint16_t));
    asc_copy_ub2gm_align(high, high_local, DST_COUNT * sizeof(uint16_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> src(PADDED_SRC_COUNT, 0);
    std::vector<uint16_t> low(DST_COUNT, 0), high(DST_COUNT, 0);
    std::vector<uint16_t> golden_low(DST_COUNT, 0), golden_high(DST_COUNT, 0);
    for (uint32_t i = 0; i < SRC_COUNT; ++i) src[i] = static_cast<uint8_t>(i);
    for (uint32_t offset = 0; offset < SRC_COUNT; offset += SRC_ELEMENTS_PER_REGISTER) {
        const uint32_t dst_offset = (offset / SRC_ELEMENTS_PER_REGISTER) * DST_ELEMENTS_PER_REGISTER;
        const uint32_t chunk_count = (offset + SRC_ELEMENTS_PER_REGISTER < SRC_COUNT) ?
            SRC_ELEMENTS_PER_REGISTER : SRC_COUNT - offset;
        for (uint32_t i = 0; i < DST_ELEMENTS_PER_REGISTER; ++i) {
            for (uint32_t j = 0; j < chunk_count; ++j) {
                golden_low[dst_offset + i] += src[offset + j] <= i ? 1 : 0;
                golden_high[dst_offset + i] += src[offset + j] <= 128 + i ? 1 : 0;
            }
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), PADDED_SRC_COUNT * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* low_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&low_device), (DST_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* high_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&high_device), (DST_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(uint8_t), src.data(), src.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(low_device, low.size() * sizeof(uint16_t), low.data(), low.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(high_device, high.size() * sizeof(uint16_t), high.data(), high.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_cumulative_histogram_kernel<<<1, 0>>>(low_device, high_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(low.data(), low.size() * sizeof(uint16_t), low_device, low.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(high.data(), high.size() * sizeof(uint16_t), high_device, high.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", src);
    print_data("BIN0 output", low);
    print_data("BIN1 output", high);
    print_data("BIN0 Golden", golden_low);
    print_data("BIN1 Golden", golden_high);
    const bool passed = compare_data(low, golden_low) && compare_data(high, golden_high);
    std::cout << (passed ? "[Success] asc_cumulative_histogram_bin0/bin1 passed." : "[Failed] asc_cumulative_histogram_bin0/bin1 failed.") << std::endl;
    aclrtFree(high_device);
    aclrtFree(low_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
