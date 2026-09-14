# asc_shiftleft

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:20:41.758Z pushedAt=2026-09-08T03:45:05.586Z -->

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

Shifts all elements left by **distance** bits. The calculation formula is as follows:
$$
dst_i = src_i << distance
$$

## Prototype

- Calculation of the first n data elements

    ```cpp
    __aicore__ inline void asc_shiftleft(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint32_t count)
    ```

- High-dimensional segmented calculation

    ```cpp
    __aicore__ inline void asc_shiftleft(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftleft(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftleft(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_shiftleft(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_shiftleft_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint32_t count)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| distance | Input | Source operand (scalar), indicating the number of bits to shift left. |
| count | Input | Number of elements involved in the calculation. |
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

- **distance** does not support being set to a negative number.
- The value of **distance** cannot exceed the bit width of the **src** data type.
- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
constexpr uint64_t total_length = 128;
uint32_t distance = 1;
__ubuf__ int16_t src[total_length];
__ubuf__ int16_t dst[total_length];
asc_shiftleft(dst, src, distance, total_length);
```