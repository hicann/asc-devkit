# asc_copy_gm2l0b

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:38:44.838Z -->

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

Copies matrix data from **Global Memory** to the **L0B Buffer**.

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_gm2l0b(__cb__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b(__cb__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b(__cb__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b(__cb__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b(__cb__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b(__cb__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b(__cb__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b(__cb__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_gm2l0b_sync(__cb__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b_sync(__cb__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b_sync(__cb__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b_sync(__cb__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b_sync(__cb__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b_sync(__cb__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b_sync(__cb__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0b_sync(__cb__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| **dst** | Output | Start address of the destination operand (vector). |
| **src** | Input | Start address of the source operand (vector). |
| **base_idx** | Input | Starting block ID for the copy, where the matrix is divided into blocks of 16*16 elements. |
| **repeat** | Input | Number of iterations. |
| **src_stride** | Input | Gap between the start addresses of the previous fractal and the next fractal of the source operand across adjacent iterations. |
| **dst_gap** | Input | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand across adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

None

## Example

```cpp
// The starting block for the copy is 1.
constexpr uint16_t base_idx = 1;
// The number of copy iterations is 2.
constexpr uint8_t repeat = 2; 
// The input copy stride is 0 bytes, and the output copy stride is 1024 bytes.
constexpr uint16_t src_stride = 0;
constexpr uint16_t dst_gap = 1;
__gm__ half src[256];
__cb__ half dst[256];
asc_copy_gm2l0b(dst, src, base_idx, repeat, src_stride, dst_gap);
```
