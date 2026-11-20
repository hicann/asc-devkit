---
okftype: reference.api
description: "Broadcasts each of the 8 numbers in the input vector to fill 8 DataBlocks in the result vector, with each number corresponding to one 32-byte DataBlock. The source and destination must not be the same buffer."
tag:
  - roles: [compute]
  - operations: [broadcast]
  - traits: [datablock_granularity]
dependencies: []
version: ">=8.5.0"
version_changes: []
---
# asc_brcb

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.711077+00:00 pushedAt=2026-11-16T02:10:58.296Z nl=2 -->

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

Given an input vector, each time 8 numbers from the input vector are filled into the 8 datablocks (32 Bytes) of the result vector, with each number corresponding to one datablock.

## Prototype

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_brcb(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_brcb(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride, uint8_t repeat)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| repeat | Input | Number of iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- src and dst cannot be the same memory address.

## Examples

```cpp
constexpr uint32_t src_length = 16;
constexpr uint32_t dst_length = 256;
__ubuf__ uint16_t src[src_length];
__ubuf__ uint16_t dst[dst_length];
asc_brcb(dst, src, 1, 8, 2);
```

Result example:

```
Input data src: [1 2 3 ... 16]
Output data dst: [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 ... 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16]
```
