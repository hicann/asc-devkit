---
okftype: reference.api
description: "Deprecated synchronous math API that computes the maximum of src and the scalar value element-wise and writes the result to dst. It provides four overloads for int16_t, half, int32_t, and float. The replacement is asc_max_scalar used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [maximum]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For the operand address overlap constraint, see the generic address overlap constraint section on that page"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Used together with the non-synchronous API to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_arith/asc_max_scalar.md
    kind: replaced_by
    condition: "The non-synchronous API that replaces this synchronous scalar maximum API"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "All four type overloads carry the ASC_DEPRECATED(9.2.0, 2027/09/07, asc_max_scalar and asc_sync) marker (the macro line before the corresponding @override_id in include/c_api/composite/vector_compute_composite.h); the replacement is the non-synchronous interface asc_max_scalar plus asc_sync. All prototypes on this page fall within the deprecation scope, constituting page-level deprecation with no upper limit on availability."
---
# asc_max_scalar_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:14:21.487Z pushedAt=2026-11-18T03:12:16.600Z nl=2 -->

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

**The `asc_max_scalar_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_arith/asc_max_scalar.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Compares each element of the vector src with the scalar value, and then stores the maximum value into dst according to the corresponding bit. The calculation formula is as follows:

$$
dst_i = max(src_i, value)
$$

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_max_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
  __aicore__ inline void asc_max_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
  __aicore__ inline void asc_max_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
   __aicore__ inline void asc_max_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Start address of the destination operand (vector).            |
| src | Input    | Start address of the source operand (vector).           |
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
constexpr int total_length = 256; //total_length refers to the data length involved in the calculation.
half value = 3;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_max_scalar_sync(dst, src, value, total_length);
// src : [1, 2, 3, 4, ... , 256]
// value : 3
// dst : [3, 3, 3, 5, ... , 256]
```
