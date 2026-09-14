# asc_shiftright

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:22:07.043Z pushedAt=2026-09-08T03:45:05.587Z -->

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

Performs a right shift on each element in **src** by **value** bits.

- For **int16_t**/**int32_t**, performs an arithmetic right shift (drops the least significant bit and replicates the sign bit into the most significant bit).
- For **uint16_t**/**uint32_t**, performs a logical right shift (drops the least significant bit and fills the most significant bit with 0).

## Prototype

- Computation of the first n elements

    ```cpp
    __aicore__ inline void asc_shiftright(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_round(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_round(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
    ```

- High-dimensional segmented computation

    ```cpp
    __aicore__ inline void asc_shiftright(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftright(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftright(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftright(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftright_round(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftright_round(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_shiftright_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_round_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_round_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| value | Input | Source operand (scalar), indicating the number of bits to shift right. |
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

- **value** cannot be set to a negative value.
- The value of **value** must not exceed the bit width of the **src** data type.
- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the data length involved in the computation.
int32_t value = 1;
__ubuf__ int16_t src[total_length];
__ubuf__ int16_t dst[total_length];
asc_shiftright(dst, src, value, total_length);    // Perform an arithmetic right shift by one bit on each element in src, and store the result in dst.
// src[0] = 0b1010, dst[0] = 0b0101

asc_shiftright_round(dst, src, value, total_length);    // Perform an arithmetic right shift by one bit on each element in src, round the result, and store it in dst.
// src[0] = 0b1010, dst[0] = 0b0101. The last bit to be rounded is 0, so no carry occurs.
// src[0] = 0b1001, dst[0] = 0b0101. The last bit to be rounded is 1, so a carry occurs.
```
