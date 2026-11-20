---
okftype: reference.api
description: "Deprecated float precision conversion synchronous interfaces that perform precision conversion on float vectors using the RINT, ROUND, FLOOR, CEIL, or TRUNC rounding mode and wait for PIPE_V. The alternative is to use the corresponding asynchronous interface combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the asynchronous interface, you need to call asc_sync to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_float2float.md
    kind: replaced_by
    condition: "The asynchronous precision conversion interface replaces the synchronous interface on this page"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header annotates each of the five sync overloads rn/rd/ru/rna/rz with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2float_<mode> and asc_sync); the CANN_9_2_0 Release Notes deprecation table covers this class of *_sync composite interfaces; the interfaces are retained and available only in 2201."
---
# asc_float2float_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:07:32.667Z pushedAt=2026-11-18T03:12:16.490Z nl=2 -->

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

**The `asc_float2float_rn_sync`, `asc_float2float_rd_sync`, `asc_float2float_ru_sync`, `asc_float2float_rna_sync`, and `asc_float2float_rz_sync` interfaces are deprecated. Use the [corresponding asynchronous interface](../vector_compute/vector_convert/asc_float2float.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs precision conversion on float type data and supports multiple rounding modes:

- RINT rounding mode: round half to even
- ROUND rounding mode: round half away from zero
- FLOOR rounding mode: round toward negative infinity
- CEIL rounding mode: round toward positive infinity
- TRUNC rounding mode: round toward zero

## Prototype

- Synchronous mode

    ```cpp
    // RINT mode
    __aicore__ inline void asc_float2float_rn_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // FLOOR mode
    __aicore__ inline void asc_float2float_rd_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // CEIL mode
    __aicore__ inline void asc_float2float_ru_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // ROUND mode
    __aicore__ inline void asc_float2float_rna_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    // TRUNC mode
    __aicore__ inline void asc_float2float_rz_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
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
__ubuf__ float src[total_length];
__ubuf__ float dst[total_length];
asc_float2float_rn_sync(dst, src, total_length);
```
