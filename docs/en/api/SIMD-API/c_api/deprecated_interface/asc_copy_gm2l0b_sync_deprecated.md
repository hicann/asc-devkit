---
okftype: reference.api
description: "Deprecated synchronous data movement interface that moves matrix data from Global Memory into the L0B Buffer by start block, iteration count, source stride, and destination gap, covering eight data types from int4b_t to float. It is replaced by asc_copy_gm2l0b combined with asc_sync."
tag:
  - roles: [data_movement]
  - operations: [copy, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l0b.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite data movement interface is deprecated; use asc_copy_gm2l0b and asc_sync instead. All 8 declarations in include/c_api/composite/cube_datamove_composite.h carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table covers this with the asc_copy_*_sync composite interface entry."
---
# asc_copy_gm2l0b_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:25:47.576Z pushedAt=2026-11-18T03:12:16.378Z nl=2 -->

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

**The `asc_copy_gm2l0b_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_gm2l0b.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves matrix data from Global Memory to the L0B Buffer.

## Prototype

- Synchronous move

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

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| base_idx | Input | The matrix is divided into blocks of 16*16 elements. The start block ID of the move. |
| repeat | Input | Number of iterations. |
| src_stride | Input | Gap between the start addresses of the previous fractal and the next fractal of the source operand between adjacent iterations. |
| dst_gap | Input | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).

## Examples

```cpp
// The start block of the move is 1.
constexpr uint16_t base_idx = 1;
// The number of move iterations is 2.
constexpr uint8_t repeat = 2;
//The input transfer stride is 0 bytes, and the output transfer stride is 1024 bytes.
constexpr uint16_t src_stride = 0;
constexpr uint16_t dst_gap = 1;
// src indicates the start address of the source operand.
__cb__ half dst[256];
asc_copy_gm2l0b_sync(dst, src, base_idx, repeat, src_stride, dst_gap);
```
