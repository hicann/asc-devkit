---
okftype: reference.api
description: "Gathers data blocks from the source vector into the destination vector according to the DataBlock-granularity address offsets given by src_offset. Supports uint16_t and uint32_t, and provides only the high-dimensional slicing computation form."
tag:
  - roles: [data_movement]
  - operations: [gather]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Must comply with the general address overlap constraints"
version: ">=9.0.0"
version_changes: []
---
# asc_gather_datablock

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.904535+00:00 pushedAt=2026-11-16T02:10:58.963Z nl=2 -->

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

Given an input vector and an address offset vector, gathers the input vector into the destination vector at the granularity of a DataBlock according to the offset addresses.

## Prototype

- High-dimensional slicing computation

  ```cpp
  __aicore__ inline void asc_gather_datablock(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t dst_block_stride, uint8_t repeat)
  __aicore__ inline void asc_gather_datablock(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t dst_block_stride, uint8_t repeat)
  ```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| src_offset | Input | Address offset of each Datablock in the source operand. |
| dst_repeat_stride |Input| Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| dst_block_stride |Input| Address stride between different DataBlocks of the destination operand within a single iteration. |
| repeat |Input| Number of iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The value of src_offset must ensure alignment with the bit width of the src element type.
- The address obtained by adding the offset in src_offset to src must not exceed the range of the Unified Buffer (UB) data size.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the length of data involved in the computation.
__ubuf__ uint32_t src_offset[total_length * sizeof(uint32_t) / 32];
__ubuf__ uint32_t dst[total_length];
__ubuf__ uint32_t src[total_length];
asc_gather_datablock(dst, src, src_offset, 8, 1, 2);
// src [1, 2, 3, ..., 126, 127, 128] 128 uint32_t data elements.
// src_offset [0, 32, 64, ..., 416, 448, 480]
// dst [1, 2, 3, ..., 126, 127, 128]
```
