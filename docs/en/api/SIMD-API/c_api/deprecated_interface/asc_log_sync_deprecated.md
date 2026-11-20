---
okftype: reference.api
description: "Deprecated synchronous interface that computes the natural logarithm element-wise and calculates based on the PIPE_V pipeline, providing overloads for half and float source types. The replacement is asc_log used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [natural_log]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For the operand address overlap constraint, see the generic address overlap constraint section on that page."
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Works with the asynchronous interface to complete synchronization."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_arith/asc_log.md
    kind: replaced_by
    condition: "The asynchronous interface that replaces this synchronous interface."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Both source type overloads carry the ASC_DEPRECATED(9.2.0, 2027/09/07, asc_log and asc_sync) marker (the macro line before the corresponding @override_id in include/c_api/composite/vector_compute_composite.h). The replacement is the asynchronous interface asc_log plus asc_sync. All prototypes on this page fall within the deprecation scope, which is a page-level deprecation with no upper limit on availability."
---
# asc_log_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:11:14.857Z pushedAt=2026-11-18T03:12:16.597Z nl=2 -->

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

**The `asc_log_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_arith/asc_log.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Computes the natural logarithm element-wise. The calculation formula is as follows:
$$
dst_i = ln({src_i})
$$

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_log_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
  __aicore__ inline void asc_log_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector). |
|src|Input|Start address of the source operand (vector). |
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
__ubuf__ half dst[256];
__ubuf__ half src[256];
asc_log_sync(dst, src, 256);
```
