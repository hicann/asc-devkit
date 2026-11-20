---
okftype: reference.api
description: "Moves fractal cube data from Global Memory to the L0A Buffer, using base_idx to specify the starting fractal and repeat to specify the number of iterations. src_stride and dst_gap control the address interval between adjacent iterations in units of 512 bytes. Available only on the Atlas A2 and A3 products."
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
    condition: "storage unit space size and alignment requirements, generic address overlap constraint"
version: ">=9.1.0"
version_changes: []
---
# asc_copy_gm2l0a

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:25:31.179Z pushedAt=2026-11-18T08:15:34.242Z nl=2 -->

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

Moves data from GM to the L0A Buffer.

For the data layout before the move, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md). For address alignment and move granularity requirements, see [Cube Compute Input Move Constraints](cube_compute_input_move_constraint.md), and use them together with the parameter and constraint descriptions of this API.

## Prototype

- Regular move

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

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                        |
|-----| ------------ |-----------------------------------------------------------|
| dst | Output | Start address of the destination operand.                                                |
| src | Input | Start address of the source operand.                                                 |
| base_idx | Input | Fractal cube ID, indicating the position of the fractal from which the move starts in the source operand (0 indicates the first fractal cube in the source operand). Value range: [0, 65535]. |
| repeat | Input | Number of iterations. Each iteration can process 512B of data. Value range: [1, 255].                        |
| src_stride | Input | Interval between the start addresses of the previous fractal and the next fractal of the source operand in adjacent iterations. Unit: 512B. Value range: [0, 65535].    |
| dst_gap | Input | Interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand in adjacent iterations. Unit: 512B. Value range: [0, 65535].   |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
__ca__ half dst[256];
// src indicates the start address of the source operand.
asc_copy_gm2l0a(dst, src, 0, 1, 64, 64);
```
