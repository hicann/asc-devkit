---
okftype: reference.api
description: "Deprecated vector-scalar addition synchronization interface that adds the scalar value to the source vector element by element and writes the result to the destination vector. It provides four same-type forms: int16_t, half, int32_t, and float. The replacement is asc_add_scalar used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [add, synchronization]
  - traits: [scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_arith/asc_add_scalar.md
    kind: replaced_by
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The composite sync interface family is deprecated. Use asc_add_scalar and asc_sync instead. All 4 declarations in include/c_api/composite/vector_compute_composite.h:87-129 carry ASC_DEPRECATED(9.2.0)."
---
# asc_add_scalar_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:00:10.995Z pushedAt=2026-11-16T09:22:44.193Z nl=2 -->

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

**The `asc_add_scalar_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_arith/asc_add_scalar.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs vector addition. The calculation formula is as follows:

$$
dst_i = src_i + value
$$

## Prototype

- Synchronous computation

```cpp
__aicore__ inline void asc_add_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand. |
| src | Input    | Start address of the source operand (vector). |
| value | Input    | Source operand (scalar). |
| count     | Input    | Number of elements involved in continuous computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
//total_length indicates the data length involved in the computation.
constexpr uint64_t total_length = 64;
half scalar_val = 0.0f;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_add_scalar_sync(dst, src, scalar_val, total_length);
```
