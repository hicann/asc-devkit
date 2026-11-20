---
okftype: reference.api
description: "Moves multidimensional data from GM to UB, supporting up to 5 dimensions. Each dimension can have its stride configured independently and padding set through companion APIs, supporting either constant fill or nearest-value fill. The total address range must not exceed 40 bits."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [high_dimensional, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_dci.md
    kind: requires
    condition: "Flush the NDDMA DataCache first when multiple cores may read the same GM address"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_loop_stride.md
    kind: requires
    condition: "The source/destination stride of each dimension must be configured in advance"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_count.md
    kind: requires
    condition: "The left and right padding element counts for loop1 through loop4 dimensions must be configured in advance"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_value.md
    kind: requires
    condition: "The fill value must be set in advance for constant fill"
version: ">=9.0.0"
version_changes: []
---
# asc_ndim_copy_gm2ub

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.113526+00:00 pushedAt=2026-11-16T02:10:59.540Z nl=2 -->

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

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

This API is used to copy multidimensional data from Global Memory (GM) to Unified Buffer (UB). Compared with basic data movement APIs, this API can flexibly configure the dimension information of the data to be copied.

This API supports up to 5 dimensions. Each dimension can be configured with Stride independently, and on-the-fly Padding is supported. Related configuration parameters must be passed through this API and the supporting configuration APIs.

**Table 1** Stride configuration parameters and their input APIs, where `i` indicates the `i`-th dimension and can take values in [0, 4]

| Parameter | Input API | Description |
|---|---|---|
| loopi_size | This API | Number of elements processed within each dimension, excluding padding elements.<br>Unit: number of elements. Value range: [1, 2^20-1]. |
| loopi_src_stride | [asc_set_ndim_loopi_stride](asc_set_ndim_loop_stride.md) | Interval between the current source operand element and the next element within each dimension.<br>Unit: number of elements. Value range: [0, 2^40-1]. |
| loopi_dst_stride | [asc_set_ndim_loopi_stride](asc_set_ndim_loop_stride.md) | Interval between the current destination operand element and the next element within each dimension.<br>Unit: number of elements. Value range: [0, 2^20-1]. |

**Table 2** Padding configuration parameters and their input APIs, where `i` indicates the `i`-th dimension and can take values in [0, 4]

| Parameter | Input API | Description |
|---|---|---|
| padding_mode | This API | Padding method, of type `bool`.<br>&bull; `true`: Uses constant padding, where all padding values are the fixed value `pad_value`.<br>&bull; `false`: Uses nearest-value padding, where the left and right padding values are the leftmost or rightmost value of the current dimension. |
| pad_value | [asc_set_ndim_pad_value](asc_set_ndim_pad_value.md) | When `padding_mode` is `true`, this value is used as the fixed padding value; when `padding_mode` is `false`, this value does not take effect. |
| loopi_lp_count | <br>&bull; Dimension 0: this API <br>&bull; Dimensions 1 to 4: [asc_set_ndim_pad_count](asc_set_ndim_pad_count.md) | Number of elements to be padded on the left of each dimension.<br>Unit: number of elements. Value range: [0, 255]. |
| loopi_rp_count | <br>&bull; Dimension 0: this API <br>&bull; Dimensions 1 to 4: [asc_set_ndim_pad_count](asc_set_ndim_pad_count.md) | Number of elements to be padded on the right of each dimension.<br>Unit: number of elements. Value range: [0, 255]. |

The following uses a 2D example to introduce several typical usage scenarios.

- 2D Padding scenario, using the fixed-value padding method
![](../../../figures/asc_ndim_copy_gm2ub_2d_padding_constant.png "2D Padding scenario using constant value padding")

- 2D Padding scenario, using the nearest-value padding method
![](../../../figures/asc_ndim_copy_gm2ub_2d_padding_nearest.png "2D Padding scenario using constant value padding")

- 2D Transpose scenario
![](../../../figures/asc_ndim_copy_gm2ub_2d_transpose.png "2D Padding scenario using constant value padding")

- 2D Slice scenario
![](../../../figures/asc_ndim_copy_gm2ub_2d_slice.png "2D Padding scenario using constant value padding")

- 2D Broadcast scenario
![](../../../figures/asc_ndim_copy_gm2ub_2d_broadcast.png "2D Padding scenario using constant value padding")

This API takes effect only on AIV.

## Prototype

```c
// Placeholder form.
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ <dtype>* dst,
                                           __gm__ <dtype>* src,
                                           uint32_t loop0_size,
                                           uint32_t loop1_size,
                                           uint32_t loop2_size,
                                           uint32_t loop3_size,
                                           uint32_t loop4_size,
                                           uint8_t loop0_lp_count,
                                           uint8_t loop0_rp_count,
                                           bool padding_mode,
                                           asc_load_l2_cache_mode l2_cache_mode)
```

### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Typical Prototype Example

```c
// Example: source and destination data type is half.
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ half* dst,
                                           __gm__ half* src,
                                           uint32_t loop0_size,
                                           uint32_t loop1_size,
                                           uint32_t loop2_size,
                                           uint32_t loop3_size,
                                           uint32_t loop4_size,
                                           uint8_t loop0_lp_count,
                                           uint8_t loop0_rp_count,
                                           bool padding_mode,
                                           asc_load_l2_cache_mode l2_cache_mode)
```

## Parameters

**Table 3** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination UB, pointing to the start position of the data after padding. Must be 1-byte aligned. |
| src | Input | Start address of the source GM. Must be aligned to dtype bytes (b8 type requires 1-byte alignment, b16 type requires 2-byte alignment, and b32 type requires 4-byte alignment). |
| loop0_size | Input | Number of elements processed in dimension 0, excluding padding elements.<br>Unit: number of elements. Value range: [1, $2^{20}−1$]. |
| loop1_size | Input | Number of elements processed in dimension 1, excluding padding elements.<br>Unit: number of elements. Value range: [1, $2^{20}−1$]. |
| loop2_size | Input | Number of elements processed in dimension 2, excluding padding elements.<br>Unit: number of elements. Value range: [1, $2^{20}−1$]. |
| loop3_size | Input | Number of elements processed in dimension 3, excluding padding elements.<br>Unit: number of elements. Value range: [1, $2^{20}−1$]. |
| loop4_size | Input | Number of elements processed in dimension 4, excluding padding elements.<br>The unit is the number of elements. The value range is [1, $2^{20}−1$]. |
| loop0_lp_count | Input | Number of elements to be padded on the left side of dimension 0.<br>The unit is the number of elements. The value range is [0, 255]. |
| loop0_rp_count | Input | Number of elements to be padded on the right side of dimension 0.<br>The unit is the number of elements. The value range is [0, 255]. |
| padding_mode | Input | Padding method.<br>&bull; `true`: Uses constant padding, where all padding values are the fixed value `pad_value`.<br>&bull; `false`: Uses nearest-value padding, where the left and right padding values select the leftmost or rightmost value of the current dimension. |
| l2_cache_mode | Input | Enum value of the [asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md) type, used to configure the management policy of data in L2 Cache. |
| cache_mode | Input | Deprecated L2 Cache management policy configuration parameter. Use `l2_cache_mode` instead. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- Calling this API on a non-AIV returns directly.
- For the capacity and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If there is a data dependency between this instruction and other instructions, you must insert a [synchronization instruction](../sync/overview.md#synchronization-control-overview) to ensure that the instructions execute serially and prevent abnormal data from occurring.
- In multi-core scenarios, if multiple AI Cores may read the same GM address and the data at that GM address may be updated by other cores, call [asc_ndim_copy_dci](asc_ndim_copy_dci.md) before calling this API to flush the NDDMA DataCache and ensure data consistency among multiple cores.
- The address range width of all data that a single instruction can obtain must not exceed 40 bits (1 TB). That is, the i-dimension size of the source operand is (loopi_lp_size + loopi_size + loopi_rp_size - 1) * loop0_src_stride, and the i-dimension size of the destination operand is (loopi_lp_size + loopi_size + loopi_rp_size - 1) * loop0_dst_stride. The sum of the sizes of all dimensions must not exceed 2^40 bits.
- When loopi_dst_stride of each dimension is an ascending sequence, the address spaces between different loops must not interleave or overlap.
- The source/destination stride of each dimension must be preconfigured through the companion API ([asc_set_ndim_loop_stride](asc_set_ndim_loop_stride.md)), in elements. The destination stride of each dimension must ensure no write overlap: if the actual number of iterations of a dimension is greater than 1, the destination stride of that dimension must be greater than the cumulative number of bytes written by lower dimensions; otherwise, a write overlap exception is triggered.
- The left and right padding element counts of loop1 to loop4 dimensions must be preconfigured through the companion API ([asc_set_ndim_pad_count](asc_set_ndim_pad_count.md)). This API does not set padding for these four dimensions.

## Examples

2D Padding scenario, using the fixed-value padding method
![](../../../figures/asc_ndim_copy_gm2ub_2d_padding_constant.png "2D Padding scenario using constant value padding")

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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
#include "acl/acl.h"

namespace {

constexpr uint32_t INPUT_BYTES = 12;
constexpr uint32_t OUTPUT_COLUMNS = 6;
constexpr uint32_t OUTPUT_BYTES = OUTPUT_COLUMNS * 9;
constexpr uint8_t PAD_VALUE = 88;

__global__ __vector__ void asc_ndim_copy_gm2ub_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[64];
    // Each input row contains 4 bytes. Padding 2 bytes on the left and 3 bytes on the right extends each output row to 9 bytes.
    asc_set_ndim_loop0_stride(1, 1);
    asc_set_ndim_loop1_stride(6, 3);
    asc_set_ndim_loop2_stride(0, 0);
    asc_set_ndim_loop3_stride(0, 0);
    asc_set_ndim_loop4_stride(0, 0);
    asc_set_ndim_pad_value(PAD_VALUE);
    asc_ndim_pad_count_config config;
    config.loop1_lp_count = 2;
    config.loop1_rp_count = 3;
    asc_set_ndim_pad_count(config);
    asc_ndim_copy_gm2ub(local, input, 3, 4, 1, 1, 1, 2, 1, true,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    // PIPE_MTE3 waits for the PIPE_MTE2 load to complete before copying UB data to GM.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, local, OUTPUT_BYTES);
    asc_sync();
}

void print_data(const char* name, const std::vector<uint8_t>& data)
{
    std::cout << name << ":";
    const uint32_t count = data.size() < 32 ? data.size() : 32;
    for (uint32_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, PAD_VALUE);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i);
    for (uint32_t row = 0; row < 4; ++row) {
        for (uint32_t column = 0; column < 3; ++column) {
            golden[(row + 2) * OUTPUT_COLUMNS + column + 2] = input[row * 3 + column];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_ndim_copy_gm2ub_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_ndim_copy_gm2ub passed." : "[Failed] asc_ndim_copy_gm2ub failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
