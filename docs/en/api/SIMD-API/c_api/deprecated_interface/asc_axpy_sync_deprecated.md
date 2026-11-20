---
okftype: reference.api
description: "Deprecated vector multiply-accumulate synchronous interface that computes by multiplying each source element by the scalar value and then adding it to the destination element, providing three forms: half, half source with float destination, and float. The replacement is asc_axpy combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [fused_multiply_add, synchronization]
  - traits: [scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_axpy.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The composite synchronous interface family is deprecated; use asc_axpy and asc_sync instead. All 3 declarations in include/c_api/composite/vector_compute_composite.h:3601-3629 carry ASC_DEPRECATED(9.2.0)."
---
# asc_axpy_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:14:12.227Z pushedAt=2026-11-18T03:12:16.360Z nl=2 -->

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

**The `asc_axpy_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_fused/asc_axpy.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Each element in the source operand src is multiplied by the scalar value, and the result is added to the corresponding element in the destination operand dst. The calculation formula is as follows:

$$
dst_i = src_i * value + dst_i
$$

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_axpy_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
  __aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ half* src, half value, uint32_t count)
  __aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Start address of the destination operand (vector).            |
| src | Input    | Start address of the source operand (vector).            |
| value | Input    | Source operand (scalar).       |
| count     | Input    | Number of elements involved in continuous computation.      |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the data length involved in the calculation.
half value = 10;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_axpy_sync(dst, src, value, total_length);
```
