---
okftype: reference.api
description: "Transfers cube data from Global Memory to the L1 Buffer on Atlas A2 and A3 products, including three overloads: normal transfer, normal and high-dimensional split transfer with eight pad modes, and 2D transfer iterated by fractal."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [datablock_granularity, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md
    kind: requires
    condition: "Source data must be arranged in fractal format before transfer"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/cube_compute_input_move_constraint.md
    kind: constrained_by
    condition: "Address alignment and transfer granularity requirements"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: requires
    condition: "When there is a data dependency with other instructions, a synchronization instruction must be inserted"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_copy_gm2l1__PL1bf16_PGbf16_u16_u8_u16_u16"
    scope_basis: resolved
    note: "9.1.0 adds a 2D transfer mode overload (parameters base_idx, repeat, src_stride, dst_gap, with base_idx and stride in units of 512B). The same form covers 7 dtypes in total: half, float, int32_t, int8_t, uint32_t, and uint8_t. Available only on the 2201 architecture."
---
# asc_copy_gm2l1 (NPU Architecture Version 2201)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:27:55.489Z pushedAt=2026-11-18T08:15:34.248Z nl=2 -->

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

Transfers cube data from Global Memory to the L1 Buffer, and supports different types of pad modes.

For the data layout before transfer, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and transfer granularity requirements, see [Cube Computation Input Transfer Constraints](cube_compute_input_move_constraint.md), and use them in conjunction with the parameters and constraints of this API.

## Prototype

- Normal transfer mode

    ```cpp
    // No pad operation is performed.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 31B of pad for every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 15B of pad for every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 14B of pad for every 2B.
    __aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 12B of pad for every 4B.
    __aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 8B of pad every 8B.
    __aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 28B of every 32B, and store the lowest 4B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 24B of every 32B, and store the lowest 8B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 16B of every 32B, and store the lowest 16B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    ```

- Normal transfer mode (high-dimensional slicing)

    ```cpp
    // No pad operation is performed.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 31B of pad every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 15B of pad every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 14B of pad every 2B.
    __aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 12B of pad every 4B.
    __aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // Insert 8B pad every 8B in units of 32B.
    __aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // Delete the highest 28B of every 32B, and store the lowest 4B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // Delete the highest 24B of every 32B, and store the lowest 8B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // Delete the highest 16B of every 32B, and store the lowest 16B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

- 2D transfer mode

    ```cpp
    // 2D transfer of bfloat16_t cube data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D transfer of half cube data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D transfer of float cube data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D transfer of int32_t cube data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D transfer of int8_t cube data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // Transfer uint32_t cube data in 2D mode.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // Transfer uint8_t cube data in 2D mode.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

## Parameters

### Normal Transfer Mode Parameters

**Table 1** Normal transfer mode parameters

| Parameter | Input/Output | Description                                                              |
|:----------|:------|:----------------------------------------------------------------|
| dst       | Output    | Start address of the destination operand (vector).                                                 |
| src       | Input    | Start address of the source operand.                                                      |
| size      | Input    | Size of the data to be transferred (bytes).                                                     |
| n_burst   | Input    | Number of consecutive data blocks to be transferred. Value range: [1, 4095].                                   |
| len_burst | Input    | Length of each consecutive data block to be transferred, unit: DataBlock (32 bytes). Value range: [1, 65535].            |
| src_gap   | Input    | Interval between adjacent consecutive data blocks of the source operand (the interval between the tail of the preceding data block and the head of the following data block).<br>Unit: DataBlock (32 bytes).  |
| dst_gap   | Input    | Interval between adjacent consecutive data blocks of the destination operand (the interval between the tail of the preceding data block and the head of the following data block).<br>Unit: DataBlock (32 bytes). |

### 2D Transfer Mode Parameters

**Table 2** 2D transfer mode parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| base_idx | Input | Fractal cube ID, indicating the fractal position in the source operand from which the transfer starts (0 indicates the first fractal cube in the source operand). Value range: [0, 65535]. Unit: 512B. Default: 0. |
| repeat | Input | Number of iterations. |
| src_stride | Input | Interval between the start addresses of the previous fractal and the next fractal of the source operand in adjacent iterations. Unit: 512B. Value range: [0, 65535]. Default: 0. |
| dst_gap | Input | Interval between the end address of the previous fractal and the start address of the next fractal of the destination operand in adjacent iterations. Unit: 512B. Value range: [0, 65536]. Default: 0. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If this instruction has data dependencies with other instructions, insert [synchronization instructions](../../sync/overview.md#synchronization-control-overview) to ensure that multiple instructions are serialized and prevent abnormal data.

## Examples

```cpp
// Normal data transfer mode
// total_length indicates the total length of data involved in the transfer.
constexpr uint64_t total_length = 128;
__cbuf__ half dst[total_length];
// Number of data blocks transferred consecutively
constexpr uint16_t n_burst = 1;
// Length of the data block transferred consecutively, which is 512 half elements here
constexpr uint16_t len_burst = 512 * sizeof(half) / 32;
// Perform continuous transfer of the source operand.
constexpr uint16_t src_gap = 0;
// Arrange the destination operand continuously.
constexpr uint16_t dst_gap = 0;
// src indicates the start address of the source operand.
asc_copy_gm2l1((__cbuf__ void*)dst, (__gm__ void*)src, n_burst, len_burst, src_gap, dst_gap);

// 2D data transfer mode.
// The start block of the transfer is 1.
constexpr uint16_t base_idx = 1;
// The number of transfer iterations is 2.
constexpr uint8_t repeat = 2;
// The input transfer stride is 0 bytes, and the output transfer stride is 1024 bytes.
constexpr uint16_t src_stride = 0;
constexpr uint16_t dst_gap = 1;
// src indicates the input address of the source operand.
__cbuf__ half dst[256];
asc_copy_gm2l1(dst, src, base_idx, repeat, src_stride, dst_gap);
```
