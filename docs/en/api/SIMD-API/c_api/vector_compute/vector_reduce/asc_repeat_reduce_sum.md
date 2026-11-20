---
okftype: reference.api
description: "Sums all elements in each repeat. Internally uses a binary tree for pairwise addition. Each repeat can compute 128 half elements. When the sum exceeds 65504, it is saved as 65504. Supports two prototypes: first-n data computation and high-dimensional slicing."
tag:
  - roles: [compute]
  - operations: [reduction]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The general address overlap constraints must be followed"
version: ">=8.5.0"
version_changes: []
---
# asc_repeat_reduce_sum

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.030676+00:00 pushedAt=2026-11-16T02:10:59.322Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_reduce.h"`.

Sums all elements within each Repeat.

The source operands are added in a binary tree manner through pairwise addition. Taking the summation of 256 half-type data as an example, each Repeat can compute 128 half-type data, which are divided into 2 Repeats for computation. Within each Repeat, pairwise addition is performed in a binary tree manner.

Note that during pairwise addition, when a computation result is greater than 65504, the result is saved as 65504. For example, if the source operands are [60000, 60000, -30000, 100], first 60000 + 60000 overflows and the result is 65504, then -30000 + 100 = -29900 is computed, and finally 65504 - 29900 = 35604 is computed.

## Prototype

- Computation on the first n elements

    ```cpp
    __aicore__ inline void asc_repeat_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_repeat_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|----|-----|-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector).|
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations.|
| dst_repeat_stride | Input | Address stride between adjacent iterations of the destination operand, in units of the length computed by one repeat, as follows:<br>If the output data type is half, the unit is 2 bytes.<br> If the output data type is float, the unit is 4 bytes. |
| src_block_stride | Input | Address stride between different DataBlocks of the source operand within a single iteration.|
| src_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start address of dst must be 2-byte aligned when the input type is half, and 4-byte aligned when the input type is float.
- The start address of src must be 32-byte aligned.

## Examples

```cpp
// Example: Compute the sum of 256 half values in 2 iterations, producing 2 groups of summed values.
constexpr uint32_t total_length = 256;
__ubuf__ half src[total_length];
constexpr uint32_t dst_length = 2;
__ubuf__ half dst[dst_length];

// When using the high-dimensional slicing computation API, set the mask.
asc_set_mask_norm();
asc_set_vector_mask(0xffffffffffffffff, 0xffffffffffffffff);    // Set all elements in each iteration to participate in the computation.

uint8_t repeat = 2;
// The unit is 2 bytes, stored contiguously.
uint16_t dst_repeat_stride = 1;
uint16_t src_block_stride = 1;
uint16_t src_repeat_stride = 8;
asc_repeat_reduce_sum(dst, src, repeat, dst_repeat_stride, src_block_stride, src_repeat_stride);
```

Result example:

```
Input data src: [1, 1, 1, ..., 1]
Output data dst: [128, 128]
```
