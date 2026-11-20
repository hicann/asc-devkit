---
okftype: reference.api
description: "Deprecated sync type conversion interface that converts half data to int4 by RINT, ROUND, FLOOR, CEIL, or TRUNC rounding and writes the result to dst. It runs on PIPE_V and is supported only on the 2201 architecture. Use asc_half2int4 with asc_sync instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Replaces the original synchronous wait semantics of this interface."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_half2int4.md
    kind: replaced_by
    condition: "Replaces the capability of this interface to convert to int4 by rounding mode; synchronization must be inserted separately."
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "asc_half2int4_sync and asc_half2int4_rn/rna/ru/rd/rz_sync in include/c_api/composite/vector_compute_composite.h (starting from lines 1096-1161) are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_half2int4* and asc_sync). The main text uses the non-sync interface plus asc_sync instead. The declaration is still retained in 9.3.0-beta.1, which is deprecation rather than removal."
---
# asc_half2int4_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:31:16.559Z pushedAt=2026-11-18T03:12:16.528Z nl=2 -->

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

**The `asc_half2int4_sync`, `asc_half2int4_rn_sync`, `asc_half2int4_rd_sync`, `asc_half2int4_rna_sync`, `asc_half2int4_ru_sync`, and `asc_half2int4_rz_sync` interfaces are deprecated. Use the [corresponding non-synchronous interfaces](../vector_compute/vector_convert/asc_half2int4.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts half type data to int4 type, supporting multiple rounding modes:

- RINT rounding mode: round half to even
- ROUND rounding mode: round half up
- FLOOR rounding mode: round toward negative infinity
- CEIL rounding mode: round toward positive infinity
- TRUNC rounding mode: round toward zero

## Prototype

- Synchronous computation

    ```cpp
    //Indicates the RINT rounding mode when conversion has precision loss, and represents no rounding when no precision loss is involved.
    __aicore__ inline void asc_half2int4_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //RINT rounding mode
    __aicore__ inline void asc_half2int4_rn_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //FLOOR rounding mode
    __aicore__ inline void asc_half2int4_rd_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //ROUND rounding mode
    __aicore__ inline void asc_half2int4_rna_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //CEIL rounding mode
    __aicore__ inline void asc_half2int4_ru_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //TRUNC rounding mode
    __aicore__ inline void asc_half2int4_rz_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
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

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- When dst is int4b_t, the count of the first n data computation interfaces must be an even number.

## Examples

```cpp
constexpr uint32_t total_length = 256;
uint64_t offset = 0;
__ubuf__ half* src = (__ubuf__ half*)asc_get_phy_buf_addr(0);
offset += total_length * sizeof(half);
__ubuf__ int4b_t* dst = (__ubuf__ int4b_t*)asc_get_phy_buf_addr(offset);
asc_half2int4_sync(dst, src, total_length);
```
