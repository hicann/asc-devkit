# asc_vdeq_int162b8

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T03:59:22.754Z -->

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

Converts int16_t data to int8_t or uint8_t and stores the data in the upper half-block or lower half-block of each DataBlock. Before using this API, call **asc_set_deq_scale** to set the quantization parameters.

- **asc_vdeq_int162b8_h**: Stores the data in the upper half-block of each DataBlock.
- **asc_vdeq_int162b8_l**: Stores the data in the lower half-block of each DataBlock.
The calculation formula is as follows:

$$
dst_i = (src_i * scale_j) + offset_j
$$

## Prototype

- Computation of the first n data elements

    ```cpp
   __aicore__ inline void asc_vdeq_int162b8_h(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
   __aicore__ inline void asc_vdeq_int162b8_h(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
   __aicore__ inline void asc_vdeq_int162b8_l(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
   __aicore__ inline void asc_vdeq_int162b8_l(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    ```

- High-dimensional split computation

    ```cpp
    __aicore__ inline void asc_vdeq_int162b8_h(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    __aicore__ inline void asc_vdeq_int162b8_h(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    __aicore__ inline void asc_vdeq_int162b8_l(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    __aicore__ inline void asc_vdeq_int162b8_l(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride | Input | Address stride between different DataBlocks within a single iteration of the source operand. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
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
// total_length indicates the length of data involved in the computation.
constexpr uint32_t total_length = 32;
__ubuf__ int8_t dst[total_length];
__ubuf__ int16_t src[total_length];
asc_vdeq_int162b8_h(dst, src, total_length);
asc_vdeq_int162b8_l(dst, src, total_length);
```