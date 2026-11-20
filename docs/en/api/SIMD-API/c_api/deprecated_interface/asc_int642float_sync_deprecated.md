---
okftype: reference.api
description: "Deprecated synchronous type conversion interface family that converts int64_t data to float using one of five rounding modes (RINT, ROUND, FLOOR, CEIL, or TRUNC) and computes on the PIPE_V pipeline. The replacement is the corresponding asynchronous interface used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address overlap constraints, see the generic address overlap constraint section on that page"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Used together with the asynchronous interface to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_int642float.md
    kind: replaced_by
    condition: "asynchronous interface that replaces this group of synchronous interfaces"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The five synchronous prototypes each carry an ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int642float_<mode> and asc_sync) marker in include/c_api/composite/vector_compute_composite.h (_rd/_rn/_rna/_ru/_rz correspond to their respective rounding modes), and the replacement is the corresponding asynchronous interface plus asc_sync. All prototypes on this page fall within the deprecation scope, which is a page-wide deprecation with no upper availability limit."
---
# asc_int642float_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:56:47.090Z pushedAt=2026-11-18T03:12:16.571Z nl=2 -->

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

**The `asc_int642float_rd_sync`, `asc_int642float_rn_sync`, `asc_int642float_rna_sync`, `asc_int642float_ru_sync`, and `asc_int642float_rz_sync` interfaces are deprecated. Use the [corresponding asynchronous interfaces](../vector_compute/vector_convert/asc_int642float.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts int64_t data to float.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_int642float_rd_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
    __aicore__ inline void asc_int642float_rn_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
    __aicore__ inline void asc_int642float_rna_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
    __aicore__ inline void asc_int642float_ru_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
    __aicore__ inline void asc_int642float_rz_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter    | Input/Output | Description                                |
| ------ | ----- |-----------------------------------|
| dst    | Output    | Start address of the destination operand (vector).                   |
| src    | Input    | Start address of the source operand (vector).                             |
| count  | Input    | Number of elements involved in continuous computation.                      |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
// total_length indicates the total length of the data involved in the conversion
constexpr uint64_t total_length = 128;
__ubuf__ int64_t src[total_length];
__ubuf__ float dst[total_length];
asc_int642float_rn_sync(dst, src, total_length);    // Convert src to float and store the result in dst
```
