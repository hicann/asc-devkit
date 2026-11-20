---
okftype: reference.api
description: "Deprecated vector ReLU synchronization interface that computes element-wise using the formula where the destination element equals the larger of the source element and 0. Supports half, int32_t, and float. The replacement is asc_relu used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [relu, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_relu.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite interface is deprecated; use asc_relu and asc_sync instead. All 3 declarations in include/c_api/composite/vector_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_relu_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:38:05.513Z pushedAt=2026-11-18T03:12:16.641Z nl=2 -->

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

The header file path is `"c_api/composite/vector_compute_composite.h"`.

**The `asc_relu_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_fused/asc_relu.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs linear rectification (ReLU) element-wise. The calculation formula is as follows:
$$
dst_i = relu(src_i)
$$

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_relu_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
  __aicore__ inline void asc_relu_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)
  __aicore__ inline void asc_relu_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector).|
|src|Input|Start address of the source operand (vector).|
|count|Input|Number of elements involved in the computation.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
constexpr int total_length = 256; // total_length indicates the total length of data involved in the computation.
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_relu_sync(dst, src, total_length);
// src : [6.938, -8.86, -0.2263, ..., 1.778]
// dst : [6.938, 0, 0, ..., 1.778]
```
