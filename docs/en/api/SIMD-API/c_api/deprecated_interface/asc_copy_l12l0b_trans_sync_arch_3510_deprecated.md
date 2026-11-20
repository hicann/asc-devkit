---
okftype: reference.api
description: "Deprecated synchronous 2D data movement interface with transpose, targeting 4/8/16/32-bit data types on Ascend 950PR&950DT, which transposes L1 data and writes it to L0B. The replacement is asc_copy_l12l0b_trans plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [load, transpose]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md
    kind: replaced_by
    condition: "The replacement non-synchronous interface declared in the body"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "The synchronization part of the replacement"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, the 3510 group declaration in include is also marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_l12l0b_trans and asc_sync); the interface declaration is retained, indicating deprecation rather than removal, and the page does not set an availability upper limit."
---
# asc_copy_l12l0b_trans_sync (NPU architecture version 3510, deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:28:09.141Z pushedAt=2026-11-18T03:12:16.440Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l12l0b_trans_sync` API is deprecated. Use the [corresponding non-synchronous API](../cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

This API loads 2D-format data with transpose from the L1 Buffer to the L0B Buffer.

The following examples explain the API functionality and key parameters: in the figures below, an N shape or a Z shape represents one fractal.

- For the uint8_t/int8_t data type, each iteration processes 32 × 32 × 1B of data and can handle 2 fractals (one fractal is 512B). In each iteration, 2 consecutive 16 × 32 fractals in the source operand are merged into one 32 × 32 block matrix, transposed based on the block matrix, and then split into 2 16 × 32 fractals after transposition. Different layouts are possible depending on parameters such as the destination operand fractal gap.

As shown in the following example:

- A total of 3072B of data is processed. Each iteration processes 32 × 32 × 1B of data, requiring 3 iterations to complete, with repeat_time = 3;
- src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 1 (unit: 32 × 32 × 1B). The unit here is actually the size of the merged block matrix;
- dst_gap = 1, indicating that between adjacent iterations, the interval from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B);
- dst_frac_gap = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512B).
![](../../../figures/asc_copy_l12l0_trans_b8_example1.png)

The following figure shows an example:

- The explanation of repeat_time and src_stride is consistent with the example in the figure above.
- dst_gap = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand.
- dst_frac_gap = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).
![](../../../figures/asc_copy_l12l0_trans_b8_example2.png)

- For the half/bfloat16_t data type, each iteration processes 16 × 16 × 2B data and can process 1 fractal (one fractal is 512B). In each iteration, one 16 × 16 fractal in the source operand is transposed.
  - Because each iteration processes 16 × 16 × 2B data, 3 iterations are required to complete, repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the gap between the start addresses of the previous block matrix and the next block matrix of the source operand is 1 (unit: 16 × 16 × 2B);
  - dst_gap = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand;
  - In this scenario, because the fractal is exactly the block matrix, each iteration processes one fractal, and there is no gap between fractals within an iteration, so this parameter is ineffective.
![](../../../figures/asc_copy_l12l0_trans_b16_example.png)

- For the float/int32_t/uint32_t data types, each iteration processes 16 × 16 × 4B of data and can process 2 fractals (one fractal is 512B). In each iteration, 2 consecutive 16 × 8 fractals of the source operand are merged into one 16 × 16 block matrix, transposed based on the block matrix, and then split into 2 16 × 8 fractals after transposition. Different layouts can be achieved based on parameters such as the destination operand fractal gap.
The following figure shows an example:
  - Because each iteration processes 16 × 16 × 4B of data, 3 iterations are required to complete, so repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 1 (unit: 16 × 16 × 4B). Here, the unit is actually the size of the merged block matrix;
  - dst_gap = 1, indicating that between adjacent iterations, the gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B);
  - dst_frac_gap = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 0 (unit: 512B).
![](../../../figures/asc_copy_l12l0_trans_b32_example1.png)

As shown in the following example:
- The explanations of repeat_time and src_stride are consistent with the example above.
- dst_gap = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand.
- dst_frac_gap = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 2 (unit: 512B).
![](../../../figures/asc_copy_l12l0_trans_b32_example2.png)

