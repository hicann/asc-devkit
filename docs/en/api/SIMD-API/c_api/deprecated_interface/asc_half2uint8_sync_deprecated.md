---
okftype: reference.api
description: "Deprecated synchronous type conversion interface that converts half data to uint8_t under NONE or RINT, ROUND, FLOOR, CEIL, TRUNC rounding and writes it to dst. Only the 2201 architecture is supported. Use asc_half2uint8 together with asc_sync instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Replaces the original synchronous wait semantics of this interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_half2uint8.md
    kind: replaced_by
    condition: "Replaces the capability of this interface to convert to uint8_t by rounding mode. Synchronization must be inserted separately"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "asc_half2uint8_sync, asc_half2uint8_rna_sync, asc_half2uint8_ru_sync, asc_half2uint8_rd_sync, asc_half2uint8_rn_sync, asc_half2uint8_rz_sync in include/c_api/composite/vector_compute_composite.h (starting at lines 4074-4139) are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_half2uint8* and asc_sync), and the body text now uses the non-sync interface plus asc_sync; the declaration is still retained in 9.3.0-beta.1, being deprecated rather than removed."
---
# asc_half2uint8_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:36:07.764Z pushedAt=2026-11-18T03:12:16.536Z nl=2 -->

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

**The `asc_half2uint8_sync`, `asc_half2uint8_rn_sync`, `asc_half2uint8_rd_sync`, `asc_half2uint8_rna_sync`, `asc_half2uint8_ru_sync`, and `asc_half2uint8_rz_sync` APIs are deprecated. Use the [corresponding non-sync APIs](../vector_compute/vector_convert/asc_half2uint8.md) together with the sync API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts half data to the uint8_t type, supporting multiple rounding modes:

- NONE rounding mode: indicates the RINT rounding mode when the conversion involves precision loss, and indicates no rounding when no precision loss is involved
- RINT rounding mode: round half to even
- ROUND rounding mode: round half away from zero
- FLOOR rounding mode: round toward negative infinity
- CEIL rounding mode: round toward positive infinity
- TRUNC rounding mode: round toward zero

## Prototype

- Synchronous computation

    ```cpp
    // NONE rounding mode
    __aicore__ inline void asc_half2uint8_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
    // RINT rounding mode
    __aicore__ inline void asc_half2uint8_rn_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
    // FLOOR rounding mode
    __aicore__ inline void asc_half2uint8_rd_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
    // ROUND rounding mode
    __aicore__ inline void asc_half2uint8_rna_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
    // CEIL rounding mode
    __aicore__ inline void asc_half2uint8_ru_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
    // TRUNC rounding mode
    __aicore__ inline void asc_half2uint8_rz_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
// total_length indicates the total length of data involved in the conversion.
constexpr uint32_t total_length = 128;
__ubuf__ half src[total_length];
__ubuf__ uint8_t dst[total_length];
asc_half2uint8_sync(dst, src, total_length);
```
