# asc_copy_gm2l0a

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:38:41.314Z -->

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

Copies data from GM to the L0A Buffer.

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_gm2l0a(__ca__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a(__ca__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a(__ca__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a(__ca__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a(__ca__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a(__ca__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a(__ca__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a(__ca__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_gm2l0a_sync(__ca__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a_sync(__ca__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a_sync(__ca__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a_sync(__ca__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a_sync(__ca__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a_sync(__ca__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a_sync(__ca__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_gm2l0a_sync(__ca__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

## Parameters

| Parameter | Input/Output | Description                                                        |
|-----| ------------ |-----------------------------------------------------------|
| dst | Output | Start address of the destination operand.                                                |
| src | Input | Start address of the source operand.                                                 | 
| base_idx | Input | Fractal matrix ID, indicating the fractal position in the source operand from which the copy starts (0 indicates the first fractal matrix in the source operand). Value range: [0, 65535]. |
| repeat | Input | Number of iterations. Each iteration processes 512B of data. Value range: [1, 255].                        |
| src_stride | Input | Interval between the start addresses of the previous and next fractals of the source operand in adjacent iterations, in 512B. Value range: [0, 65535].    |
| dst_gap | Input | Interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand, in 512B. Value range: [0, 65535].   |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example
```cpp
__ca__ half dst[256];
__gm__ half src[256];
asc_copy_gm2l0a(dst, src, 0, 1, 64, 64);
```
