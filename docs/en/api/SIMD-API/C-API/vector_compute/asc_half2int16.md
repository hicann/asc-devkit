# asc_half2int16

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:22:59.331Z pushedAt=2026-09-08T03:45:05.504Z -->

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

Converts **half** data to **int16_t** and supports multiple rounding modes.
- RINT rounding mode: round to nearest, ties to even.
- ROUND rounding mode: round half up.
- FLOOR rounding mode: round toward negative infinity.
- CEIL rounding mode: round toward positive infinity.
- TRUNC rounding mode: round toward zero.

## Prototype

- Computation of the first n data elements
    ```cpp
    // RINT rounding mode
    __aicore__ inline void asc_half2int16_rn(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // ROUND rounding mode
    __aicore__ inline void asc_half2int16_rna(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    //FLOOR rounding mode
    __aicore__ inline void asc_half2int16_rd(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // CEIL rounding mode
    __aicore__ inline void asc_half2int16_ru(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // TRUNC rounding mode
    __aicore__ inline void asc_half2int16_rz(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    ```

- High-dimensional split computation
    ```cpp
    // RINT rounding mode
    __aicore__ inline void asc_half2int16_rn(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // ROUND rounding mode
    __aicore__ inline void asc_half2int16_rna(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // FLOOR rounding mode
    __aicore__ inline void asc_half2int16_rd(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // CEIL rounding mode
    __aicore__ inline void asc_half2int16_ru(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    // TRUNC rounding mode
    __aicore__ inline void asc_half2int16_rz(__ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```

- Synchronous computation
    ```cpp
    // RINT rounding mode.
    __aicore__ inline void asc_half2int16_rn_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // ROUND rounding mode.
    __aicore__ inline void asc_half2int16_rna_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // FLOOR rounding mode.
    __aicore__ inline void asc_half2int16_rd_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // CEIL rounding mode.
    __aicore__ inline void asc_half2int16_ru_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    // TRUNC rounding mode.
    __aicore__ inline void asc_half2int16_rz_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
    ```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Starting address of the destination operand (vector).|
|src|Input|Starting address of the source operand (vector).|
|count|Input|Number of elements involved in the computation.|
| repeat |Input| Number of iterations. |
| dst_block_stride |Input| Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride |Input| Address stride between different DataBlocks of the source operand within a single iteration. |
| dst_repeat_stride |Input| Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src_repeat_stride |Input| Address stride of the same DataBlock of the source operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```cpp
constexpr uint32_t total_length = 256;
__ubuf__ half src[total_length];
__ubuf__ int16_t dst[total_length];
asc_half2int16_rn(dst, src, total_length);
```