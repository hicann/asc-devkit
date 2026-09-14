# asc_mul_add_relu

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:52:59.741Z pushedAt=2026-09-08T03:45:05.547Z -->

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

Multiplies **src0** and **dst** element-wise, adds **src1**, and then performs ReLU computation (comparing the result with 0 and taking the larger value). The final result is stored in **dst**. The calculation formula is as follows:

$$
dst_i = relu((src0_i * dst_i) + src1_i)
$$

## Prototype

- Computation of the first n data elements

    ```cpp
    __aicore__ inline void asc_mul_add_relu(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_mul_add_relu(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    ```

- High-dimensional split computation

    ```cpp
    __aicore__ inline void asc_mul_add_relu(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_mul_add_relu(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_mul_add_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_mul_add_relu_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :----| :-----| :-----|
| dst | Output | Start address of the destination operand (vector). |
| src0 | Input | Start address of the source operand (vector). |
| src1 | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src0_block_stride | Input | Address stride between different DataBlocks of source operand 0 within a single iteration. |
| src1_block_stride | Input | Address stride between different DataBlocks of source operand 1 within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src0_repeat_stride | Input | Address stride of the same DataBlock of source operand 0 between adjacent iterations. |
| src1_repeat_stride | Input | Address stride of the same DataBlock of source operand 1 between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of **dst**, **src0**, and **src1** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
// total_length refers to the data length involved in the computation.
constexpr uint32_t total_length = 128;
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ half dst[total_length];
asc_mul_add_relu(dst, src0, src1, total_length);
```