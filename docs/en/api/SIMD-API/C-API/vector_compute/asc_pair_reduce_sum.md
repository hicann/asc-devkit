# asc_pair_reduce_sum

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:04:20.110Z pushedAt=2026-09-08T03:45:05.564Z -->

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

Performs a reduction operation on the input data to obtain the total sum. Every two adjacent (odd-even) elements are summed. For example, for (a1, a2, a3, a4, a5, a6...), the sum of each adjacent pair is (a1+a2, a3+a4, a5+a6...).

## Prototype

- Continuous computation of the first n elements

    ```c++
    __aicore__ inline void asc_pair_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_pair_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

- High-dimensional split computation

    ```c++
    __aicore__ inline void asc_pair_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_pair_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    ```

- Synchronous computation

    ```c++
    __aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| dst     | Output     | Start address of the destination operand (vector). |
| src     | Input     | Start address of the source operand (vector). |
| count   | Input     | Number of elements involved in continuous computation. |
| repeat | Input | Number of iterations.|
| dst_repeat_stride | Input | Address stride between the same DataBlock in adjacent iterations of the destination operand.<br>Measured in units of the length after one repeat reduction. After PairReduce completes, the length of one repeat is halved.|
| src_block_stride | Input | Address stride between different DataBlocks within a single iteration of the source operand.|
| src_repeat_stride | Input | Address stride between the same DataBlock in adjacent iterations of the source operand.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```c++
constexpr uint32_t src_length = 256;
constexpr uint32_t dst_length = 16;
__ubuf__ half src[src_length];
__ubuf__ half dst[dst_length];
// 256B per repeat, 2 repeats, no gap.
asc_pair_reduce_sum(dst, src, 2, 1, 1, 8);
```

Result example:

```
Input data src: [1 2 3 ... 16 17 ... 32 ... 225... 256]
Output data dst: [3 7 ... 511]
```
