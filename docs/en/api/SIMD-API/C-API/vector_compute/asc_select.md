# asc_select

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:13:11.590Z pushedAt=2026-09-08T03:45:05.577Z -->

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

Performs a vector selection operation. Given two source operands **src0** and **src1**, it selects elements based on a condition to obtain the destination operand **dst**. This API is used together with [asc_set_cmp_mask(sel_mask)](asc_set_cmp_mask.md). The rule is as follows: when a bit of **sel_mask** is 1, the element is selected from **src0**; when the bit is 0, the element is selected from **src1**.

## Prototype

- Computation over the first n elements

    ```c++
    __aicore__ inline void asc_select(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_select(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    ```

- High-dimensional segmented computation

    ```c++
    __aicore__ inline void asc_select(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_select(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    ```

- Synchronous computation

    ```c++
    __aicore__ inline void asc_select_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_select_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    ```

## Parameters

|Parameter|Input/Output|Description|
| :--- | :--- | :--- |
| dst  | Output  | Start address of the destination operand (vector). |
| src0 | Input  | Start address of source operand 0 (vector). |
| src1 | Input  | Start address of source operand 1 (vector). |
| count | Input    | Number of elements involved in the computation. |
| repeat | Input    | Number of iterations. |
| dst_block_stride | Input    | Address stride between different DataBlocks within a single iteration of the destination operand. |
| src0_block_stride | Input    | Address stride between different DataBlocks within a single iteration of source operand 0. |
| src1_block_stride | Input    | Address stride between different DataBlocks within a single iteration of source operand 1. |
| dst_repeat_stride | Input    | Address stride of the same DataBlock between adjacent iterations of the destination operand. |
| src0_repeat_stride | Input    | Address stride of the same DataBlock between adjacent iterations of source operand 0. |
| src1_repeat_stride | Input    | Address stride of the same DataBlock between adjacent iterations of source operand 1. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- This API must be used together with [asc_set_cmp_mask](asc_set_cmp_mask.md).
- The start addresses of **dst**, **src0**, and **src1** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```c++
constexpr uint32_t total_length = 256;
constexpr uint32_t sel_length = 8;
__ubuf__ uint8_t sel_mask[sel_length];
__ubuf__ float dst[total_length];
__ubuf__ float src0[total_length];
__ubuf__ float src1[total_length];
asc_set_cmp_mask(sel_mask);
asc_sync_pipe(PIPE_V);
asc_select(dst, src0, src1, total_length);
```
