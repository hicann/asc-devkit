# asc_sub_scalar

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:27:59.904Z pushedAt=2026-09-08T03:45:05.589Z -->

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

Performs subtraction between a vector and a scalar.

$$
dst_i = src_i - value
$$

## Prototype

- Computes the first n data elements continuously.

```c++
__aicore__ inline void asc_sub_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
__aicore__ inline void asc_sub_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
__aicore__ inline void asc_sub_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
__aicore__ inline void asc_sub_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
```

- Computes non-contiguous data.

```c++
__aicore__ inline void asc_sub_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, 
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
__aicore__ inline void asc_sub_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, 
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
__aicore__ inline void asc_sub_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, 
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
__aicore__ inline void asc_sub_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, 
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
```

- Computes data synchronously.

```c++
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| dst     | Output     | Start address of the destination operand (vector).   |
| src     | Input     | Start address of the source operand (vector).|
| value     | Input     | Source operand (scalar).|
| repeat     | Input    | Number of iterations.        |
| dst_block_stride |Input| Address stride between different DataBlocks within a single iteration of the destination operand. |
| src_block_stride |Input| Address stride between different DataBlocks within a single iteration of the source operand. |
| dst_repeat_stride |Input| Address stride of the same DataBlock between adjacent iterations of the destination operand. |
| src_repeat_stride |Input| Address stride of the same DataBlock between adjacent iterations of the source operand. |
| count   | Input     | Number of elements involved in continuous computation.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```c++
// total_length indicates the total length of the data involved in the computation.
constexpr uint64_t total_length = 64;
half scalar_val = 0.0f;
__ubuf__ half dst[total_length];
__ubuf__ half src[total_length];
asc_sub_scalar_sync(dst, src, scalar_val, total_length);
```
