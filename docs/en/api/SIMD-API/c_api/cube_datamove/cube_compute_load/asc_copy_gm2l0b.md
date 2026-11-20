---
okftype: reference.api
description: "Moves cube data in Global Memory to the L0B Buffer in 16x16 element blocks, with base_idx specifying the starting block and repeat specifying the number of iterations. Provides overloads for eight data types from int4b_t to float. Available only on Atlas A2 and A3 products."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md
    kind: requires
    condition: "Source data must be arranged in fractal format before being moved in"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/cube_compute_input_move_constraint.md
    kind: constrained_by
    condition: "Address alignment and transfer granularity requirements"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
version: ">=9.1.0"
version_changes: []
---
# asc_copy_gm2l0b

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:25:42.171Z pushedAt=2026-11-18T08:15:34.244Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Moves cube data from Global Memory to the L0B Buffer.

For the data layout before the move, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md). For address alignment and move granularity requirements, see [Cube Compute Input Move Constraints](cube_compute_input_move_constraint.md), and use them together with the parameter and constraint descriptions of this API.

## Prototype

- Regular move

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

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| base_idx | Input | Starting block ID of the move. The cube is divided into blocks of 16*16 elements. |
| repeat | Input | Number of iterations. |
| src_stride | Input | Gap between the start addresses of the previous fractal and the next fractal of the source operand in adjacent iterations. |
| dst_gap | Input | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).

## Examples

```cpp
//The start block of the move is 1.
constexpr uint16_t base_idx = 1;
//The number of move iterations is 2.
constexpr uint8_t repeat = 2;
// The input move stride is 0 bytes, and the output move stride is 1024 bytes.
constexpr uint16_t src_stride = 0;
constexpr uint16_t dst_gap = 1;
// src indicates the start address of the source operand.
__cb__ half dst[256];
asc_copy_gm2l0b(dst, src, base_idx, repeat, src_stride, dst_gap);
```
