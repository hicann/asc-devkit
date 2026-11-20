---
okftype: reference.api
description: "Deprecated API page: Under the 2201 architecture, asc_sqrt_sync computes the element-wise square root of a half or float vector and writes the result to dst. It requires src to be positive and dst and src to be 32-byte aligned. It is recommended to use the asynchronous asc_sqrt plus asc_sync instead."
tag:
  - roles: [compute]
  - operations: [sqrt]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The address overlap constraint follows its generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles synchronous waiting in the alternative approach"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_arith/asc_sqrt.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding asynchronous interface and explicitly call asc_sync"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header annotates ASC_DEPRECATED(9.2.0) before the asc_sqrt_sync declaration, and Release Notes 9.2.0 lists the _sync composite interfaces as deprecated as a whole; the interface is not removed."
---
# asc_sqrt_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:58:32.567Z pushedAt=2026-11-18T03:12:16.673Z nl=2 -->

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

**The `asc_sqrt_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_arith/asc_sqrt.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Computes the square root element-wise. The calculation formula is as follows:
$$
dst_i = \sqrt{src_i}
$$

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_sqrt_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
  __aicore__ inline void asc_sqrt_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
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

- src only supports positive numbers.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length refers to the data length involved in the computation.
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_sqrt_sync(dst, src, total_length);
```
