---
okftype: reference.api
description: "Deprecated API page: asc_copy_l12l0a_trans_sync implements 2D data loading with transpose from L1 Buffer to L0A Buffer on the 2201 architecture, iterating by block matrix and supporting address decrement and intra-iteration fractal intervals. It is recommended to use the non-synchronous asc_copy_l12l0a_trans plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [load, transpose]
  - traits: [post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a_trans.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous API and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Space and alignment requirements follow the storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "The alternative handles the synchronous wait"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header is marked ASC_DEPRECATED(9.2.0) before the asc_copy_l12l0a_trans_sync declaration. The alternative path is the non-synchronous asc_copy_l12l0a_trans plus asc_sync; the interface has not been removed."
---
# asc_copy_l12l0a_trans_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:14:47.548Z pushedAt=2026-11-18T03:12:16.431Z nl=2 -->

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

**The `asc_copy_l12l0a_trans_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12l0a_trans.md) together with the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

This interface loads 2D data with transpose from the L1 Buffer to the L0A Buffer.

The following examples explain the interface functions and key parameters: in the figures below, an N shape or a Z shape represents one fractal.

- For the uint8_t/int8_t data types, each iteration processes 32 × 32 × 1B of data and can handle 2 fractals (one fractal is 512B). In each iteration, 2 consecutive 16 × 32 fractals in the source operand are merged into one 32 × 32 block matrix, transposed based on the block matrix, and then split into 2 16 × 32 fractals after transposition. Different layouts are possible depending on parameters such as the destination operand fractal interval.

The following figure shows an example:

- A total of 3072B of data is processed. Each iteration processes 32 × 32 × 1B of data, so 3 iterations are required to complete, with repeat_time = 3;
- src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 1 (unit: 32 × 32 × 1B). Here the unit is actually the size of the merged block matrix;
- dst_gap = 1, indicating that between adjacent iterations, the interval from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B);
- dst_frac_gap = 0, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512B).

![](../../../figures/asc_copy_l12l0_trans_b8_example1.png)

As shown in the following example:

- The explanations of repeat_time and src_stride are the same as those in the example above.
- dst_gap = 0, indicating that between adjacent iterations, there is no interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand.
- dst_frac_gap = 2, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).

![](../../../figures/asc_copy_l12l0_trans_b8_example2.png)

- For the half/bfloat16_t data type, each iteration processes 16 × 16 × 2B of data and can process 1 fractal (one fractal is 512B). In each iteration, one 16 × 16 fractal in the source operand is transposed.
  - Because each iteration processes 16 × 16 × 2B of data, 3 iterations are required to complete, with repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the interval between the start addresses of the previous block matrix and the next block matrix of the source operand is 1 (unit: 16 × 16 × 2B);
  - dst_gap = 0, indicating that between adjacent iterations, there is no interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand;
  - In this scenario, because the fractal is the block matrix itself, each iteration processes one fractal, and there is no intra-iteration fractal interval, so this parameter is invalid.

![](../../../figures/asc_copy_l12l0_trans_b16_example.png)

- For the float/int32_t/uint32_t data types, each iteration processes 16 × 16 × 4B data and can process 2 fractals (one fractal is 512B). In each iteration, 2 consecutive 16 × 8 fractals of the source operand are merged into one 16 × 16 block matrix, transposed based on the block matrix, and then split into 2 16 × 8 fractals after transposition. Different layouts can be achieved based on parameters such as the destination operand fractal interval.
As shown in the following example:
  - Because each iteration processes 16 × 16 × 4B data, 3 iterations are required to complete, so repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the interval between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 1 (unit: 16 × 16 × 4B). Here, the unit is actually the size of the merged block matrix;
  - dst_gap = 1, indicating that between adjacent iterations, the interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B);
  - dst_frac_gap = 0, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal in the destination operand is 0 (unit: 512B).

![](../../../figures/asc_copy_l12l0_trans_b32_example1.png)

As shown in the following example:
- The explanation of repeat_time and src_stride is the same as in the preceding example.
- dst_gap = 0, indicating that between adjacent iterations, there is no interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand.
- dst_frac_gap = 2, indicating that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal in the destination operand is 2 (unit: 512B).

![](../../../figures/asc_copy_l12l0_trans_b32_example2.png)

## Prototype

- Synchronous high-dimensional split data movement

    ```cpp
    __aicore__ inline void asc_copy_l12l0a_trans_sync(__ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
    __aicore__ inline void asc_copy_l12l0a_trans_sync(__ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
    __aicore__ inline void asc_copy_l12l0a_trans_sync(__ca__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
    __aicore__ inline void asc_copy_l12l0a_trans_sync(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
    __aicore__ inline void asc_copy_l12l0a_trans_sync(__ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
    __aicore__ inline void asc_copy_l12l0a_trans_sync(__ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
    __aicore__ inline void asc_copy_l12l0a_trans_sync(__ca__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ---------------- |
| dst | Output | Destination operand. |
| src | Input | Source operand. |
| index_id | Input | ID of the block matrix. The movement start position is the index of the block matrix in the source operand (0 indicates the first block matrix in the source operand). Value range: index_id∈[0, 65535]. |
| repeat | Input | Number of iteration repetitions. repeat∈[0, 255].<br> For the uint8_t/int8_t data type, each iteration processes 32 × 32 × 1B data;<br> For the half/bfloat16_t data type, each iteration processes 16 × 16 × 2B data;<br> For the float/int32_t/uint32_t data type, each iteration processes 16 × 16 × 4B data. |
| src_stride | Input | Indicates the interval between the start addresses of the previous block matrix and the next block matrix in the source operand between adjacent iterations, unit: (1024B). Value range: src_stride∈[0, 65535].<br> For the uint8_t/int8_t data type, the unit is 32 × 32 × 1B;<br> For the half/bfloat16_t data type, the unit is 16 × 16 × 2B;<br> For the float/int32_t/uint32_t data type, the unit is 16 × 16 × 4B;<br> For the int4b_t data type, each iteration processes 16 × 64 × 0.5B data. |
| dst_gap | Input | Indicates the interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand between adjacent iterations. Value range: dst_gap∈[0, 65535]. |
| enable_addr_decrement | Input | Controls the address update mode.<br>false: Increment. In each iteration, add src_stride to the previous address.<br>true: Decrement. In each iteration, subtract src_stride from the previous address. |
| dst_frac_gap | Input | The interval between the end address of the previous fractal and the start address of the next fractal after transposition in the destination operand within each iteration, in units of 512B. It is valid only when the data type is float/int32_t/uint32_t/uint8_t/int8_t. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- repeat=0 indicates that no data movement operation is performed.
- The developer must ensure that the transposed fractals of the destination operand do not overlap.

## Examples

```cpp
// Set the source operand and destination operand. total_length indicates the data length involved in the computation.
constexpr uint64_t total_length = 512;
__cbuf__ int32_t src[total_length];
__ca__ int32_t dst[total_length];

// Set the configuration during the data movement.
uint8_t n = 64;
uint8_t nBlockSize = 32;
uint8_t repeat = n / nBlockSize;
uint16_t index_id = 0;
uint16_t src_stride = 1;
uint16_t dst_gap = 1;
bool enable_addr_decrement = false;
uint16_t dst_frac_gap = 0;

// Data movement process.
asc_copy_l12l0a_trans_sync(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
```
