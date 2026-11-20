---
okftype: reference.api
description: "Deprecated API page: asc_transpose_sync transposes a 16x16 two-dimensional matrix data block, providing int16_t and uint16_t forms. dst and src must be 32-byte aligned. It is recommended to use the non-sync asc_transpose plus asc_sync instead."
tag:
  - roles: [compute]
  - operations: [transpose]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The address overlap constraint follows its generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles sync waiting in the alternative"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_permute_sel/asc_transpose.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-sync interface and explicitly call asc_sync"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks ASC_DEPRECATED(9.2.0) before the asc_transpose_sync declaration. Release Notes 9.2.0 lists the _sync composite interface as deprecated as a whole; the interface has not been removed."
---
# asc_transpose_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:11:39.767Z pushedAt=2026-11-18T03:12:16.687Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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

**The `asc_transpose_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_permute_sel/asc_transpose.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Used to transpose a 16*16 two-dimensional matrix data block.

## Prototype

- Synchronous computation

  ```cpp
    __aicore__ inline void asc_transpose_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src)
    __aicore__ inline void asc_transpose_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src)
  ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector).|
|src|Input|Start address of the source operand (vector).|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
// total_length refers to the total length of the data involved in the computation.
constexpr int total_length = 256;
__ubuf__ int16_t dst[total_length];
__ubuf__ int16_t src[total_length];
// dst refers to the address of the destination operand.
asc_transpose_sync(dst, src);
```
