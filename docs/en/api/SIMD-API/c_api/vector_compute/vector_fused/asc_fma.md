---
okftype: reference.api
description: "Multiplies two source vectors element-wise, adds the result to the destination vector, and writes the result back to the destination vector, covering half same-type, half source with float destination, and float same-type, providing two forms: computation on the first n elements and high-dimensional slicing computation."
tag:
  - roles: [compute]
  - operations: [fused_multiply_add]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Must comply with the general address overlap constraints"
version: ">=9.0.0"
version_changes: []
---
# asc_fma

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.866968+00:00 pushedAt=2026-11-16T02:10:58.794Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_fused.h"`.

Multiplies src0 and src1 element-wise and adds the result to dst, then stores the final result into dst. The calculation formula is as follows:
$$
dst_i = (src0_i * src1_i) + dst_i
$$

## Prototype

- Computation on the first n elements

  ```cpp
  __aicore__ inline void asc_fma(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
  __aicore__ inline void asc_fma(__ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
  __aicore__ inline void asc_fma(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
  ```

- High-dimensional slicing computation

  ```cpp
  __aicore__ inline void asc_fma(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_fma(__ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_fma(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  ```

## Parameters

**Table 1** Parameters

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| dst       | Output    | Start address of the destination operand (vector). |
| src0      | Input    | Start address of the source operand (vector). |
| src1      | Input    | Start address of the source operand (vector). |
| count     | Input    | Number of elements involved in the computation.        |
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

- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst, src0, and src1 must be 32-byte aligned.

## Examples

```cpp
__ubuf__ half dst[256];
__ubuf__ half src0[256];
__ubuf__ half src1[256];
asc_fma(dst, src0, src1, 256);
```
