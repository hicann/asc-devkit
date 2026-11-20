---
okftype: reference.api
description: "Deprecated API page: On the 2201 architecture, asc_copy_gm2l1_nd2nz_sync performs ND-to-NZ format conversion during synchronous data movement from GM to L1, using nd_num, row/column counts, and each stride parameter to describe the movement layout of multiple ND matrices. It is officially recommended to use the non-synchronous asc_copy_gm2l1_nd2nz on the same architecture plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy, layout_transform]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_2201.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous API and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The space size and alignment requirements of each storage unit follow its storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles synchronous waiting in the replacement solution"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this synchronous interface with ASC_DEPRECATED(9.2.0). The replacement path given in Release Notes 9.2.0 is the non-synchronous asc_copy_gm2l1_nd2nz plus asc_sync; the interface has not been removed."
---
# asc_copy_gm2l1_nd2nz_sync (NPU architecture version 2201, deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:37:37.596Z pushedAt=2026-11-18T03:12:16.393Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_gm2l1_nd2nz_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_2201.md) and the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from Global Memory (GM) to Level 1 cache (L1), supporting ND-to-NZ format conversion during the data move.

## Prototype

- Synchronous move

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ half* dst, __gm__ half* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ float* dst, __gm__ float* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    ```

## Parameters

**Table 1** Parameter description

| Parameter         | Input/Output | Description                                                                                                                                                                                                                          |
|:---------------------|:------|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dst                  | Output | Start address of the destination operand (vector).                                                                                                                                                                                |
| src                  | Input  | Starting address of the source operand.                                                                                                                                                                                              |
| nd_num               | Input  | Number of ND matrices to move. Value range: nd_num∈[0, 4095].                                                                                                                                                                        |
| n_value              | Input  | Number of rows in the ND matrix. Value range: n_value∈[0, 16384].                                                                                                                                                                    |
| d_value              | Input  | Number of columns in the ND matrix. Value range: d_value∈[0, 65535].                                                                                                                                                                 |
| src_nd_matrix_stride | Input  | Offset between the starting addresses of adjacent ND matrices of the source operand. Value range: src_nd_matrix_stride∈[0, 65535], in elements.                                                                                      |
| src_d_value          | Input  | Offset between the starting addresses of adjacent rows in the same ND matrix of the source operand. Value range: src_d_value∈[1, 65535], in elements.                                                                                |
| dst_nz_c0_stride     | Input  | After ND-to-NZ conversion, one row of the source operand is converted into multiple rows of the destination operand. dst_nz_c0_stride indicates the offset between the starting addresses of the multiple rows in the destination NZ matrix that come from the same row of the source operand. Value range: dst_nz_c0_stride∈[1, 16384], unit: C0_SIZE (32B). |
| dst_nz_n_stride      | Input  | Offset between the starting addresses of adjacent rows of the Z-shaped matrix in the destination NZ matrix. Value range: dst_nz_n_stride∈[1, 16384], unit: C0_SIZE (32B).                                                            |
| dst_nz_matrix_stride | Input  | Offset between the starting addresses of adjacent NZ matrices in the destination NZ matrix. Value range: dst_nz_matrix_stride∈[1, 65535], in elements.                                                                               |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If multiple asc_copy_gm2l1_nd2nz instructions need to be executed and their destination addresses overlap, a synchronization instruction must be inserted to ensure serialization of the multiple asc_copy_gm2l1_nd2nz instructions and prevent abnormal data.

## Examples

```cpp
//total_length indicates the total length of the data involved in the move.
constexpr uint64_t total_length = 128;
__cbuf__ int8_t dst[total_length];
// Number of nd matrices to transfer.
constexpr uint16_t nd_num = 2;
// Number of rows in the nd matrix.
constexpr uint16_t n_value = 4;
// Number of columns in the nd matrix.
constexpr uint16_t d_value = 24;
// Start offset between adjacent nd matrices.
constexpr uint16_t src_nd_matrix_stride = 144;
// Number of elements in one row.
constexpr uint16_t src_d_value = 48;
// Offset between the start addresses of multiple rows after the format is converted from nd to nz.
constexpr uint16_t dst_nz_c0_stride = 11;
// Offset in dst between row x and row x+1 of an nd matrix in src after conversion to nz format.
constexpr uint16_t dst_nz_n_stride = 2;
// Offset between the start of the x-th nd matrix and the start of the (x+1)-th nd matrix in dst.
constexpr uint16_t dst_nz_matrix_stride = 96;
// src indicates the start address of the source operand.
asc_copy_gm2l1_nd2nz_sync(dst, src, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride, dst_nz_matrix_stride);
```
