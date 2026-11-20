---
okftype: reference.api
description: "Deprecated API page: Under the 2201 architecture, asc_sub_scalar_sync performs elementwise subtraction between a vector and a scalar. dst and src must be 32-byte aligned. It supports four scalar types: int16_t, half, int32_t, and float. It is recommended to use the non-synchronous asc_sub_scalar together with asc_sync instead."
tag:
  - roles: [compute]
  - operations: [subtract]
  - traits: [elementwise, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The address overlap constraint follows its generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles synchronous waiting in the alternative solution"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_arith/asc_sub_scalar.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding asynchronous interface instead and explicitly call asc_sync"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks ASC_DEPRECATED(9.2.0) before the asc_sub_scalar_sync declaration, and Release Notes 9.2.0 lists the _sync composite interface as deprecated as a whole; the interface is not removed."
---
# asc_sub_scalar_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:08:42.504Z pushedAt=2026-11-18T03:12:16.685Z nl=2 -->

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

**The `asc_sub_scalar_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_arith/asc_sub_scalar.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs subtraction between a vector and a scalar.

$$
dst_i = src_i - value
$$

## Prototype

- Synchronous data computation

```cpp
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| dst     | Output     | Start address of the destination operand (vector).   |
| src     | Input     | Start address of the source operand (vector).|
| value     | Input     | Source operand (scalar).|
| count   | Input     | Number of elements involved in continuous computation.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
// total_length indicates the total length of the data involved in the computation.
constexpr uint64_t total_length = 64;
half scalar_val = 0.0f;
__ubuf__ half dst[total_length];
__ubuf__ half src[total_length];
asc_sub_scalar_sync(dst, src, scalar_val, total_length);
```
