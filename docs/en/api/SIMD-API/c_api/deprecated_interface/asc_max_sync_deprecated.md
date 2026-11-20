---
okftype: reference.api
description: "Deprecated synchronous math interface that computes the maximum of two source vectors element by element and writes the result to dst. It provides four overloads: int16_t, half, int32_t, and float. The replacement is asc_max combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [maximum]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address overlap constraints, see the generic address overlap constraint section on this page"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Works with the asynchronous interface to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_arith/asc_max.md
    kind: replaced_by
    condition: "The asynchronous interface that replaces this synchronous maximum interface"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "All four type overloads carry the ASC_DEPRECATED(9.2.0, 2027/09/07, asc_max and asc_sync) marker (the macro line before the corresponding @override_id in include/c_api/composite/vector_compute_composite.h), and the replacement is the asynchronous interface asc_max plus asc_sync; all prototypes on this page fall within the deprecation scope, which is a page-wide deprecation with no upper availability limit."
---
# asc_max_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:14:43.813Z pushedAt=2026-11-18T03:12:16.601Z nl=2 -->

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

**The `asc_max_sync` interface is deprecated. Use the [corresponding asynchronous interface](../vector_compute/vector_arith/asc_max.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs element-wise maximum computation. The calculation formula is as follows:

$$
dst_i = max(src0_i, src1_i)
$$

## Prototype

- Synchronous computation

```cpp
__aicore__ inline void asc_max_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
__aicore__ inline void asc_max_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
__aicore__ inline void asc_max_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
__aicore__ inline void asc_max_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description               |
| :--- | :--- | :--- |
| dst       | Output    | Start address of the destination operand (vector). |
| src0, src1 | Input    | Start address of the source operand (vector). |
| count     | Input    | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
//total_length refers to the total length of the data to be computed.
constexpr uint64_t total_length = 64;
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ half dst[total_length];
asc_max_sync(dst, src0, src1, total_length);
```
