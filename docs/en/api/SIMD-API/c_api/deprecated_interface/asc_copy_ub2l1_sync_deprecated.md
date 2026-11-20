---
okftype: reference.api
description: "Deprecated synchronous interface for continuous transfer from UB to L1 Buffer. It performs synchronization after the transfer is complete, takes effect only on AIV, and returns directly when called by a non-AIV. The alternative is asc_copy_ub2l1 plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "the synchronization part of the alternative solution"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md
    kind: replaced_by
    condition: "the alternative non-sync interface declared in the body text"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Since 9.2.0, the include marker ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_ub2l1 and asc_sync), and the body text provides the alternative solution; the declaration is retained, classified as deprecated rather than removed."
---
# asc_copy_ub2l1_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:39:19.728Z pushedAt=2026-11-18T03:12:16.451Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

**The `asc_copy_ub2l1_sync` interface is deprecated. Use [asc_copy_ub2l1](../vector_datamove/asc_copy_ub2l1.md) and the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Copies a contiguous block of data from the Unified Buffer (UB) to the L1 Buffer, and performs synchronization after the copy is complete.

This interface takes effect only on the AIV.

## Prototype

```cpp
__aicore__ inline void asc_copy_ub2l1_sync(__cbuf__ void* dst, __ubuf__ void* src, uint32_t size)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination L1 Buffer. |
| src | Input | Start address of the source UB. |
| size | Input | Size of the data to copy, in bytes. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- When this interface is called on a non-AIV, it returns directly.
- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- Synchronous computation includes synchronization waiting.

## Examples

```cpp
constexpr uint32_t size = 32;
__ubuf__ uint8_t src[size];
__cbuf__ uint8_t dst[size];
asc_copy_ub2l1_sync(dst, src, size);
```
