# asc_squeeze

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:23:41.551Z pushedAt=2026-09-08T03:45:05.584Z -->

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

Compresses the vector in **src0** into a shorter vector by comparing it with the mask **src1**, and stores the result in **dst**.

## Prototype

  ```c++
  __aicore__ inline void asc_squeeze(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_squeeze(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  ```

## Parameters

| Parameter       | Input/Output | Description                                |
|:----------| :-----|:----------------------------------|
| dst       | Output | Start address of the destination operand (vector).                   |
| src0, src1 | Input | Start addresses of the source operands (vectors).                        |
| repeat | Input | Number of iterations.                       |
| dst_block_stride | Input | Address stride between different DataBlocks within a single iteration of the destination operand. |
| src0_block_stride | Input | Address stride between different DataBlocks within a single iteration of source operand 0. |
| src1_block_stride | Input | Address stride between different DataBlocks within a single iteration of source operand 1. |
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the destination operand. |
| src0_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of source operand 0. |
| src1_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of source operand 1. |


## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of **dst**, **src0**, and **src1** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

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
