---
okftype: reference.api
description: "Deprecated synchronous math interface that computes the element-wise minimum of src and scalar value and writes the result to dst. It provides four overloads: half, float, int16_t, and int32_t. The replacement is asc_min_scalar used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [minimum]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For the operand address overlap constraint, see the general-purpose address overlap constraint section on that page."
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Used together with the non-synchronous interface to complete synchronization."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_arith/asc_min_scalar.md
    kind: replaced_by
    condition: "The non-synchronous interface that replaces this synchronous scalar minimum interface."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "All four type overloads carry the ASC_DEPRECATED(9.2.0, 2027/09/07, asc_min_scalar and asc_sync) marker (the macro line before the corresponding @override_id in include/c_api/composite/vector_compute_composite.h). The replacement is the non-synchronous interface asc_min_scalar plus asc_sync. All prototypes on this page fall within the deprecation scope, which is page-wide deprecation with no upper availability limit."
---
# asc_min_scalar_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:16:08.849Z pushedAt=2026-11-18T03:12:16.603Z nl=2 -->

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

**The `asc_min_scalar_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_arith/asc_min_scalar.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

The source operand vector is compared with the scalar element by element. If an element is greater than the scalar value, the scalar value is taken; if it is smaller than the scalar value, the source operand value is taken. The calculation formula is as follows:

$$
dst_i = min(src_i, value)
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_min_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
    __aicore__ inline void asc_min_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
    __aicore__ inline void asc_min_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
    __aicore__ inline void asc_min_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| value | Input | Source operand (scalar). |
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
// total_length indicates the data length involved in the calculation.
constexpr uint32_t total_length = 32;
__ubuf__ half dst[total_length];
__ubuf__ half src[total_length];
half value = 3;
asc_min_scalar_sync(dst, src, value, total_length);
// src : [1, 2, 3, 4, ... , 32]
// value : 3
// dst : [1, 2, 3, 3, ... , 3]
```
