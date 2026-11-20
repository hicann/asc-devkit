---
okftype: reference.api
description: "Deprecated synchronous 2D copy interface with transpose, which loads L1 Buffer data into L0B Buffer after transposition, targeting Atlas A3 and Atlas A2 products. The replacement is asc_copy_l12l0b_trans plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [load, transpose]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_2201.md
    kind: replaced_by
    condition: "The alternative non-synchronous interface declared in the body"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "The synchronization part of the alternative solution"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, this interface is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_l12l0b_trans and asc_sync) in the include, and the body text provides the replacement solution accordingly; deprecation does not mean removal, the interface declaration is still retained and still exists in 9.3.0-beta.1, and the page does not set any availability upper limit."
---
# asc_copy_l12l0b_trans_sync (NPU architecture version 2201, deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:26:41.438Z pushedAt=2026-11-18T03:12:16.438Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

**The `asc_copy_l12l0b_trans_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_2201.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

This interface loads 2D data with transpose from the L1 Buffer to the L0B Buffer.

The following examples explain the interface functions and key parameters: in the figures below, an N shape or a Z shape represents a fractal.

- For the uint8_t/int8_t data type, each iteration processes 32 × 32 × 1B data and can process 2 fractals (one fractal is 512B). In each iteration, 2 consecutive 16 × 32 fractals in the source operand are merged into one 32 × 32 block matrix, transpose is performed based on the block matrix, and after transpose it is split into 2 16 × 32 fractals. Different layouts are possible depending on parameters such as the destination operand fractal interval.

The following figure shows an example:

- A total of 3072B of data is processed. Each iteration processes 32 × 32 × 1B data, so 3 iterations are required to complete, repeat_time = 3;
- src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 1 (unit: 32 × 32 × 1B). The unit here is actually the size of the merged block matrix;
- dst_gap = 1, indicating that between adjacent iterations, the interval from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B);
- dst_frac_gap = 0, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512B).

![](../../../figures/asc_copy_l12l0_trans_b8_example1.png)

The following figure shows an example:

- The explanations of repeat_time and src_stride are consistent with the example in the figure above.
- dst_gap = 0, indicating that between adjacent iterations, there is no interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand.
- dst_frac_gap = 2, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).

![](../../../figures/asc_copy_l12l0_trans_b8_example2.png)

- For the half/bfloat16_t data type, each iteration processes 16 × 16 × 2B data and can process 1 fractal (one fractal is 512B). In each iteration, one 16 × 16 fractal in the source operand is transposed.
  - Because each iteration processes 16 × 16 × 2B data, 3 iterations are required to complete, repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the interval between the start addresses of the previous block matrix and the next block matrix of the source operand is 1 (unit: 16 × 16 × 2B);
  - dst_gap = 0, indicating that between adjacent iterations, there is no interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration for the destination operand;
  - In this scenario, because the fractal is the block matrix, each iteration processes one fractal, and there is no intra-iteration fractal interval, so this parameter setting is invalid.

![](../../../figures/asc_copy_l12l0_trans_b16_example.png)

- For the float/int32_t/uint32_t data types, each iteration processes 16 × 16 × 4B data and can process 2 fractals (one fractal is 512B). In each iteration, 2 consecutive 16 × 8 fractals of the source operand are merged into one 16 × 16 block matrix, transposed based on the block matrix, and then split into 2 16 × 8 fractals after transposition. Different layouts can be achieved based on parameters such as the destination operand fractal interval.
As shown in the following example:
  - Because each iteration processes 16 × 16 × 4B data, 3 iterations are required to complete, repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block matrix and the start address of the next block matrix of the source operand is 1 (unit: 16 × 16 × 4B). Here, the unit is actually the size of the merged block matrix;
  - dst_gap = 1, indicating that between adjacent iterations, the interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration for the destination operand is 1 (unit: 512B);
  - dst_frac_gap = 0, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512B).

![](../../../figures/asc_copy_l12l0_trans_b32_example1.png)

As shown in the following figure:
- The explanation of repeat_time and src_stride is consistent with the example in the figure above.
- dst_gap = 0, indicating that across adjacent iterations, there is no interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand.
- dst_frac_gap = 2, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).

![](../../../figures/asc_copy_l12l0_trans_b32_example2.png)

## Prototype

- Synchronous high-dimensional split copy

```cpp
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand.            |
| src | Input    | Source operand.            |
| index_id |Input |ID of the block matrix, indicating which block matrix in the source operand the copy starts from (0 indicates the first block matrix in the source operand). Value range: index_id∈[0, 65535].|
| repeat |Input |Number of iteration repetitions. repeat∈[0, 255].<br> For the uint8_t/int8_t data type, each iteration processes 32 × 32 × 1B data;<br> For the half/bfloat16_t data type, each iteration processes 16 × 16 × 2B data;<br> For the float/int32_t/uint32_t data type, each iteration processes 16 × 16 × 4B data.|
| src_stride | Input|Indicates the interval between the start addresses of the previous and next block matrices of the source operand across adjacent iterations, unit: (1024B). Value range: src_stride∈[0, 65535].<br> For the uint8_t/int8_t data type, the unit is 32 × 32 × 1B; <br> For the half/bfloat16_t data type, the unit is 16 × 16 × 2B;<br> For the float/int32_t/uint32_t data type, the unit is 16 × 16 × 4B; <br> For the int4b_t data type, each iteration processes 16 × 64 × 0.5B data.|
| dst_gap | Input|Indicates the interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand across adjacent iterations. Value range: dst_gap∈[0, 65535]. |
| enable_addr_decrement | Input| Controls the address update mode.<br>false: Increment, each iteration adds src_stride to the previous address.<br>true: Decrement, each iteration subtracts src_stride from the previous address. |
| dst_frac_gap | Input  | The interval between the end address of the previous fractal and the start address of the next fractal after transposition of the destination operand within each iteration, in units of 512B. It is valid only when the data type is float/int32_t/uint32_t/uint8_t/int8_t.|

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- repeat=0 indicates that no copy operation is performed.
- The developer must ensure that the transposed fractals of the destination operand do not overlap.

## Examples

```cpp
// Set the source operand and destination operand. total_length indicates the length of data involved in the computation.
constexpr uint64_t total_length = 512;
__cbuf__ int32_t src[total_length];
__cb__ int32_t dst[total_length];

// Set the configuration for the copy process.
uint8_t n = 64;
uint8_t nBlockSize = 32;
uint8_t repeat = n / nBlockSize;
uint16_t index_id = 0;
uint16_t src_stride = 1;
uint16_t dst_gap = 1;
bool enable_addr_decrement = false;
uint16_t dst_frac_gap = 0;

// Copy process.
asc_copy_l12l0b_trans_sync(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
```
