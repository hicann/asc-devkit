---
okftype: reference.api
description: "Moves an ND-layout cube from Global Memory to the L1 Buffer and converts it to NZ fractal layout. Multi-cube movement is controlled through the nd_num, n_value, d_value, and Nz stride parameters for subsequent cube computation reads."
tag:
  - roles: [data_movement]
  - operations: [copy, layout_transform]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/cube_compute_input_move_constraint.md
    kind: constrained_by
version: ">=9.0.0"
version_changes: []
---
# asc_copy_gm2l1_nd2nz (NPU Architecture Version 2201)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:32:52.826Z pushedAt=2026-11-18T08:15:34.253Z nl=2 -->

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

Moves data from Global Memory (GM) to Level 1 cache (L1), supporting conversion from ND format to NZ format during data movement.

For the data layout before moving in, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and movement granularity requirements, see [Cube Computation Input Movement Constraints](cube_compute_input_move_constraint.md), and use them in combination with the parameters and constraint descriptions of this API.

## Prototype

- Regular movement

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ half* dst, __gm__ half* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ float* dst, __gm__ float* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    ```

## Parameters

**Table 1** Parameter description

| Parameter         | Input/Output | Description                                                                                                                                                                                                                                                                    |
|:---------------------|:------|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dst                  | Output    | Start address of the destination operand (vector).                                                                                                                                                                                                                                                       |
| src                  | Input    | Start address of the source operand.                                                                                                                                                                                                                                                            |
| nd_num               | Input    | Number of ND cubes to transfer. Value range: nd_num∈[0, 4095].                                                                                                                                                                                                                                       |
| n_value              | Input    | Number of rows of the ND cube. Value range: n_value∈[0, 16384].                                                                                                                                                                                                                                      |
| d_value              | Input    | Number of columns of the ND cube. Value range: d_value∈[0, 65535].                                                                                                                                                                                                                                      |
| src_nd_matrix_stride | Input    | Offset between the start addresses of adjacent ND cubes of the source operand. Value range: src_nd_matrix_stride∈[0, 65535], in elements.                                                                                                                                                                                                        |
| src_d_value          | Input    | Offset between the start addresses of adjacent rows of the same ND cube of the source operand. Value range: src_d_value∈[1, 65535], in elements.                                                                                                                                                                                                             |
| dst_nz_c0_stride     | Input    | After ND is converted to NZ format, one row in the source operand is converted into multiple rows in the destination operand. dst_nz_c0_stride indicates the offset between the start addresses of adjacent rows of data that come from the same row of the source operand in the destination NZ cube. Value range: dst_nz_c0_stride∈[1, 16384], unit: C0_SIZE (32B). |
| dst_nz_n_stride      | Input    | Offset between the start addresses of adjacent rows of the Z-shaped cube in the destination NZ cube. Value range: dst_nz_n_stride∈[1, 16384], unit: C0_SIZE (32B).                                                                                                                                                                                             |
| dst_nz_matrix_stride | Input    | Offset between the start addresses of adjacent NZ cubes in the destination NZ cube. Value range: dst_nz_matrix_stride∈[1, 65535], in elements.                                                                                                                                                                                                    |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If this instruction has data dependency on other instructions, insert [synchronization instructions](../../sync/overview.md#synchronization-control-overview) to ensure that multiple instructions are serialized and prevent abnormal data.

## Examples

```cpp
//total_length indicates the total length of data involved in the movement.
constexpr uint64_t total_length = 128;
__cbuf__ int8_t dst[total_length];
// Number of nd cubes to transfer.
constexpr uint16_t nd_num = 2;
// Number of rows in the nd cube.
constexpr uint16_t n_value = 4;
// Number of columns in the nd cube.
constexpr uint16_t d_value = 24;
// Start offset between adjacent nd cubes.
constexpr uint16_t src_nd_matrix_stride = 144;
// Number of elements in one row.
constexpr uint16_t src_d_value = 48;
// Offset between the start addresses of multiple rows after the format is converted from nd to nz.
constexpr uint16_t dst_nz_c0_stride = 11;
// Offset in dst between row x and row x+1 of an nd cube in src after conversion to nz format.
constexpr uint16_t dst_nz_n_stride = 2;
// Offset between the start of the x-th nd cube and the start of the (x+1)-th nd cube in dst.
constexpr uint16_t dst_nz_matrix_stride = 96;
// src indicates the start address of the source operand.
asc_copy_gm2l1_nd2nz(dst, src, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride, dst_nz_matrix_stride);
```
