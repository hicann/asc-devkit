---
okftype: reference.api
description: "Sets the source address stride and destination L1 address stride of adjacent iteration data blocks in the outer loop of the asc_copy_gm2l1_align Loop Fill mode. It works together with the inner stride and loop count, and takes effect only on the AIC."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md
    kind: configures
    condition: "Used only in Loop Fill mode, and this mode does not support left/right fill"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop1_stride.md
    kind: related
    condition: "The inner loop stride must be configured to match it"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop_size.md
    kind: related
    condition: "The two-level loop count must be configured to match it, and reset after the data movement"
version: ">=9.0.0"
version_changes: []
---
# asc_set_gm2l1_loop2_stride

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:30:19.926Z pushedAt=2026-11-18T08:15:34.324Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

When moving data from Global Memory (GM) to the L1 Buffer, sets the source and destination L1 stride of adjacent iteration data blocks in the outer loop of the [asc_copy_gm2l1_align](../cube_compute_load/asc_copy_gm2l1_align.md) Loop Fill mode.

The inner loop stride and the two-level loop counts must be configured through [asc_set_gm2l1_loop1_stride](asc_set_gm2l1_loop1_stride.md) and [asc_set_gm2l1_loop_size](asc_set_gm2l1_loop_size.md), respectively.

This API takes effect only on the AIC.

## Prototype

```cpp
__aicore__ inline void asc_set_gm2l1_loop2_stride(uint64_t loop2_src_stride, uint64_t loop2_dst_stride)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| loop2_src_stride | Input | Stride of the source operand data blocks of adjacent iterations in the outer loop, in bytes, with a value range of [0, $2^{40}-1$]. |
| loop2_dst_stride | Input | Stride of the destination L1 data blocks of adjacent iterations in the outer loop, in bytes, with a value range of [0, $2^{21}-1$], and must be 32-byte aligned. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This API returns directly when called on a non-AIC.
- The configuration is used only by the [asc_copy_gm2l1_align](../cube_compute_load/asc_copy_gm2l1_align.md) Loop Fill mode; the Loop Fill mode does not support left/right padding.
- `loop2_dst_stride` must be 32-byte aligned, which takes effect even when `loop2_size` is 1.

## Examples

For a complete example of 2×2 GM-to-L1 loop transfer, L1-to-UB readback, and host-side byte-by-byte verification, see [asc_set_gm2l1_loop_size Example](asc_set_gm2l1_loop_size.md#examples).

The following code configures the source stride of adjacent data blocks in the outer loop to 128B and the destination L1 stride to 64B:

```cpp
// Execute loop1 twice and loop2 twice.
asc_set_gm2l1_loop_size(2, 2);
asc_set_gm2l1_loop1_stride(64, 32);
// Outer loop: adjacent iterations are 128B apart in GM and 64B apart in L1.
asc_set_gm2l1_loop2_stride(128, 64);
asc_copy_gm2l1_align(dst, src, 1, 32, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_KEEP, 0, 32);
// Reset the loop count after the transfer is complete.
asc_set_gm2l1_loop_size(1, 1);
```
