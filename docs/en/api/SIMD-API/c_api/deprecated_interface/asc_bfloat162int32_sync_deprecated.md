---
okftype: reference.api
description: "Deprecated synchronous conversion interface that converts bfloat16_t elements in the Unified Buffer to int32_t and supports five rounding modes (rn, rna, rd, ru, rz). It is supported only on Atlas A2 and A3, and the replacement is asc_bfloat162int32 combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion, synchronization]
  - traits: [rounding_mode, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_bfloat162int32.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The five synchronous composite interfaces with rounding suffixes are deprecated. Use asc_bfloat162int32 and asc_sync instead. All 5 declarations in include/c_api/composite/vector_compute_composite.h:667-719 carry ASC_DEPRECATED(9.2.0)."
---
# asc_bfloat162int32_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:20:20.762Z pushedAt=2026-11-18T03:12:16.368Z nl=2 -->

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

**The `asc_bfloat162int32_rn_sync`, `asc_bfloat162int32_rna_sync`, `asc_bfloat162int32_rd_sync`, `asc_bfloat162int32_ru_sync`, and `asc_bfloat162int32_rz_sync` interfaces are deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_convert/asc_bfloat162int32.md) together with the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts bfloat16_t data to int32_t and supports multiple rounding modes:

- RINT rounding mode: round half to even
- ROUND rounding mode: round half away from zero
- FLOOR rounding mode: round toward negative infinity
- CEIL rounding mode: round toward positive infinity
- TRUNC rounding mode: round toward zero

## Prototype

- Synchronous computation

  ```cpp
  // RINT rounding mode
  __aicore__ inline void asc_bfloat162int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)

  // ROUND rounding mode
  __aicore__ inline void asc_bfloat162int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)

  // FLOOR rounding mode
  __aicore__ inline void asc_bfloat162int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)

  // CEIL rounding mode
  __aicore__ inline void asc_bfloat162int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)

  // TRUNC rounding mode
  __aicore__ inline void asc_bfloat162int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)
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
constexpr uint32_t total_length = 256;
__ubuf__ bfloat16_t src[total_length];
__ubuf__ int32_t dst[total_length];
asc_bfloat162int32_rna_sync(dst, src, 128);
```