- For the int4b\_t data type, each iteration processes 64\*64\*0.5B data and can handle 4 fractals (one fractal is 512B). In each iteration, 4 consecutive 16\*64 fractals in the source operand are merged into 1 64\*64 block matrix, transposed based on the block matrix, and then split into 4 16\*64 fractals after transposition. Different layouts can be achieved based on parameters such as the destination operand fractal gap.

    The int4b\_t data type requires two numbers to be combined into one int8\_t or uint8\_t number. The combination rules are as follows:

    ![](../../../figures/int4b_example.png)

  As shown in the following figure:
  - A total of 6144B of data needs to be processed. Each iteration processes 64\*64\*0.5B data, requiring 3 iterations to complete, repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block matrix and the start address of the next block matrix of the source operand is 1 (unit: 64\*64\*0.5B), where the unit is actually the size of the concatenated block matrix;
  - dst_gap = 1, indicating that between adjacent iterations, the interval from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration of the destination operand is 1 (unit: 512B);
  - dst_frac_gap = 0, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512B).

  ![](../../../figures/asc_copy_l12l0_trans_b4_example1.png)

  As shown in the following example:

  - The explanations of repeat_time and src_stride are consistent with the example in the figure above.
  - dst_gap = 0, indicating that between adjacent iterations, there is no interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand.
  - dst_frac_gap = 2, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).

  ![](../../../figures/asc_copy_l12l0_trans_b4_example2.png)

## Prototype

```c
// Placeholder form.
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ <dtype>* dst,
                                                  __cbuf__ <dtype>* src,
                                                  uint16_t index_id,
                                                  uint8_t repeat,
                                                  uint16_t src_stride,
                                                  uint16_t dst_gap,
                                                  uint16_t dst_frac_gap,
                                                  uint16_t src_frac_gap)
```

### Data Types Supported by dtype

`dtype` values: `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|:-----------------| :--- |:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dst | Output | Destination L0B Buffer address. |
| src | Input | Source L1 Buffer address. |
| index_id | Input | Matrix ID, indicating the index of the fractal in the source operand from which the copy starts. Value range: [0, 65535]. |
| repeat | Input | Number of iterations.<br> For data types with a bit width of 4, each iteration processes 4 fractals, and each fractal is 16\*64\*0.5B data;<br> For data types with a bit width of 8, each iteration processes 2 fractals, and each fractal is 16\*32\*1B data;<br> For data types with a bit width of 16, each iteration processes 1 fractal, and each fractal is 16\*16\*2B data; For data types with a bit width of 32, each iteration processes 4 fractals, and each fractal is 16\*8\*4B data. Value range: [1, 255]. |
| src_stride | Input | Interval between the start addresses of the previous fractal and the next fractal of the source operand between adjacent iterations. The unit is a single fractal of 512B. Value range: [0, 65535]. |
| dst_gap | Input | Interval between the end address of the previous fractal and the start address of the next fractal of the destination operand between adjacent iterations. The unit is a single fractal of 512B. Value range: [0, 65535]. |
| dst_frac_gap | Input | Interval between the end address of the previous fractal and the start address of the next fractal of the destination operand before transpose within each iteration. The unit is 512B. Valid only when the data bit width is 4, 8, or 32. |
| src_frac_gap | Input | Interval between the end address of the previous fractal and the start address of the next fractal of the source operand before transpose within each iteration. The unit is 512B. Valid only when the data bit width is 4, 8, or 32. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- repeat=0 indicates that no copy operation is performed.
- The developer must ensure that the fractals of the destination operand do not overlap after transpose.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
// Set the source operand and destination operand. total_length indicates the data length involved in the computation.
constexpr uint64_t total_length = 512;
__cbuf__ int32_t src[total_length];
__cb__ int32_t dst[total_length];

// Set the configuration during the copy process.
uint8_t n = 64;
uint8_t nBlockSize = 32;
uint8_t repeat = n / nBlockSize;
uint16_t index_id = 0;
uint16_t src_stride = 1;
uint16_t dst_gap = 1;
uint16_t dst_frac_gap = 15;
uint16_t src_frac_gap = 8;

asc_copy_l12l0b_trans_sync(dst, src, index_id, repeat, src_stride, dst_gap, dst_frac_gap, src_frac_gap);
```
