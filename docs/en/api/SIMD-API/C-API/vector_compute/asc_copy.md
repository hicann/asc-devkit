# asc_copy

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:41:35.608Z pushedAt=2026-09-08T03:45:05.458Z -->

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

Copies data from Unified Buffer to Unified Buffer. Supports high-dimensional slicing.

## Prototype

- Calculation of the first n data elements

```cpp
__aicore__ inline void asc_copy(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count)

__aicore__ inline void asc_copy(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count)

__aicore__ inline void asc_copy(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)

__aicore__ inline void asc_copy(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count)

```

- High-dimensional slicing calculation

```cpp
__aicore__ inline void asc_copy(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint8_t repeat,
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

__aicore__ inline void asc_copy(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint8_t repeat,
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

__aicore__ inline void asc_copy(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint8_t repeat,
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)

__aicore__ inline void asc_copy(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint8_t repeat,
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
```

- Synchronous calculation

```cpp
__aicore__ inline void asc_copy_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count)

__aicore__ inline void asc_copy_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count)

__aicore__ inline void asc_copy_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)

__aicore__ inline void asc_copy_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
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

- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
//total_length refers to the length of data involved in the computation.
constexpr uint32_t total_length = 128;
__ubuf__ int16_t src[total_length];
__ubuf__ int16_t dst[total_length];
asc_copy(dst, src, total_length);
```
