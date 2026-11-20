---
okftype: reference.api
description: "Deprecated synchronous type conversion API that converts int16_t data to half with no rounding or under RINT, ROUND, FLOOR, CEIL, or TRUNC rounding and writes it to dst. It is supported only on the 2201 architecture. Use asc_int162half together with asc_sync instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Replaces the original synchronous wait semantics of this API"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_int162half.md
    kind: replaced_by
    condition: "Replaces the capability of this API to convert to half by rounding mode, and requires a separate synchronization to be inserted"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "asc_int162half_sync, asc_int162half_rna_sync, asc_int162half_ru_sync, asc_int162half_rd_sync, asc_int162half_rn_sync, and asc_int162half_rz_sync in include/c_api/composite/vector_compute_composite.h (starting at lines 3771-3834) are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_int162half* and asc_sync). Use the non-sync interfaces with asc_sync instead. The declarations are still retained in 9.3.0-beta.1, which is a deprecation rather than a removal."
---
# asc_int162half_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:42:59.637Z pushedAt=2026-11-18T03:12:16.545Z nl=2 -->

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

**The `asc_int162half_sync`, `asc_int162half_rn_sync`, `asc_int162half_rna_sync`, `asc_int162half_rd_sync`, `asc_int162half_ru_sync`, and `asc_int162half_rz_sync` interfaces are deprecated. Use the [corresponding non-sync interfaces](../vector_compute/vector_convert/asc_int162half.md) and the sync interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts int16_t data to the half type, supporting multiple rounding modes:
- No rounding mode specified: no rounding.
- RINT rounding mode: round half to even.
- ROUND rounding mode: round half away from zero.
- FLOOR rounding mode: round toward negative infinity.
- CEIL rounding mode: round toward positive infinity.
- TRUNC rounding mode: round toward zero.

## Prototype

- Synchronous computation

  ```cpp
  // Indicates the RINT rounding mode when the conversion has precision loss, and indicates no rounding when no precision loss is involved.
  __aicore__ inline void asc_int162half_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
  // RINT rounding mode
  __aicore__ inline void asc_int162half_rn_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
  // ROUND rounding mode
  __aicore__ inline void asc_int162half_rna_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
  // FLOOR rounding mode
  __aicore__ inline void asc_int162half_rd_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
  // CEIL rounding mode
  __aicore__ inline void asc_int162half_ru_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
  // TRUNC rounding mode
  __aicore__ inline void asc_int162half_rz_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----| :-----| :-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector).|
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length refers to the data length involved in the computation
__ubuf__ half dst[total_length];
__ubuf__ int16_t src[total_length];
asc_int162half_sync(dst, src, total_length);
```
