# asc_half2int4

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:25:04.788Z pushedAt=2026-09-08T03:45:05.503Z -->

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

Converts half type data to int4 type, supporting multiple rounding modes:

- RINT rounding mode: round to nearest, ties to even.
- ROUND rounding mode: round half up.
- FLOOR rounding mode: round toward negative infinity.
- CEIL rounding mode: round toward positive infinity.
- TRUNC rounding mode: round toward zero.

## Prototype

- Computation of the first n data elements

    ```c++
    //Indicates the RINT rounding mode when the conversion involves precision loss, and indicates no rounding when no precision loss is involved.
    __aicore__ inline void asc_half2int4(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //RINT rounding mode
    __aicore__ inline void asc_half2int4_rn(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //FLOOR rounding mode
    __aicore__ inline void asc_half2int4_rd(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //ROUND rounding mode
    __aicore__ inline void asc_half2int4_rna(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //CEIL rounding mode
    __aicore__ inline void asc_half2int4_ru(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //TRUNC rounding mode
    __aicore__ inline void asc_half2int4_rz(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    ```

- Computes high-dimensional split data

    ```c++
    //Indicates the RINT rounding mode when the conversion involves precision loss, and no rounding when no precision loss is involved.
    __aicore__ inline void asc_half2int4(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    //RINT rounding mode.
    __aicore__ inline void asc_half2int4_rn(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    //FLOOR rounding mode.
    __aicore__ inline void asc_half2int4_rd(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    //ROUND rounding mode.
    __aicore__ inline void asc_half2int4_rna(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    //CEIL rounding mode.
    __aicore__ inline void asc_half2int4_ru(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    //TRUNC rounding mode.
    __aicore__ inline void asc_half2int4_rz(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```

- Synchronous computation

    ```c++
    //Indicates the RINT rounding mode when the conversion involves precision loss, and no rounding when no precision loss is involved.
    __aicore__ inline void asc_half2int4_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //RINT rounding mode.
    __aicore__ inline void asc_half2int4_rn_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //FLOOR rounding mode.
    __aicore__ inline void asc_half2int4_rd_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //ROUND rounding mode.
    __aicore__ inline void asc_half2int4_rna_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //CEIL rounding mode.
    __aicore__ inline void asc_half2int4_ru_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //TRUNC rounding mode.
    __aicore__ inline void asc_half2int4_rz_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| **dst** | Output | Starting address of the destination operand (vector). |
| **src** | Input | Starting address of the source operand (vector). |
| **count** | Input | Number of elements involved in the computation. |
| **repeat** | Input | Number of iterations. |
| **dst_block_stride** | Input | Address stride between different DataBlocks within a single iteration of the destination operand. |
| **src_block_stride** | Input | Address stride between different DataBlocks within a single iteration of the source operand. |
| **dst_repeat_stride** | Input | Address stride of the same DataBlock between adjacent iterations of the destination operand. |
| **src_repeat_stride** | Input | Address stride of the same DataBlock between adjacent iterations of the source operand. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- When **dst** is **int4b_t**, the **count** of the first-n-data computation API must be an even number.

## Example

```cpp
constexpr uint32_t total_length = 256;
uint64_t offset = 0;
__ubuf__ half* src = (__ubuf__ half*)asc_get_phy_buf_addr(0);
offset += total_length * sizeof(half);
__ubuf__ int4b_t* dst = (__ubuf__ int4b_t*)asc_get_phy_buf_addr(offset);
asc_half2int4(dst, src, total_length);
```
