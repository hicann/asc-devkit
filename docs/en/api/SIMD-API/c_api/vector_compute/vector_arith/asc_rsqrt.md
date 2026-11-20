---
okftype: reference.api
description: "Computes the reciprocal square root element-wise for half or float vectors. Supports two prototypes: computing the first n data elements and high-dimensional slicing with repeat and DataBlock strides. The result is written to the destination vector and requires 32-byte address alignment."
tag:
  - roles: [compute]
  - operations: [reciprocal_sqrt]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The general address overlap constraints must be followed"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Fixes an issue on the 2201 architecture where the synchronous overload of asc_rsqrt incorrectly called the non-synchronous implementation (original entry from the CANN_9_2_0 Release Notes); this page documents the non-synchronous prototype, and this fix ensures its correct behavior under the synchronous wrapper path."
---
# asc_rsqrt

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.696825+00:00 pushedAt=2026-11-16T02:10:58.243Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_arith.h"`.

Computes the reciprocal of the square root of each element. The calculation formula is as follows:

$$
dst_i = 1/(sqrt(src_i))
$$

## Prototype

- Computation on the first n elements

  ```cpp
  __aicore__ inline void asc_rsqrt(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
  __aicore__ inline void asc_rsqrt(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
  ```

- High-dimensional slicing computation

  ```cpp
  __aicore__ inline void asc_rsqrt(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  __aicore__ inline void asc_rsqrt(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description                          |
| :----| :-----|:----------------------------|
| dst | Output | Start address of the destination operand (vector).             |
| src  | Input | Start address of the source operand (vector).              |
| count | Input | Number of elements involved in the computation.                  |
| repeat | Input | Number of iterations.                       |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride | Input | Address stride between different DataBlocks of the source operand within a single iteration.  |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations.  |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
// total_length refers to the total length of the data involved in the computation.
constexpr uint64_t total_length = 128;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_rsqrt(dst, src, total_length);
```
