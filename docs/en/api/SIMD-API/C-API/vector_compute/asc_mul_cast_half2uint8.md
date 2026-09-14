# asc_mul_cast_half2uint8

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:56:02.121Z pushedAt=2026-09-08T03:45:05.551Z -->

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

Computes the element-wise product and converts the result to the uint8_t type. The calculation formula is as follows:
$$
dst_i = Cast(src0_i * src1_i)
$$

- Precision conversion rule: The product of the source operands is rounded using the RINT rounding mode (round half to even), and overflow is handled by saturation.

## Prototype

- Computation of the first n data elements

    ```cpp
    __aicore__ inline void asc_mul_cast_half2uint8(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    ```

- High-dimensional split computation

    ```cpp
    __aicore__ inline void asc_mul_cast_half2uint8(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_mul_cast_half2uint8_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src0 | Input | Start address of source operand 0 (vector). |
| src1 | Input | Start address of source operand 1 (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations. |
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

- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
// total_length indicates the data length involved in the computation.
constexpr uint32_t total_length = 128;
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ uint8_t dst[total_length];
asc_mul_cast_half2uint8(dst, src0, src1, total_length);
```