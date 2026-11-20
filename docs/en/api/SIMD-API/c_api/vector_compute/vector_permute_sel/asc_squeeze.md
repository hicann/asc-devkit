---
okftype: reference.api
description: "C API that compresses a source vector into a shorter vector according to a comparison mask and writes it to the destination address. It provides uint16_t and uint32_t overloads, controls high-dimensional slicing computation through repeat and 6 stride parameters, and has pipeline type PIPE_V."
tag:
  - roles: [compute]
  - operations: [compact]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address overlap constraints, see the general address overlap constraints section on this page."
version: ">=9.1.0"
version_changes: []
---
# asc_squeeze

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.943907+00:00 pushedAt=2026-11-16T02:10:59.121Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_permute_sel.h"`.

Compresses the vector in src0 into a shorter vector by comparing it with the mask src1, and stores the result in dst.

## Prototype

  ```cpp
  __aicore__ inline void asc_squeeze(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_squeeze(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  ```

## Parameters

**Table 1** Parameters

| Parameter       | Input/Output | Description                                |
|:----------| :-----|:----------------------------------|
| dst       | Output | Start address of the destination operand (vector).                   |
| src0, src1 | Input | Start address of the source operand (vector).                        |
| repeat | Input | Number of iterations.                       |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src0_block_stride | Input | Address stride between different DataBlocks of source operand 0 within a single iteration. |
| src1_block_stride | Input | Address stride between different DataBlocks of source operand 1 within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src0_repeat_stride | Input | Address stride of the same DataBlock of source operand 0 between adjacent iterations. |
| src1_repeat_stride | Input | Address stride of the same DataBlock of source operand 1 between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
// total_length refers to the total length of the data involved in the computation.
constexpr uint64_t total_length = 128;
__ubuf__ uint16_t src0[total_length];
__ubuf__ uint16_t src1[total_length];
__ubuf__ uint16_t dst[total_length];
uint8_t repeat = 1;
uint8_t dst_block_stride = 1;
uint8_t src0_block_stride = 1;
uint8_t src1_block_stride = 1;
uint8_t dst_repeat_stride = 8;
uint8_t src0_repeat_stride = 8;
uint8_t src1_repeat_stride = 8;
asc_squeeze(dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```
