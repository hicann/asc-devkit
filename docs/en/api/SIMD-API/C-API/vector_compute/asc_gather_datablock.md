# asc_gather_datablock

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:02:56.891Z pushedAt=2026-09-08T03:45:05.482Z -->

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

Given an input vector and an address offset vector, gathers the input vector into the destination vector at the granularity of a DataBlock according to the offset addresses.

## Prototype

- High-dimensional split computation

  ```cpp
  __aicore__ inline void asc_gather_datablock(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t dst_block_stride, uint8_t repeat)
  __aicore__ inline void asc_gather_datablock(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t dst_block_stride, uint8_t repeat)
  ```

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_gather_datablock_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t dst_block_stride, uint8_t repeat)
  __aicore__ inline void asc_gather_datablock_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t dst_block_stride, uint8_t repeat)
  ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| src_offset | Input | Address offset of each DataBlock in the source operand. |
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations in the destination operand. |
| dst_block_stride | Input | Address stride between different DataBlocks within a single iteration in the destination operand. |
| repeat | Input | Number of iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints
- The value of **src_offset** must ensure that the bit width of the **src** element type is aligned.
- The address obtained by adding the offset to **src** must not exceed the range of the Unified Buffer (UB) size.
- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
constexpr uint64_t total_length = 128;    // total_length refers to the data length involved in the computation.
__ubuf__ uint32_t src_offset[total_length * sizeof(uint32_t) / 32];
__ubuf__ uint32_t dst[total_length];
__ubuf__ uint32_t src[total_length];
asc_gather_datablock(dst, src, src_offset, 8, 1, 2);
// src [1, 2, 3, ..., 126, 127, 128] 128 uint32_t data
// src_offset [0, 32, 64, ..., 416, 448, 480] 
// dst [1, 2, 3, ..., 126, 127, 128]
```