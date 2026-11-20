---
okftype: reference.api
description: "C API that shifts vector elements left by a specified number of bits. Provides two computation forms: computation on the first n elements and high-dimensional slicing computation. Supports int16_t, uint16_t, int32_t, and uint32_t, with pipeline type PIPE_V."
tag:
  - roles: [compute]
  - operations: [bitwise_shift]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address overlap constraints, see the general address overlap constraints section on this page."
version: ">=9.0.0"
version_changes: []
---

# asc_shiftleft

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.917572+00:00 pushedAt=2026-11-16T02:10:59.014Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_logic.h"`.

Shifts all elements left by distance bits. The calculation formula is as follows:
$$
dst_i = src_i << distance
$$

## Prototype

- Computation on the first n elements

    ```cpp
    __aicore__ inline void asc_shiftleft(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_shiftleft(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftleft(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftleft(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftleft(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| distance | Input | Source operand (scalar), indicating the number of bits to shift left. |
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

- distance does not support being set to a negative number.
- The value of distance cannot exceed the bit width of the src data type.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;
uint32_t distance = 1;
__ubuf__ int16_t src[total_length];
__ubuf__ int16_t dst[total_length];
asc_shiftleft(dst, src, distance, total_length);
```
