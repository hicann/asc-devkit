---
okftype: reference.api
description: "C API that selects data element by element from two source vectors based on a comparison mask: mask bit 1 takes src0 and 0 takes src1. It must be used with asc_set_cmp_mask, provides first n data and high-dimensional slicing forms, supports half and float, and has pipeline type PIPE_V."
tag:
  - roles: [compute]
  - operations: [selection]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address overlap constraints, see the general address overlap constraints section on this page."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_set_cmp_mask.md
    kind: related
    condition: "The selection rule is determined by the comparison mask set by this API"
version: ">=8.5.0"
version_changes: []
---
# asc_select

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.935807+00:00 pushedAt=2026-11-16T02:10:59.099Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_permute_sel.h"`.

Performs a vector selection operation. Given two source operands src0 and src1, it selects elements based on a condition to obtain the destination operand dst. This API is used together with [asc_set_cmp_mask(sel_mask)](../vector_compare/asc_set_cmp_mask.md). The rule is as follows: when a bit of sel_mask is 1, the element is selected from src0; when the bit is 0, the element is selected from src1.

## Prototype

- Computation on the first n elements

    ```cpp
    __aicore__ inline void asc_select(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_select(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_select(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_select(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    ```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| :--- | :--- | :--- |
| dst  | Output  | Start address of the destination operand (vector). |
| src0 | Input  | Start address of source operand 0 (vector). |
| src1 | Input  | Start address of source operand 1 (vector). |
| count | Input    | Number of elements involved in the computation. |
| repeat | Input    | Number of iterations. |
| dst_block_stride | Input    | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src0_block_stride | Input    | Address stride between different DataBlocks of source operand 0 within a single iteration. |
| src1_block_stride | Input    | Address stride between different DataBlocks of source operand 1 within a single iteration. |
| dst_repeat_stride | Input    | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src0_repeat_stride | Input    | Address stride of the same DataBlock of source operand 0 between adjacent iterations. |
| src1_repeat_stride | Input    | Address stride of the same DataBlock of source operand 1 between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- This API must be used together with [asc_set_cmp_mask](../vector_compare/asc_set_cmp_mask.md).
- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint32_t total_length = 256;
constexpr uint32_t sel_length = 8;
__ubuf__ uint8_t sel_mask[sel_length];
__ubuf__ float dst[total_length];
__ubuf__ float src0[total_length];
__ubuf__ float src1[total_length];
asc_set_cmp_mask(sel_mask);
asc_sync_pipe(PIPE_V);
asc_select(dst, src0, src1, total_length);
```
