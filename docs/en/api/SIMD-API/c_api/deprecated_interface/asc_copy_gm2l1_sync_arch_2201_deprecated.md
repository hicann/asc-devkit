---
okftype: reference.api
description: "Deprecated API page: asc_copy_gm2l1_sync synchronously transfers matrix data from GM to L1 Buffer under the 2201 architecture, providing normal continuous transfer, 32-byte granularity insertion or deletion padding from pad1 to pad8, and 2D transfer with fractal numbering. It is officially recommended to use the non-synchronous API plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [datablock_granularity, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_arch_2201.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous API and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The space size and alignment requirements of each storage unit follow its storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Performs synchronous waiting in the alternative solution"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_copy_gm2l1_sync__PL1bf16_PGbf16_u16_u8_u16_u16@2201"
    scope_basis: resolved
    note: "9.1.0 adds a 2D transfer overload with base_idx, repeat, src_stride, and dst_gap, covering the bfloat16_t, half, float, int32_t, int8_t, uint32_t, and uint8_t dtype forms in the 2D transfer mode on this page"
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks all forms of normal transfer, pad1 through pad8, and 2D with ASC_DEPRECATED(9.2.0). The alternative path given in Release Notes 9.2.0 is the non-synchronous asc_copy_gm2l1 plus asc_sync; the interface is not removed."
---
# asc_copy_gm2l1_sync (NPU Architecture Version 2201, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:41:38.064Z pushedAt=2026-11-18T03:12:16.397Z nl=2 -->

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

**The `asc_copy_gm2l1_sync`, `asc_copy_gm2l1_pad1_sync`, `asc_copy_gm2l1_pad2_sync`, `asc_copy_gm2l1_pad3_sync`, `asc_copy_gm2l1_pad4_sync`, `asc_copy_gm2l1_pad5_sync`, `asc_copy_gm2l1_pad6_sync`, `asc_copy_gm2l1_pad7_sync`, and `asc_copy_gm2l1_pad8_sync` interfaces are deprecated. Use the [corresponding non-synchronous interfaces](../cube_datamove/cube_compute_load/asc_copy_gm2l1_arch_2201.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Transfers matrix data from Global Memory to the L1 Buffer and supports different types of pad modes.

## Prototype

- Normal transfer mode (synchronous)

    ```cpp
    // No pad operation is performed.
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, a 31B pad is inserted for every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, a 15B pad is inserted for every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad2_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, a 14B pad is inserted for every 2B.
    __aicore__ inline void asc_copy_gm2l1_pad3_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, a 12B pad is inserted for every 4B.
    __aicore__ inline void asc_copy_gm2l1_pad4_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 8B of pad every 8B.
    __aicore__ inline void asc_copy_gm2l1_pad5_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 28B of every 32B, and store the lowest 4B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad6_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 24B of every 32B, and store the lowest 8B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad7_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 16B of every 32B, and store the lowest 16B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad8_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    ```

- 2D transfer mode (synchronous)

    ```cpp
    // 2D transfer of bfloat16_t type matrix data (synchronous).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D transfer of half type matrix data (synchronous).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D transfer of float type matrix data (synchronous).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D transfer of int32_t type matrix data (synchronous).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D transfer of int8_t type matrix data (synchronous).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // Transfer uint32_t matrix data in 2D mode (synchronous).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // Transfer uint8_t matrix data in 2D mode (synchronous).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

## Parameters

### Normal Transfer Mode Parameters

**Table 1** Normal transfer mode parameters

| Parameter | Input/Output | Description                                                              |
|:----------|:------|:----------------------------------------------------------------|
| dst       | Output    | Start address of the destination operand (vector).                                                 |
| src       | Input    | Start address of the source operand.                                                      |
| size      | Input    | Size of the data to transfer (in bytes).                                                     |
| dst_gap   | Input    | Gap between adjacent contiguous data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is DataBlock (32 bytes). |

### 2D Transfer Mode Parameters

**Table 2** 2D transfer mode parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| base_idx | Input | Fractal matrix ID, indicating the transfer start position as the ordinal number of the fractal in the source operand (0 indicates the first fractal matrix in the source operand). Value range: [0, 65535]. Unit: 512B. Default is 0. |
| repeat | Input | Number of iterations. |
| src_stride | Input | Interval between the start address of the previous fractal and the next fractal of the source operand between adjacent iterations. Unit: 512B. Value range: [0, 65535]. Default is 0. |
| dst_gap | Input | Interval between the end address of the previous fractal and the start address of the next fractal of the destination operand between adjacent iterations. Unit: 512B. Value range: [0, 65536]. Default is 0. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If multiple asc_copy_gm2l1 instructions need to be executed and their destination addresses overlap, insert synchronization instructions to ensure serialization of the multiple asc_copy_gm2l1 instructions and prevent abnormal data.

## Examples

```cpp
// Normal data transfer mode
// total_length indicates the total length of data involved in the transfer.
constexpr uint64_t total_length = 128;
__cbuf__ half dst[total_length];
// Number of consecutively transferred data blocks
constexpr uint16_t n_burst = 1;
// Length of consecutively transferred data blocks, which is 512 half elements here
constexpr uint16_t len_burst = 512 * sizeof(half) / 32;
// Perform continuous transfer of the source operand.
constexpr uint16_t src_gap = 0;
// The destination operand is arranged continuously.
constexpr uint16_t dst_gap = 0;
// src indicates the start address of the source operand.
asc_copy_gm2l1_sync((__cbuf__ void*)dst, (__gm__ void*)src, n_burst, len_burst, src_gap, dst_gap);

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
asc_copy_gm2l1_sync(dst, src, base_idx, repeat, src_stride, dst_gap);
```
