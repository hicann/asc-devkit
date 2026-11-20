---
okftype: reference.api
description: "Deprecated synchronous Unified Buffer internal data movement interface, supporting 16-bit and 32-bit integers and high-dimensional slicing. The replacement is asc_copy plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "the sync part of the replacement"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy.md
    kind: replaced_by
    condition: "the alternative non-sync interface declared in the body"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Since 9.2.0, the include marks ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy and asc_sync), and the body text provides the replacement solution; the declaration is retained, so this is deprecated rather than removed."
---
# asc_copy_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:33:14.685Z pushedAt=2026-11-18T03:12:16.443Z nl=2 -->

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

**The `asc_copy_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_datamove/asc_copy.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from Unified Buffer (UB) to UB. Supports high-dimensional slicing.

## Prototype

- Synchronous computation

```cpp
__aicore__ inline void asc_copy_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count)

__aicore__ inline void asc_copy_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count)

__aicore__ inline void asc_copy_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)

__aicore__ inline void asc_copy_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count)
```

## Parameters

**Table 1** Parameter description

| Parameter  | Input/Output | Description |
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
//total_length refers to the data length involved in the computation.
constexpr uint32_t total_length = 128;
__ubuf__ int16_t src[total_length];
__ubuf__ int16_t dst[total_length];
asc_copy_sync(dst, src, total_length);
```
