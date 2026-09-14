# asc_relu

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:06:56.693Z pushedAt=2026-09-08T03:45:05.571Z -->

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

Performs element-wise linear rectification (ReLU). The calculation formula is as follows:
$$
dst_i = relu(src_i)
$$

## Prototype

- Computation of the first n data elements
  ```cpp
  __aicore__ inline void asc_relu(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
  __aicore__ inline void asc_relu(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)
  __aicore__ inline void asc_relu(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
  ```

- High-dimensional split computation
  ```cpp
  __aicore__ inline void asc_relu(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  __aicore__ inline void asc_relu(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  __aicore__ inline void asc_relu(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  ```

- Synchronous computation
  ```cpp
  __aicore__ inline void asc_relu_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
  __aicore__ inline void asc_relu_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)
  __aicore__ inline void asc_relu_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
  ```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector).|
|src|Input|Start address of the source operand (vector).|
|count|Input|Number of elements involved in the computation.|
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

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```cpp
constexpr int total_length = 256; // total_length indicates the total length of data involved in the computation.
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_relu(dst, src, total_length);
// src : [6.938, -8.86, -0.2263, ..., 1.778]
// dst : [6.938, 0, 0, ..., 1.778]
```