---
okftype: reference.api
description: "Gathers source vector elements into the destination vector according to the byte offsets given by src_offset. Supports seven types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float, and provides two forms: first n elements and high-dimensional slicing."
tag:
  - roles: [data_movement]
  - operations: [gather]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Must comply with the general address overlap constraints"
version: ">=9.0.0"
version_changes: []
---

# asc_gather

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.900540+00:00 pushedAt=2026-11-16T02:10:58.953Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_gather.h"`.

Collects elements from src into dst according to the offsets specified by src_offset.

## Prototype

- Computation on the first n elements

    ```cpp
    __aicore__ inline void asc_gather(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_gather(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    ```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| src_offset | Input | Address offset corresponding to each element of the source operand. The offset is relative to the start base address of the source operand, in bytes. |
| count | Input | Number of elements involved in the computation. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| repeat | Input | Number of iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The offset values in src_offset must ensure that the element type of src is bit-width aligned.
- The address obtained by adding the offset in src_offset to src must not exceed the range of the Unified Buffer (UB) data size.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the length of data involved in the computation.
__ubuf__ half src[total_length];
__ubuf__ uint32_t src_offset[total_length];
__ubuf__ half dst[total_length];
asc_gather(dst, src, src_offset, total_length);
// src [1, 2, 3, ..., 126, 127, 128], 128 half data elements.
// src_offset [254, 252, 250, ..., 4, 2, 0], the offset of each element in src, based on the start base address of src, in bytes.
// dst [128, 127, 126, ..., 3, 2, 1]
```
