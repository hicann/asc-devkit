# asc_max_scalar

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:47:09.073Z pushedAt=2026-09-08T03:45:05.538Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Compares each element of the vector **src** with the scalar **value**, and stores the maximum value into **dst** according to the corresponding bit positions. The calculation formula is as follows:

$$
dst_i = max(src_i, value)
$$

## Prototype

- Computation of the first n data elements

  ```cpp
  __aicore__ inline void asc_max_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
  __aicore__ inline void asc_max_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
  __aicore__ inline void asc_max_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
  __aicore__ inline void asc_max_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
  ```

- High-dimensional split computation

  ```cpp
  __aicore__ inline void asc_max_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  __aicore__ inline void asc_max_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  __aicore__ inline void asc_max_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  __aicore__ inline void asc_max_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  ```

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_max_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
  __aicore__ inline void asc_max_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
  __aicore__ inline void asc_max_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
   __aicore__ inline void asc_max_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
  ```

## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Starting address of the destination operand (vector).            |
| src | Input    | Starting address of the source operand (vector).           |
| value | Input    | Source operand (scalar).       |
| count     | Input    | Number of elements involved in continuous computation.      |
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks within a single iteration of the destination operand. |
| src_block_stride | Input | Address stride between different DataBlocks within a single iteration of the source operand. |
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the destination operand. |
| src_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the source operand. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```cpp
constexpr int total_length = 256; //total_length indicates the length of data involved in the computation.
half value = 3;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_max_scalar(dst, src, value, total_length);
// src : [1, 2, 3, 4, ... , 256]
// value : 3 
// dst : [3, 3, 3, 5, ... , 256]
```
