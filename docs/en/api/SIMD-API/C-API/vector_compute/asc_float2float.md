# asc_float2float

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:55:08.240Z pushedAt=2026-09-08T03:45:05.475Z -->

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

Performs precision conversion on float data, supporting multiple rounding modes:

- RINT rounding mode: round to nearest, ties to even.
- ROUND rounding mode: round half up.
- FLOOR rounding mode: round toward negative infinity.
- CEIL rounding mode: round toward positive infinity.
- TRUNC rounding mode: round toward zero.

## Prototype

- Computation of the first n data elements

    ```cpp
    // RINT mode.
    __aicore__ inline void asc_float2float_rn(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // FLOOR mode.
    __aicore__ inline void asc_float2float_rd(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // CEIL mode.
    __aicore__ inline void asc_float2float_ru(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // ROUND mode.
    __aicore__ inline void asc_float2float_rna(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // TRUNC mode.
    __aicore__ inline void asc_float2float_rz(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

- High-dimensional tiled computation

    ```cpp
    // RINT mode.
    __aicore__ inline void asc_float2float_rn(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // FLOOR mode.
    __aicore__ inline void asc_float2float_rd(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // CEIL mode.
    __aicore__ inline void asc_float2float_ru(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // ROUND mode.
    __aicore__ inline void asc_float2float_rna(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // TRUNC mode.
    __aicore__ inline void asc_float2float_rz(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```

- Synchronous mode

    ```cpp
    // RINT mode.
    __aicore__ inline void asc_float2float_rn_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // FLOOR mode.
    __aicore__ inline void asc_float2float_rd_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // CEIL mode.
    __aicore__ inline void asc_float2float_ru_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // ROUND mode.
    __aicore__ inline void asc_float2float_rna_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // TRUNC mode.
    __aicore__ inline void asc_float2float_rz_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
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
constexpr uint32_t total_length = 256;
__ubuf__ float src[total_length];
__ubuf__ float dst[total_length];
asc_float2float_rn(dst, src, total_length);
```
