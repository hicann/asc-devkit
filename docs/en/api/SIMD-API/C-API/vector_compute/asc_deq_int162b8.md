# asc_deq_int162b8

<!-- md-trans-meta sourceCommit=b89e273df218e4b8c2c6857aeec20f0a35233b01 translatedAt=2026-09-05T13:46:13.271Z pushedAt=2026-09-08T03:45:05.462Z -->

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

Converts int16_t data to int8_t or uint8_t and stores the data in the high half-block or low half-block of each DataBlock. Before using this API, call [asc_set_deq_scale](./asc_set_deq_scale.md) to set the quantization parameters.

- **asc_deq_int162b8_h**: Stores the data in the high half-block of each DataBlock.
- **asc_deq_int162b8_l**: Stores the data in the low half-block of each DataBlock.

As shown in the following figure:
![](../figures/asc_deq_int162b8_halfblock.png)

The calculation formula is as follows:

$$
dst_i = (src_i * scale) + offset
$$

## Prototype

- Computation of the first n elements

    ```cpp
    __aicore__ inline void asc_deq_int162b8_h(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_h(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_l(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_l(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    ```

- High-dimensional split computation

    ```cpp
    __aicore__ inline void asc_deq_int162b8_h(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    __aicore__ inline void asc_deq_int162b8_h(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    __aicore__ inline void asc_deq_int162b8_l(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    __aicore__ inline void asc_deq_int162b8_l(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Starting address of the destination operand (vector). |
| src | Input | Starting address of the source operand (vector). |
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
constexpr uint64_t total_length = 128;    // total_length is the data length involved in the computation.
__ubuf__ int16_t src[total_length];
__ubuf__ int8_t dst[total_length];
float scale = 1.0;        // The quantization parameter is 1.
int16_t offset = 0;       // No offset.
bool sign_mode = true;    // The quantization result is signed (dst is of the int8_t type).
asc_set_deq_scale(scale, offset, sign_mode);    // The calculation formula is dst = src.
asc_deq_int162b8_h(dst, src, total_length);    // Convert src to the int8_t type and store it in the high half-block of each DataBlock of dst.
```