---
okftype: reference.api
description: "Copies data from Unified Buffer to Unified Buffer within the Unified Buffer. Supports first-n data computation and high-dimensional slicing by repeat and DataBlock stride for four element types: int16_t, uint16_t, int32_t, and uint32_t. Requires 32-byte address alignment."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The general address overlap constraints must be followed"
version: ">=9.0.0"
version_changes: []
---

# asc_copy

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.050287+00:00 pushedAt=2026-11-16T02:10:59.384Z nl=2 -->

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

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

Copies data from Unified Buffer (UB) to UB. Supports high-dimensional slicing.

## Prototype

- Computation on the first n elements

```cpp
__aicore__ inline void asc_copy(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count)

__aicore__ inline void asc_copy(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count)

__aicore__ inline void asc_copy(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)

__aicore__ inline void asc_copy(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count)
```

- High-dimensional slicing computation

```cpp
__aicore__ inline void asc_copy(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint8_t repeat,
                                uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

__aicore__ inline void asc_copy(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint8_t repeat,
                                uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

__aicore__ inline void asc_copy(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint8_t repeat,
                                uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

__aicore__ inline void asc_copy(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint8_t repeat,
                                uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride | Input | Address stride between different DataBlocks of the source operand within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
//total_length indicates the length of data involved in the computation.
constexpr uint32_t total_length = 128;
__ubuf__ int16_t src[total_length];
__ubuf__ int16_t dst[total_length];
asc_copy(dst, src, total_length);
```
