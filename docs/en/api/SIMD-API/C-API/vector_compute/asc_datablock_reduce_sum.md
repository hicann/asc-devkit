# asc_datablock_reduce_sum

<!-- md-trans-meta sourceCommit=b89e273df218e4b8c2c6857aeec20f0a35233b01 translatedAt=2026-09-05T13:44:07.123Z pushedAt=2026-09-08T03:45:05.463Z -->

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

Sums all elements in each DataBlock.

The source operands are added pairwise in a binary tree manner. Taking the summation of 128 half-type data as an example, each DataBlock can compute 16 half-type data elements, which are divided into 8 DataBlocks for computation. Within each DataBlock, pairwise addition is performed in a binary tree manner.

Note that during pairwise addition, when a computation result is greater than 65504, the result is saved as 65504. For example, if the source operands are [60000, 60000, -30000, 100], first 60000 + 60000 overflows and the result is 65504, then -30000 + 100 = -29900 is computed, and finally 65504 - 29900 = 35604.

## Prototype

- Computation of the first n data

    ```cpp
    __aicore__ inline void asc_datablock_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_datablock_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

- High-dimensional split computation

    ```cpp
    __aicore__ inline void asc_datablock_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_datablock_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

Table 1 Parameter description

| Parameter | Input/Output | Description |
| :----| :-----| :-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations.|
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the destination operand.<br>When the input type bit width is 16 bits, the unit is 16 bytes; when the input type bit width is 32 bits, the unit is 32 bytes.|
| src_block_stride | Input | Address stride between different DataBlocks within a single iteration of the source operand.|
| src_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the source operand.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).


## Example

```cpp
constexpr uint32_t src_length = 256;
constexpr uint32_t dst_length = 16;
__ubuf__ half src[src_length];
__ubuf__ half dst[dst_length];

// Use the high-dimensional split computation API, which requires setting the mask.
asc_set_mask_norm();
asc_set_vector_mask(0xffffffffffffffff, 0xffffffffffffffff);    // Set all elements in each iteration to participate in the computation.

// Each repeat is 256B, with 2 repeats and no interval.
asc_datablock_reduce_sum(dst, src, 2, 1, 1, 8);
```

Result example:

```
Input data src: [1 1 1 ... 1]
Output data dst: [16 16 16 ... 16]
```
