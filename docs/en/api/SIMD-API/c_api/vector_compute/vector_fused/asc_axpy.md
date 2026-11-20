---
okftype: reference.api
description: "Multiplies the source vector element-wise by the scalar value, adds the result to the destination vector, and writes the result back to the destination vector, covering half same-type, half source with float destination, and float same-type, providing two forms: contiguous data and high-dimensional slicing."
tag:
  - roles: [compute]
  - operations: [fused_multiply_add]
  - traits: [scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Must comply with the general address overlap constraints"
version: ">=9.0.0"
version_changes: []
---
# asc_axpy

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.857318+00:00 pushedAt=2026-11-16T02:10:58.768Z nl=2 -->

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

The product of each element in the source operand src and the scalar value is added to the corresponding element in the destination operand dst. The calculation formula is as follows:

$$
dst_i = src_i * value + dst_i
$$

## Prototype

- Computation on contiguous data

  ```cpp
  __aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
  __aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ half* src, half value, uint32_t count)
  __aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
  ```

- High-dimensional slicing computation

  ```cpp
  __aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  __aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  __aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  ```

## Parameters

**Table 1** Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Start address of the destination operand (vector).            |
| src | Input    | Start address of the source operand (vector).            |
| value | Input    | Source operand (scalar).       |
| count     | Input    | Number of contiguous elements involved in computation.      |
| repeat |Input| Number of iterations. |
| dst_block_stride |Input| Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride |Input| Address stride between different DataBlocks of the source operand within a single iteration. |
| dst_repeat_stride |Input| Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src_repeat_stride |Input| Address stride of the same DataBlock of the source operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the length of data involved in the computation.
half value = 10;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_axpy(dst, src, value, total_length);
```
