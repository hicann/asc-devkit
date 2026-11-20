---
okftype: reference.api
description: "Reduction C API that sums adjacent odd-even element pairs. The length is halved after one repeat reduction. It provides first n data contiguous computation and high-dimensional slicing computation forms, supports half and float, and has pipeline type PIPE_V."
tag:
  - roles: [compute]
  - operations: [reduction]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address overlap constraints, see the general address overlap constraints section on this page."
version: ">=9.0.0"
version_changes: []
---
# asc_pair_reduce_sum

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.976065+00:00 pushedAt=2026-11-16T02:10:59.257Z nl=2 -->

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

Performs a reduction operation on the input data to obtain the total sum. Every two adjacent (odd-even) elements are summed. For example, for (a1, a2, a3, a4, a5, a6...), the sum of each adjacent pair is (a1+a2, a3+a4, a5+a6...).

## Prototype

- Computes the first n contiguous data elements.

    ```cpp
    __aicore__ inline void asc_pair_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_pair_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_pair_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_pair_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    ```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| dst     | Output     | Start address of the destination operand (vector). |
| src     | Input     | Start address of the source operand (vector). |
| count   | Input     | Number of contiguous elements involved in computation. |
| repeat | Input | Number of iterations.|
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations.<br>Measured in units of the length after one repeat reduction. After PairReduce completes, the length of one repeat is halved.|
| src_block_stride | Input | Address stride between different DataBlocks of the source operand within a single iteration.|
| src_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
// Example: Compute the sum of adjacent elements for 256 half values in 2 iterations, processing 128 elements per iteration and producing 64 results.
constexpr uint32_t src_length = 256;
constexpr uint32_t dst_length = 128;
__ubuf__ half src[src_length];
__ubuf__ half dst[dst_length];

// When using the high-dimensional slicing computation API, set the mask.
asc_set_mask_norm();
asc_set_vector_mask(0xffffffffffffffff, 0xffffffffffffffff);    // Set all elements in each iteration to participate in the computation.

// Repeat 256B each time, 2 repeats, no interval.
asc_pair_reduce_sum(dst, src, 2, 1, 1, 8);
```

Result example:

```
Input data src: [1 2 3 ... 16 17 ... 32 ... 225... 256]
Output data dst: [3 7 ... 511]
```
