---
okftype: reference.api
description: "Configuration API that sets the Mask mode to the system default Normal mode. In this mode, the Mask parameter controls the elements participating in computation within a single iteration of high-dimensional slicing computation. Must be used together with asc_set_vector_mask, with pipeline type PIPE_S."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_mask_config/asc_set_vector_mask.md
    kind: related
    condition: "The Normal mode set by this API must be used together with the mask value setting API."
version: ">=8.5.0"
version_changes: []
---
# asc_set_mask_norm

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.926576+00:00 pushedAt=2026-11-16T02:10:59.066Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_mask_config.h"`.

Sets the mask mode to Normal mode, which is the system default mode. In this mode, the Mask parameter controls the elements that participate in computation within a single iteration during high-dimensional slicing computation.

## Prototype

```cpp
__aicore__ inline void asc_set_mask_norm()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

This API must be used together with [asc_set_vector_mask](asc_set_vector_mask.md).

## Examples

```cpp
asc_set_mask_norm();
asc_set_vector_mask(0xffffffffffffffff, 0xffffffffffffffff);    // Set all elements in each iteration to participate in the computation.

// Perform high-dimensional slicing computation.
constexpr uint32_t total_length = 128; //total_length refers to the total length of the data involved in the computation.
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ half dst[total_length];

uint8_t repeat = 1; // 1 repeat can process 128 half-type data elements.
uint8_t dst_block_stride = 1;
uint8_t src0_block_stride = 1;
uint8_t src1_block_stride = 1;
uint8_t dst_repeat_stride = 8;
uint8_t src0_repeat_stride = 8;
uint8_t src1_repeat_stride = 8;
asc_add(dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride,
    dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```
