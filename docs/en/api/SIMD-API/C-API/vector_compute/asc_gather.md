# asc_gather

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:03:31.727Z pushedAt=2026-09-08T03:45:05.484Z -->

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

Collects elements from **src** into **dst** according to the offsets specified by **src_offset**.

## Prototype

- Computation of the first n data elements

    ```cpp
    __aicore__ inline void asc_gather(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather(__ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    ```

- High-dimensional split computation

    ```cpp
    __aicore__ inline void asc_gather(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_gather(__ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_gather_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| src_offset | Input | Address offset corresponding to each element of the source operand. The offset is relative to the start base address of the source operand, in bytes. |
| count | Input | Number of elements involved in the computation. |
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the destination operand. |
| repeat | Input | Number of iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The offset values in **src_offset** must ensure that the element type of **src** is bit-width aligned.
- The address obtained by adding the offset in **src_offset** to **src** must not exceed the range of the Unified Buffer (UB) data size.
- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
constexpr uint64_t total_length = 128;    // total_length refers to the data length involved in the computation.
__ubuf__ half src[total_length];
__ubuf__ uint32_t src_offset[total_length];
__ubuf__ half dst[total_length];
asc_gather(dst, src, src_offset, total_length);
// src [1, 2, 3, ..., 126, 127, 128], 128 half-type data elements.
// src_offset [254, 252, 250, ..., 4, 2, 0], the offset of each element in src, based on the start base address of src, in bytes.
// dst [128, 127, 126, ..., 3, 2, 1]
```