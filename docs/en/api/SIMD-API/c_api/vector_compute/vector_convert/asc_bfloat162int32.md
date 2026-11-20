---
okftype: reference.api
description: "Converts bfloat16_t data to int32_t. Provides 5 rounding modes (RINT/ROUND/FLOOR/CEIL/TRUNC). Includes two overloads: computation of the first n data elements and high-dimensional slicing computation."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "General address overlap constraints and 32-byte start address alignment requirements."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location moved from include/c_api/vector_compute/vector_compute.h (9.1.0 and earlier) to include/c_api/vector_compute/compute/vector_convert.h (9.2.0 and later), consistent with the header file path in the description."
---
# asc_bfloat162int32

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.772326+00:00 pushedAt=2026-11-16T02:10:58.495Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_convert.h"`.

Converts data of the bfloat16_t type to int32_t and supports multiple rounding modes:

- RINT rounding mode: round to nearest, ties to even.
- ROUND rounding mode: round half up.
- FLOOR rounding mode: round toward negative infinity.
- CEIL rounding mode: round toward positive infinity.
- TRUNC rounding mode: round toward zero.

## Prototype

- Computation on the first n elements

  ```cpp
  // RINT rounding mode.
  __aicore__ inline void asc_bfloat162int32_rn(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)

  // ROUND rounding mode.
  __aicore__ inline void asc_bfloat162int32_rna(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)

  // FLOOR rounding mode.
  __aicore__ inline void asc_bfloat162int32_rd(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)

  // CEIL rounding mode.
  __aicore__ inline void asc_bfloat162int32_ru(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)

  // TRUNC rounding mode.
  __aicore__ inline void asc_bfloat162int32_rz(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)
  ```

- High-dimensional slicing computation

  ```cpp
  // RINT rounding mode.
  __aicore__ inline void asc_bfloat162int32_rn(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

  // ROUND rounding mode.
  __aicore__ inline void asc_bfloat162int32_rna(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

  // FLOOR rounding mode.
  __aicore__ inline void asc_bfloat162int32_rd(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

  // CEIL rounding mode.
  __aicore__ inline void asc_bfloat162int32_ru(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

  // TRUNC rounding mode.
  __aicore__ inline void asc_bfloat162int32_rz(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :----| :-----| :-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector).|
| count | Input | Number of elements involved in the computation. |
| repeat |Input | Number of iterations. |
| dst_block_stride |Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride |Input | Address stride between different DataBlocks of the source operand within a single iteration. |
| dst_repeat_stride |Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src_repeat_stride |Input | Address stride of the same DataBlock of the source operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint32_t total_length = 256;
__ubuf__ bfloat16_t src[total_length];
__ubuf__ int32_t dst[total_length];
asc_bfloat162int32_rna(dst, src, 128);
```
