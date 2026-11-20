---
okftype: reference.api
description: "Computes the minimum value and the index of its first occurrence for elements in each repeat. One repeat processes 256 bytes. Provides four output combinations: index_value, value_index, only_value, and only_index. Supports two prototypes: first-n data computation and high-dimensional slicing."
tag:
  - roles: [compute]
  - operations: [minimum, reduction]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The general address overlap constraints must be followed"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_get_reduce_min_cnt.md
    kind: related
    condition: "You can continue to use this API to obtain the minimum value and the index of its first occurrence in all repeats."
version: ">=8.5.0"
version_changes: []
---
# asc_repeat_reduce_min

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.026664+00:00 pushedAt=2026-11-16T02:10:59.311Z nl=2 -->

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

Computes the minimum value of all elements in each Repeat, as well as the index of the first minimum element within the current Repeat. Each Repeat computation processes 256 bytes.

index_value API: Indicates that the index is in the lower half, and the return result storage order is [index, value]. When the input data type is half, the index data type is uint16; when the input data type is float, the index data type is uint32.
value_index API: Indicates that the value is in the lower half, and the return result storage order is [value, index]. When the input data type is half, the index data type is uint16; when the input data type is float, the index data type is uint32.
only_value API: Indicates that only the extreme value is returned, and the return result storage order is [value].
only_index API: Indicates that only the index of the extreme value is returned, and the return result storage order is [index]. The index data type is fixed to uint32.

After executing this API, you can continue to execute [asc_get_reduce_min_cnt](asc_get_reduce_min_cnt.md) to obtain the minimum value across all repeats and the index of the first minimum value.

## Prototype

- Computation on the first n elements

    ```cpp
    __aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|----|-----|-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations.|
| dst_repeat_stride | Input | Address stride between adjacent iterations of the destination operand, in units of the length computed by one repeat, as follows:<br>When both the extreme value and index are returned, if the output data type is T, the unit is sizeof(T)*2.<br>When only the extreme value is returned and the input type is T, the unit is sizeof(T).<br>When only the index is returned, the unit is 4 bytes (sizeof(uint32)). |
| src_block_stride | Input | Address stride between different DataBlocks of the source operand within a single iteration.|
| src_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The index data in the output is stored according to the data type of dst. For example, the index data of a float output should be uint32, so when reading it, the float must be reinterpreted as uint32.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start address of dst requires 4-byte alignment when the input type is half, and 8-byte alignment when the input type is float.
- The start address of src must be 32-byte aligned.

## Examples

```cpp
// Example: Compute the minimum value and index of 256 half values in 2 iterations, producing 2 groups of value_index.
constexpr uint32_t total_length = 256;
__ubuf__ half src[total_length];
constexpr uint32_t dst_length = 4;
__ubuf__ half dst[dst_length];

// When using the high-dimensional slicing computation API, set the mask.
asc_set_mask_norm();
asc_set_vector_mask(0xffffffffffffffff, 0xffffffffffffffff);    // Set all elements in each iteration to participate in the computation.

uint8_t repeat = 2;
// The unit is sizeof(half)*2 = 4 bytes, stored contiguously.
uint16_t dst_repeat_stride = 1;
uint16_t src_block_stride = 1;
uint16_t src_repeat_stride = 8;
asc_repeat_reduce_min_value_index(dst, src, repeat, dst_repeat_stride, src_block_stride, src_repeat_stride);
```

Result example:

```
Input data src: [256, 255, 254, ..., 129, 128, ..., 2, 1]
Output data dst: [129, 7.6e-6, 1, 7.6e-6]
Output description: The binary representation of the half value 7.6e-6 is 0x007F. When reinterpreted as uint32, it is the minimum value index 127.
```
