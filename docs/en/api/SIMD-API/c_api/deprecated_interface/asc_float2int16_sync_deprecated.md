---
okftype: reference.api
description: "Deprecated float-to-int16_t sync interface that converts a float vector to int16_t using the RINT, ROUND, FLOOR, CEIL, or TRUNC rounding mode and waits for PIPE_V. The alternative is the corresponding non-sync interface used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the non-sync interface, call asc_sync to complete synchronization."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_float2int16.md
    kind: replaced_by
    condition: "The non-sync conversion interface replaces the sync interface on this page."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks the five synchronous overloads rn/rna/rd/ru/rz individually with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2int16_<mode> and asc_sync); the CANN_9_2_0 Release Notes deprecation table covers this class of *_sync composite interfaces; the interfaces are retained and available only on 2201."
---
# asc_float2int16_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:11:33.820Z pushedAt=2026-11-18T03:12:16.499Z nl=2 -->

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

**The `asc_float2int16_rn_sync`, `asc_float2int16_rna_sync`, `asc_float2int16_rd_sync`, `asc_float2int16_ru_sync`, and `asc_float2int16_rz_sync` APIs are deprecated. Use the [corresponding asynchronous APIs](../vector_compute/vector_convert/asc_float2int16.md) together with the synchronization API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts float data to int16_t, supporting multiple rounding modes:

- RINT rounding mode: round half to even.
- ROUND rounding mode: round half away from zero.
- FLOOR rounding mode: round toward negative infinity.
- CEIL rounding mode: round toward positive infinity.
- TRUNC rounding mode: round toward zero.

## Prototype

- Synchronous computation

    ```cpp
  // RINT rounding mode
  __aicore__ inline void asc_float2int16_rn_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
  // ROUND rounding mode
  __aicore__ inline void asc_float2int16_rna_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
  // FLOOR rounding mode
  __aicore__ inline void asc_float2int16_rd_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
  // CEIL rounding mode
  __aicore__ inline void asc_float2int16_ru_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
  // TRUNC rounding mode
  __aicore__ inline void asc_float2int16_rz_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----| :-----| :-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector). |
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
// total_length indicates the data length involved in the computation
constexpr uint64_t total_length = 64;
__ubuf__ float src[total_length];
__ubuf__ int16_t dst[total_length];
asc_float2int16_rn_sync(dst, src, total_length);
```
