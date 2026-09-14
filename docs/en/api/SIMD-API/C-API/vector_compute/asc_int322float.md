# asc_int322float

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:32:39.151Z pushedAt=2026-09-08T03:45:05.516Z -->

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

Converts **int32_t** data to **float** data and supports multiple rounding modes:

- RINT rounding mode: round to nearest, ties to even.
- ROUND rounding mode: round half up.
- FLOOR rounding mode: round toward negative infinity.
- CEIL rounding mode: round toward positive infinity.
- TRUNC rounding mode: round toward zero.

## Prototype

- Computation of the first n data elements

  ```cpp
  // Indicates the RINT rounding mode when the conversion incurs precision loss, and no rounding when no precision loss is involved.
  __aicore__ inline void asc_int322float(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // RINT rounding mode
  __aicore__ inline void asc_int322float_rn(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // ROUND rounding mode
  __aicore__ inline void asc_int322float_rna(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // FLOOR rounding mode
  __aicore__ inline void asc_int322float_rd(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // CEIL rounding mode
  __aicore__ inline void asc_int322float_ru(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // TRUNC rounding mode
  __aicore__ inline void asc_int322float_rz(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  ```

- High-dimensional split computation

  ```cpp
  // Indicates the RINT rounding mode when the conversion involves precision loss, and no rounding when no precision loss is involved.
  __aicore__ inline void asc_int322float(__ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  // RINT rounding mode.
  __aicore__ inline void asc_int322float_rn(__ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  // ROUND rounding mode.
  __aicore__ inline void asc_int322float_rna(__ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  // FLOOR rounding mode.
  __aicore__ inline void asc_int322float_rd(__ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  // CEIL rounding mode.
  __aicore__ inline void asc_int322float_ru(__ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  // TRUNC rounding mode.
  __aicore__ inline void asc_int322float_rz(__ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
  ```

- Synchronous computation

```cpp
  // Indicates the RINT rounding mode when the conversion involves precision loss, and no rounding when no precision loss is involved.
  __aicore__ inline void asc_int322float_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // RINT rounding mode.
  __aicore__ inline void asc_int322float_rn_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // ROUND rounding mode.
  __aicore__ inline void asc_int322float_rna_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // FLOOR rounding mode.
  __aicore__ inline void asc_int322float_rd_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // CEIL rounding mode.
  __aicore__ inline void asc_int322float_ru_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
  // TRUNC rounding mode.
  __aicore__ inline void asc_int322float_rz_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
```

## Parameters

Table 1 Parameter description

| Parameter | Input/Output | Description |
| :----| :-----| :-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride | Input | Address stride between different DataBlocks of the source operand within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
// total_length indicates the data length involved in the computation.
constexpr uint32_t total_length = 32;
__ubuf__ float dst[total_length];
__ubuf__ int32_t src[total_length];
asc_int322float(dst, src, total_length);
```
