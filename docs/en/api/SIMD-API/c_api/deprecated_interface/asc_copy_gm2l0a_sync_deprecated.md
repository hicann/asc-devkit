---
okftype: reference.api
description: "Deprecated synchronous data movement interface that moves matrix data from Global Memory into the L0A Buffer by fractal id, iteration count, source stride, and destination interval, covering eight types including bfloat16_t and half. The replacement is asc_copy_gm2l0a combined with asc_sync."
tag:
  - roles: [data_movement]
  - operations: [copy, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l0a.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The matrix data movement synchronous composite interface is deprecated; use asc_copy_gm2l0a and asc_sync instead. The 8 declarations starting from include/c_api/composite/cube_datamove_composite.h:2301 all carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table covers them under the asc_copy_*_sync composite interface entry."
---
# asc_copy_gm2l0a_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:25:10.698Z pushedAt=2026-11-18T03:12:16.377Z nl=2 -->

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

**The `asc_copy_gm2l0a_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_gm2l0a.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from GM to the L0A Buffer.

## Prototype

- Synchronous data movement

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

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                        |
|-----| ------------ |-----------------------------------------------------------|
| dst | Output | Start address of the destination operand.                                                |
| src | Input | Start address of the source operand.                                                 |
| base_idx | Input | Fractal matrix ID, indicating which fractal of the source operand the movement starts from (0 indicates the first fractal matrix in the source operand). Value range: [0, 65535]. |
| repeat | Input | Number of iterations. Each iteration can process 512B of data. Value range: [1, 255].                        |
| src_stride | Input | Gap between the start addresses of the previous fractal and the next fractal of the source operand between adjacent iterations, in 512B. Value range: [0, 65535].    |
| dst_gap | Input | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand between adjacent iterations, in 512B. Value range: [0, 65535].   |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
__ca__ half dst[256];
// src indicates the start address of the source operand
asc_copy_gm2l0a_sync(dst, src, 0, 1, 64, 64);
```
