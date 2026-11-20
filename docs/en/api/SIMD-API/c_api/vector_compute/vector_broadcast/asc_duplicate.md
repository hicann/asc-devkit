---
okftype: reference.api
description: "Copies and fills a scalar value to the vector destination address. Provides two modes: contiguous computation of the first n data elements and non-contiguous high-dimensional slicing. Covers half, bfloat16, float, and 16-bit and 32-bit integer types."
tag:
  - roles: [data_initialization]
  - operations: [fill]
  - traits: [scalar_operand]
dependencies: []
version: ">=8.5.0"
version_changes: []
---
# asc_duplicate

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.713084+00:00 pushedAt=2026-11-16T02:10:58.306Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_broadcast.h"`.

Performs a vector duplicate operation to copy and fill a scalar value into a vector.

## Prototype

- Computation on the first n elements

    ```cpp
    __aicore__ inline void asc_duplicate(__ubuf__ half* dst, half src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ int16_t* dst, int16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ uint16_t* dst, uint16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ float* dst, float src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ int32_t* dst, int32_t src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ uint32_t* dst, uint32_t src, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_duplicate(__ubuf__ half* dst, half src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ int16_t* dst, int16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ uint16_t* dst, uint16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ float* dst, float src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ int32_t* dst, int32_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ uint32_t* dst, uint32_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Source operand (scalar). |
| count | Input | Number of contiguous elements involved in duplication. |
| repeat | Input  | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of dst must be 32-byte aligned.

## Examples

```cpp
constexpr uint32_t dst_length = 128;
__ubuf__ uint16_t dst[dst_length];
uint16_t val = 18;
asc_duplicate(dst, val, dst_length);
```

Result example:

```
Output data dst: [18 18 18 ... 18]
```
