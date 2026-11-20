---
okftype: reference.api
description: "Deprecated synchronous type conversion interface that converts a half source vector to float by count elements and writes the result to dst. It runs on PIPE_V and is supported only on the 2201 architecture. Use asc_half2float together with asc_sync instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Replaces the original synchronous wait semantics of this interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_half2float.md
    kind: replaced_by
    condition: "Replaces the half-to-float conversion capability of this interface; synchronization must be inserted separately"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "include/c_api/composite/vector_compute_composite.h:1083 is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_half2float and asc_sync). The body text uses the non-synchronous interface plus asc_sync. The declaration is still retained in 9.3.0-beta.1, which is deprecation rather than removal, and no upper version limit is set."
---
# asc_half2float_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:24:43.466Z pushedAt=2026-11-18T03:12:16.522Z nl=2 -->

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

**The `asc_half2float_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_convert/asc_half2float.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts the half type to the float type, with no rounding mode.

## Prototype

* Computes the first n data items

- Synchronous computation

  ```cpp
  // Indicates the RINT rounding mode when the conversion has precision loss, and indicates no rounding when no precision loss is involved.
  __aicore__ inline void asc_half2float_sync(__ubuf__ float* dst, __ubuf__ half* src, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

| Parameter    | Input/Output | Description                |
| ------ | ----- | ----------------- |
| dst    | Output    | Start address of the destination operand (vector).            |
| src    | Input    | Start address of the source operand (vector).             |
| count  | Input    | Number of elements involved in continuous computation.      |

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
__ubuf__ half src[total_length];
__ubuf__ float dst[total_length];
asc_half2float_sync(dst, src, total_length);
```
