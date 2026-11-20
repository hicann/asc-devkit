---
okftype: reference.api
description: "Deprecated synchronous interface for converting float to bfloat16_t. It converts a float vector to bfloat16_t using the RINT, ROUND, FLOOR, CEIL, or TRUNC rounding mode and waits for PIPE_V. The replacement is the corresponding asynchronous interface used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the asynchronous interface, you must call asc_sync yourself to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_float2bfloat16.md
    kind: replaced_by
    condition: "The asynchronous conversion interface replaces the synchronous interface on this page"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks the five synchronous overloads rn/rna/rd/ru/rz with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2bfloat16_<mode> and asc_sync) one by one; the CANN_9_2_0 Release Notes deprecation table covers this class of *_sync composite interfaces; the interfaces are retained and available only on 2201."
---
# asc_float2bfloat16_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:02:29.538Z pushedAt=2026-11-18T03:12:16.484Z nl=2 -->

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

**The `asc_float2bfloat16_rn_sync`, `asc_float2bfloat16_rna_sync`, `asc_float2bfloat16_rd_sync`, `asc_float2bfloat16_ru_sync`, and `asc_float2bfloat16_rz_sync` interfaces are deprecated. Use the [corresponding non-synchronous interfaces](../vector_compute/vector_convert/asc_float2bfloat16.md) together with the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts float type data to bfloat16_t type and supports multiple rounding modes:

- RINT rounding mode: round half to even
- ROUND rounding mode: round half away from zero
- FLOOR rounding mode: round toward negative infinity
- CEIL rounding mode: round toward positive infinity
- TRUNC rounding mode: round toward zero

## Prototype

- Synchronous computation

    ```cpp
    // RINT rounding mode
    __aicore__ inline void asc_float2bfloat16_rn_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
    // ROUND rounding mode
    __aicore__ inline void asc_float2bfloat16_rna_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
    // FLOOR rounding mode
    __aicore__ inline void asc_float2bfloat16_rd_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
    // CEIL rounding mode
    __aicore__ inline void asc_float2bfloat16_ru_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
    // TRUNC rounding mode
    __aicore__ inline void asc_float2bfloat16_rz_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|----|-----|-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector).|
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
constexpr uint32_t total_length = 256;
__ubuf__ float src[total_length];
__ubuf__ bfloat16_t dst[total_length];
asc_float2bfloat16_rn_sync(dst, src, total_length);
```
