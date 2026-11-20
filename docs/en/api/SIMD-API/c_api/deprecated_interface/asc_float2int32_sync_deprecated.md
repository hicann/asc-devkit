---
okftype: reference.api
description: "Deprecated synchronous interface for converting float to int32_t. Converts a float vector to int32_t using the RINT, ROUND, FLOOR, CEIL, or TRUNC rounding mode and waits for PIPE_V. The replacement is the corresponding non-synchronous interface used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the non-synchronous interface, you must call asc_sync to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_float2int32.md
    kind: replaced_by
    condition: "The non-synchronous conversion interface replaces the synchronous interface on this page"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks the five sync overloads rn/rna/rd/ru/rz individually with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2int32_<mode> and asc_sync); the CANN_9_2_0 Release Notes deprecation table covers this class of *_sync composite interfaces; the interfaces are retained and available only on 2201."
---
# asc_float2int32_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:13:06.641Z pushedAt=2026-11-18T03:12:16.501Z nl=2 -->

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

**The `asc_float2int32_rn_sync`, `asc_float2int32_rna_sync`, `asc_float2int32_rd_sync`, `asc_float2int32_ru_sync`, and `asc_float2int32_rz_sync` APIs are deprecated. Use the [corresponding non-synchronous APIs](../vector_compute/vector_convert/asc_float2int32.md) together with the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts the float type to the int32_t type and supports multiple rounding modes:

* RINT rounding mode: round half to even
* ROUND rounding mode: round half away from zero
* FLOOR rounding mode: round toward negative infinity
* CEIL rounding mode: round toward positive infinity
* TRUNC rounding mode: round toward zero

## Prototype

* Computes the first n data elements

- Synchronous computation

  ```cpp
  // RINT rounding mode
  __aicore__ inline void asc_float2int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
  // ROUND rounding mode.
  __aicore__ inline void asc_float2int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
  // FLOOR rounding mode.
  __aicore__ inline void asc_float2int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
  // CEIL rounding mode.
  __aicore__ inline void asc_float2int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
  // TRUNC rounding mode.
  __aicore__ inline void asc_float2int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

| Parameter    | Input/Output | Description                |
| ------ | ----- | ----------------- |
| dst    | Output    | Start address of the destination operand (vector). |
| src    | Input    | Start address of the source operand (vector).   |
| count  | Input    | Number of elements involved in continuous computation.      |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
__ubuf__ int32_t dst[256];
__ubuf__ float src[256];
asc_float2int32_rn_sync(dst, src, 256);
```
