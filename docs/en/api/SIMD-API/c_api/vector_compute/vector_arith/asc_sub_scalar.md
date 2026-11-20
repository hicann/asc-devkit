---
okftype: reference.api
description: "Performs element-wise subtraction between a vector and a scalar, subtracting a scalar of the same type from each source vector element and writing the result to the destination address. Provides two modes: contiguous computation of the first n data elements and non-contiguous high-dimensional slicing."
tag:
  - roles: [compute]
  - operations: [subtract]
  - traits: [elementwise, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: related
    condition: "The main text references the general address overlap constraint accordingly."
version: ">=8.5.0"
version_changes: []
---
# asc_sub_scalar

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.706078+00:00 pushedAt=2026-11-16T02:10:58.277Z nl=2 -->

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

Performs subtraction between a vector and a scalar.

$$
dst_i = src_i - value
$$

## Prototype

- Computes the first n contiguous data elements.

```cpp
__aicore__ inline void asc_sub_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
__aicore__ inline void asc_sub_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
__aicore__ inline void asc_sub_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
__aicore__ inline void asc_sub_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
```

- Computes non-contiguous data.

```cpp
__aicore__ inline void asc_sub_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat,
                                      uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
__aicore__ inline void asc_sub_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat,
                                      uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
__aicore__ inline void asc_sub_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat,
                                      uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
__aicore__ inline void asc_sub_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat,
                                      uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| dst     | Output     | Start address of the destination operand (vector).   |
| src     | Input     | Start address of the source operand (vector).|
| value     | Input     | Source operand (scalar).|
| repeat     | Input    | Number of iterations.        |
| dst_block_stride |Input| Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride |Input| Address stride between different DataBlocks of the source operand within a single iteration. |
| dst_repeat_stride |Input| Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src_repeat_stride |Input| Address stride of the same DataBlock of the source operand between adjacent iterations. |
| count   | Input     | Number of contiguous elements involved in computation.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
// total_length refers to the total length of the data involved in the computation.
constexpr uint64_t total_length = 64;
half scalar_val = 0.0f;
__ubuf__ half dst[total_length];
__ubuf__ half src[total_length];
asc_sub_scalar(dst, src, scalar_val, total_length);
asc_sync();
```
